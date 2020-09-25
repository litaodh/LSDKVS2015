// Copyright 2019 SICK AG. All rights reserved.

#pragma once

#include "GenIRanger.h"
#include "SampleUtils.h"

#include <ostream>

// ----------------------------------------------------------------------------
// Class declarations

namespace common {

typedef uint32_t PixelFormat;

class BufferInfo
{
public:
  BufferInfo(GenTLApi* tl,
             GenTL::DS_HANDLE dataStreamHandle,
             GenTL::BUFFER_HANDLE bufferHandle);
  void log(std::ostream& logFile) const;

public:
  bool8_t mBufferIncomplete;
  size_t mBufferFilledSize;
  size_t mBufferDataSize;
  uint64_t mBufferFrameID;
  size_t mBufferPayloadType;
  uint32_t mPartCount;
};

class BufferPartInfo
{
public:
  BufferPartInfo(GenTLApi* tl,
                 GenTL::DS_HANDLE dataStreamHandle,
                 GenTL::BUFFER_HANDLE bufferHandle,
                 uint32_t partNumber);
  void log(std::ostream& logFile) const;

public:
  uint32_t mPartNumber;
  uint8_t* mPartDataPointer;
  GenTL::PARTDATATYPE_ID mPartDataType;
  PixelFormat mPartDataFormat;
  uint64_t mRegionId;
  uint64_t mPurposeId;
  size_t mPartDataSize;
  size_t mPartDataWidth;
  size_t mPartDataHeight;
};

}
