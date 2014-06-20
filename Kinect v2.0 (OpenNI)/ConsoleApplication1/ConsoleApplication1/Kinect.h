#pragma once
#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <OpenNI.h>
#include <NiTE.h>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>

class FrameGrabber : public openni::VideoStream::NewFrameListener,public nite::UserTracker::NewFrameListener
{
public :
	FrameGrabber(void){ m_hNewFrame = CreateEvent(NULL, FALSE, FALSE, NULL); }	
	~FrameGrabber(void)
	{
		if( m_vfrNewFrame.isValid() )
		{
			m_vfrNewFrame.release();
		}

		if ( m_utfrNewFrame.isValid() )
		{
			m_utfrNewFrame.release();
		}

		if(m_hNewFrame != NULL)
		{
			CloseHandle(m_hNewFrame);
		}
	}
	HANDLE FrameGrabber::getHandle() { return m_hNewFrame; }
	openni::VideoFrameRef FrameGrabber::getFrame() { return m_vfrNewFrame; }
	nite::UserTrackerFrameRef FrameGrabber::getUserFrame() { return m_utfrNewFrame; }

private :
	void onNewFrame(openni::VideoStream& stream);	
	void onNewFrame(nite::UserTracker& userTracker);
	openni::VideoFrameRef m_vfrNewFrame;
	nite::UserTrackerFrameRef m_utfrNewFrame;
	HANDLE m_hNewFrame;
};

class Kinect : 	public openni::OpenNI::DeviceConnectedListener,
				public openni::OpenNI::DeviceDisconnectedListener,
				public openni::OpenNI::DeviceStateChangedListener
{

private:
	openni::Device m_dDevice;
	openni::VideoStream m_vsDepthStream;
	openni::VideoStream m_vsColorStream;	
	FrameGrabber m_fgColor;
	FrameGrabber m_fgDepth;
	nite::UserTracker* m_utUserTracker;

public:
	Kinect(void);
	~Kinect(void);
	HANDLE Kinect::GetColorFrameHandle() { return m_fgColor.getHandle(); }
	HANDLE Kinect::GetDepthFrameHandle() { return m_fgDepth.getHandle(); }
	openni::VideoFrameRef Kinect::GetNextColorFrame() { return m_fgColor.getFrame(); }
	nite::UserTrackerFrameRef Kinect::GetNextDepthFrame() { return m_fgDepth.getUserFrame(); }
	nite::UserTracker* Kinect::GetUserTracker() { return m_utUserTracker; }
	HRESULT Kinect::Initialize();

	
	virtual void onDeviceStateChanged(const openni::DeviceInfo* pInfo, openni::DeviceState state) 
	{
		printf("Device \"%s\" error state changed to %d\n", pInfo->getUri(), state);
	}

	virtual void onDeviceConnected(const openni::DeviceInfo* pInfo)
	{
		printf("Device \"%s\" connected\n", pInfo->getUri());
	}

	virtual void onDeviceDisconnected(const openni::DeviceInfo* pInfo)
	{
		printf("Device \"%s\" disconnected\n", pInfo->getUri());
	}
};

