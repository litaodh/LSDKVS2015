/**	@file	BufferInfoVLB.cpp
*
*	@brief	Class of output buffer.
*
*	@attention
*	Modified from BufferInfo.h
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/



#include "BufferInfoVLB.h"


// From GenICam Pixel Format Naming Convention we get the
// pixel format values, imported from "PFNC.h" available emva.org
#define PFNC_Mono8			0x01080001			// Monochrome 8-bit
#define PFNC_Mono12p		0x010C0047			// Monochrome 12-bit packed
#define PFNC_Mono16			0x01100007			// Monochrome 16-bit
#define PFNC_Coord3D_C12p	0x010C00DA			// 3D coordinate C 12-bit packed
#define PFNC_Coord3D_C16	0x011000B8			// 3D coordinate C 16-bit
#define PFNC_Mono4p			0x01040039			// Monochrome 4-bit packed


// ----------------------------------------------------------------------------
// BufferInfo function definitions


BufferInfo::BufferInfo(	GenTLApi* tl,
						GenTL::DS_HANDLE dataStreamHandle,
						GenTL::BUFFER_HANDLE bufferHandle)
{
	GenTL::INFO_DATATYPE infoType = GenTL::INFO_DATATYPE_UNKNOWN;
	size_t infoSize = sizeof(mBufferIncomplete);
	CC(tl, tl->DSGetBufferInfo(dataStreamHandle, bufferHandle, GenTL::BUFFER_INFO_IS_INCOMPLETE, &infoType, &mBufferIncomplete, &infoSize));

	infoSize = sizeof(mBufferFilledSize);
	CC(tl, tl->DSGetBufferInfo(dataStreamHandle, bufferHandle, GenTL::BUFFER_INFO_SIZE_FILLED, &infoType, &mBufferFilledSize, &infoSize));

	infoSize = sizeof(mBufferFrameID);
	CC(tl, tl->DSGetBufferInfo(dataStreamHandle, bufferHandle, GenTL::BUFFER_INFO_FRAMEID, &infoType, &mBufferFrameID, &infoSize));

	// add 
	infoSize = static_cast<size_t>(sizeof(mDataPointer));
	CC(tl, tl->DSGetBufferInfo(dataStreamHandle, bufferHandle, GenTL::BUFFER_INFO_BASE, &infoType, &mDataPointer, &infoSize));

	infoSize = sizeof(mBufferDataSize);
	CC(tl, tl->DSGetBufferInfo(dataStreamHandle, bufferHandle, GenTL::BUFFER_INFO_DATA_SIZE, &infoType, &mBufferDataSize, &infoSize));

	//infoSize = static_cast<size_t>(sizeof(mDataWidth));
	//CC(tl, tl->DSGetBufferInfo(dataStreamHandle, bufferHandle, GenTL::BUFFER_INFO_WIDTH, &infoType, &mDataWidth, &infoSize));
	//
	//infoSize = static_cast<size_t>(sizeof(mDataHeight));
	//CC(tl, tl->DSGetBufferInfo(dataStreamHandle, bufferHandle, GenTL::BUFFER_INFO_HEIGHT, &infoType, &mDataHeight, &infoSize));

	infoSize = sizeof(mBufferPayloadType);
	CC(tl,tl->DSGetBufferInfo(dataStreamHandle,bufferHandle,GenTL::BUFFER_INFO_PAYLOADTYPE,&infoType,&mBufferPayloadType,&infoSize));

	if (mBufferPayloadType == GenTL::PAYLOAD_TYPE_MULTI_PART)
	{
		CC(tl, tl->DSGetNumBufferParts(dataStreamHandle, bufferHandle, &mPartCount));
	}
	else
	{
		mPartCount = 0;
		
		infoSize = static_cast<size_t>(sizeof(mDataFormat));
		CC(tl, tl->DSGetBufferInfo(dataStreamHandle, bufferHandle, GenTL::BUFFER_INFO_PIXELFORMAT, &infoType, &mDataFormat, &infoSize));
	}
}


// ----------------------------------------------------------------------------
// BufferPartInfo function definitions

BufferPartInfo::BufferPartInfo(	GenTLApi* tl,
								GenTL::DS_HANDLE dataStreamHandle,
								GenTL::BUFFER_HANDLE bufferHandle,
								uint32_t partNumber)
	: mPartNumber(partNumber)
{
	GenTL::INFO_DATATYPE infoType = GenTL::INFO_DATATYPE_UNKNOWN;
	size_t infoSize = static_cast<size_t>(sizeof(mPartDataPointer));
	CC(tl, tl->DSGetBufferPartInfo(dataStreamHandle, bufferHandle, mPartNumber,
		GenTL::BUFFER_PART_INFO_BASE,
		&infoType, &mPartDataPointer, &infoSize));

	infoSize = static_cast<size_t>(sizeof(mPartDataType));
	CC(tl, tl->DSGetBufferPartInfo(dataStreamHandle, bufferHandle, mPartNumber,
		GenTL::BUFFER_PART_INFO_DATA_TYPE,
		&infoType, &mPartDataType, &infoSize));

	infoSize = static_cast<size_t>(sizeof(mPartDataFormat));
	CC(tl, tl->DSGetBufferPartInfo(dataStreamHandle, bufferHandle, mPartNumber,
		GenTL::BUFFER_PART_INFO_DATA_FORMAT,
		&infoType, &mPartDataFormat, &infoSize));

	infoSize = static_cast<size_t>(sizeof(mPartDataSize));
	CC(tl, tl->DSGetBufferPartInfo(dataStreamHandle, bufferHandle, mPartNumber,
		GenTL::BUFFER_PART_INFO_DATA_SIZE,
		&infoType, &mPartDataSize, &infoSize));

	infoSize = static_cast<size_t>(sizeof(mPartDataWidth));
	CC(tl, tl->DSGetBufferPartInfo(dataStreamHandle, bufferHandle, mPartNumber,
		GenTL::BUFFER_PART_INFO_WIDTH,
		&infoType, &mPartDataWidth, &infoSize));

	infoSize = static_cast<size_t>(sizeof(mPartDataHeight));
	CC(tl, tl->DSGetBufferPartInfo(dataStreamHandle, bufferHandle, mPartNumber,
		GenTL::BUFFER_PART_INFO_HEIGHT,
		&infoType, &mPartDataHeight, &infoSize));

	infoSize = static_cast<size_t>(sizeof(mRegionID));
	CC(tl, tl->DSGetBufferPartInfo(dataStreamHandle, bufferHandle, mPartNumber,
		GenIRanger::BUFFER_PART_CUSTOM_INFO_CMD_LIST::
		BUFFER_PART_CUSTOM_INFO_REGION_ID,
		&infoType, &mRegionID, &infoSize));

	infoSize = static_cast<size_t>(sizeof(mPurposeID));
	CC(tl, tl->DSGetBufferPartInfo(dataStreamHandle, bufferHandle, mPartNumber,
		GenIRanger::BUFFER_PART_CUSTOM_INFO_CMD_LIST::
		BUFFER_PART_CUSTOM_INFO_DATA_PURPOSE_ID,
		&infoType, &mPurposeID, &infoSize));

	infoSize = sizeof(mRegionId);
	CC(tl,
		tl->DSGetBufferPartInfo(dataStreamHandle,
			bufferHandle,
			mPartNumber,
			GenIRanger::BUFFER_PART_CUSTOM_INFO_CMD_LIST::
			BUFFER_PART_CUSTOM_INFO_REGION_ID,
			&infoType,
			&mRegionId,
			&infoSize));

	infoSize = sizeof(mPurposeId);
	CC(tl,
		tl->DSGetBufferPartInfo(dataStreamHandle,
			bufferHandle,
			mPartNumber,
			GenIRanger::BUFFER_PART_CUSTOM_INFO_CMD_LIST::
			BUFFER_PART_CUSTOM_INFO_DATA_PURPOSE_ID,
			&infoType,
			&mPurposeId,
			&infoSize));

}


