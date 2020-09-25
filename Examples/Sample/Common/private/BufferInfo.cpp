// Copyright 2019 SICK AG. All rights reserved.

#include "BufferInfo.h"
#include "Consumer.h"

// From GenICam Pixel Format Naming Convention we get the
// pixel format values, imported from "PFNC.h" available emva.org
#define PFNC_Mono8 0x01080001 // Monochrome 8-bit
#define PFNC_Mono12p 0x010C0047 // Monochrome 12-bit packed
#define PFNC_Mono16 0x01100007 // Monochrome 16-bit
#define PFNC_Coord3D_C12p 0x010C00DA // 3D coordinate C 12-bit packed
#define PFNC_Coord3D_C16 0x011000B8 // 3D coordinate C 16-bit
#define PFNC_Mono4p 0x01040039 // Monochrome 4-bit packed

namespace common {

BufferInfo::BufferInfo(GenTLApi* tl,
                       GenTL::DS_HANDLE dataStreamHandle,
                       GenTL::BUFFER_HANDLE bufferHandle)
{
  GenTL::INFO_DATATYPE infoType = GenTL::INFO_DATATYPE_UNKNOWN;
  size_t infoSize = sizeof(mBufferIncomplete);
  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_IS_INCOMPLETE,
                         &infoType,
                         &mBufferIncomplete,
                         &infoSize));

  infoSize = sizeof(mBufferFilledSize);
  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_SIZE_FILLED,
                         &infoType,
                         &mBufferFilledSize,
                         &infoSize));

  infoSize = sizeof(mBufferDataSize);
  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_DATA_SIZE,
                         &infoType,
                         &mBufferDataSize,
                         &infoSize));

  infoSize = sizeof(mBufferFrameID);
  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_FRAMEID,
                         &infoType,
                         &mBufferFrameID,
                         &infoSize));

  infoSize = sizeof(mBufferPayloadType);
  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_PAYLOADTYPE,
                         &infoType,
                         &mBufferPayloadType,
                         &infoSize));

  CC(tl, tl->DSGetNumBufferParts(dataStreamHandle, bufferHandle, &mPartCount));
}

void BufferInfo::log(std::ostream& logFile) const
{
  logFile << "Frame ID: " << mBufferFrameID
          << ", Incomplete buffers: " << mBufferIncomplete
          << ", Buffer size filled: " << mBufferFilledSize
          << ", Buffer data size:" << mBufferDataSize << std::endl;
}

BufferPartInfo::BufferPartInfo(GenTLApi* tl,
                               GenTL::DS_HANDLE dataStreamHandle,
                               GenTL::BUFFER_HANDLE bufferHandle,
                               uint32_t partNumber)
  : mPartNumber(partNumber)
{
  GenTL::INFO_DATATYPE infoType = GenTL::INFO_DATATYPE_UNKNOWN;
  size_t infoSize = sizeof(mPartDataPointer);
  CC(tl,
     tl->DSGetBufferPartInfo(dataStreamHandle,
                             bufferHandle,
                             mPartNumber,
                             GenTL::BUFFER_PART_INFO_BASE,
                             &infoType,
                             &mPartDataPointer,
                             &infoSize));

  // Read to temporary variable of type size_t to avoid data type size problems
  size_t tmpDataType;
  infoSize = sizeof(tmpDataType);
  CC(tl,
     tl->DSGetBufferPartInfo(dataStreamHandle,
                             bufferHandle,
                             mPartNumber,
                             GenTL::BUFFER_PART_INFO_DATA_TYPE,
                             &infoType,
                             &tmpDataType,
                             &infoSize));
  mPartDataType = static_cast<GenTL::PARTDATATYPE_ID>(tmpDataType);

  uint64_t tmpDataFormat;
  infoSize = sizeof(tmpDataFormat);
  CC(tl,
     tl->DSGetBufferPartInfo(dataStreamHandle,
                             bufferHandle,
                             mPartNumber,
                             GenTL::BUFFER_PART_INFO_DATA_FORMAT,
                             &infoType,
                             &tmpDataFormat,
                             &infoSize));
  mPartDataFormat = static_cast<PixelFormat>(tmpDataFormat);

  infoSize = sizeof(mPartDataSize);
  CC(tl,
     tl->DSGetBufferPartInfo(dataStreamHandle,
                             bufferHandle,
                             mPartNumber,
                             GenTL::BUFFER_PART_INFO_DATA_SIZE,
                             &infoType,
                             &mPartDataSize,
                             &infoSize));

  infoSize = sizeof(mPartDataWidth);
  CC(tl,
     tl->DSGetBufferPartInfo(dataStreamHandle,
                             bufferHandle,
                             mPartNumber,
                             GenTL::BUFFER_PART_INFO_WIDTH,
                             &infoType,
                             &mPartDataWidth,
                             &infoSize));

  infoSize = sizeof(mPartDataHeight);
  CC(tl,
     tl->DSGetBufferPartInfo(dataStreamHandle,
                             bufferHandle,
                             mPartNumber,
                             GenTL::BUFFER_PART_INFO_HEIGHT,
                             &infoType,
                             &mPartDataHeight,
                             &infoSize));

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

void BufferPartInfo::log(std::ostream& logFile) const
{
  logFile << "Part number: " << mPartNumber << ", size: " << mPartDataSize;
  logFile << ", Height: " << mPartDataHeight << ", Width: " << mPartDataWidth;
  logFile << std::endl;

  // Log the data format in the buffer part
  switch (mPartDataType)
  {
  case GenTL::PART_DATATYPE_2D_IMAGE:
    logFile << " Type: PART_DATATYPE_2D_IMAGE, ";
    break;
  case GenTL::PART_DATATYPE_3D_IMAGE:
    logFile << " Type: PART_DATATYPE_3D_IMAGE, ";
    break;
  default: logFile << " Unknown Part " << mPartDataType; break;
  }

  // Log the pixel format in the buffer part
  switch (mPartDataFormat)
  {
  case PFNC_Mono8:
    // Reflectance expected as Mono8
    logFile << " Format: DATA_FORMAT_MONO_8 ";
    break;
  case PFNC_Coord3D_C16:
    // Range data 16 bits either Coord_C or Mono
    logFile << " Format: DATA_FORMAT_COORD_C16 ";
    break;
  case PFNC_Mono16: logFile << " Format: DATA_FORMAT_MONO_16 "; break;
  case PFNC_Coord3D_C12p:
    // Range data 12 bits either Coord_C or Mono
    logFile << " Format: DATA_FORMAT_COORD_C12p ";
    break;
  case PFNC_Mono12p: logFile << " Format: DATA_FORMAT_MONO_12p "; break;
  default: logFile << " Unknown Format " << mPartDataFormat; break;
  }
  logFile << std::endl;
}

}
