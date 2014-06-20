#include "stdafx.h"
#include "Kinect.h"

Kinect::Kinect(void)
{
	m_utUserTracker = new nite::UserTracker();
}

Kinect::~Kinect(void)
{
	openni::OpenNI::removeDeviceConnectedListener(this);
	openni::OpenNI::removeDeviceDisconnectedListener(this);
	openni::OpenNI::removeDeviceStateChangedListener(this);

	if ( m_utUserTracker->isValid() )
	{
		m_utUserTracker->removeNewFrameListener(&m_fgDepth);
		m_utUserTracker->destroy();
	}

	if(m_vsColorStream.isValid())
	{
		m_vsColorStream.removeNewFrameListener(&m_fgColor);
		m_vsColorStream.stop();
		m_vsColorStream.destroy();
	}
	
	if(m_vsDepthStream.isValid())
	{
		m_vsDepthStream.stop();	
		m_vsDepthStream.destroy();
	}
	
	if( m_dDevice.isValid() )
	{
		m_dDevice.close();
	}

	openni::OpenNI::shutdown();
	nite::NiTE::shutdown();
}

HRESULT Kinect::Initialize()
{
	openni::Status rc = openni::STATUS_OK;
	const char* deviceURI = openni::ANY_DEVICE;

	rc = openni::OpenNI::initialize();
	printf("After OpenNI Initialization : \n%s\n", openni::OpenNI::getExtendedError());

	openni::OpenNI::addDeviceConnectedListener(this);
	openni::OpenNI::addDeviceDisconnectedListener(this);
	openni::OpenNI::addDeviceStateChangedListener(this);

	rc = m_dDevice.open(deviceURI);
	if ( rc != openni::STATUS_OK )
	{
		printf("Kinect::Initialize(): Device open failed : \n%s\n", openni::OpenNI::getExtendedError());
		openni::OpenNI::shutdown();
		return E_FAIL;
	}

	rc = m_vsDepthStream.create(m_dDevice, openni::SENSOR_DEPTH);
	if ( rc == openni::STATUS_OK )
	{
		rc = m_vsDepthStream.start();
		if ( rc != openni::STATUS_OK )
		{
			printf("Kinect::Initialize(): Couldn't start depth stream:\n%s\n" , openni::OpenNI::getExtendedError());
			m_vsDepthStream.destroy();
		}
	}
	else
	{
		printf("Kinect::Initialize(): Couldn't find depth stream: \n%s\n", openni::OpenNI::getExtendedError());
	}

	rc = m_vsColorStream.create(m_dDevice, openni::SENSOR_COLOR);
	if ( rc == openni::STATUS_OK )
	{
		rc = m_vsColorStream.start();
		if ( rc != openni::STATUS_OK )
		{
			printf("Kinect::Initialize(): Couldn't start color stream:\n%s\n" , openni::OpenNI::getExtendedError());
			m_vsColorStream.destroy();
		}
		else
		{
			m_vsColorStream.addNewFrameListener(&m_fgColor);
		}
	}
	else
	{
		printf("Kinect::Initialize(): Couldn't find color stream: \n%s\n", openni::OpenNI::getExtendedError());
	}

	if (!m_vsDepthStream.isValid() || !m_vsColorStream.isValid())
	{
		printf("Kinect::Initialize(): No valid streams. Exiting\n");
		openni::OpenNI::shutdown();
		return E_FAIL;
	}

	nite::NiTE::initialize();

	if (m_utUserTracker->create(&m_dDevice) != nite::STATUS_OK)
	{
		printf("Kinect::Initialize(): Can't initialize user tracker.\n");
		return E_FAIL;
	}
	else
	{
		m_utUserTracker->addNewFrameListener(&m_fgDepth);
	}

	m_dDevice.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
	
	return S_OK;
}

void FrameGrabber::onNewFrame(openni::VideoStream& stream)
{
	stream.readFrame(&m_vfrNewFrame);
	if(!SetEvent(m_hNewFrame))
	{
		printf("SetEvent error (%d)\n", GetLastError());
	}
}

void FrameGrabber::onNewFrame(nite::UserTracker& userTracker)
{
	userTracker.readFrame(&m_utfrNewFrame);
	if(!SetEvent(m_hNewFrame))
	{
		printf("SetEvent error (%d)\n", GetLastError());
	}
}