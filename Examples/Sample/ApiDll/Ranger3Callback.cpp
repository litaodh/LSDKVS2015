/**	@file	Ranger3Callback.cpp
*
*	@brief	Class Ranger3Callback.
*
*	@attention
*		< Not Release!! 未正式发布代码>
*		Register a callback function to get image data
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/


#include "Ranger3Callback.h"


namespace SickCam 
{

#ifdef NEW_FEATURE

//====================================================================
// Ranger3Callback : constructor/destructor   


Ranger3Callback::Ranger3Callback(std::shared_ptr<R3> pCam, CallbackEvent pCallBack, void* any)
	:
		m_pCam (pCam),
		m_any (any),
		m_pCallBack( pCallBack)
{
}

Ranger3Callback::~Ranger3Callback()
{
}

EXPORT_TO_DLL CAM_STATUS 
Ranger3Callback::registerCallback()
{
	m_startRun = true;
	m_reqstoptRun = false;

	mThread = std::make_unique<std::thread>(&Ranger3Callback::run, this);
	mThread->detach();

	return CAM_STATUS::All_OK;
}

EXPORT_TO_DLL CAM_STATUS 
Ranger3Callback::unRegisterCallback()
{
	// stop run
	m_reqstoptRun = true;
	while (m_startRun == true)
	{
		m_reqstoptRun = true;
		Sleep(5);
	}

	return CAM_STATUS::All_OK;
}


//====================================================================
// Ranger3Callback : public interface


void 
Ranger3Callback::run()
{
	int i = 0;
	while (true)
	{
		if (m_reqstoptRun)		break;

		SickCam::ImgT img;
		//std::cout << "Cam Status: " << m_pCam->getStatus() << std::endl;

		if (Ecode::CAM_IS_STARTED == m_pCam->getStatus() && 
			Ecode::All_OK == m_pCam->getImageData(img)	 &&
			!img.isEmpty())
		{
			m_pCallBack(&img, m_any);
			Sleep(5);
		}
		else
			Sleep(100);

	}
	m_startRun = false;
}

int
Ranger3Callback::_runRate()
{
	bool  is_sensor = m_pCam->getParameterValue("DeviceScanType") == "Areascan";

	if (is_sensor)
	{
		float linerate = atof(m_pCam->getParameterValue("AcquisitionFrameRate").c_str());
		return static_cast<int>(1000 / linerate / 10);
	}
	else
	{
		float linerate = atof(m_pCam->getParameterValue("AcquisitionLineRate").c_str());
		int height = atoi(m_pCam->getParameterValue("Height_RegionSelector_Region1").c_str());
		return static_cast<int>(height / linerate * 1000 / 10);
	}
}

#endif

}
