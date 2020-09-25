// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

#include "GenICam.h"
#include "GenTLApi.h"
#include "GenTLPort.h"

#include <exception>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace common {
// Simplified macro for checking if a the return value of a GenTL call succeeded
#define CR(CALL) CC(tl(), CALL);

typedef std::string DeviceId;
typedef std::string InterfaceId;
typedef std::vector<std::pair<InterfaceId, std::string>> InterfaceList;
typedef std::vector<std::pair<DeviceId, std::string>> DeviceList;

/**
   Helper class finding and connecting to devices.
*/
class __declspec(dllexport) Consumer
{
public:
  /** \param ctiPath Path to producer CTI-file */
  Consumer(std::string& ctiPath);
  ~Consumer();

  /** Init producer and transport layer */
  GenTL::TL_HANDLE open();

  /** Close producer and transport layer*/
  void close();

  /** Opens an interface

      \param interfaceName User friendly name of interface,
                           e.g., Local Area Connection 1
  */
  GenTL::IF_HANDLE openInterfaceByName(std::string& interfaceName);

  /** Opens an interface

      \param interfaceId Id given by the producer
  */
  GenTL::IF_HANDLE openInterfaceById(InterfaceId& interfaceId);

  /** Closes an interface

      \param interfaceHandle Handle to interface returned from
                             #openInterfaceByName or #openInterfaceById
  */
  void closeInterface(GenTL::IF_HANDLE interfaceHandle);

  /** Opens device on the associated interface

      \param interfaceHandle Handle to interface returned from
                             #openInterfaceByName or #openInterfaceById
      \param deviceName User defined name for the device to open
  */
  GenTL::DEV_HANDLE openDeviceByName(GenTL::IF_HANDLE interfaceHandle,
                                     std::string& deviceName);

  /** Opens device on the associated interface

      \param interfaceHandle Handle to interface returned from
                             #openInterfaceByName or #openInterfaceById
      \param deviceId Unique id for this device given by the producer.
                      Use #findDeviceByName or #findDeviceById
  */
  GenTL::DEV_HANDLE openDeviceById(GenTL::IF_HANDLE interfaceHandle,
                                   DeviceId& deviceId);

  /** Closes an opened device

      \param deviceHandle Handle to device returned from #openDeviceByName
                          or #openDeviceByIndex
  */
  void closeDevice(GenTL::DEV_HANDLE deviceHandle);

  /** Opens data stream with index 0 and increases the receiver thread priority.

      \param deviceHandle Handle to device returned from #openDeviceByName
                          or #openDeviceByIndex
  */
  GenTL::DS_HANDLE openDataStream(GenTL::DEV_HANDLE deviceHandle);

  /** Close data stream

      \param deviceHandle Handle to device returned from #openDataStream
  */
  void closeDataStream(GenTL::DS_HANDLE dataStreamHandle);

  /** Returns the node map from the associated port. Uses the default
      port name "Device".

      \param port SimplePort created with one of the following GenTL handles:
                  TL_HANDLE, IF_HANDLE, DEV_HANDLE, DS_HANDLE, and BUFFER_HANDLE
  */
  GenApi::CNodeMapRef getNodeMap(GenTLPort* port);

  /** Returns the node map from the associated port.

      \param port SimplePort created with one of the following GenTL handles:
                  TL_HANDLE, IF_HANDLE, DEV_HANDLE, DS_HANDLE, and BUFFER_HANDLE
      \param portName Port name, e.g. "Device" or "StreamPort"
  */
  GenApi::CNodeMapRef getNodeMap(GenTLPort* port, const std::string& portName);

  /** Returns the last error from a GenTL call */
  std::string getLastError();

  /** Returns a list of available interfaces. */
  InterfaceList getInterfaces(GenTL::TL_HANDLE hTL);

  /** Returns the id of interface by using its user friendly name,
      e.g., Local Area Connection 1.

      \param interfaces Interface list from #getInterfaces
      \param name User friendly name of interface, e.g.,
                  "Local Area Connection 1"
  */
  InterfaceId findInterfaceByName(InterfaceList interfaces, std::string& name);

  /** Returns the id of interface by using the its enumeration from
      the producer

      \param interfaces Interface list from #getInterfaces
      \param index Index in the list produced by #getInterfaces
  */
  InterfaceId findInterfaceByIndex(InterfaceList interfaces, uint32_t index);

  /** Returns the associated name of the interface id */
  std::string getInterfaceName(InterfaceList iterfaces, InterfaceId& id);

  /** Returns the unique device ID by using its user defined name.
      GENTL_INVALID_HANDLE is returned if the interface cannot be
      found.

      \param devices Device list returned from #getDevices
      \param deviceName User defined name of device
  */
  DeviceId findDeviceByName(DeviceList devices, std::string& deviceName);

  /** Returns the unique device ID by using its enumeration from the
      producer. Empty string is returned if device cannot be found.

      \param devices Device list returned from #getDevices
      \param index Index in the list produced by #getDevices
  */
  DeviceId findDeviceByIndex(DeviceList devices, uint32_t index);

  /** Returns a list of all devices found on passed interface */
  DeviceList getDevices(GenTL::IF_HANDLE ifHandle);

  /** Returns a struct containing the XML name, address and length */
  LocalUrl getNodeMapUrl(GenTL::PORT_HANDLE hRemote);

  /** Returns true of the node map XML is zipped */
  bool isDeviceXmlZipped(GenTL::PORT_HANDLE hRemote);

  /** Reads the node map XML into buffer. This method should only be
      used if #isDeviceXmlZipped returns true.

      \param hRemote Device handle
      \param url LocalUrl object containing filename, address, length of XML
      \param buffer Allocated buffer to store XML in, size should match length
                    member of the LocalUrl
  */
  void
  getNodeMapXML(GenTL::PORT_HANDLE hRemote, LocalUrl& url, uint8_t* buffer);

  /** Returns the node map XML as a string.

      This method should only be used if #isDeviceXmlZipped returns false.
  */
  std::string getNodeMapXML(GenTL::PORT_HANDLE hRemote);

  /** Direct access to GenTL methods.

      Use handles from #init, #openInterfaceByName, #openDeviceByName
  */
  GenTLApi* tl();

  /** Returns the tl handle */
  GenTL::TL_HANDLE tlHandle();

private:
  void increaseThreadPriority(GenTL::DS_HANDLE dataStreamHandle);

private:
  std::string mCtiPath;
  GenTL::TL_HANDLE mTlHandle;

  std::unique_ptr<GenTLApi> mTl;
  bool mOpened;
};

}
