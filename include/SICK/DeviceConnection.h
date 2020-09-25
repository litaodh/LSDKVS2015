/**	@file	DeviceConnection.h
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


#pragma once


#include "Typedef.h"
#include "Consumer.h"
#include "GenIRanger.h"
#include "SampleUtils.h"

#include <string>
#include <memory>
#include <set>
#include <map>


#ifdef __cplusplus
namespace SickCam {
	extern "C" {
#endif


enum CAM_STATUS
{
	All_OK							= 0,
	CAM_IS_CONNECTED				,
	CAM_IS_STARTED					,
	CAM_IS_STOPPED					,
	CAM_IS_DISCONNECTED				,
	ERROR_OPEN_CAM					,
	ERROR_SCAN_EMPTY_DEVICE			,
	ERROR_CTI_NOT_FOUND				,
	ERROR_OPEN_CONSUMER				,
	ERROR_OPEN_TL_HANDLE			,
	ERROR_EMPTY_IF_ID				,
	ERROR_EMPTY_IF_HANDLE			,
	ERROR_CSV_PATH					,
	ERROR_XML_PATH					,
	ERROR_EMPTY_PATH				,
	ERROR_EMPTY_IMG					,
	ERROR_SET_CAM					,
	ERROR_CLO_CAM					,				///< close camera error
	ERROR_GET_IMG					,				///< range image
	ERROR_GET_SEN_IMG				,				///< sensor image
	ERROR_RST_CAM					,				///< restart camera error
	ERROR_UPDATE_PARA				,				///< update parameters error
	ERROR_STOP_ACQUISITION			,
	ERROR_START_ACQUISITION			,
	ERROR_PARAMETER_FORMAT_DISMATCH	,
	ERROR_PARAMETER_VALUE_DISMATCH	,
	ERROR_PARAMETER_READ_ONLY		,
	ERROR_PARAMETER_INVALID			,
	ERROR_PARAMETER_VALUE_INVALID	,
	ERROR_PARAMETERS_EMPTY			,
	ERROR_SCAN_PARAMETERS			,
	ERROR_COMMUNICATION				,
	ERROR_SET_IP					,
	ERROR_SET_SUBNET				,
	ERROR_NULL_PTR_R3				,				///< initial value if m_status.
	ERROR_NULL_DEV_HANDLE			,
	ERROR_NULL_DS_HANDLE			,
	ERROR_OPEN_DATASTREAM			,
	ERROR_CAM_NOT_CONNECT			,
	ERROR_CAM_NOT_START				, 
	ERROR_CAM_NOT_STOP				,
	ERROR_CAM_NOT_DISCONNECT		,
	ERROR_CAM_IS_CONNECTED			,
	ERROR_CAM_IS_STARTED			,
	ERROR_CAM_IS_STOPPED			,
	ERROR_CAM_IS_DISCONNECTED		,
	ERROR_IMAGE_NOT_READY			,
	TIME_OUT						,
	UNKNOWN							,
	DEFAULT
};



////////////////////////////////////////////////////////////////////////////////


/** @brief DeviceConnection															  
*	
*	Midifed based on the code in MultiCamera.cpp;
*	Called by Ranger3Shared.
*/
class DeviceConnection
{
public:
	DeviceConnection				(
		ConsumerPtr consumer, GenTL::IF_HANDLE interfaceHandle,
		SiDeviceList devices, int64_t id);

	~DeviceConnection				();

	bool openDevice					(SiDeviceList devices);
	bool openDevice					();
	bool closeDevice				();

	bool openDataStream				();
	bool closeDataStream			();

	/** Allocates, announces and queues a number of buffers to the data stream */
	void initializeBuffers			(size_t buffersCount, size_t payloadSize);
	void teardownBuffers			();

	void registerNewBufferEvent		();
	void unregisterNewBufferEvent	();

	void startAcquisition			();
	void stopAcquisition			();
	bool isAcquisitionRunning		()		const;

	void createDeviceNodeMap		(SiConsumer& consumer);
	void createDataStreamNodeMap	(SiConsumer& consumer);

	
	VecBufferHandle		&		getBufferHandles	()	{	return mBufferHandles;	}
	Vec<uint8_t*>		&		getBufferData		()	{	return mBufferData;		}

	void updateIp					();
	void updateSubnet				();
	Str getIp						() const { return mDeviceIP; };
	Str getSubnet					() const { return mDeviceSubnet; };
	Str getMac						() const { return mDeviceMac; };

protected:
	void _queryIP();
	void _querySubnet();
	void _queryMAC();

private:
	bool						mIsOpenDevice;
	bool						mIsOpenDataStream;
	bool						mAcquisitionRunning;
	Str							mDeviceIP;
	Str							mDeviceSubnet;
	Str							mDeviceMac;
	SiDeviceList				mDeviceList;
	ConsumerPtr					mpConsumer;
	Vec<GenTL::BUFFER_HANDLE>	mBufferHandles;
	Vec<uint8_t*>				mBufferData;			
	UPtr<SiGenTLPort>			mDevicePort;
	UPtr<SiGenTLPort>			mDataStreamPort;
	GenApi::CIntegerPtr			mDeviceSelector;

public:
	int64_t						mId;					//! < id of current device, marked in Ranger3Shard::scan(), will be used in queryIP(). 
	Str							mDeviceName;
	SPtr<SiGenTLPort>			mInterfacePort;
	GenTL::IF_HANDLE			mInterfaceHandle;
	GenTL::DEV_HANDLE			mDeviceHandle;
	GenTL::DS_HANDLE			mDataStreamHandle;
	GenTL::EVENT_HANDLE			mNewBufferEventHandle;
	GenApi::CNodeMapRef			mDeviceNodeMap;			//! < Node map for reading/setting device parameters
	GenApi::CNodeMapRef			mDataStreamNodeMap;		//! < Node map for data stream module to boost receiver thread priority
	GenApi::CNodeMapRef			mInterfaceNodeMap;

};
typedef std::map<Str, SPtr<DeviceConnection>>	deviceList;
typedef SPtr<DeviceConnection>					DCPtr;



#ifdef __cplusplus
	};

};
#endif