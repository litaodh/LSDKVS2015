// Copyright 2016-2019 SICK AG. All rights reserved.

#include "ChunkAdapter.h"
#include "DeviceSelector.h"
#include "FastMetadataExtractor.h"
#include "GenIRanger.h"
#include "SampleUtils.h"
#include "SingleDeviceConsumer.h"
#include "StreamData.h"

#include <fstream>
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

// Extracts the Ranger3 specific line metadata from chunks in a buffer
GenIRanger::LineMetadata extractMetadata(GenTLApi* tl,
                                         GenApi::CNodeMapRef& device,
                                         GenTL::DS_HANDLE dataStreamHandle,
                                         GenTL::BUFFER_HANDLE hBuffer,
                                         uint8_t* pBuffer)
{
  // See inside the common::ChunkAdapter class to understand how GenTL
  // is used to attach the chunk adapter.
  common::ChunkAdapter chunkAdapter(tl, dataStreamHandle);
  chunkAdapter.attachNodeMap(device._Ptr);
  chunkAdapter.attachBuffer(hBuffer, pBuffer);

  FastMetadataExtractor fastExtractor(device);
  auto result = fastExtractor.extract();

  GenIRanger::LineMetadata metadata;
  for (Ranger3LineMetadata line : result)
  {
    metadata.encoderA.push_back(line.encoderA);
    metadata.encoderB.push_back(line.encoderB);
    metadata.encoderValues.push_back(line.encoderValue);
    metadata.encoderResetActive.push_back(line.encoderResetActive);
    metadata.timestamps.push_back(line.timestamp);
    metadata.frameTriggerActive.push_back(line.frameTriggerActive);
    metadata.lineTriggerActive.push_back(line.lineTriggerActive);
    metadata.overtriggerCounts.push_back(line.overtriggerCount);
  }
  return metadata;
}

