// Copyright 2016-2019 SICK AG. All rights reserved.

#include "Consumer.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

LocalUrl parseLocalUrl(const std::string& url)
{
  size_t pos;
  pos = url.find(':');
  std::string without_prefix = url.substr(pos + 1);

  std::vector<std::string> tokens;
  std::stringstream ss(without_prefix);
  std::string token;
  while (getline(ss, token, ';'))
  {
    tokens.push_back(token);
  }

  char *endptr_addr, *endptr_len;
  LocalUrl localUrl;
  localUrl.filename = tokens[0];
  localUrl.address = _strtoui64(tokens[1].c_str(), &endptr_addr, 16);
  localUrl.length =
    static_cast<size_t>(_strtoui64(tokens[2].c_str(), &endptr_len, 16));
  return localUrl;
}

namespace common {

Consumer::Consumer(std::string& ctiPath)
  : mCtiPath(ctiPath)
  , mTlHandle(0)
  , mOpened(false)
{
}

Consumer::~Consumer()
{
  if (mOpened)
  {
    close();
  }
}

GenTL::TL_HANDLE Consumer::open()
{
  mTl = loadProducer(mCtiPath);
  CR(mTl->GCInitLib());
  CR(mTl->TLOpen(&mTlHandle));
  mOpened = true;
  return mTlHandle;
}

void Consumer::close()
{
  CR(mTl->TLClose(mTlHandle));
  CR(mTl->GCCloseLib());
  mOpened = false;
}

GenTL::IF_HANDLE Consumer::openInterfaceByName(std::string& interfaceName)
{
  InterfaceList interfaces = getInterfaces(mTlHandle);
  InterfaceId interfaceId = findInterfaceByName(interfaces, interfaceName);

  if (interfaceId.empty())
  {
    std::cout << "Cannot find interface with name " << interfaceName
              << std::endl;
    return GENTL_INVALID_HANDLE;
  }

  GenTL::IF_HANDLE interfaceHandle;
  mTl->TLOpenInterface(mTlHandle, interfaceId.c_str(), &interfaceHandle);

  if (interfaceHandle != GENTL_INVALID_HANDLE)
  {
    std::cout << "Using network adapter: " << interfaceName << std::endl
              << std::endl;
  }
  else
  {
    std::cout << "Interface " << interfaceName << " cannot be opened."
              << std::endl;
  }
  return interfaceHandle;
}

GenTL::IF_HANDLE Consumer::openInterfaceById(InterfaceId& interfaceId)
{
  if (interfaceId.empty())
  {
    std::cout << "Cannot find interface with id " << interfaceId << std::endl;
    return GENTL_INVALID_HANDLE;
  }

  GenTL::IF_HANDLE interfaceHandle;
  mTl->TLOpenInterface(mTlHandle, interfaceId.c_str(), &interfaceHandle);

  if (interfaceHandle == GENTL_INVALID_HANDLE)
  {
    std::cout << "Interface " << interfaceId << " cannot be opened."
              << std::endl;
  }
  return interfaceHandle;
}

void Consumer::closeInterface(GenTL::IF_HANDLE interfaceHandle)
{
  CR(mTl->IFClose(interfaceHandle));
}

GenTL::DEV_HANDLE Consumer::openDeviceByName(GenTL::IF_HANDLE interfaceHandle,
                                             std::string& deviceName)
{
  // Find device
  DeviceList devices = getDevices(interfaceHandle);
  DeviceId deviceId = findDeviceByName(devices, deviceName);

  if (deviceId.empty())
  {
    std::cout << "Device " << deviceName << " cannot be found." << std::endl;
    return GENTL_INVALID_HANDLE;
  }

  GenTL::DEVICE_ACCESS_FLAGS deviceAccess = GenTL::DEVICE_ACCESS_CONTROL;
  GenTL::DEV_HANDLE deviceHandle;
  CR(mTl->IFOpenDevice(
    interfaceHandle, deviceId.c_str(), deviceAccess, &deviceHandle));
  if (deviceHandle == GENTL_INVALID_HANDLE)
  {
    std::cout << "Could not open device. " << getLastError() << std::endl;
    return GENTL_INVALID_HANDLE;
  }
  else
  {
    std::cout << "Connecting to device " << deviceName << std::endl
              << std::endl;
  }

  return deviceHandle;
}

GenTL::DEV_HANDLE Consumer::openDeviceById(GenTL::IF_HANDLE interfaceHandle,
                                           DeviceId& deviceId)
{
  if (deviceId.empty())
  {
    std::cout << "Device id string was empty." << std::endl;
    return GENTL_INVALID_HANDLE;
  }

  GenTL::DEVICE_ACCESS_FLAGS deviceAccess = GenTL::DEVICE_ACCESS_CONTROL;
  GenTL::DEV_HANDLE deviceHandle;
  GenTL::GC_ERROR status = mTl->IFOpenDevice(
    interfaceHandle, deviceId.c_str(), deviceAccess, &deviceHandle);
  if (deviceHandle == GENTL_INVALID_HANDLE || status != GenTL::GC_ERR_SUCCESS)
  {
    std::cout << "Could not open device. " << getLastError() << std::endl;
    return GENTL_INVALID_HANDLE;
  }
  else
  {
    std::cout << "Connecting to device " << deviceId << std::endl << std::endl;
  }

  return deviceHandle;
}


void Consumer::closeDevice(GenTL::DEV_HANDLE deviceHandle)
{
  CR(mTl->DevClose(deviceHandle));
}

GenTL::DS_HANDLE Consumer::openDataStream(GenTL::DEV_HANDLE deviceHandle)
{
  char streamId[1024];
  size_t idSize = sizeof(streamId);
  GenTL::DS_HANDLE dataStreamHandle;
  CR(mTl->DevGetDataStreamID(deviceHandle, 0, streamId, &idSize));
  CR(mTl->DevOpenDataStream(deviceHandle, streamId, &dataStreamHandle));

  increaseThreadPriority(dataStreamHandle);

  return dataStreamHandle;
}

void Consumer::closeDataStream(GenTL::DS_HANDLE dataStreamHandle)
{
  CR(mTl->DSClose(dataStreamHandle));
}

GenApi::CNodeMapRef Consumer::getNodeMap(GenTLPort* port)
{
  return getNodeMap(port, "Device");
}

/** Version with explicit node map port name */
GenApi::CNodeMapRef Consumer::getNodeMap(GenTLPort* port,
                                         const std::string& portName)
{
  GenApi::CNodeMapRef nodeMap;
  GenTL::PORT_HANDLE handle = port->getHandle();
  if (isDeviceXmlZipped(handle))
  {
    LocalUrl url = getNodeMapUrl(handle);
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[url.length]);
    getNodeMapXML(handle, url, buffer.get());
    nodeMap._LoadXMLFromZIPData(buffer.get(), url.length);
  }
  else
  {
    std::string deviceXML = getNodeMapXML(handle);
    nodeMap._LoadXMLFromString(deviceXML.c_str());
  }

