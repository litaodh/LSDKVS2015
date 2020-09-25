/**	@file	Ranger3.cpp
*
*	@brief	Class Ranger3 Head File.
*
*	@attention
*	This is the Ranger3 GenIcam API wrapper.
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/


#include "Ranger3.h"


namespace SickCam 
{

//////////////// Ranger3Shared function definitions //////////////////


Ranger3Shared::Ranger3Shared(cStr& logPath,
								cStr& ctiPath,
								const bool enShow,
								const bool enWrite)
	: m_nDevices(0), m_isCtiFound(false), m_isDevFound(false), m_enableLogOutput(enShow), m_enableLogWriteToFile(enWrite)
{
	_setProcessPriority();

	// init log
	auto logFilePath = logPath;
	if (logFilePath.empty())
	{
		logFilePath = (*SiGetPathToProducer)();
	}
	_initLog(logFilePath);

	// load cti
	if (All_OK == loadCtiFile(ctiPath))
	{
		scanDevice();
	}
}

Ranger3Shared::~Ranger3Shared()
{
	_clearR3S();
	m_pconsumer->close();
}

EXPORT_TO_DLL CAM_STATUS
Ranger3Shared::scanDevice()
{
	if (m_tlHandle == GENTL_INVALID_HANDLE)	return	ERROR_OPEN_TL_HANDLE;

	_clearR3S();

	// Keep track of interfaces that we open, so that we can close them later
	auto interfaces = m_pconsumer->getInterfaces(m_tlHandle);
	auto tl = m_pconsumer->tl();

	for (size_t i = 0; i < interfaces.size(); ++i)
	{
		auto interfaceIds = interfaces[i].first;
		auto displayName = interfaces[i].second;

		auto interfaceId = m_pconsumer->findInterfaceByIndex(interfaces, i);
		if (interfaceId.empty())
		{
			continue;
		}

		auto interfaceHandle = m_pconsumer->openInterfaceById(interfaceId);
		if (interfaceHandle == GENTL_INVALID_HANDLE)
		{
			continue;
		}

		m_openInterfaces.insert(interfaceHandle);

		// Will print out a list of all available devices on interface
		auto devices = m_pconsumer->getDevices(interfaceHandle);

		// If Ranger3s are connected, the devices will not be empty.
		if (!devices.empty())
		{
			*m_log << "* Find the " << ++m_nDevices << " (st|nd|rd|th) Ranger3:" << std::endl;
			
			// Create a NodeMap of the GenTL Producer interface module
			// This allows access to GigEVision specific functionality, such as ForceIP,
			// that is not part of the GenTL C api.
			bool8_t changed = false;
			CC(tl, tl->IFUpdateDeviceList(interfaceHandle, &changed, 500));

			for (size_t j = 0; j < devices.size(); ++j)
			{
				auto pDev = std::make_shared<DeviceConnection>(m_pconsumer, 
					interfaceHandle, devices, static_cast<int64_t>(j));

				m_connectedDevicesIP.insert({ pDev->getIp(), pDev });
				m_connectedDevices.insert({ pDev->mDeviceName, pDev });
			}
		}
	}

	// the example of deviceName: SICKGigEVisionTL_DEV_000677ff0607
	if (m_nDevices == 0)
	{
		*m_log << "Error: [Ranger3Shared::scanDevice]: No conneted Ranger3 found! Please check the IP or restart the Ranger3 and wait until the device is ready!" << endl;
		m_isDevFound = false;
		return ERROR_SCAN_EMPTY_DEVICE;
	}
	m_isDevFound = true;
	return All_OK;
}

EXPORT_TO_DLL CAM_STATUS
Ranger3Shared::loadCtiFile(cStr & ctiPath)
{	
	Str ctiFile = ctiPath;
	
	if (ctiFile.empty())
	{
		ctiFile = (*SiGetPathToProducer)();
		if (ctiFile.empty())
		{
			//m_log = std::make_shared<CustomerLog>("log.txt");
			*m_log << "Error: [Ranger3Shared::loadCtiFile]: cti file no exist!" << endl;
			return ERROR_CTI_NOT_FOUND;
		}
	}

	m_pconsumer = std::make_unique<SiConsumer>(ctiFile);
	m_tlHandle = m_pconsumer->open();
	if (m_tlHandle != GENTL_INVALID_HANDLE)
	{
		*m_log << "[Ranger3Shared::loadCtiFile] : common::Consumer open ok!" << endl;
		m_isCtiFound = true;
		return All_OK;
	}
	else
		return ERROR_OPEN_CONSUMER;
}

void
Ranger3Shared::_setProcessPriority()
{
	if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
	{
		*m_log << "Error: [Ranger3Shared::_setProcessPriority]: Failed to set priority class REALTIME!" << endl;
		return;
	}

	if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST))
	{
		*m_log << "Error: [Ranger3Shared::_setProcessPriority]: Failed to set thread priority TIME CRITICAL!" << endl;
		return;
	}

	// We have seen situations where it is necessary to set ES_DISPLAY_REQUIRED to
	// make sure that buffer recording is not disturbed. In addition we also need
	// to ensure that the processor does not enter a lower power state.
	if (!SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED | ES_DISPLAY_REQUIRED))
	{
		*m_log << "Error: [Ranger3Shared::_setProcessPriority]: Failed to disable various power-saving modes!" << endl;
		return;
	}
}

bool
Ranger3Shared::_clearR3S()
{
	m_nDevices = 0;
	m_connectedDevices.clear();
	m_connectedDevicesIP.clear();

	for (auto it = m_openInterfaces.begin(); it != m_openInterfaces.end(); ++it)
		m_pconsumer->closeInterface(*it);

	m_openInterfaces.clear();
	*m_log << "[Ranger3Shared] : Close Interfaces.";
	return m_openInterfaces.empty();
}

bool
Ranger3Shared::_initLog(cStr & logPath)
{
	if (logPath.empty()) return false;

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	struct tm ltm = { 0 };
	localtime_s(&ltm, &tt);
	std::stringstream stm;
	stm << std::setfill('0');
	stm << std::setw(4) << (ltm.tm_year + 1900);
	stm << std::setw(2) << (ltm.tm_mon + 1);
	stm << std::setw(2) << (ltm.tm_mday);
	stm << std::setw(2) << (ltm.tm_hour);
	stm << std::setw(2) << (ltm.tm_min);
	stm << std::setw(2) << (ltm.tm_sec);

	m_logPath = logPath + "_" + stm.str() + "_log.txt";
	m_log = std::make_shared<CustomerLog>(m_logPath, m_enableLogOutput, m_enableLogWriteToFile);

	return true;
}


//////////////// Ranger3 function definitions //////////////////////////


Ranger3::Ranger3(const SPtr<Ranger3Shared>& RS,  cStr & ip_mac,  const bool useIP)
	:	m_pR3S(nullptr),
		m_device(nullptr),
		m_consumer(nullptr),
		m_chunkAdapter(nullptr),
		m_pCalibrationWrapper(nullptr),
		m_pParts(nullptr),
		m_ChunkModeActive(false),
		m_isUsingReflectance(false),
		m_IsOutputSensor(false),
		m_buffer16Size(0),
		m_payloadSize(0),
		m_DeviceName("ERROR"), 
		m_status(ERROR_NULL_PTR_R3),
		m_canStop(true)
{
	m_pR3S = RS;
	m_log = m_pR3S->m_log;
	Str deviceStr  = useIP ? ip_mac : ("SICKGigEVisionTL_DEV_" + ip_mac + "_Ranger3");
	auto diviceList = useIP ? m_pR3S->getConDevListIP() : m_pR3S->getConDevList();

	if (diviceList.count(deviceStr) == 1)
	{
		m_device = diviceList.at(deviceStr);
		m_deviceHandle = m_device->mDeviceHandle;
		m_deviceNodeMap = m_device->mDeviceNodeMap;
		m_DeviceName = m_device->mDeviceName;
		m_consumer = m_pR3S->getConsumer();
		m_status = CAM_IS_DISCONNECTED;

		*m_log << "Init " << m_DeviceName.c_str() << " by "
			<< (useIP ? "IP( " : "MAC(") 
			<< deviceStr << "), OK!" << endl;
	}	
}

Ranger3::~Ranger3()
{
	if (m_device != nullptr)
	{
		m_device->closeDataStream();
		m_device->closeDevice();
	}
}

EXPORT_TO_DLL CAM_STATUS 
Ranger3::isReady()
{
	return (getStatus() == ERROR_NULL_PTR_R3) ? ERROR_NULL_PTR_R3 : All_OK;
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::connectCamera()
{
	if (getStatus() == CAM_IS_CONNECTED)	return All_OK;
	if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
	if (getStatus() == CAM_IS_STOPPED)		return All_OK;

	std::stringstream ss;
	ss	<< m_DeviceName.c_str() 
		<< " : connectCamera(_scaleY="	<< m_Param.getScaleY()
		<< ", \r\npara_csv_path="		<< m_Param.getParametPath()
		<< ", \r\ncalib_xml_path="		<< m_Param.getCalibraPath()
		<< ", \r\ntmp_path="			<< m_Param.getTmpPath()
		<< ", \r\nmethod="				<< m_Param.getRectMethos()
		<< ", \r\nmissingData="			<< m_Param.getMissingData()
		<< ", \r\n_doRectfy="			<< m_Param.getDoRectify()
		<< "\r\n";

	// connect
	auto e = _connectCamera();
	
	if (e != All_OK) { m_status = CAM_IS_DISCONNECTED; return e; }
	m_status = CAM_IS_CONNECTED;	ss << "Connect OK!\n";

	// load csv if has.
	if (!m_Param.getParametPath().empty())
	{
		e = loadParameterFrCSV(m_Param.getParametPath());
		if (e != All_OK) { m_status = CAM_IS_DISCONNECTED; return e; }
		m_status = CAM_IS_CONNECTED;	ss << "Load csv file OK!\n";
	}
	else
		ss << "Skip loading csv file!\n";

	// scan device parameter.
	e = _scanDeviceParameter(m_Param.getTmpPath());
	
	if (e != All_OK) { m_status = CAM_IS_DISCONNECTED; return e; }
	m_status = CAM_IS_CONNECTED;	ss << "Scan device parameter OK!\n";

	return e;
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::startCamera()
{
	if (getStatus() == CAM_IS_STARTED)		return All_OK;
	if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED;
	if (getStatus() == ERROR_NULL_PTR_R3)	return ERROR_CAM_IS_DISCONNECTED;

	auto e = _startAcquisition();
	if (e == All_OK) m_status = CAM_IS_STARTED; 
	return e; 
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::stopCamera()
{
	if (getStatus() == CAM_IS_CONNECTED)	return All_OK;
	if (getStatus() == CAM_IS_STOPPED)		return All_OK;
	if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED;
	if (getStatus() == ERROR_NULL_PTR_R3)	return ERROR_CAM_IS_DISCONNECTED;

	while (!m_canStop)
	{
		Sleep(10);
	};

	auto e = _stopAcquisition();
	if (e == All_OK) m_status = CAM_IS_STOPPED;
	return e; 
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::disconnectCamera()
{
	if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
	if (getStatus() == CAM_IS_DISCONNECTED)	return All_OK;
	if (getStatus() == CAM_IS_CONNECTED || getStatus() == CAM_IS_STOPPED)
	{
		auto e = _disconnectCamera();
		if (e == All_OK) m_status = CAM_IS_DISCONNECTED;
		return e;
	}
	return UNKNOWN;
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::getImageData(ImgT & imgTable)
{
	if (getStatus() == CAM_IS_CONNECTED)	return ERROR_CAM_IS_CONNECTED;
	if ((getStatus() == CAM_IS_STOPPED))		return ERROR_CAM_IS_STOPPED;
	if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED;

	m_canStop = false;
	auto res = _getImageData(imgTable);
	//OutputDebugString("_getImageData()\n");
	//todo: 5 copy x
	if (!getDoRectify())
	{
		size_t xSize;
		if (imgTable.has(DN::RAN_CAL))
		{
			xSize = imgTable.getC_cols()*imgTable.getC_rows();
			
			imgTable.RangX = new float[xSize];			
			memcpy(imgTable.RangX, XXXX(), xSize*sizeof(float));			
		}
		/*else if(imgTable.has(DN::RAN))
		{
			int cols = imgTable.getR_cols();
			int rows = imgTable.getR_rows();
			xSize = cols*rows;
			imgTable.RangXRaw = new uint16_t[xSize];
			

			uint16_t* singleRowXs=new uint16_t[cols];
			double lowerBoundX=imgTable.getC_lower_bound_x();
			double scaleX = imgTable.getC_scaleX();
			for (int i = 0; i < cols; i++)
			{
				singleRowXs[i] = lowerBoundX + i*scaleX;
			}
			for (int i = 0; i < rows; i++)
			{
				memcpy(imgTable.RangXRaw + i*cols, singleRowXs, cols*sizeof(uint16_t));
			}
		}*/
	}
	else
	{
		size_t xSize;
		if (imgTable.has(DN::RAN_CAL))
		{
			int cols = imgTable.getC_cols();
			int rows = imgTable.getC_rows();
			xSize = cols*rows;

			imgTable.RangX = new float[xSize];			

			float* singleRowXs = new float[cols];
			double lowerBoundX = imgTable.getC_lower_bound_x();
			double scaleX = imgTable.getC_scaleX();
			for (int i = 0; i < cols; i++)
			{
				singleRowXs[i] = lowerBoundX + i*scaleX;
			}
			for (int i = 0; i < rows; i++)
			{
				memcpy(imgTable.RangX + i*cols, singleRowXs, cols*sizeof(float));
			}
			if (singleRowXs != nullptr)
			{
				delete[] singleRowXs;
				singleRowXs = nullptr;
			}
		}
	}
	
	m_canStop = true;
	return res;
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::setParametPath(const Str & value)
{
	if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
	if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED;

	return loadParameterFrCSV(value);
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::setIp(cStr& _IP, bool isPersistent)
{
	if (isPersistent)
	{
		if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
		if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED;
	}
	else
	{
		if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
	}

	if (isPersistent && m_status == CAM_IS_CONNECTED)
	{
		if (true == m_Param.setIP(m_device->mDeviceNodeMap, m_device->mInterfaceHandle, m_consumer->tl(),
			m_device->mId, _IP, true))
		{
			// diconnect \ forceIP \ connect --------------
			if (All_OK == disconnectCamera())
			{
				if (true == m_Param.setIP(m_device->mInterfaceNodeMap, m_device->mInterfaceHandle, m_consumer->tl(),
					m_device->mId, _IP, false))
				{
					return connectCamera();
				}
			}
		}
	}
	else
	{
		if (true == m_Param.setIP(m_device->mInterfaceNodeMap, m_device->mInterfaceHandle, m_consumer->tl(),
			m_device->mId, _IP, false))
			return All_OK;
	}
	
	return ERROR_SET_IP;
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::setSubnet(cStr& _Su, bool isPersistent)
{
	if (isPersistent)
	{
		if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
		if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED;
	}
	else
	{
		if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
	}

	if (isPersistent && m_status == CAM_IS_CONNECTED)
	{
		if (true == m_Param.setSubnet(m_device->mDeviceNodeMap, m_device->mInterfaceHandle, m_consumer->tl(),
			m_device->mId, _Su, true))
		{
			// diconnect \ forceIP \ connect --------------
			if (All_OK == disconnectCamera())
			{
				if (true == m_Param.setSubnet(m_device->mInterfaceNodeMap, m_device->mInterfaceHandle, m_consumer->tl(),
					m_device->mId, _Su, false))
				{
					return connectCamera();
				}
			}
			
		}
	}
	else
	{
		if (true == m_Param.setSubnet(m_device->mInterfaceNodeMap, m_device->mInterfaceHandle, m_consumer->tl(),
			m_device->mId, _Su, false))
			return All_OK;
	}

	return ERROR_SET_SUBNET;
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::setParameterValue(cStr& _ParamterName, cStr& value)
{
	if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
	if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED;

	if (!m_Param.isValid())
	{
		auto err = _scanDeviceParameter();
		if (err != All_OK)					return err;
	}

	if (m_Param.isValid())
	{
		if (true == m_Param.setParameter(m_deviceNodeMap, _ParamterName, value))
			return All_OK;
		else
			return ERROR_PARAMETER_VALUE_INVALID;
	}
	return ERROR_PARAMETER_INVALID;
}

EXPORT_TO_DLL Str
Ranger3::getIp(bool isPersistent)
{
	if (isPersistent)
	{
		if (getStatus() == CAM_IS_STARTED)		return Str();
		if (getStatus() == CAM_IS_DISCONNECTED)	return Str();

		return m_Param.getIP(
			m_device->mDeviceNodeMap, m_device->mInterfaceHandle, m_consumer->tl(),
			m_device->mId, true);
	}
	else
	{
		if (getStatus() == CAM_IS_STARTED)		return Str();

		return m_Param.getIP(
			m_device->mInterfaceNodeMap, m_device->mInterfaceHandle, m_consumer->tl(),
			m_device->mId, false);
	}
}

EXPORT_TO_DLL Str
Ranger3::getSubNet(bool isPersistent)
{
	if (isPersistent)
	{
		if (getStatus() == CAM_IS_STARTED)		return Str();
		if (getStatus() == CAM_IS_DISCONNECTED)	return Str();

		return m_Param.getSubnet(
			m_device->mDeviceNodeMap, m_device->mInterfaceHandle, m_consumer->tl(),
			m_device->mId, true);
	}
	else
	{
		if (getStatus() == CAM_IS_STARTED)		return Str();
		
		return m_Param.getSubnet(
			m_device->mInterfaceNodeMap, m_device->mInterfaceHandle, m_consumer->tl(),
			m_device->mId, false);
	}
}

EXPORT_TO_DLL cStr
Ranger3::getParameterValue(cStr& _ParamterName)
{
	if (getStatus() == CAM_IS_STARTED)		return Str();
	if (getStatus() == CAM_IS_DISCONNECTED)	return Str();

	//if (!m_Param.isValid())
	//	_scanDeviceParameter();
	CAM_STATUS status = All_OK;
	if (!m_Param.isValid())
		status = _scanDeviceParameter();
	if (All_OK != status)					return Str();

	if (m_Param.isValid())
	{
		Str value;
		if (true == m_Param.getParameter(m_deviceNodeMap, _ParamterName, value))
			return value;
	}
	return Str();
}

EXPORT_TO_DLL CAM_STATUS Ranger3::getParameterValueConstraint(cStr& _ParamterName, Str& minV, Str& maxV, Str& incV)
{
	if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
	if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED;

	CAM_STATUS status = All_OK;
	if (!m_Param.isValid())
		status = _scanDeviceParameter();
	if (All_OK != status)					return ERROR_SCAN_PARAMETERS;

	if (m_Param.isValid())
	{		
		if (true == m_Param.getConstraintParameter(m_deviceNodeMap, _ParamterName,minV,maxV,incV))
			return All_OK;
	}

	return ERROR_PARAMETER_INVALID;
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::saveParameterToCSV(cStr& path)
{
	if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
	if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED; 
	
	std::ofstream file(path);
	if (file.is_open())
	{
		GenIRanger::exportDeviceParameters(m_deviceNodeMap._Ptr, file);
		file.close();
		return CAM_STATUS::All_OK;
	}
	else
		return CAM_STATUS::ERROR_CSV_PATH;
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::loadParameterFrCSV(cStr & para_csv_path)
{
	if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
	if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED;

	if (!para_csv_path.empty())
	{
		// Connect device node map to GenTL
		std::ifstream inputStream(para_csv_path);
		if (inputStream.good())
		{
			try {
				GenIRanger::importDeviceParameters(m_deviceNodeMap._Ptr, inputStream);
			}
			catch (std::exception e)
			{
				*m_log << e.what() << endl;
				return CAM_STATUS::ERROR_COMMUNICATION;
			}
			catch (...)
			{
				*m_log << "Fatal Error happend when importing parameters from " << para_csv_path << endl;
				return CAM_STATUS::ERROR_COMMUNICATION;
			}

			*m_log << "Imported device parameters from " << para_csv_path << endl;
			m_Param.setParametPath(para_csv_path);
		}
		else
		{
			*m_log << "ECode: ERROR_CSV_PATH. Parameter file " << para_csv_path << " could not be opened!" << endl;
			return CAM_STATUS::ERROR_CSV_PATH;
		}
	}
	else
	{
		*m_log << "* Warning: Parameter file is empty! Using inner parameters instead!" << endl;
		return CAM_STATUS::All_OK;
	}

	return CAM_STATUS::All_OK;
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::getAllParameterInfo(AllParams & Info, const bool forceUpdate)
{
	if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
	if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED;

	CAM_STATUS status = All_OK;
	if (!m_Param.isValid() || forceUpdate)
		status = _scanDeviceParameter();

	if (All_OK != status)					return status;

	Info = m_Param.getAllParams();
	if (Info.empty())
		return CAM_STATUS::ERROR_PARAMETERS_EMPTY;
	
	return status;
}

EXPORT_TO_DLL CAM_STATUS
Ranger3::getAllParameterInfo(Categories & cate, Params & para, const bool forceUpdate)
{
	if (getStatus() == CAM_IS_STARTED)		return ERROR_CAM_IS_STARTED;
	if (getStatus() == CAM_IS_DISCONNECTED)	return ERROR_CAM_IS_DISCONNECTED;

	CAM_STATUS status = All_OK;
	if (!m_Param.isValid() || forceUpdate)
		status = _scanDeviceParameter();
	if (All_OK != status)					return status;

	cate = m_Param.getCategories();
	para = m_Param.getParams();

	return CAM_STATUS::All_OK;
}

void
Ranger3::_setThreadPriority(GNodeMap &dataStreamNodeMap)
{
	GenApi::CIntegerPtr threadPriority
		= dataStreamNodeMap._GetNode("StreamThreadPriority");
	GenApi::CCommandPtr setStreamPrio
		= dataStreamNodeMap._GetNode("StreamThreadApplyPriority");
	if (threadPriority.IsValid())
	{
		// Default priority is "HIGHEST" = 2, set to TIME CRITICAL = 15
		threadPriority->SetValue(15);
	}
	else
	{
		*m_log << "WARNING: Could not set thread priority!" << endl;
	}
	if (setStreamPrio.IsValid())
	{
		setStreamPrio->Execute();
	}
	else
	{
		*m_log << "WARNING: Could not set thread priority!" << endl;
	}
}

void
Ranger3::_praseData(GTl tl, 
					Vec<GBufferH>& bufferHandles, 
					const size_t bufferId, 
					ImgT & imgTable)
{

	imgTable = ImgT();

	if (!m_IsOutputSensor)
	{
		// get buffer info
		BufferInfo bufferInfo(tl, m_device->mDataStreamHandle, bufferHandles[bufferId]);
		bool isMultiPart = bufferInfo.mBufferPayloadType == GenTL::PAYLOAD_TYPE_MULTI_PART;

		// prepare raw date
		if (isMultiPart)
		{
			m_pParts = std::make_unique<PartInfoCollection>(PartInfoCollection::fromBuffer(
				tl, m_device->mDataStreamHandle, bufferHandles[bufferId], bufferInfo.mPartCount));
		}

		// chunkMode
		if (m_ChunkModeActive)
		{
			
			try {
				// Attach the chunk adapter to this buffer to be able to access metadata
				m_chunkAdapter->attachBuffer(bufferHandles[bufferId], m_device->getBufferData()[bufferId]);
				FastMetadataExtractor FastExtractor(m_deviceNodeMap);
				imgTable.setChunkInfo(FastExtractor.extract());
			}
			catch (const std::exception&)
			{	throw std::exception("Enable ChunkDataControl->ChunkModeActive failed!");	}
		}

		// image data converting to 16 bits
		if (isMultiPart)
		{
			_convertAndCalibrate(imgTable, "Scan3dExtraction1", "Range");
			_convertAndCalibrate(imgTable, "Scan3dExtraction1", "Reflectance");
			_convertAndCalibrate(imgTable, "Scan3dExtraction1", "Scatter");
			_convertAndCalibrate(imgTable, "Scan3dExtraction2", "Range");
			_convertAndCalibrate(imgTable, "Scan3dExtraction2", "Reflectance");
			_convertAndCalibrate(imgTable, "Scan3dExtraction2", "Scatter");
		}
		else
		{
			uint8_t * pBuffer16 = new uint8_t[m_buffer16Size];
			if (bufferInfo.mDataFormat == PFNC_Coord3D_C12p || bufferInfo.mDataFormat == PFNC_Mono12p)
			{
				GenIRanger::convert12pTo16(bufferInfo.mDataPointer, bufferInfo.mBufferDataSize, pBuffer16, &m_buffer16Size);
			}
			else if (bufferInfo.mDataPointer != nullptr)
			{
				std::memcpy(pBuffer16, bufferInfo.mDataPointer, bufferInfo.mBufferDataSize);
			}
			else
			{		
				delete[] pBuffer16;
				return;
			}

			// calibration
			if (m_pCalibrationWrapper != nullptr)
			{
				m_pCalibrationWrapper->calibrate(pBuffer16, nullptr);
				auto cpData = m_pCalibrationWrapper->getData();
				imgTable.setCaliInfo(
					m_info.m_CI.cols, m_info.m_CI.rows,
					cpData->offsetX, 0.0, cpData->scaleX, m_Param.getScaleY(),
					cpData->lower_bound_x, cpData->upper_bound_x, cpData->lower_bound_r, cpData->upper_bound_r);
				imgTable.insertDataCopy(RAN_CAL, (uint8_t*)(cpData->getData(PN::RAN_C)), m_info.m_id);
			}

			// raw data
			imgTable.setRangeInfo(m_info.m_RI.cols, m_info.m_RI.rows, m_info.m_RI.aoiOffsetX, m_info.m_RI.aoiOffsetY, m_info.m_RI.aoiHeight, m_info.m_RI.aoiWidth, m_info.m_RI.m_RangeAxis);
			imgTable.insertDataCopy(RAN, pBuffer16, m_info.m_id);

			delete[] pBuffer16;
		}
		if (m_ChunkModeActive)	m_chunkAdapter->detachBuffer();
	}
	else
	{
		imgTable.setSensorInfo(m_info.m_SI.cols, m_info.m_SI.rows, m_info.m_SI.senOffsetX, m_info.m_SI.senOffsetY);
		imgTable.insertDataCopy(SEN, m_device->getBufferData()[bufferId], m_info.m_id);

	}
}

void
Ranger3::_convertAndCalibrate(ImgT & imgTable, cStr _region, cStr _component)
{
	auto regionLookup = EnumSelectorEntries(m_deviceNodeMap, "RegionSelector");
	auto componentLookup = EnumSelectorEntries(m_deviceNodeMap, "ComponentSelector");

	if (m_pParts->hasPart(regionLookup.value(_region.c_str()), componentLookup.value(_component.c_str())))
	{
		auto bpi = m_pParts->findPart(regionLookup.value(_region.c_str()), componentLookup.value(_component.c_str()));
		
		if (_component == "Range")
		{	
			uint8_t * pBuffer16 = new uint8_t[m_buffer16Size];
			// data format
			if (bpi.mPartDataFormat == PFNC_Coord3D_C12p || bpi.mPartDataFormat == PFNC_Mono12p)
				GenIRanger::convert12pTo16(bpi.mPartDataPointer, bpi.mPartDataSize, pBuffer16, &m_buffer16Size);
			else
				std::memcpy(pBuffer16, bpi.mPartDataPointer, bpi.mPartDataSize);

			// get reflectance
			bool hasRef = m_pParts->hasPart(regionLookup.value(_region.c_str()), componentLookup.value("Reflectance"));
			uint8_t * pRef = nullptr;
			if (hasRef)
			{
				auto bpiRef = m_pParts->findPart(regionLookup.value(_region.c_str()), componentLookup.value("Reflectance"));
				pRef = bpiRef.mPartDataPointer;
			}
			
			// calibration
			if (m_pCalibrationWrapper != nullptr)
			{
				m_pCalibrationWrapper->calibrate(pBuffer16, pRef);
				auto cpData = m_pCalibrationWrapper->getData();
				float * pfRef = hasRef ? (float *)cpData->getData(PN::REF_C) : nullptr;

				DN RANGE_CAL = ImgT::Str2DN("ComponentSelector_" + _component + "_RegionSelector_" + _region + "_CAL");
				DN REFLE_CAL = ImgT::Str2DN("ComponentSelector_Reflectance_RegionSelector_" + _region + "_CAL"); // float only now.
				
				imgTable.setCaliInfo(
					m_info.m_CI.cols, m_info.m_CI.rows,
					cpData->offsetX, 0.0, cpData->scaleX, m_Param.getScaleY(),
					cpData->lower_bound_x, cpData->upper_bound_x, cpData->lower_bound_r, cpData->upper_bound_r
				);
				imgTable.insertDataCopy(RANGE_CAL,	(uint8_t *)cpData->getData(PN::RAN_C),	m_info.m_id);
				imgTable.insertDataCopy(REFLE_CAL,	(uint8_t *)pfRef,						m_info.m_id);
			}

			// range
			imgTable.setRangeInfo(m_info.m_RI.cols, m_info.m_RI.rows, m_info.m_RI.aoiOffsetX, m_info.m_RI.aoiOffsetY, m_info.m_RI.aoiHeight, m_info.m_RI.aoiWidth, m_info.m_RI.m_RangeAxis);
			DN RANGE = ImgT::Str2DN("ComponentSelector_" + _component + "_RegionSelector_" + _region);
			DN REFLE = ImgT::Str2DN("ComponentSelector_Reflectance_RegionSelector_" + _region);
			imgTable.insertDataCopy(RANGE, pBuffer16, m_info.m_id);
			imgTable.insertDataCopy(REFLE, pRef, m_info.m_id); 
			
			delete[] pBuffer16;
		}
		else if (_component == "Reflectance")
		{
			// Been proccessed with Range
		}
		else if (_component == "Scatter" || _component == "Peak2")
		{
			uint8_t * pData = bpi.mPartDataPointer;
			imgTable.setRangeInfo(m_info.m_RI.cols, m_info.m_RI.rows, m_info.m_RI.aoiOffsetX, m_info.m_RI.aoiOffsetY, m_info.m_RI.aoiHeight, m_info.m_RI.aoiWidth, m_info.m_RI.m_RangeAxis);
			DN NAME = ImgT::Str2DN("ComponentSelector_" + _component + "_RegionSelector_" + _region);
			imgTable.insertDataCopy(NAME, pData, m_info.m_id);
		}
	}
}

CAM_STATUS
Ranger3::_grabSetting(	cStr & calib_xml_path, 
						const SiCaliWrapper::RectMethod method, 
						const float missingData, 
						const bool _doRectify)
{
	m_info.m_CI.rows = m_info.m_RI.rows = atoi(getParameterValue("Height_RegionSelector_Scan3dExtraction1").c_str());
	m_info.m_CI.cols = m_info.m_RI.cols = atoi(getParameterValue("Width_RegionSelector_Region1").c_str());
	
	m_buffer16Size = m_info.m_RI.cols * m_info.m_RI.rows * (m_isUsingReflectance ? 3 : 2); // range 2 bytes, reflectance 1 byte.
	m_info.m_RI.aoiHeight = atoi(getParameterValue("Height_RegionSelector_Region1").c_str());
	m_info.m_RI.aoiWidth = atoi(getParameterValue("Width_RegionSelector_Region1").c_str());
	m_info.m_RI.aoiOffsetY = atoi(getParameterValue("OffsetY_RegionSelector_Region1").c_str());
	m_info.m_RI.aoiOffsetX = atoi(getParameterValue("OffsetX_RegionSelector_Region1").c_str());

	// Get Region0 offsetXY
	{	
		GenApi::CEnumerationPtr regionSelector = m_deviceNodeMap._GetNode("RegionSelector");
		*regionSelector = "Region0";
		GenApi::CIntegerPtr offsetX = m_deviceNodeMap._GetNode("OffsetX");
		GenApi::CIntegerPtr offsetY = m_deviceNodeMap._GetNode("OffsetY");
		m_info.m_SI.senOffsetX = offsetX->GetValue();
		m_info.m_SI.senOffsetY = offsetY->GetValue();
	}

	// Get range axis
	bool rangeAxisStandard = "Reverse" != getParameterValue("RangeAxis_Scan3dExtractionSelector_Scan3dExtraction1");
	float scaleZ = rangeAxisStandard ? 0.0625f : -0.0625f;
	float originZ = rangeAxisStandard ? (float)m_info.m_RI.aoiOffsetY + 2.0f * scaleZ : m_info.m_RI.aoiOffsetY + m_info.m_RI.aoiHeight - 2.0f * scaleZ;

	m_info.m_RI.m_RangeAxis = rangeAxisStandard ? RA::STANDARD : RA::REVERSED;

	//Initialize the calibrationFilter and rectification filter
	const size_t rectificationWidth = 2560;

	if (calib_xml_path.empty())
	{
		*m_log << "* Warning: No using calibration file!! Get raw data!" << endl;
		m_pCalibrationWrapper = nullptr;
		return CAM_STATUS::All_OK;
	}
	else
	{
		m_Param.setDoRectify(_doRectify);
		m_pCalibrationWrapper = std::make_unique<SiCaliWrapper::CalibrationWrapper>();
		if (false == m_pCalibrationWrapper->initCalibrationFilter(
			calib_xml_path, (int)m_info.m_CI.cols, (int)m_info.m_CI.rows, (int)m_info.m_RI.aoiHeight,
			scaleZ, originZ, method, rectificationWidth, missingData, m_Param.getDoRectify()))
		{
			m_pCalibrationWrapper = nullptr;
			return CAM_STATUS::ERROR_XML_PATH;
		}
	}
	return CAM_STATUS::All_OK;
}

void
Ranger3::_chunkDataSetting()
{
	GenApi::CBooleanPtr chunkModeActive = m_deviceNodeMap._GetNode("ChunkModeActive");
	m_ChunkModeActive = chunkModeActive->GetValue();

	if (m_ChunkModeActive)
	{
		GenTLApi* tl = m_pR3S->getConsumer()->tl();
		m_chunkAdapter = std::make_unique<SiChunkAdapter>(tl, m_device->mDataStreamHandle);
		m_chunkAdapter->attachNodeMap(m_deviceNodeMap._Ptr);		
		*m_log << "ChunkModeActive : Enable " << endl;
	}
	else
	{
		m_chunkAdapter = nullptr;
		*m_log << "ChunkModeActive : Disable " << endl;
	}
}

void
Ranger3::_reflectanceCheck()
{
	Str enable = getParameterValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1");
	m_isUsingReflectance = enable == "1";
	*m_log << "Reflectance: " << (m_isUsingReflectance ? "Enable" : "Disable") << endl;
}

CAM_STATUS
Ranger3::_connectCamera()
{
	if (m_pR3S == nullptr)			return CAM_STATUS::ERROR_NULL_PTR_R3;
	if (m_deviceHandle == nullptr)	return CAM_STATUS::ERROR_NULL_DEV_HANDLE;

	if (m_status == CAM_IS_DISCONNECTED)
	{
		m_device->openDevice();
		m_device->openDataStream();

		if (m_device->mDataStreamHandle == GENTL_INVALID_HANDLE)
		{
			*m_log << "Error: dataStreamHandle == GENTL_INVALID_HANDLE " << endl;
			return ERROR_NULL_DS_HANDLE;
		}

		// Get node map for data stream module to boost receiver thread priority
		m_device->createDataStreamNodeMap(*m_pR3S->getConsumer());
		_setThreadPriority(m_device->mDataStreamNodeMap);

		// Connect device node map to GenTL
		m_device->createDeviceNodeMap(*m_pR3S->getConsumer());
		m_deviceNodeMap = m_device->mDeviceNodeMap;


	}

	return CAM_STATUS::All_OK;
}

CAM_STATUS
Ranger3::_getImageData(ImgT & imgTable)
{
	GenTLApi* tl = m_pR3S->getConsumer()->tl();
	// Wait for buffer to be received
	GenTL::EVENT_NEW_BUFFER_DATA bufferData;
	size_t eventSize = sizeof(bufferData);
	try {
		CC(tl, tl->EventGetData(m_device->mNewBufferEventHandle, &bufferData, &eventSize, m_Param.getTimeOut()));
	}
	catch (const std::exception& e) {
		Str str_e(e.what());
		if (str_e == "GenTL call failed: -1011, Message: GetEventData: wait operation timed out")
		{
			*m_log << str_e.c_str() << endl;
			return CAM_STATUS::TIME_OUT;
		}
		else
			throw;
	}
	catch (...) { throw; }
	

	intptr_t bufferId = reinterpret_cast<intptr_t>(bufferData.pUserPointer);
	std::vector<GenTL::BUFFER_HANDLE> bufferHandles = m_device->getBufferHandles();
	try
	{
		//OutputDebugString("_praseData()\n");

		_praseData(tl, bufferHandles, bufferId, imgTable);	
		if (imgTable.isEmpty())
			return CAM_STATUS::ERROR_EMPTY_IMG;
		++m_info.m_id;
	}
	catch (...) {
		return CAM_STATUS::ERROR_GET_IMG;
	}

	CC(tl, tl->DSQueueBuffer(m_device->mDataStreamHandle, bufferHandles[bufferId]));

	return CAM_STATUS::All_OK;
}

CAM_STATUS
Ranger3::_disconnectCamera()
{
	if (m_status != CAM_IS_DISCONNECTED)
	{
		if (m_device->isAcquisitionRunning())
			_stopAcquisition();
		
		m_device->closeDataStream();
		m_device->closeDevice();
	}

	*m_log << "Device disconnected!" << endl;
	return All_OK;
}

CAM_STATUS
Ranger3::_startAcquisition()
{
	if (!m_device->isAcquisitionRunning())
	{
		_reflectanceCheck();
		auto isOK = _grabSetting(m_Param.getCalibraPath(), m_Param.getRectMethos(), m_Param.getMissingData(), m_Param.getDoRectify());
		if (isOK != All_OK)
			return isOK;
		
		// Wrapper to be able to access chunk metadata
		_chunkDataSetting();

		// init buffer
		{
			// Setup a sufficient amount of buffers
			GenApi::CIntegerPtr payload = m_deviceNodeMap._GetNode("PayloadSize");
			m_payloadSize = static_cast<size_t>(payload->GetValue());
			m_device->initializeBuffers(m_Param.getBufferCount(), m_payloadSize);
			size_t totalAllocatedMemory = m_Param.getBufferCount() * m_payloadSize;

			*m_log << "Total memory used for buffers: " << (totalAllocatedMemory / 1024 / 1024) << " MB\n" << "Init buffer done. \n" << endl;
		}

		// register event
		m_device->registerNewBufferEvent();
		*m_log << "register New Buffer Event done. " << endl;

		// Lock all parameters before starting
		GenApi::CIntegerPtr paramLock = m_deviceNodeMap._GetNode("TLParamsLocked");
		paramLock->SetValue(1);

		// Start acquisition
		GenTLApi* tl = m_pR3S->getConsumer()->tl();
		CC(tl, tl->DSStartAcquisition(m_device->mDataStreamHandle, GenTL::ACQ_START_FLAGS_DEFAULT, GENTL_INFINITE));
		m_device->startAcquisition();
		*m_log << "start Acquisition. " << endl;
		m_info.m_id = 0;
	}

	if (m_device->isAcquisitionRunning())
	{
		Str DeviceScanType = getParameterValue("DeviceScanType");

		m_IsOutputSensor = DeviceScanType == "Areascan";
		GenApi::CEnumerationPtr regionSelector = m_deviceNodeMap._GetNode("RegionSelector");
		
		if (m_IsOutputSensor)
		{
			*regionSelector = "Region0";
			GenApi::CIntegerPtr height = m_deviceNodeMap._GetNode("Height");
			GenApi::CIntegerPtr width = m_deviceNodeMap._GetNode("Width");
			m_info.m_SI.cols = width->GetValue();
			m_info.m_SI.rows = height->GetValue();
		}
		else
		{
			*regionSelector = "Scan3dExtraction1";
			GenApi::CIntegerPtr height = m_deviceNodeMap._GetNode("Height");
			GenApi::CIntegerPtr width = m_deviceNodeMap._GetNode("Width");
			m_info.m_CI.cols = m_info.m_RI.cols = width->GetValue();
			m_info.m_CI.rows = m_info.m_RI.rows = height->GetValue();
		}
		return All_OK;
	}
	else
		return ERROR_START_ACQUISITION;
}

CAM_STATUS
Ranger3::_stopAcquisition()
{
	GenTLApi* tl = m_pR3S->getConsumer()->tl();
	if (m_device->isAcquisitionRunning())
	{
		m_device->stopAcquisition();
		CC(tl, tl->DSStopAcquisition(m_device->mDataStreamHandle, GenTL::ACQ_STOP_FLAGS_KILL));
		GenApi::CIntegerPtr paramLock = m_deviceNodeMap._GetNode("TLParamsLocked");
		paramLock->SetValue(0);
	}
	else
		return All_OK;

	if (!m_device->isAcquisitionRunning())
	{
		CC(tl, tl->DSFlushQueue(m_device->mDataStreamHandle, GenTL::ACQ_QUEUE_ALL_DISCARD));
		m_device->unregisterNewBufferEvent();

		if (m_ChunkModeActive)
		{
			m_chunkAdapter->detachNodeMap();
			m_chunkAdapter.reset();
		}

		m_device->teardownBuffers();
		return All_OK;
	}
	else
		return ERROR_STOP_ACQUISITION;
}

CAM_STATUS
Ranger3::_scanDeviceParameter(cStr path)
{
	if (m_device->isAcquisitionRunning())
		return CAM_STATUS::ERROR_CAM_IS_STARTED;

	if (m_Param.ScanDeviceParameters(m_deviceNodeMap._Ptr, path))
		return CAM_STATUS::All_OK;

	return CAM_STATUS::ERROR_SCAN_PARAMETERS;
}


std::string VER::m_version =
	std::string("### 3050_5 ### \n\n")
	+ "1. 更新 ImgT::loadFromIconFile(cStr path)，支持输入路径带有扩展名 xml XML dat DAT。\n\n"
	+ "2. 更新 ImgT::loadFromIconFile(cStr path)，支持输入路径不带有扩展名。\n\n"
	+ "3. 更新 ImgT::saveFromIconFile(cStr path)，支持输入路径带有扩展名 xml XML dat DAT，支持输入路径不带有扩展名。\n\n"
	+ "4. 更新 ImgT::_clearBuffer(ImgT & img)中的内存释放。\n\n"
	+ "5. 更新 ImgT::saveFromIconFile()，增加对涂胶固件的支持。\n\n"
	+ "6. 正式发布 C# API。 \n\n"
	;


}
