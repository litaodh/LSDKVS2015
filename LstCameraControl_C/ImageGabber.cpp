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


#include "ImageGabber.h"


//====================================================================
// ImageGabber : constructor/destructor   


ImageGabber::ImageGabber()
	: is_connected(false), is_require_disconnect(false), is_grabbed(false), 
	is_require_restart(false), is_start_acquisition(false), is_require_update_parameter(false), 
	is_require_update_timeout(false), mMacAddress(""), is_require_start_acquisition(false), 
	is_require_stop_acquisition(false), is_require_change_output_mode(false), m_gCamStatus("<Disconnected>"), mRunRate(100)
{
	m_pCam = nullptr;
	m_pCam->setScaleY(1);
	this->isImageGraberThreadRunning = false;
}

ImageGabber::ImageGabber(const std::shared_ptr<R3S>& _R3S, const std::string& _IP_MAC, const bool isIP)
	: is_connected(false), is_require_disconnect(false), is_grabbed(false), 
	is_require_restart(false), is_start_acquisition(false), is_require_update_parameter(false), 
	is_require_update_timeout(false), is_require_start_acquisition(false), 
	is_require_stop_acquisition(false), is_require_change_output_mode(false), m_gCamStatus("<Disconnected>"), mRunRate(100)
{
	m_pCam = std::make_unique<R3>(_R3S, _IP_MAC, isIP);
	mMacAddress = m_pCam->getMac();
	mIpAddress  = m_pCam->getIp();
	this->isImageGraberThreadRunning = false;
}


//====================================================================
// ImageGabber : public interface


bool 
ImageGabber::setUserParameter(const double _scaleY, const std::string & para_csv_path,
	const std::string & calib_xml_path, const SiCaliWrapper::RectMethod method,
	const uint64_t timeOut, const float missing_data_of_calibrated_image)
{
	m_pCam->setScaleY(_scaleY);
	m_pCam->setParametPath(para_csv_path);	
	m_pCam->setCalibraPath(calib_xml_path);	
	m_pCam->setRectMethos(method);
	m_pCam->setTimeOut(timeOut);
	m_pCam->setMissingData(missing_data_of_calibrated_image);

	return true;
}

bool 
ImageGabber::connect()
{
	// check
	if (m_pCam->getDeviceName() == "ERROR")
	{
		printf("Error: Init camara failed, check mMacAddress please. input mac: %s, input ip: %s \n", (mMacAddress == "" ? "empty!" : mMacAddress.c_str()), (mIpAddress == "" ? "empty!" : mIpAddress.c_str()));
		return false;
	}

	//  --- Set one Ranger3.
	m_pCam->setTimeOut(m_pCam->getTimeOut());
	Ecode ec;
	try
	{
		ec = m_pCam->connectCamera();
			
		if (ec != Ecode::All_OK)
		{
			printf("Error: setCamera failed. para_csv_path=%s, calib_xml_path=%s \n", 
				m_pCam->getParametPath().c_str(), 
				m_pCam->getCalibraPath().c_str());

			switch (ec)
			{
			case Ecode::ERROR_SET_CAM:
				printf("Error: ERROR_SET_CAM \n");
				return false;
				break;
			case Ecode::ERROR_XML_PATH:
				printf("Error: ERROR_XML_PATH, Path: %s \n", m_pCam->getParametPath().c_str());
				return false;
				break;
			case Ecode::ERROR_CSV_PATH:
				printf("Error: ERROR_CSV_PATH, Path: %s \n", m_pCam->getCalibraPath().c_str());
				return false;
				break;
			default:
				printf("Error: UNKNOWN_ERROR \n");
				return false;
				break;
			}
		}

		is_connected = true;
		m_gCamStatus = "<Connected___>";
		is_sensor = getParameter("DeviceScanType") == "Areascan";
	}
	catch (const char * msg)
	{
		std::cerr << msg << std::endl;
		return false;
	}
	catch (...)
	{
		std::cerr << "Unknown Error happened in connectCamera() !" << std::endl;
		return false;
	}
	return true;
}

//std::wstring string2wstring(std::string str)
//{
//	std::wstring result;
//	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
//	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
//	TCHAR* buffer = new TCHAR[len + 1];
//	//多字节编码转换成宽字节编码  
//	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
//	buffer[len] = '\0';             //添加字符串结尾  
//									//删除缓冲区并返回值  
//	result.append(buffer);
//	delete[] buffer;
//	return result;
//}

