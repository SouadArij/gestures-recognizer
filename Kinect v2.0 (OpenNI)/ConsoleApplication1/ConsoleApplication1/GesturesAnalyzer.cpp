#include "stdafx.h"
#include "GesturesAnalyzer.h"

GesturesAnalyzer::GesturesAnalyzer(void)
{
}

GesturesAnalyzer::~GesturesAnalyzer(void)
{
}

HRESULT GesturesAnalyzer::Init()
{	
	m_DTWTrainingData.loadDatasetFromFile("TrainingData\\DTWTrainingData.txt");
	m_ANBCTrainingData.loadDatasetFromFile("TrainingData\\ANBCTrainingData.txt");
	m_HandTrainingData.loadDatasetFromFile("TrainingData\\HandTrainingData.txt");


	m_hand.enableScaling( true );
	m_hand.enableNullRejection ( true );
	m_hand.setNullRejectionCoeff( 5 );

	m_anbc.enableScaling( true );
	m_anbc.enableNullRejection( true );
	m_anbc.setNullRejectionCoeff( 1 );
    
    m_dtw.addPreProcessingModule( GRT::MovingAverageFilter( 5, m_DTWTrainingData.getNumDimensions() ) );
    m_dtw.setClassifier( GRT::DTW() );
	//m_dtw.getClassifier()->enableNullRejection( true );

	//m_dtw.enableScaling(true);
	////m_dtw.enableZNormalization(true);
	//m_dtw.enableNullRejection( true );
	//m_dtw.enableTrimTrainingData(true,0.1,90);
	//m_dtw.setOffsetTimeseriesUsingFirstSample( true );
	
	GRT::LabelledTimeSeriesClassificationData testData = m_DTWTrainingData.partition( 80, true );
	
	if( !m_anbc.train( m_ANBCTrainingData ) ){
		std::cout << "Failed to train ANBC classifier!\n";
		return E_FAIL;
	} 

	if( !m_dtw.train( m_DTWTrainingData ) ){
		std::cout << "Failed to train DTW classifier!\n";
		return E_FAIL;
	} 

	double accuracy = 0;
    for(UINT i=0; i<testData.getNumSamples(); i++){
        //Get the i'th test sample - this is a timeseries
        UINT classLabel = testData[i].getClassLabel();
        GRT::MatrixDouble timeseries = testData[i].getData();
        
        //Perform a prediction using the classifier
        if( !m_dtw.predict( timeseries ) ){
            cout << "Failed to perform prediction for test sampel: " << i <<"\n";
            return EXIT_FAILURE;
        }
        
        //Get the predicted class label
        UINT predictedClassLabel = m_dtw.getPredictedClassLabel();
        double maximumLikelihood = m_dtw.getMaximumLikelihood();
        GRT::VectorDouble classLikelihoods = m_dtw.getClassLikelihoods();
        GRT::VectorDouble classDistances = m_dtw.getClassDistances();
        
        //Update the accuracy
        if( classLabel == predictedClassLabel ) accuracy++;
        
        cout << "TestSample: " << i <<  "\tClassLabel: " << classLabel << "\tPredictedClassLabel: " << predictedClassLabel << "\tMaximumLikelihood: " << maximumLikelihood << endl;
    }
    
    cout << "Test Accuracy: " << accuracy/double(testData.getNumSamples())*100.0 << "%" << endl;

	if( !m_hand.train(m_HandTrainingData))
	{
		std::cout << "Failed to train Hand classifier!\n";
		return E_FAIL;
	}

	return S_OK;
}

HRESULT GesturesAnalyzer::SaveSkeleton(vector<vector<cv::Point3d>>& pRecordedSkeleton, const nite::Array<nite::UserData>& pUsers)
{
	vector<cv::Point3d> skeletonTemp(11);
	for (int i = 0; i < pUsers.getSize(); ++i)
	{
		const nite::UserData& userData = pUsers[i];
		if (!userData.isLost())
		{
			if (pUsers[i].getSkeleton().getState() == nite::SKELETON_TRACKED)
			{
				for(int iterator = 0; iterator < 11; iterator++)
				{
					cv::Point3d p = cv::Point3d();
					nite::SkeletonJoint joint = userData.getSkeleton().getJoint((nite::JointType)iterator);
					p.x = joint.getPosition().x;
					p.y = joint.getPosition().y;
					p.z = joint.getPosition().z;
					skeletonTemp[iterator] = p;
				}
			}
		}
	}
	pRecordedSkeleton.push_back(skeletonTemp);
	return S_OK;
}