  nodeMap._Connect(port, portName.c_str());
  return nodeMap;
}

GenTLApi* Consumer::tl()
{
  return mTl.get();
}

GenTL::TL_HANDLE Consumer::tlHandle()
{
  return mTlHandle;
}

void Consumer::increaseThreadPriority(GenTL::DS_HANDLE dataStreamHandle)
{
  common::GenTLPort dataStreamPort = common::GenTLPort(dataStreamHandle, tl());
  GenApi::CNodeMapRef dataStreamNodeMap =
    getNodeMap(&dataStreamPort, "StreamPort");

  GenApi::CIntegerPtr threadPriority =
    dataStreamNodeMap._GetNode("StreamThreadPriority");
  GenApi::CCommandPtr setStreamPrio =
    dataStreamNodeMap._GetNode("StreamThreadApplyPriority");
  if (threadPriority.IsValid())
  {
    // Default priority is "HIGHEST" = 2, set to TIME CRITICAL = 15
    threadPriority->SetValue(15);
  }
  else
  {
    std::cerr << "WARNING: Could not set thread priority!" << std::endl;
  }
  if (setStreamPrio.IsValid())
  {
    setStreamPrio->Execute();
  }
  else
  {
    std::cerr << "WARNING: Could not set thread priority!" << std::endl;
  }
}

std::string Consumer::getLastError()
{
  char err_msg[1024];
  memset(err_msg, 0, sizeof(err_msg));
  size_t err_size = sizeof(err_msg);
  GenTL::GC_ERROR err_code;
  mTl->GCGetLastError(&err_code, err_msg, &err_size);
  std::stringstream message;
  message << "Last error code: " << err_code << ", Message: " << err_msg;
  return message.str();
}

InterfaceList Consumer::getInterfaces(GenTL::TL_HANDLE hTL)
{
  bool8_t changed = false;
  // Timeout in milliseconds
  uint64_t timeout = 100;
  CR(mTl->TLUpdateInterfaceList(hTL, &changed, timeout));

  uint32_t numCandidates = 0;
  CR(mTl->TLGetNumInterfaces(hTL, &numCandidates));

  InterfaceList interfaceList;

  for (uint32_t i = 0; i < numCandidates; ++i)
  {
    char interfaceId[1024];
    size_t interfaceIdSize = sizeof(interfaceId);
    CR(mTl->TLGetInterfaceID(hTL, i, interfaceId, &interfaceIdSize));

    GenTL::INFO_DATATYPE dataType;
    char displayName[1024];
    size_t displayNameSize = sizeof(displayName);
    CR(mTl->TLGetInterfaceInfo(hTL,
                               interfaceId,
                               GenTL::INTERFACE_INFO_DISPLAYNAME,
                               &dataType,
                               displayName,
                               &displayNameSize));

    interfaceList.push_back(std::make_pair(interfaceId, displayName));
  }

  return interfaceList;
}