void 
ImageGabber::run()
{
	time_t start, end;
	this->isImageGraberThreadRunning = true;
	while (is_connected)
	{
		//std::cout << mMacAddress <<" is_connected = " << is_connected << std:: endl;
		//SickCam::ImgT imgTable;
	/*	std::wstring deName= string2wstring(m_pCam->getDeviceName())+L"\n\n";
		OutputDebugString(deName.c_str());*/
		if (!is_require_disconnect && !is_require_restart && is_start_acquisition && 
			!is_require_start_acquisition && !is_require_stop_acquisition && 
			!is_require_update_parameter && !is_require_update_timeout &&
			!is_require_change_output_mode)
		{
			SickCam::ImgT imgTable;
			try
			{
				start = clock();
				Ecode ec = m_pCam->getImageData(imgTable);
				if (ec == Ecode::TIME_OUT)
				{
					Sleep(100);
					continue;
				}

				if (ec != Ecode::All_OK)
				{
					//std::cout << "getImageData failed! Ecode : " << ec << std::endl;
					is_grabbed = false;
					continue;
				}
				end = clock() - start;
				//std::cout << "getImageData in run time: "<<double(end) / CLOCKS_PER_SEC << " sec" << std::endl;

			}
			catch (const char * msg)
			{
				std::cerr << msg << std::endl;
				is_require_disconnect = false;
				is_grabbed = false;
				m_gCamStatus = "<Disconnected>";
				this->isImageGraberThreadRunning = false;
				break;
			}
			catch (...)
			{
				std::cout << "Grab run error happended in getImageData!" << std::endl;
				is_require_disconnect = false;
				is_grabbed = false;
				m_gCamStatus = "<Disconnected>";
				this->isImageGraberThreadRunning = false;
				break;
			}
			start = clock();
			// save image
			std::unique_lock<std::mutex> lock(mMutexImageData);
			std::unique_lock<std::mutex> lock2(mMutexIsGrabbed);
		
			m_listImgTable.push_back(imgTable);
			end = clock() - start;
			//std::cout << "m_listImgTable.push_back time: " << double(end) / CLOCKS_PER_SEC << " sec" << std::endl;
			is_grabbed = true;
		}
		else if (is_require_start_acquisition && !is_require_stop_acquisition)
		{
			if (is_start_acquisition)
			{
				is_require_start_acquisition = false;
				is_require_stop_acquisition = false;
			}
			else
			{
				mRunRate = _runRate();
				m_listImgTable.clear();
				Ecode ec = m_pCam->startCamera();
				is_start_acquisition = (ec == Ecode::All_OK);
				if (is_start_acquisition)
					m_gCamStatus = "<Started_____>";
			}
		}
		else if (!is_require_start_acquisition && is_require_stop_acquisition)
		{
			if (!is_start_acquisition)
			{
				is_require_start_acquisition = false;
				is_require_stop_acquisition = false;
			}
			else
			{
				Ecode ec = m_pCam->stopCamera();
				is_start_acquisition = !(ec == Ecode::All_OK);
				if (!is_start_acquisition)
				{
					m_gCamStatus = "<Stopped_____>";
				/*	std::unique_lock<std::mutex> lock(mMutexImageData);
					m_listImgTable.clear();*/
				}
			}
		}
		else if (is_require_disconnect)
		{
			if (isStartAcquition())
			{
				Ecode ec = m_pCam->stopCamera();
				is_start_acquisition = !(ec == Ecode::All_OK);
				is_require_start_acquisition = is_start_acquisition;
			}

			if (!isStartAcquition())
			{
				Ecode ec = m_pCam->disconnectCamera();
				is_connected = !(ec == Ecode::All_OK);
				is_require_disconnect = !(ec == Ecode::All_OK);
				if (!is_connected)
				{
					m_gCamStatus = "<Disconnected>"; 
					std::unique_lock<std::mutex> lock(mMutexImageData);
					m_listImgTable.clear();
				}
			}
		}
		else if (is_require_change_output_mode)
		{
			if (isStartAcquition())
				requireStopAcquition();

			if (!isStartAcquition() && _setSensorMode(!is_sensor))
			{
				printf("Stop, \n");
				is_require_change_output_mode = false;
				requireStartAcquition();
			}
		}

		Sleep(mRunRate);

	}
}

SickCam::ImgT 
ImageGabber::getImgT()
{
	std::unique_lock<std::mutex> lock(mMutexImageData);
	if (m_listImgTable.empty())		return SickCam::ImgT();

	SickCam::ImgT head = m_listImgTable.front();
	m_listImgTable.pop_front();
	is_grabbed = !m_listImgTable.empty();

#if DebugMem
	std::cout << "<getImgT pop> m_listImgTable size = " << m_listImgTable.size() << std::endl;

#endif // DeubgMem

	return head;
}

std::unique_ptr<R3>& 
ImageGabber::getR3()
{
	return m_pCam;
}