string GesturesAnalyzer::PredictCurrentPosition(vector<vector<cv::Point3d>>& pRecordedSkeleton)
{
	vector<double> vecTemp;
	PositionType position;
	UINT predictedClassLabel;
	int iterator = 0;

	int x0 = pRecordedSkeleton[iterator][nite::JOINT_TORSO].x;
	int y0 = pRecordedSkeleton[iterator][nite::JOINT_TORSO].y;
	int z0 = pRecordedSkeleton[iterator][nite::JOINT_TORSO].z;

	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_LEFT_HAND].x - x0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_LEFT_HAND].y - y0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_LEFT_HAND].z - z0);

	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_LEFT_ELBOW].x - x0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_LEFT_ELBOW].y - y0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_LEFT_ELBOW].z - z0);

	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_LEFT_SHOULDER].x - x0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_LEFT_SHOULDER].y - y0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_LEFT_SHOULDER].z - z0);

	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_RIGHT_SHOULDER].x - x0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_RIGHT_SHOULDER].y - y0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_RIGHT_SHOULDER].z - z0);

	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_RIGHT_ELBOW].x - x0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_RIGHT_ELBOW].y - y0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_RIGHT_ELBOW].z - z0);

	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_RIGHT_HAND].x - x0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_RIGHT_HAND].y - y0);
	vecTemp.push_back(pRecordedSkeleton[iterator][nite::JOINT_RIGHT_HAND].z - z0);

	if( !m_anbc.predict(vecTemp))
	{
		printf("Failed prediction !\n");
	}
	else
	{
		predictedClassLabel = m_anbc.getPredictedClassLabel();
	}
	vecTemp.clear();
	return m_ANBCTrainingData.getClassNameForCorrespondingClassLabel(predictedClassLabel);
}

UINT GesturesAnalyzer::PredictGesture(const vector<vector<cv::Point3d>>& pSampleVec)
{
	GRT::MatrixDouble sample = GRT::MatrixDouble( );
	vector<double> vecTemp;
	UINT predictedClassLabel = 0;
	for(int iterator = 0; iterator < pSampleVec.size(); iterator++)
	{
		int x0 = pSampleVec[iterator][nite::JOINT_TORSO].x;
		int y0 = pSampleVec[iterator][nite::JOINT_TORSO].y;
		int z0 = pSampleVec[iterator][nite::JOINT_TORSO].z;

		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_HAND].x - x0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_HAND].y - y0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_HAND].z - z0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_HAND].x - x0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_HAND].y - y0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_HAND].z - z0);

		sample.push_back(vecTemp);
		vecTemp.clear();
	}	

	if( !m_dtw.predict(sample))
	{
		printf("Failed prediction !\n");
	}
	else
	{
		predictedClassLabel = m_dtw.getPredictedClassLabel();
	}
	return predictedClassLabel;
}

HRESULT GesturesAnalyzer::DTWSaveSampleAndTrain(UINT pClasse, const vector<vector<cv::Point3d>>& pSampleVec)
{
	printf("Start saving DTW sample\n");
	GRT::MatrixDouble sample = GRT::MatrixDouble( );
	vector<double> vecTemp;
	for(int iterator = 0; iterator < pSampleVec.size(); iterator++)
	{
		int x0 = pSampleVec[iterator][nite::JOINT_TORSO].x;
		int y0 = pSampleVec[iterator][nite::JOINT_TORSO].y;
		int z0 = pSampleVec[iterator][nite::JOINT_TORSO].z;

		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_HAND].x - x0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_HAND].y - y0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_HAND].z - z0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_HAND].x - x0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_HAND].y - y0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_HAND].z - z0);

		sample.push_back(vecTemp);
		vecTemp.clear();
	}	

	printf("Adding sample to DTW data\n");
	if( !m_DTWTrainingData.addSample(pClasse, sample))
	{
		printf("Failed to add DTW sample\n");
		return E_FAIL;
	}

	if( !m_DTWTrainingData.saveDatasetToFile("TrainingData\\DTWTrainingData.txt"))
	{
		printf("Failed to save DTW data\n");
		return E_FAIL;
	}

	printf("Training DTW classifier\n");
	if( !m_dtw.train( m_DTWTrainingData ) ){
		cout << "Failed to train DTW classifier!\n";
		return E_FAIL;
	} 
	else
	{
		printf("Succesfully trained DTW Classifier\n");
	}
	return S_OK;
}

