/**	@file	SampleApiDllCSharpWrapper.h
*
*	@brief	C# Wrapper Class of Ranger3.
*
*	@attention
*	This is the C# API wrapper of C++ Class Ranger3 and Ranger3Shared in ranger3.cpp|h.
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/

#include "stdafx.h"
#include "Ranger3CSharpWrapper.h"

namespace SICK3D
{

////////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// Wrapper for C# Ranger3Shared

	
GenICam3D::GenICam3D(	const String^ logPath,
				const String^ ctiPath,
				const bool enShow,
				const bool enWrite)
{
	pin_ptr<const wchar_t> wpStrlogPath = PtrToStringChars(logPath);
	auto strlogPath = wchart2string(wpStrlogPath);

	pin_ptr<const wchar_t> wpStrctiPath = PtrToStringChars(ctiPath);
	auto strctiPath = wchart2string(wpStrctiPath);
	gp_R3S = std::make_shared<R3S>(strlogPath, strctiPath, enShow, enWrite);
}

GenICam3D::~GenICam3D()
{
	cout << "Release CSR3S ----------------\n\n";
	gp_R3S->~Ranger3Shared();
	gp_R3S.reset();
}

deviceList 
GenICam3D::getConDevList()
{
	auto devs = gp_R3S->getConDevList();
	auto res = gcnew array<String^>(devs.size());
	int i = 0;
	for (auto sub : devs)
	{
		std::wstring ws = string2wstring(sub.first);
		//StringToWstring(ws, sub.first);
		res[i] = gcnew String(ws.c_str());
	}
	return res;
}

deviceList 
GenICam3D::getConDevListIP()
{
	auto devs = gp_R3S->getConDevListIP();
	auto res = gcnew array<String^>(devs.size());
	int i = 0;
	for (auto sub : devs)
	{
		std::wstring ws = string2wstring(sub.first);
		//StringToWstring(ws, sub.first);
		res[i] = gcnew String(ws.c_str());
	}
	return res;
}

void GenICam3D::_release()
{
	gp_R3S.reset(); 
}


////////////////////////////////////////////////////////////////////////////////

GrabberThread::GrabberThread(R3 * pr3)
	: mp_R3(pr3), m_IsRun(false), m_IsFinishGrab(true), m_req(RequireCode::REQ_Nothing)
{
	locker_data = gcnew Object;
	locker_req = gcnew Object;
}

GrabberThread::~GrabberThread()
{
	_clear_and_break();
}

void
GrabberThread::run()
{
	m_IsRun = true;
	while (true)
	{
		SickCam::ImgT img;

		if (_doRequest()) break;

		if (m_req == RequireCode::REQ_Nothing)
		{
			m_IsFinishGrab = false;

			if (Ecode::All_OK == mp_R3->getImageData(img) && !img.isEmpty())
			{
				// copy data
				CSImgT^ imgTable = gcnew CSImgT();
				imgTable->setData(img);

				lock l(locker_data);
				m_ImageData.push_back(imgTable);
				//std::cout << " donot m_ImageData.push_back(imgTable);" << std::endl;
				m_IsGrabbed = true;

			}

			m_IsFinishGrab = true;

		}

		if (_doRequest()) break;

		Sleep(50);
	}
	_clear_and_break();
	m_IsRun = false;
}



//CSImgT ^ GrabberThread::getData()
//{
//	throw gcnew System::NotImplementedException();
//	// TODO: 在此处插入 return 语句
//}

CSImgT^ 
GrabberThread::getData()
{
	lock l(locker_data);
	
	//auto head = m_ImageData.front() ; 
	CSImgT^ head = gcnew CSImgT(); // return to caller. Deleted by caller.
	head->_deepCopy(m_ImageData.front());
	
	std::cout << "getData delete: " << m_ImageData.front()->get_ID() << std::endl;
	delete m_ImageData.front();
	m_ImageData.pop_front(); // delete data generate by "gcnew" in run().

	m_IsGrabbed = !m_ImageData.empty();

	return head;
}

//CSImgT^
//GrabberThread::getDataHeadOnly()
//{
//	if (m_ImageData.size() == 0)
//	{
//		std::cout << "m_ImageData.size() = 0"<< std::endl;
//		m_IsGrabbed = false;
//
//		return gcnew CSImgT();
//	}
//
//	lock l(locker_data);
//
//	auto head = m_ImageData.front();
//	//m_ImageData.pop_front();
//	m_IsGrabbed = !m_ImageData.empty();
//
//	std::cout << "m_ImageData.size() = " << m_ImageData.size() << std::endl;
//
//	return head;
//}
//
//void GrabberThread::freeDataHead()
//{
//	lock l(locker_data);
//
//	if (m_ImageData.size()!=0)
//	{
//		auto head = m_ImageData.front();
//		m_ImageData.pop_front();
//		delete head;
//	}
//}

bool
GrabberThread::sendRequest(const RequireCode req)
{
	lock l(locker_req);
	m_req = req;
	return true;
}

bool
GrabberThread::_doRequest()
{
	switch (m_req)
	{
	case RequireCode::REQ_QUIT_RUN:
		return _clear_and_break();
		break;
	case RequireCode::REQ_STOP_CAMERA:
		// do nothing, but wait grabbing routine over in Run().
		_clear_and_break();
		break;
	case RequireCode::REQ_Nothing:
		break;
	default:
		break;
	}
	return false;
}

bool 
GrabberThread::_clear_and_break()
{
	lock l(locker_data);

	m_IsGrabbed = false;
	for (auto it = m_ImageData.begin(); it != m_ImageData.end(); ++it)
	{
		std::cout << "delete: " << (*it)->get_ID() << std::endl;
		delete *it;
	}
	m_ImageData.clear();


	GC::Collect();
	GC::WaitForFullGCComplete();

	return m_ImageData.empty();
}

////////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// Wrapper for C# Ranger3 

Ranger3::Ranger3(const String ^ ip_mac, const bool isIP)
 : m_enable_CPP_Grab_thread(false)
{
	pin_ptr<const wchar_t> wStr = PtrToStringChars(ip_mac);
	std::string sIpMac = wchart2string(wStr);

	mp_R3					= new SickCam::Ranger3(gp_R3S, sIpMac, isIP);
	mp_cate					= new SickCam::Categories;
	mp_map_Cata_ParamStr	= new SickCam::Params;
	mp_map_allPramPtr		= new SickCam::AllParams;

	mp_gt = gcnew GrabberThread(mp_R3);
	locker_GetData = gcnew Object;

}

Ranger3::~Ranger3()
{
	if (mp_R3 != nullptr)
	{
		delete mp_R3;
		mp_R3 = nullptr;
	}

	if (mp_cate != nullptr)
	{
		delete mp_cate;
		mp_cate = nullptr;
	}

	if (mp_map_Cata_ParamStr != nullptr)
	{
		delete mp_map_Cata_ParamStr;
		mp_map_Cata_ParamStr = nullptr;
	}

	if (mp_map_allPramPtr != nullptr)
	{
		delete mp_map_allPramPtr;
		mp_map_allPramPtr = nullptr;
	}
}

CAM_STATUS 
Ranger3::isReady()
{
	return static_cast<CAM_STATUS>(mp_R3->isReady());
}

CAM_STATUS
Ranger3::connectCamera(const bool enable_CPP_Grab_thread)
{
	m_enable_CPP_Grab_thread = enable_CPP_Grab_thread;
	if (m_enable_CPP_Grab_thread)
	{
		mp_gt->sendRequest(RequireCode::REQ_Nothing);
		m_Thread = gcnew Thread(gcnew ThreadStart(mp_gt, &(GrabberThread::run)));
		m_Thread->Start();
	}

	return static_cast<CAM_STATUS>(mp_R3->connectCamera());
}

CAM_STATUS 
Ranger3::startCamera()
{
	// start run 
	if (m_enable_CPP_Grab_thread)
	{
		mp_gt->sendRequest(RequireCode::REQ_Nothing);
		while (!mp_gt->isRun()) Sleep(5);
	}

	return static_cast<CAM_STATUS>(mp_R3->startCamera());
}

CAM_STATUS 
Ranger3::stopCamera()
{
	//if (mp_R3->getStatus() == SickCam::CAM_IS_STARTED)
	{
		lock l(locker_GetData);

		// stop run 
		if (m_enable_CPP_Grab_thread && mp_R3->getStatus() == SickCam::CAM_IS_STARTED)
		{
			mp_gt->sendRequest(RequireCode::REQ_STOP_CAMERA);
			while (!mp_gt->isFinishGrab()) Sleep(5);
		}

		auto status = mp_R3->stopCamera();
		return static_cast<CAM_STATUS>(status);
	}
	//else if (mp_R3->getStatus() == SickCam::CAM_IS_STARTED)
	//	return CAM_STATUS::CAM_IS_STOPPED;
}

CAM_STATUS
Ranger3::disconnectCamera()
{
	if (m_enable_CPP_Grab_thread)
	{
		mp_gt->sendRequest(RequireCode::REQ_QUIT_RUN);
		while (mp_gt->isRun()) Sleep(5);
	}

	auto status = mp_R3->disconnectCamera();
	return static_cast<CAM_STATUS>(status);
}

CAM_STATUS	
Ranger3::getImageData(CSImgT^ imgTable)
{
	lock l(locker_GetData);
	if (m_enable_CPP_Grab_thread)
	{
		// getData from GrabberThread
		if (!isGrabbed())
			return CAM_STATUS::ERROR_IMAGE_NOT_READY;
		
		// 原来的代码
		//imgTable->_deepCopy(mp_gt->getData());

		// todo: test code here ------------------
		//CSImgT^ test = gcnew CSImgT();
		//test->_deepCopy(mp_gt->getData());
		//delete test;

		//mp_gt->getData(); // todo: test -------------

		auto pData = mp_gt->getData(); // gcnew an extra memory
		imgTable->_deepCopy(pData); // copy memory
		delete pData; // delete here

		//imgTable = mp_gt->getData(); // gcnew an extra memory

		return CAM_STATUS::All_OK;
	} 
	else
	{
		if (mp_R3->getStatus() != SickCam::CAM_IS_STARTED)
			return CAM_STATUS::ERROR_CAM_NOT_START;

		auto atus = mp_R3->getStatus();
		SickCam::ImgT img;
		auto status = mp_R3->getImageData(img);
		imgTable->setData(img);
		return static_cast<CAM_STATUS>(status);
	}
}

CAM_STATUS
Ranger3::setCalibraPath(const String^ value)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(value);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->setCalibraPath(str));
}

CAM_STATUS
Ranger3::setParametPath(const String^ value)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(value);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->setParametPath(str));
}

CAM_STATUS 
Ranger3::setIp(const String ^ IP, const bool isPersistent)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(IP);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->setIp(str, isPersistent));
}

CAM_STATUS 
Ranger3::setSubnet(const String ^ Su, const bool isPersistent)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(Su);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->setSubnet(str, isPersistent));
}

CAM_STATUS 
Ranger3::setParameterValue(const String ^ ParamterName, const String ^ value)
{
	pin_ptr<const wchar_t> wpStrPN = PtrToStringChars(ParamterName);
	auto strPN = wchart2string(wpStrPN);

	pin_ptr<const wchar_t> wpStrVa = PtrToStringChars(value);
	auto strVa = wchart2string(wpStrVa);

	return static_cast<CAM_STATUS>(mp_R3->setParameterValue(strPN, strVa));
}

String ^ 
Ranger3::getCalibraPath()
{
	auto str = mp_R3->getCalibraPath();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

String ^ 
Ranger3::getParametPath()
{
	auto str = mp_R3->getParametPath();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

String ^ 
Ranger3::getIp(const bool isPersistent)
{
	auto str = mp_R3->getIp(isPersistent);
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

String ^ 
Ranger3::getSubNet(const bool isPersistent)
{
	auto str = mp_R3->getSubNet(isPersistent);
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

String ^ 
Ranger3::getMac()
{
	auto str = mp_R3->getMac();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

String ^ 
Ranger3::getDeviceName()
{
	auto str = mp_R3->getDeviceName();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

String ^ 
Ranger3::getParameterValue(const String ^ ParamterName)
{
	pin_ptr<const wchar_t> wpStrPN = PtrToStringChars(ParamterName);
	auto strPN = wchart2string(wpStrPN);

	auto strV = mp_R3->getParameterValue(strPN);
	auto wstrV = string2wstring(strV);
	return gcnew String(wstrV.c_str());
}

CAM_STATUS 
Ranger3::saveParameterToCSV(const String ^ path)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->saveParameterToCSV(str));
}

CAM_STATUS 
Ranger3::loadParameterFrCSV(const String ^ path)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->loadParameterFrCSV(str));
}

bool 
Ranger3::updateParamtersInfo()
{
	auto ok1 = mp_R3->getAllParameterInfo(*mp_cate, *mp_map_Cata_ParamStr, true); // force to update all parameters
	auto ok2 = mp_R3->getAllParameterInfo(*mp_map_allPramPtr);
	return ok1 == ok2 && ok1 == SickCam::All_OK;
}

array<String^>^
Ranger3::paramCategoryList()
{
	array<String^>^ arrCate = gcnew array<String^>((*mp_cate).size());
	for (int i = 0; i<(*mp_cate).size(); ++i)
	{
		arrCate[i] = gcnew String(string2wstring((*mp_cate)[i]).c_str());
	}

	return arrCate;
}

array<String^>^ 
Ranger3::paramFullNameOfCategory(const String ^ categoryName)
{
	if (mp_map_Cata_ParamStr == nullptr)
		return gcnew array<String^>(0);

	auto cataName = StringRef2string(categoryName);

	if (mp_map_Cata_ParamStr->count(cataName) != 1)
		return gcnew array<String^>(0);

	auto paramPtr = mp_map_Cata_ParamStr->at(cataName);

	array<String^>^ arrStr = gcnew array<String^>(paramPtr.size());
	for (int i = 0; i<paramPtr.size(); ++i)
	{
		arrStr[i] = gcnew String(string2wstring(paramPtr[i]->getCsvName()).c_str());
	}

	return arrStr;
}

array<String^>^ 
Ranger3::paramFullNameList()
{
	auto paramPtr = mp_map_allPramPtr;

	array<String^>^ arrStr = gcnew array<String^>(mp_map_allPramPtr->size());
	int i = 0; 
	for (auto sub : (*mp_map_allPramPtr))
	{
		arrStr[i] = gcnew String(string2wstring(sub.second->getCsvName()).c_str());
		++i;
	}

	return arrStr;
}

String ^ 
Ranger3::paramCurrentValue(const String ^ paramFullName)
{
	if (mp_map_allPramPtr == nullptr)
		return gcnew String(std::wstring().c_str());

	auto fullName = StringRef2string(paramFullName);

	if (mp_map_allPramPtr->count(fullName) != 1)
		return gcnew String(std::wstring().c_str());

	auto valueStr = mp_map_allPramPtr->at(fullName)->getStrValue();
	return gcnew String(string2wstring(valueStr).c_str());
}

array<String^>^ 
Ranger3::paramOptionalValues(const String ^ paramFullName, int% currentValueId)
{
	if (mp_map_allPramPtr == nullptr)
		return gcnew array<String^>(0);

	auto currentValueStr = paramCurrentValue(paramFullName);

	currentValueId = 0;

	auto fullName = StringRef2string(paramFullName);

	if (mp_map_allPramPtr->count(fullName) != 1)
		return gcnew array<String^>(0);

	auto optValus = mp_map_allPramPtr->at(fullName)->getOptions();
	array<String^>^ arrStr = gcnew array<String^>(optValus.size());
	for (int i = 0; i<optValus.size(); ++i)
	{
		arrStr[i] = gcnew String(string2wstring(optValus[i]).c_str());
		if (currentValueStr == arrStr[i])
			currentValueId = i;
	}

	return arrStr;
}

String ^
VER::__Version__()
{
	auto str = SickCam::VER::__version__();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

}