bool 
ImageGabber::listAllParameters()
{
	m_pCam->getAllParameterInfo(m_gParameters);
	std::cout << "--------------- Parameters list ---------------" << std::endl;
	
	size_t i(0);
	for (auto sub : m_gParameters)
	{
		std::cout << std::endl << std::endl;
		std::cout << "* Param " << ++i << std::endl;
		std::cout << "- Name          - " <<  sub.first << std::endl;
		std::cout << "- Current Value - " << sub.second->getStrValue() << std::endl;
		std::cout << "- Is Read Only  - " << (sub.second->isReadOnly() ? "Yes" : "No") << std::endl;

		const std::vector<std::string> opts = sub.second->getOptions();
		if(opts.empty())
			std::cout << "- Opts      - Number " << std::endl;
		else
		{
			for(auto opt : opts)
				std::cout << "- Opts      - " << opt << std::endl;
		}
	}

	return false;
}

bool 
ImageGabber::setParameter(const std::string & _P, const std::string & _V)
{
	if (is_start_acquisition) return false;

	if (_P.empty() || _V.empty()) return false;

	printf("%d, %s, %s\n", (Ecode::All_OK == m_pCam->setParameterValue(_P, _V)), _P.c_str(), _V.c_str());

	return getParameter(_P) == _V;
}

std::string 
ImageGabber::getParameter(const std::string & _P)
{
	if (is_start_acquisition) return false;

	if (_P.empty()) return false;

	return m_pCam->getParameterValue(_P);
}

bool 
ImageGabber::requireDisconnect()
{
	is_require_disconnect = true;
	return is_require_disconnect;
}

bool 
ImageGabber::requireRestart()
{
	is_require_restart = true;
	return is_require_restart;
}

bool 
ImageGabber::requireStartAcquition()
{
	is_require_start_acquisition = true;
	is_require_stop_acquisition = false;
	return is_require_start_acquisition;
}

bool 
ImageGabber::requireStopAcquition()
{
	is_require_stop_acquisition = true;
	is_require_start_acquisition = false;
	return is_require_stop_acquisition;
}

bool 
ImageGabber::requireUpdateParameter(const double _scaleY, const std::string & para_csv_path, const std::string & calib_xml_path, const SiCaliWrapper::RectMethod method, const float missingData)
{
	is_require_update_parameter = true;
	m_pCam->setScaleY(_scaleY);
	m_pCam->setParametPath(para_csv_path);
	m_pCam->setCalibraPath(calib_xml_path);
	m_pCam->setRectMethos(method);
	m_pCam->setMissingData(missingData);
	return is_require_update_parameter;
}

bool 
ImageGabber::requireUpdateTimeout(uint64_t _timeout)
{
	is_require_update_timeout = true;
	return is_require_update_timeout;
}

bool 
ImageGabber::requireChangeOutputMode()
{
	is_require_change_output_mode = true;
	return is_require_change_output_mode;
}

bool 
ImageGabber::isGrabbed()
{
	if (is_require_disconnect || !is_connected)
		is_grabbed = false;

	std::unique_lock<std::mutex> lock(mMutexIsGrabbed);
	return is_grabbed;
}

bool 
ImageGabber::isConnected()
{
	return is_connected;
}

bool 
ImageGabber::isDisconnected()
{
	return !is_connected;

}

bool 
ImageGabber::isStartAcquition()
{
	return is_start_acquisition;
}

bool 
ImageGabber::isUpdateParameterDone()
{
	return !is_require_update_timeout;
}

bool 
ImageGabber::isSensorMode()
{
	return is_sensor;
}

bool 
ImageGabber::clearData()
{
	m_listImgTable.clear();
	m_listImgTable.swap(std::list<SickCam::ImgT>());
	return m_listImgTable.empty();
}


//====================================================================
// ImageGabber : protected interface


bool 
ImageGabber::_setSensorMode(const bool enable)
{
	if (isStartAcquition()) return false;

	setParameter("DeviceScanType", (enable ? "Areascan" : "Linescan3D"));
	printf("IG -------  setParameter\n");

	std::string DeviceScanType = getParameter("DeviceScanType");
	printf("IG ------- %s \n", DeviceScanType.c_str());

	is_sensor = DeviceScanType == "Areascan";
	return DeviceScanType == (enable ? "Areascan" : "Linescan3D");
}

//bool 
//ImageGabber::_close()
//{
//	if (!is_connected && !is_grabbed && !is_require_disconnect)
//	{
//		Ecode ec = m_pCam->OLDcloseCamera();
//		if (ec == Ecode::ERROR_CLO_CAM)
//		{
//			printf("Error: close camara failed \n");
//			return false;
//		}
//		std::cout << "Close camera done." << std::endl;
//		return true;
//	}
//	else
//		return false;
//}

unsigned long 
ImageGabber::_runRate()
{
	is_sensor = getParameter("DeviceScanType") == "Areascan";

	if (isSensorMode())
	{
		float linerate = atof(getParameter("AcquisitionFrameRate").c_str());
		return static_cast<unsigned long>(1000 / linerate);
	}
	else 
	{
		float linerate = atof(getParameter("AcquisitionLineRate").c_str());
		int height = atoi(getParameter("Height_RegionSelector_Region1").c_str());
		return static_cast<unsigned long>(height / linerate * 1000);
	}
}

