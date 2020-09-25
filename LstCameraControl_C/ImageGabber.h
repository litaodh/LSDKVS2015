/**	@file	ImageGabber.cpp
*
*	@brief	Class ImageGabber.
*
*	@attention
*		Show a demo thread to grab image data.
*		Demo of showing how to call SICK 3D Camera APIs.
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/

#pragma once

#include <Ranger3.h>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

typedef SickCam::ImgT ImgT;
typedef SickCam::AllParams AllParams;

/**
*	This class shows how to manage threads of Ranger3.
*	It is just a sample, you should modify it in your project.
*/
class ImageGabber final
{
public:
	ImageGabber();
	ImageGabber(const std::shared_ptr<R3S> & _R3S, const std::string& _IP_MAC, const bool isIP);

	~ImageGabber() {}


	/** @brief setUserParameter()
	*																			  
	*	Set parameter file and calibration file. 
	*
	*	@param [in] _scaleY			: _scaleY = (y_scaning_distance_in_mm) / (profiles_number);
	*	@param [in] para_csv_path	: If para_csv_path=="", using device inner parameters.
	*	@param [in] calib_xml_path	: If calib_xml_path=="", output uncalibrated image;
	*	@param [in] tmp_path		: Scan device parameters and save to it. Will be delete automaticly;
	*	@param [in] method			: See RectMethod;
	*	@param [in] missingData		: The value to fillin the empty pixels;
	*	@Note : When camera is connected, image acquition is not started.
	*/
	bool setUserParameter(
						const double _scaleY = 1.0, 
						const std::string & para_csv_path = "", 
						const std::string & calib_xml_path = "", 
						const SiCaliWrapper::RectMethod method = SiCaliWrapper::RectMethod::Top, 
						const uint64_t timeOut = 1000, 
						const float missing_data_of_calibrated_image = NAN);

	bool connect();
	
	/** Main loop. */
	void					run					();

	/** Get Image data according given template. */
	ImgT					getImgT				();

	std::unique_ptr<R3> &	getR3				();
	bool					listAllParameters	();
	bool					setParameter		(const std::string & _P, const std::string & _V);
	std::string				getParameter		(const std::string & _P);

	/** old name: requireStop */
	bool requireDisconnect			();
	bool requireRestart				();
	bool requireStartAcquition		();
	bool requireStopAcquition		();

	/** Old Function */
	bool requireUpdateParameter		(
		const double _scaleY, const std::string & para_csv_path, 
		const std::string & calib_xml_path, const SiCaliWrapper::RectMethod method, 
		const float missingData); 

	bool requireUpdateTimeout		(uint64_t _timeout);
	bool requireChangeOutputMode	();
	
	bool isGrabbed					();
	bool isConnected				();
	
	/** old name: isStop */
	bool isDisconnected				();	
	bool isStartAcquition			();
	bool isUpdateParameterDone		();
	bool isSensorMode				();

	bool clearData					();
	unsigned long getRunRate		() const { return mRunRate;	};
	bool isImageGraberThreadRunning;

protected:

	bool			_setSensorMode	(const bool enable = true);
	/** Old Function */
	//bool			_close			();	
	unsigned long	_runRate		();


private:
	bool is_require_disconnect;				//! < old name: is_require_stop
	bool is_require_restart;
	bool is_require_start_acquisition;
	bool is_require_stop_acquisition;
	bool is_require_update_parameter;
	bool is_require_update_timeout;
	bool is_require_change_output_mode;
	bool is_connected;						//! < old name: is_start
	bool is_grabbed;
	bool is_start_acquisition;
	bool is_sensor;							//! < true is sensor, false is range.
	
	std::unique_ptr<R3>	m_pCam;
	std::list<ImgT>		m_listImgTable;
	std::string			mMacAddress;
	std::string			mIpAddress;
	std::mutex			mMutexImageData;
	std::mutex			mMutexIsGrabbed;

	unsigned long		mRunRate;
	
public:
	std::string			m_gCamStatus;		//! < Sample: [Disconnected]; size = 17  
	AllParams			m_gParameters;

};


