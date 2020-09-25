// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

#include "TLI/GenTL.h"

#include <exception>
#include <map>
#include <memory>
#include <sstream>
#include <windows.h>

// clang-format off
#define API_LIST(code)\
  code(GCGetInfo)\
  code(GCGetLastError)\
  code(GCInitLib)\
  code(GCCloseLib)\
  code(GCReadPort)\
  code(GCWritePort)\
  code(GCGetPortURL)\
  code(GCGetPortInfo)\
  code(GCRegisterEvent)\
  code(GCUnregisterEvent)\
  code(EventGetData)\
  code(EventGetDataInfo)\
  code(EventGetInfo)\
  code(EventFlush)\
  code(EventKill)\
  code(TLOpen)\
  code(TLClose)\
  code(TLGetInfo)\
  code(TLGetNumInterfaces)\
  code(TLGetInterfaceID)\
  code(TLGetInterfaceInfo)\
  code(TLOpenInterface)\
  code(TLUpdateInterfaceList)\
  code(IFClose)\
  code(IFGetInfo)\
  code(IFGetNumDevices)\
  code(IFGetDeviceID)\
  code(IFUpdateDeviceList)\
  code(IFGetDeviceInfo)\
  code(IFOpenDevice)\
  code(DevGetPort)\
  code(DevGetNumDataStreams)\
  code(DevGetDataStreamID)\
  code(DevOpenDataStream)\
  code(DevGetInfo)\
  code(DevClose)\
  code(DSAnnounceBuffer)\
  code(DSAllocAndAnnounceBuffer)\
  code(DSFlushQueue)\
  code(DSStartAcquisition)\
  code(DSStopAcquisition)\
  code(DSGetInfo)\
  code(DSGetBufferID)\
  code(DSClose)\
  code(DSRevokeBuffer)\
  code(DSQueueBuffer)\
  code(DSGetBufferInfo)\
  code(GCGetNumPortURLs)\
  code(GCGetPortURLInfo)\
  code(GCReadPortStacked)\
  code(GCWritePortStacked)\
  code(DSGetBufferChunkData)\
  code(IFGetParentTL)\
  code(DevGetParentIF)\
  code(DSGetParentDev)\
  code(DSGetNumBufferParts)\
  code(DSGetBufferPartInfo)
// clang-format on

class __declspec(dllexport) GenTLApi
{
public:
  GenTLApi(HMODULE module);
  ~GenTLApi();

#define FUNC_PTR(func) GenTL::P##func func;
  API_LIST(FUNC_PTR)
#undef FUNC_PTR

private:
  HMODULE mModule;
};

struct LocalUrl
{
  std::string filename;
  uint64_t address;
  size_t length;
};

std::unique_ptr<GenTLApi> __declspec(dllexport) loadProducer(std::string ctiFile);

// Macro for checking if a call to GenTL succeeded
#define CC(tl, CALL)                                                           \
  if (GenTL::GC_ERR_SUCCESS != CALL)                                           \
  {                                                                            \
    char message[1024];                                                        \
    memset(message, 0, sizeof(message));                                       \
    size_t size = sizeof(message);                                             \
    GenTL::GC_ERROR errorCode;                                                 \
    tl->GCGetLastError(&errorCode, message, &size);                            \
    std::stringstream ss;                                                      \
    ss << "GenTL call failed: " << errorCode << ", Message: " << message;      \
    throw std::exception(ss.str().c_str());                                    \
  }
