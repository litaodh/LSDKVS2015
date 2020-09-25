// Copyright 2016-2019 SICK AG. All rights reserved.

#include "SingleDeviceConsumer.h"

namespace common {

SingleDeviceConsumer::SingleDeviceConsumer(std::string& ctiPath,
                                           std::string& interfaceName,
                                           std::string& deviceUserId)
  : Consumer(ctiPath)
  , mInterfaceName(interfaceName)
  , mDeviceUserId(deviceUserId)
{
}

SingleDeviceConsumer::~SingleDeviceConsumer()
{
}

GenTL::IF_HANDLE SingleDeviceConsumer::openInterface()
{
  mInterfaceHandle = Consumer::openInterfaceByName(mInterfaceName);
  return mInterfaceHandle;
}

void SingleDeviceConsumer::closeInterface()
{
  CR(tl()->IFClose(mInterfaceHandle));
}

GenTL::DEV_HANDLE SingleDeviceConsumer::openDevice()
{
  mDeviceHandle = Consumer::openDeviceByName(mInterfaceHandle, mDeviceUserId);
  return mDeviceHandle;
}

void SingleDeviceConsumer::closeDevice()
{
  CR(tl()->DevClose(mDeviceHandle));
}

GenTL::DS_HANDLE SingleDeviceConsumer::openDataStream()
{
  char streamId[1024];
  size_t idSize = sizeof(streamId);
  CR(tl()->DevGetDataStreamID(mDeviceHandle, 0, streamId, &idSize));
  CR(tl()->DevOpenDataStream(mDeviceHandle, streamId, &mDataStreamHandle));
  return mDataStreamHandle;
}

void SingleDeviceConsumer::closeDataStream()
{
  CR(tl()->DSClose(mDataStreamHandle));
}

GenApi::CNodeMapRef SingleDeviceConsumer::getDeviceNodeMap()
{
  GenTL::PORT_HANDLE hRemote;
  CR(tl()->DevGetPort(mDeviceHandle, &hRemote));
  mPort = std::unique_ptr<GenTLPort>(new GenTLPort(hRemote, tl()));

  return getNodeMap(mPort.get());
}

}
