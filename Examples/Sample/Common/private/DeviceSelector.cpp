// Copyright 2017-2019 SICK AG. All rights reserved.

#include "Consumer.h"
#include "DeviceSelector.h"
#include "SampleUtils.h"

#include <string>

namespace common {

DeviceSelector::DeviceSelector()
  : mUseMac(false)
  , mUseIp(false)
{
  // Empty
}

void DeviceSelector::selectDeviceMac(uint64_t mac)
{
  mUseMac = true;
  mSelectedMac = mac;
}

void DeviceSelector::selectDeviceIp(uint32_t ip)
{
  mUseIp = true;
  mSelectedIp = ip;
}

bool DeviceSelector::isSelected()
{
  return mUseMac || mUseIp;
}


GenTL::DEV_HANDLE DeviceSelector::openDevice(common::Consumer& consumer,
                                             GenTL::IF_HANDLE& interfaceHandle)
{
  common::InterfaceList interfaces =
    consumer.getInterfaces(consumer.tlHandle());
  for (auto& theInterface : interfaces)
  {
    interfaceHandle = consumer.openInterfaceById(theInterface.first);
    if (interfaceHandle == GENTL_INVALID_HANDLE)
    {
      std::cerr << "Warning, could not open interface " << theInterface.first
                << std::endl;
      continue;
    }

    auto tl = consumer.tl();
    common::GenTLPort interfacePort = common::GenTLPort(interfaceHandle, tl);
    GenApi::CNodeMapRef interfaceNodeMap =
      consumer.getNodeMap(&interfacePort, "InterfacePort");
    // Perform a scan, to find devices on the interface
    bool8_t changed = false;
    CC(tl, tl->IFUpdateDeviceList(interfaceHandle, &changed, 500));

    // Use nodemap to read out IP addresses and MAC addresses
    GenApi::CIntegerPtr deviceSelector =
      interfaceNodeMap._GetNode("DeviceSelector");
    GenApi::CStringPtr deviceId = interfaceNodeMap._GetNode("DeviceID");
    GenApi::CIntegerPtr gevDeviceIpAddress =
      interfaceNodeMap._GetNode("GevDeviceIPAddress");
    GenApi::CIntegerPtr gevDeviceMacAddress =
      interfaceNodeMap._GetNode("GevDeviceMACAddress");

    if (GenApi::IsWritable(deviceSelector))
    {
      for (int64_t i = 0; i <= deviceSelector->GetMax(); i++)
      {
        deviceSelector->SetValue(i);
        DeviceId theDeviceId = deviceId->GetValue();

        if (mUseMac)
        {
          uint64_t currentMac =
            static_cast<uint64_t>(gevDeviceMacAddress->GetValue());
          if (currentMac == mSelectedMac)
          {
            return consumer.openDeviceById(interfaceHandle, theDeviceId);
          }
        }
        else if (mUseIp)
        {
          uint32_t currentIp =
            static_cast<uint32_t>(gevDeviceIpAddress->GetValue());
          if (currentIp == mSelectedIp)
          {
            return consumer.openDeviceById(interfaceHandle, theDeviceId);
          }
        }
      }
    }
    consumer.closeInterface(interfaceHandle);
  }
  std::cerr << "Could not connect to selected device" << std::endl;
  return GENTL_INVALID_HANDLE;
}

}
