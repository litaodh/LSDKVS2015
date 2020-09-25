/**	@file	BufferInfoVLB.h
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


#pragma once
#pragma warning(disable:4251)


#include "GenIRanger.h"
#include "SampleUtils.h"
#include <vector>



namespace common
{

typedef uint32_t PixelFormat;

// ----------------------------------------------------------------------------
// Class declarations

class BufferInfo
{
public:
	BufferInfo(	GenTLApi* tl,
				GenTL::DS_HANDLE dataStreamHandle,
				GenTL::BUFFER_HANDLE bufferHandle);

public:
	bool8_t		mBufferIncomplete;
	size_t		mBufferFilledSize;
	uint64_t	mBufferFrameID;
	uint8_t*	mDataPointer;
	uint64_t	mDataFormat;
	size_t		mBufferDataSize;
	size_t		mDataWidth; 
	size_t		mDataHeight; 
	size_t		mBufferPayloadType;
	uint32_t	mPartCount;

};

class BufferPartInfo
{
public:
	BufferPartInfo(	GenTLApi* tl,
					GenTL::DS_HANDLE dataStreamHandle,
					GenTL::BUFFER_HANDLE bufferHandle,
					uint32_t partNumber);

public:
	uint32_t	mPartNumber;
	uint8_t*	mPartDataPointer;
	size_t		mPartDataType;
	uint64_t	mPartDataFormat;
	uint64_t	mRegionID;
	uint64_t	mPurposeID;
	size_t		mPartDataSize;
	size_t		mPartDataWidth;
	size_t		mPartDataHeight;
	uint64_t	mRegionId;
	uint64_t	mPurposeId;

};

}

typedef common::BufferInfo		BufferInfo;
typedef common::BufferPartInfo	BufferPartInfo;