HRESULT GesturesAnalyzer::ANBCSaveSampleAndTrain(UINT pClasse, string pName, const vector<vector<cv::Point3d>>& pSampleVec)
{
	printf("Start saving ANBC sample\n");
	vector<double> vecTemp;
	printf("Adding sample to ANBC data\n");
	for(int iterator = 0; iterator < pSampleVec.size(); iterator++)
	{
		int x0 = pSampleVec[iterator][nite::JOINT_TORSO].x;
		int y0 = pSampleVec[iterator][nite::JOINT_TORSO].y;
		int z0 = pSampleVec[iterator][nite::JOINT_TORSO].z;

		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_HAND].x - x0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_HAND].y - y0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_HAND].z - z0);

		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_ELBOW].x - x0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_ELBOW].y - y0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_ELBOW].z - z0);

		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_SHOULDER].x - x0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_SHOULDER].y - y0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_LEFT_SHOULDER].z - z0);

		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_SHOULDER].x - x0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_SHOULDER].y - y0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_SHOULDER].z - z0);

		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_ELBOW].x - x0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_ELBOW].y - y0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_ELBOW].z - z0);

		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_HAND].x - x0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_HAND].y - y0);
		vecTemp.push_back(pSampleVec[iterator][nite::JOINT_RIGHT_HAND].z - z0);

		if( !m_ANBCTrainingData.addSample(pClasse, vecTemp))
		{
			printf("Failed add ANBC sample\n");
			return E_FAIL;
		}

		vecTemp.clear();
	}

	if(m_ANBCTrainingData.getClassNameForCorrespondingClassLabel(pClasse) == "NOT_SET")
	{
		m_ANBCTrainingData.setClassNameForCorrespondingClassLabel(pName, pClasse);
	}

	if( !m_ANBCTrainingData.saveDatasetToFile("TrainingData\\ANBCTrainingData.txt"))
	{
		printf("Failed save ANBC data\n");
		return E_FAIL;
	}

	printf("Training anbc classifier\n");
	if( !m_anbc.train( m_ANBCTrainingData ) ){
		cout << "Failed to train ANBC classifier!\n";
		return E_FAIL;
	}
	else
	{
		printf("Succesfully trained ANBC Classifier\n");
	}
	return S_OK;
}

HRESULT GesturesAnalyzer::HandSaveSample(UINT pClasse, string pName, const vector<double>& pSampleVec)
{
	if (!m_HandTrainingData.addSample( pClasse, pSampleVec))
	{
		printf("Failed add Hand sample\n");
		return E_FAIL;
	}

	if (m_HandTrainingData.getClassNameForCorrespondingClassLabel(pClasse) == "NOT_SET")
		m_HandTrainingData.setClassNameForCorrespondingClassLabel(pName, pClasse);

	if( !m_HandTrainingData.saveDatasetToFile( "TrainingData\\HandTrainingData.txt" ))
	{
		printf("Failed save hand data\n");
		return E_FAIL;
	}

	if (!m_hand.train(m_HandTrainingData))
	{
		printf("Failed train Hand Classifier\n");
		return E_FAIL;
	}
	else
	{
		printf("Successfully trained hand classifier\n");
	}

	return S_OK;
}

string GesturesAnalyzer::PredictHandPosition(const vector<double>& pSampleVec)
{
	UINT predictedClassLabel = 0;
	if( !m_hand.predict(pSampleVec))
	{
		printf("Failed prediction !\n");
	}
	else
	{
		predictedClassLabel = m_hand.getPredictedClassLabel();
	}
	return m_HandTrainingData.getClassNameForCorrespondingClassLabel(predictedClassLabel);
}