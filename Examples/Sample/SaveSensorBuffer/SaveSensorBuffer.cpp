// Copyright 2016-2019 SICK AG. All rights reserved.

#include "DeviceSelector.h"
#include "GenIRanger.h"
#include "SampleUtils.h"
#include "SingleDeviceConsumer.h"

#include <string>

void usage(int, char* argv[])
{
  std::cout << "Usage:" << std::endl
            << argv[0] << " <file path for buffer>" << std::endl;
  common::printDeviceOptionsHelp();
  std::cout << std::endl;
  std::cout << "File path should include the name of the file to save without "
            << "extension." << std::endl
            << "E.g. C:\\MyBuffers\\buffer" << std::endl;
}

int main(int argc, char* argv[])
{
  InputWaiter waiter;
  common::DeviceSelector selector;

  if (!common::parseDeviceOptionsHelper(argc, argv, selector))
  {
    return 1;
  }

  std::string filePath = "saved_sensor_buffer";
  if (!common::parseArgumentHelper(
        argc, argv, "file path for buffer", filePath))
  {
    usage(argc, argv);
    return 1;
  }

  std::string ctiFile = common::getPathToProducer();
  if (ctiFile.empty())
  {
    return 1;
  }

  common::Consumer consumer(ctiFile);
  if (consumer.open() == GENTL_INVALID_HANDLE)
  {
    return 1;
  }

  GenTL::IF_HANDLE interfaceHandle = nullptr;
  GenTL::DEV_HANDLE deviceHandle;
  if (selector.isSelected())
  {
    deviceHandle = selector.openDevice(consumer, interfaceHandle);
  }
  else
  {
    deviceHandle = common::openDeviceInteractively(consumer, interfaceHandle);
  }
  if (deviceHandle == GENTL_INVALID_HANDLE)
  {
    return 1;
  }

  // Function pointer to GenTL methods
  GenTLApi* tl = consumer.tl();

  // Connect device node map to GenTL
  GenTL::PORT_HANDLE devicePort;
  CC(tl, tl->DevGetPort(deviceHandle, &devicePort));
  common::GenTLPort port = common::GenTLPort(devicePort, tl);
  GenApi::CNodeMapRef device = consumer.getNodeMap(&port);

  char idBuffer[1024];
  size_t idSize = sizeof(idBuffer);

  // Open stream
  GenTL::DS_HANDLE hStream;
  CC(tl, tl->DevGetDataStreamID(deviceHandle, 0, idBuffer, &idSize));
  CC(tl, tl->DevOpenDataStream(deviceHandle, idBuffer, &hStream));

  // Set image mode to 2D
  GenApi::CEnumerationPtr deviceType = device._GetNode("DeviceScanType");
  *deviceType = "Areascan";

  // Fetch buffer width, height, and offset
  GenApi::CEnumerationPtr regionSelector = device._GetNode("RegionSelector");
  *regionSelector = "Region0";
  GenApi::CIntegerPtr height = device._GetNode("Height");
  GenApi::CIntegerPtr width = device._GetNode("Width");
  GenApi::CIntegerPtr offsetX = device._GetNode("OffsetX");
  GenApi::CIntegerPtr offsetY = device._GetNode("OffsetY");

  // Setup buffer
  GenApi::CIntegerPtr payload = device._GetNode("PayloadSize");
  int64_t payloadSize = payload->GetValue();
  uint8_t* buffer = new uint8_t[payloadSize];
  GenTL::BUFFER_HANDLE hBuffer;
  CC(tl,
     tl->DSAnnounceBuffer(
       hStream, buffer, static_cast<size_t>(payloadSize), nullptr, &hBuffer));
  CC(tl, tl->DSQueueBuffer(hStream, hBuffer));

  // Register event
  GenTL::EVENT_HANDLE hEvent;
  CC(tl, tl->GCRegisterEvent(hStream, GenTL::EVENT_NEW_BUFFER, &hEvent));

  // Start acquisition
  GenApi::CIntegerPtr paramLock = device._GetNode("TLParamsLocked");
  paramLock->SetValue(1);

  // Grab one buffer
  CC(tl, tl->DSStartAcquisition(hStream, GenTL::ACQ_START_FLAGS_DEFAULT, 1));
  GenApi::CCommandPtr command = device._GetNode("AcquisitionStart");
  command->Execute();

  std::cout << "Grabbing buffer..." << std::endl;

  // Wait for buffer to be received
  GenTL::EVENT_NEW_BUFFER_DATA bufferData;
  size_t eventSize = sizeof(bufferData);
  CC(tl, tl->EventGetData(hEvent, &bufferData, &eventSize, -1));

  // Stop acquisition and clean up
  command = device._GetNode("AcquisitionStop");
  command->Execute();
  paramLock->SetValue(0);
  CC(tl, tl->DSStopAcquisition(hStream, GenTL::ACQ_STOP_FLAGS_KILL));
  CC(tl, tl->DSFlushQueue(hStream, GenTL::ACQ_QUEUE_ALL_DISCARD));
  CC(tl, tl->GCUnregisterEvent(hStream, GenTL::EVENT_NEW_BUFFER));
  CC(tl,
     tl->DSRevokeBuffer(
       hStream, hBuffer, reinterpret_cast<void**>(&buffer), nullptr));

  std::cout << "Saving buffer to " << filePath.c_str() << "..." << std::endl;
  GenIRanger::saveBuffer8(reinterpret_cast<uint8_t*>(buffer),
                          width->GetValue(),
                          height->GetValue(),
                          offsetX->GetValue(),
                          offsetY->GetValue(),
                          filePath);

  delete[] buffer;
  CC(tl, tl->DSClose(hStream));
  consumer.closeDevice(deviceHandle);
  consumer.closeInterface(interfaceHandle);
  consumer.close();
  return 0;
}
