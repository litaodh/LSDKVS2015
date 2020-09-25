/**	@file	Ranger3Callback.cpp
*   <当前不可用>
*	@brief	Class Ranger3Callback.
*
*	@attention
*		Register a callback function to get image data
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/

#pragma once

#include "Ranger3.h"
#include "ImageTable.h"

#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

typedef SickCam::ImgT ImgT;
typedef SickCam::AllParams AllParams;

////////////////////////////////////////////////////////////////////////////////

typedef void (__stdcall *CallbackEvent)(ImgT* img, void* any);

////////////////////////////////////////////////////////////////////////////////


namespace SickCam
{

#ifdef NEW_FEATURE

/**
*   <NOT AVALIABLE YET> <当前不可用>
*	This class shows how to manage threads of Ranger3.
*	It is just a sample, you should modify it in your project.
*/
class Ranger3Callback final
{
public:
	/** <NOT AVALIABLE YET> <当前不可用>	*/
	EXPORT_TO_DLL Ranger3Callback(std::shared_ptr<R3> pCam, CallbackEvent pCallBack, void * any);
	EXPORT_TO_DLL  ~Ranger3Callback();

	/** <NOT AVALIABLE YET> <当前不可用>	*/
	EXPORT_TO_DLL CAM_STATUS registerCallback();

	/** <NOT AVALIABLE YET> <当前不可用>	*/
	EXPORT_TO_DLL CAM_STATUS unRegisterCallback();

protected:

	/** Main loop. <当前不可用> */
	void	run();

	int		_runRate();

private:
	std::unique_ptr<std::thread> mThread;
	CallbackEvent	m_pCallBack;

	// parameter of callback function.
	ImgT*			m_pImg;
	void*			m_any;

	// variables of ranger3 
	std::shared_ptr<R3>	m_pCam;
	bool			m_startRun;
	bool			m_reqstoptRun;
};

#endif // NEW_FEATURE


};


