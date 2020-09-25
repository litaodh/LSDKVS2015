/**	@file	DeviceConnection.cpp
*
*	@brief	Class DeviceConnection.
*
*	@attention
*		Used to Open & close Device \ Open & close Datastream \ connect \ start \ stop \ disconnect camera.
*		Call GenICam API and SICK API.
*		This class will be used by Class Ranger3Shared and Ranger3.
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/


#include "DeviceConnection.h"

namespace SickCam
{

////////////////////////////////////////////////////////////////////////////////



DeviceConnection::DeviceConnection(ConsumerPtr consumer, GenTL::IF_HANDLE interfaceHandle, 
	SiDeviceList devices, int64_t id)
	: mpConsumer(consumer), mAcquisitionRunning(false), mInterfaceHandle(interfaceHandle),
	mIsOpenDevice(false), mIsOpenDataStream(false), mDeviceList(devices), 
	mId(id)
{
	auto tl = mpConsumer->tl();
	mInterfacePort = std::make_shared<SiGenTLPort>(interfaceHandle, tl);
	mInterfaceNodeMap = mpConsumer->getNodeMap(mInterfacePort.get(), "InterfacePort");

	openDevice();
	closeDevice();
}

DeviceConnection::~DeviceConnection()
{
	closeDataStream();
	closeDevice();

	mBufferData.clear();
	mBufferData.swap(Vec<uint8_t*>()); // not support by vs2017
}

bool 
DeviceConnection::openDevice(SiDeviceList devices)
{
	if (mIsOpenDevice) return true;
	if (devices.empty()) return false;

	auto tl = mpConsumer->tl();

	Str deviceId = mpConsumer->findDeviceByIndex(devices, mId);
	if (deviceId.empty())
	{
		std::cerr << "Error: deviceId.empty() " << std::endl;
		return false;
	}

	mDeviceHandle = mpConsumer->openDeviceById(mInterfaceHandle, deviceId);
	if (mDeviceHandle == GENTL_INVALID_HANDLE)
	{
		std::cerr << "Error: deviceHandle == GENTL_INVALID_HANDLE " << std::endl;
		std::cout << "       If the device IP is not in the same subnet with PC, please try forceIP demo first! " << std::endl;
		std::cout << "       If the device is connected to another process, please disconnect it first and try again! " << std::endl;
		return false;
	}

	GenTL::INFO_DATATYPE dataType;
	char modelName[1024];
	size_t modelNameSize = sizeof(modelName);

	CC(tl, tl->IFGetDeviceInfo(mInterfaceHandle, deviceId.c_str(), GenTL::DEVICE_INFO_MODEL, &dataType, modelName, &modelNameSize));

	// Append user defined name if possible
	char deviceUserId[1024];
	size_t deviceUserIdSize = sizeof(deviceUserId);
	auto status = tl->IFGetDeviceInfo(mInterfaceHandle, deviceId.c_str(), GenTL::DEVICE_INFO_USER_DEFINED_NAME, &dataType, deviceUserId, &deviceUserIdSize);

	std::stringstream deviceName;
	deviceName << deviceId;
	if (status == GenTL::GC_ERR_SUCCESS)
		deviceName << "_" << deviceUserId;

	mDeviceList = devices;
	mDeviceName = deviceName.str();
	mIsOpenDevice = !mDeviceName.empty();
	_queryIP();
	_querySubnet();
	_queryMAC();
	return mIsOpenDevice;
}

bool 
DeviceConnection::openDevice()
{
	return openDevice(mDeviceList);
}

bool 
DeviceConnection::closeDevice()
{
	if (!mIsOpenDevice) return true;
	mpConsumer->closeDevice(mDeviceHandle);
	mIsOpenDevice = false;
	return !mIsOpenDevice;
}

bool 
DeviceConnection::openDataStream()
{
	if (mIsOpenDataStream) return true;
	mDataStreamHandle = mpConsumer->openDataStream(mDeviceHandle);
	mIsOpenDataStream = true;
	return mIsOpenDataStream;
}

bool 
DeviceConnection::closeDataStream()
{
	if (!mIsOpenDataStream) return true;
	mpConsumer->closeDataStream(mDataStreamHandle);
	mIsOpenDataStream = false;
	return !mIsOpenDataStream;
}

void 
DeviceConnection::initializeBuffers(size_t buffersCount, size_t payloadSize)
{
	GenTLApi * mTl = mpConsumer->tl();

	mBufferHandles.resize(buffersCount, GENTL_INVALID_HANDLE);
	mBufferData.resize(buffersCount, nullptr);

	for (size_t i = 0; i < buffersCount; i++)
	{
		mBufferData[i] = new uint8_t[payloadSize];
		if (mBufferData[i] == nullptr)
		{
			throw std::exception("Could not allocated enough memory for buffers");
		}
		// Store the pointer to the raw memory in the user data for sake of
		// simplicity. This makes it easy to access the memory when a buffer has
		// been received.
		CC(mTl, mTl->DSAnnounceBuffer(mDataStreamHandle,
			mBufferData[i],
			payloadSize,
			reinterpret_cast<void*>(i),
			&mBufferHandles[i]));
		CC(mTl, mTl->DSQueueBuffer(mDataStreamHandle, mBufferHandles[i]));
	}
}

void 
DeviceConnection::teardownBuffers()
{
	GenTLApi * mTl = mpConsumer->tl();

	for (size_t i = 0; i < mBufferHandles.size(); i++)
	{
		// Revoke buffer from stream so that memory can be deleted safely
		CC(mTl, mTl->DSRevokeBuffer(mDataStreamHandle,
			mBufferHandles[i],
			reinterpret_cast<void**>(&mBufferData[i]),
			nullptr));
		delete[] mBufferData[i];
		mBufferData[i] = nullptr;
	}
}

void 
DeviceConnection::registerNewBufferEvent()
{
	GenTLApi * mTl = mpConsumer->tl();
	CC(mTl, mTl->GCRegisterEvent(mDataStreamHandle, GenTL::EVENT_NEW_BUFFER, &mNewBufferEventHandle));
}

void 
DeviceConnection::unregisterNewBufferEvent()
{
	GenTLApi * mTl = mpConsumer->tl();
	CC(mTl, mTl->GCUnregisterEvent(mDataStreamHandle, GenTL::EVENT_NEW_BUFFER));
}

void 
DeviceConnection::startAcquisition()
{
	GenApi::CCommandPtr acquisitionStart = mDeviceNodeMap._GetNode("AcquisitionStart");
	acquisitionStart->Execute();
	mAcquisitionRunning = true;
}

void 
DeviceConnection::stopAcquisition()
{
	if (mAcquisitionRunning)
	{
		std::cout << "Stopping acquisition of " << mDeviceName << std::endl;
		GenApi::CCommandPtr acquisitionStop = mDeviceNodeMap._GetNode("AcquisitionStop");
		acquisitionStop->Execute();
		mAcquisitionRunning = false;
		std::cout << "Stopped acquisition of " << mDeviceName << std::endl;
	}
}

bool 
DeviceConnection::isAcquisitionRunning() const
{
	return mAcquisitionRunning;
}

void 
DeviceConnection::createDeviceNodeMap(SiConsumer& consumer)
{
	GenTLApi * mTl = mpConsumer->tl();
	GenTL::PORT_HANDLE devicePort;
	CC(mTl, mTl->DevGetPort(mDeviceHandle, &devicePort));
	mDevicePort.reset(new SiGenTLPort(devicePort, mTl));
	mDeviceNodeMap = consumer.getNodeMap(mDevicePort.get(), "Device");
}

void 
DeviceConnection::createDataStreamNodeMap(SiConsumer& consumer)
{
	GenTLApi * mTl = mpConsumer->tl();
	mDataStreamPort.reset(new SiGenTLPort(mDataStreamHandle, mTl));
	mDataStreamNodeMap = consumer.getNodeMap(mDataStreamPort.get(), "StreamPort");
}

void 
DeviceConnection::updateIp()
{
	if(mIsOpenDevice)
		_queryIP();
}

void 
DeviceConnection::updateSubnet()
{
	if (mIsOpenDevice)
		_querySubnet();
}

void 
DeviceConnection::_queryIP()
{
	mDeviceSelector = mInterfaceNodeMap._GetNode("DeviceSelector");
	mDeviceSelector->SetValue(mId);
	GenApi::CIntegerPtr gevDeviceIpAddress = mInterfaceNodeMap._GetNode("GevDeviceIPAddress");
	uint32_t currentIp = static_cast<uint32_t>(gevDeviceIpAddress->GetValue());
	mDeviceIP = SiStringFromIp4(currentIp);
}

void 
DeviceConnection::_querySubnet()
{
	GenApi::CIntegerPtr deviceSelector = mInterfaceNodeMap._GetNode("DeviceSelector");
	deviceSelector->SetValue(mId);
	GenApi::CIntegerPtr gevDevice = mInterfaceNodeMap._GetNode("GevDeviceSubnetMask");
	uint32_t current = static_cast<uint32_t>(gevDevice->GetValue());
	mDeviceSubnet = SiStringFromIp4(current);
}

void 
DeviceConnection::_queryMAC()
{
	GenApi::CIntegerPtr deviceSelector = mInterfaceNodeMap._GetNode("DeviceSelector");
	deviceSelector->SetValue(mId);
	GenApi::CIntegerPtr gevDeviceMacAddress = mInterfaceNodeMap._GetNode("GevDeviceMACAddress");
	uint64_t currentMac = static_cast<uint64_t>(gevDeviceMacAddress->GetValue());
	mDeviceMac = (SiStringFromMac(currentMac));
}


////////////////////////////////////////////////////////////////////////////////





}
