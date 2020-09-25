// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

#include "GenTLApi.h"
#include "TLI/GenTL.h"

#include <cstdio>
#include <exception>
#include <iomanip>
#include <ios>

namespace common {

/**
   Implementation of the GenApi IPort which serves as the connection
   between GenApi and GenTL. All calls from GenApi to the connected
   device (module) is done through this port.
*/
class __declspec(dllexport) GenTLPort : public GenApi::IPort
{
public:
  GenTLPort(GenTL::PORT_HANDLE hPort, GenTLApi* tl)
    : mPort(hPort)
    , mTl(tl)
  {
  }

  virtual ~GenTLPort() {}

  virtual void Read(void* pBuffer, int64_t Address, int64_t Length)
  {
    size_t io_size = static_cast<size_t>(Length);
    auto status = mTl->GCReadPort(mPort, Address, pBuffer, &io_size);
    checkStatus(status, pBuffer, Address, io_size);
  }

  virtual void Write(const void* pBuffer, int64_t Address, int64_t Length)
  {
    size_t io_size = static_cast<size_t>(Length);
    auto status = mTl->GCWritePort(mPort, Address, pBuffer, &io_size);
    checkStatus(status, pBuffer, Address, io_size);
  }

  virtual GenApi::EAccessMode GetAccessMode() const { return GenApi::RW; }

  GenTL::PORT_HANDLE getHandle() { return mPort; }

private:
  GenTL::PORT_HANDLE mPort;
  GenTLApi* mTl;

  void checkStatus(GenTL::GC_ERROR status,
                   const void* pBuffer,
                   int64_t Address,
                   int64_t Length)
  {
    if (status != GenTL::GC_ERR_SUCCESS)
    {
      char message[1024];
      memset(message, 0, sizeof(message));
      size_t size = sizeof(message);
      GenTL::GC_ERROR errorCode;
      mTl->GCGetLastError(&errorCode, message, &size);
      std::ostringstream oss;
      const unsigned char* pCharBuffer =
        reinterpret_cast<const unsigned char*>(pBuffer);

      oss << message << " (" << errorCode << ") (" << Address << "@" << Length
          << "): ";
      for (int i = 0; i < Length; ++i)
      {
        oss << std::fixed << std::setw(2) << std::hex << pCharBuffer[i];
      }

      oss << std::endl;
      // TODO: Fix RNX-2620:
      throw std::runtime_error(message);
    }
  }
};

}
