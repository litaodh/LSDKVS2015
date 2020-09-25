// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

#include "Consumer.h"

#include <string>

namespace common {

/**
   Helper class finding and connecting to a single device.

   Example usage:

   SimpleConsumer consumer(producer.cti, "Local Area Connection", "MyRanger3");
   consumer.open();

   GenTL::IF_HANDLE interfaceHandle = consumer.openInterface();
   if (interfaceHadle == GENTL_INVALID_HANDLE)
   {
     // Interface could not be opened
   }

   GenTL::DEV_HANDLE deviceHandle = consumer.openDevice();
   if (deviceHadle == GENTL_INVALID_HANDLE)
   {
     // Device could not be opened
   }

   GenApi::CNodeMapRef deviceNodeMap = consumer.getDeviceNodeMap();
   GenTLApi* tl = consumer.tl();

   // Perform various GenTL operations using tl, interfaceHandle, deviceHandle
   // and deviceNodeMap

   GenTL::INFO_DATATYPE type;
   char displayName[1024];
   size_t size = sizeof(displayName);
   GenTL::GC_ERROR status =  tl->DevGetInfo(deviceHandle,
                                            GenTL::DEVICE_INFO_DISPLAYNAME,
                                            &type,
                                            displayName,
                                            &size);
   if (status != GenTL::GC_ERR_SUCCESS)
   {
     // Handle error
   }

   consumer.closeDevice();
   consumer.closeInterface();
   consumer.close();
*/
class __declspec(dllexport) SingleDeviceConsumer : public Consumer
{
public:
  /**
     \param ctiPath Path to producer CTI-file
     \param interfaceName e.g. Local Area Network 1
     \param deviceUserId The custom user id set, e.g., MyRanger3
  */
  SingleDeviceConsumer(std::string& ctiPath,
                       std::string& interfaceName,
                       std::string& deviceUserId);
  ~SingleDeviceConsumer();


  /** Opens the interface specified in class constructor */
  GenTL::IF_HANDLE openInterface();

  /** Closes the opened interface specified in class constructor */
  void closeInterface();

  /** Opens the device specified in class constructor */
  GenTL::DEV_HANDLE openDevice();

  /** Closes the opened device specified in class constructor */
  void closeDevice();

  /** Opens data stream with index 0 */
  GenTL::DS_HANDLE openDataStream();

  /** Closes data stream */
  void closeDataStream();

  /** Returns the device node map which is connected to GenTL */
  GenApi::CNodeMapRef getDeviceNodeMap();

private:
  std::string mInterfaceName;
  std::string mDeviceUserId;

  GenTL::IF_HANDLE mInterfaceHandle;
  GenTL::DEV_HANDLE mDeviceHandle;
  GenTL::DS_HANDLE mDataStreamHandle;
  std::unique_ptr<GenTLPort> mPort;
};
}