InterfaceId Consumer::findInterfaceByIndex(InterfaceList interfaces,
                                           uint32_t index)
{
  if (index > interfaces.size())
  {
    std::cout << "Cannot select interface with index " << index << std::endl;
    return InterfaceId();
  }

  auto it = interfaces.begin();
  std::advance(it, index);

  return it->first;
}

InterfaceId Consumer::findInterfaceByName(InterfaceList interfaces,
                                          std::string& name)
{
  for (auto it = interfaces.begin(); it != interfaces.end(); ++it)
  {
    if (it->second.find(name) != std::string::npos)
    {
      return it->first;
    }
  }
  return InterfaceId();
}

std::string Consumer::getInterfaceName(InterfaceList interfaces,
                                       InterfaceId& id)
{
  for (auto it = interfaces.begin(); it != interfaces.end(); ++it)
  {
    if (it->first.find(id) != std::string::npos)
    {
      return it->second;
    }
  }
  return std::string();
}

DeviceList Consumer::getDevices(GenTL::IF_HANDLE ifHandle)
{
  DeviceList devices;
  bool8_t changed = false;
  CR(mTl->IFUpdateDeviceList(ifHandle, &changed, 500));
  uint32_t numDevices;
  CR(mTl->IFGetNumDevices(ifHandle, &numDevices));

  for (size_t i = 0; i < numDevices; ++i)
  {
    char deviceId[1024];
    size_t deviceIdSize = sizeof(deviceId);

    CR(mTl->IFGetDeviceID(
      ifHandle, static_cast<uint32_t>(i), deviceId, &deviceIdSize));

    GenTL::INFO_DATATYPE dataType;
    char modelName[1024];
    size_t modelNameSize = sizeof(modelName);
    CR(mTl->IFGetDeviceInfo(ifHandle,
                            deviceId,
                            GenTL::DEVICE_INFO_MODEL,
                            &dataType,
                            modelName,
                            &modelNameSize));

    // Append user defined name if possible
    char deviceUserId[1024];
    size_t deviceUserIdSize = sizeof(deviceUserId);
    GenTL::GC_ERROR status =
      mTl->IFGetDeviceInfo(ifHandle,
                           deviceId,
                           GenTL::DEVICE_INFO_USER_DEFINED_NAME,
                           &dataType,
                           deviceUserId,
                           &deviceUserIdSize);

    std::stringstream deviceName;
    deviceName << modelName;
    if (status == GenTL::GC_ERR_SUCCESS)
    {
      deviceName << ": " << deviceUserId;
    }

    devices.push_back(std::make_pair(deviceId, deviceName.str()));
  }
  return devices;
}

DeviceId Consumer::findDeviceByName(DeviceList devices, std::string& deviceName)
{
  for (auto it = devices.begin(); it != devices.end(); ++it)
  {
    if (it->second.find(deviceName) != std::string::npos)
    {
      return it->first;
    }
  }
  return DeviceId();
}


DeviceId Consumer::findDeviceByIndex(DeviceList devices, uint32_t index)
{
  if (index >= devices.size())
  {
    std::cout << "Cannot select device with index " << index << std::endl;
    return DeviceId();
  }

  auto it = devices.begin();
  std::advance(it, index);

  return it->first;
}

LocalUrl Consumer::getNodeMapUrl(GenTL::PORT_HANDLE hRemote)
{
  GenTL::INFO_DATATYPE dataType;
  char url[1024];
  size_t urlSize = sizeof(url);
  CR(mTl->GCGetPortURLInfo(
    hRemote, 0, GenTL::URL_INFO_URL, &dataType, &url, &urlSize));
  if (dataType != GenTL::INFO_DATATYPE_STRING)
  {
    std::cout << "Could not get URL_INFO_URL" << std::endl;
  }

  return parseLocalUrl(url);
}

bool Consumer::isDeviceXmlZipped(GenTL::PORT_HANDLE hRemote)
{
  auto url = getNodeMapUrl(hRemote);
  url.filename.find(".zip");
  return url.filename.find(".zip") != std::string::npos;
}

void Consumer::getNodeMapXML(GenTL::PORT_HANDLE hRemote,
                             LocalUrl& url,
                             uint8_t* buffer)
{
  // Length is an in/out parameter, it will be written with the actual bytes
  // read, assume that its correct.
  size_t length = url.length;
  CR(mTl->GCReadPort(hRemote, url.address, buffer, &length));
}

std::string Consumer::getNodeMapXML(GenTL::PORT_HANDLE hRemote)
{
  LocalUrl localUrl = getNodeMapUrl(hRemote);

  // Get device XML
  std::string xmlContents(localUrl.length, ' ');
  size_t xmlContentsSize = localUrl.length;
  CR(mTl->GCReadPort(
    hRemote, localUrl.address, &xmlContents.front(), &xmlContentsSize));

  return xmlContents;
}

}