int main(int argc, char* argv[])
{
  InputWaiter waiter;
  common::DeviceSelector selector;

  if (!common::parseDeviceOptionsHelper(argc, argv, selector))
  {
    return 1;
  }

  std::string filePath = "saved_range_buffer";
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
  // Initialize GenTL and open transport layer
  GenTL::TL_HANDLE tlHandle = consumer.open();
  if (tlHandle == GENTL_INVALID_HANDLE)
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
  GenTL::DS_HANDLE dataStreamHandle;
  CC(tl, tl->DevGetDataStreamID(deviceHandle, 0, idBuffer, &idSize));
  CC(tl, tl->DevOpenDataStream(deviceHandle, idBuffer, &dataStreamHandle));

  // Switch to 3D
  GenApi::CEnumerationPtr deviceType = device._GetNode("DeviceScanType");
  *deviceType = "Linescan3D";

  // Enable chunk metadata
  GenApi::CBooleanPtr chunkModeActive = device._GetNode("ChunkModeActive");
  *chunkModeActive = true;

  // Fetch range axis
  GenApi::CEnumerationPtr rangeAxisEnum = device._GetNode("RangeAxis");
  GenIRanger::RangeAxis rangeAxis = ("Reverse" == rangeAxisEnum->ToString())
                                      ? GenIRanger::RangeAxis::REVERSED
                                      : GenIRanger::RangeAxis::STANDARD;

  // Fetch buffer width and number of lines in buffer
  GenApi::CEnumerationPtr regionSelector = device._GetNode("RegionSelector");
  *regionSelector = "Scan3dExtraction1";
  GenApi::CIntegerPtr height = device._GetNode("Height");
  int64_t bufferHeight = height->GetValue();

  // Fetch info where on the sensor the 3D data is extracted
  *regionSelector = "Region1";
  // Width in Scan3dExtraction1 and Region1 are mirrored
  GenApi::CIntegerPtr width = device._GetNode("Width");
  GenApi::CIntegerPtr offsetX = device._GetNode("OffsetX");
  GenApi::CIntegerPtr offsetY = device._GetNode("OffsetY");
  int64_t aoiHeight = height->GetValue();
  int64_t aoiOffsetX = offsetX->GetValue();
  int64_t aoiOffsetY = offsetY->GetValue();

  int64_t bufferWidth = width->GetValue();
  int64_t buffer16Size = bufferWidth * bufferHeight * sizeof(uint16_t);
  uint8_t* pBuffer16 = new uint8_t[buffer16Size];

  // Setup buffer
  GenApi::CIntegerPtr payload = device._GetNode("PayloadSize");
  int64_t payloadSize = payload->GetValue();
  uint8_t* pBuffer = new uint8_t[payloadSize];
  GenTL::BUFFER_HANDLE hBuffer;
  CC(tl,
     tl->DSAnnounceBuffer(dataStreamHandle,
                          pBuffer,
                          static_cast<size_t>(payloadSize),
                          nullptr,
                          &hBuffer));
  CC(tl, tl->DSQueueBuffer(dataStreamHandle, hBuffer));

  // Register event
  GenTL::EVENT_HANDLE hEvent;
  CC(tl,
     tl->GCRegisterEvent(dataStreamHandle, GenTL::EVENT_NEW_BUFFER, &hEvent));

  // Start acquisition
  GenApi::CIntegerPtr paramLock = device._GetNode("TLParamsLocked");
  paramLock->SetValue(1);

  // Grab one buffer
  CC(tl,
     tl->DSStartAcquisition(
       dataStreamHandle, GenTL::ACQ_START_FLAGS_DEFAULT, 1));
  GenApi::CCommandPtr command = device._GetNode("AcquisitionStart");
  command->Execute();

  std::cout << "Grabbing buffer..." << std::endl;

  // Wait for buffer to be received
  GenTL::EVENT_NEW_BUFFER_DATA bufferData;
  size_t eventSize = sizeof(bufferData);
  CC(tl, tl->EventGetData(hEvent, &bufferData, &eventSize, -1));

  GenIRanger::LineMetadata metadata =
    extractMetadata(tl, device, dataStreamHandle, hBuffer, pBuffer);

  // Stop acquisition and clean up
  command = device._GetNode("AcquisitionStop");
  command->Execute();
  paramLock->SetValue(0);
  CC(tl, tl->DSStopAcquisition(dataStreamHandle, GenTL::ACQ_STOP_FLAGS_KILL));
  CC(tl, tl->DSFlushQueue(dataStreamHandle, GenTL::ACQ_QUEUE_ALL_DISCARD));
  CC(tl, tl->GCUnregisterEvent(dataStreamHandle, GenTL::EVENT_NEW_BUFFER));
  CC(tl,
     tl->DSRevokeBuffer(
       dataStreamHandle, hBuffer, reinterpret_cast<void**>(&pBuffer), nullptr));

  std::cout << "Converting buffer..." << std::endl;

  GenIRanger::convert12pTo16(static_cast<uint8_t*>(pBuffer),
                             bufferWidth * bufferHeight * 12 / 8,
                             pBuffer16,
                             &buffer16Size);

  std::cout << "Saving buffer to " << filePath.c_str() << "..." << std::endl;

  GenIRanger::RangeFrame frame;
  frame
    .aoiOffset(static_cast<size_t>(aoiOffsetX), static_cast<size_t>(aoiOffsetY))
    .aoiSize(static_cast<size_t>(bufferWidth), static_cast<size_t>(aoiHeight))
    .rangeAxis(rangeAxis);

  frame.createRange(buffer16Size, GenIRanger::PixelWidth::PW16);
  frame.range()->data().assign(pBuffer16, pBuffer16 + buffer16Size);

  // If you are not interested in chunk metadata you can use
  // GenIRanger::saveBuffer16() instead and avoid creating a frame yourself.
  frame.lineMetadata(std::make_shared<GenIRanger::LineMetadata>(metadata));

  saveMultipartRangeFrame(frame, filePath, filePath);

  delete[] pBuffer16;
  delete[] pBuffer;
  CC(tl, tl->DSClose(dataStreamHandle));
  consumer.closeDevice(deviceHandle);
  consumer.closeInterface(interfaceHandle);
  consumer.close();

  return 0;
}
