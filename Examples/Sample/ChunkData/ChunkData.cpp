// Copyright 2018-2019 SICK AG. All rights reserved.

#include "FastMetadataExtractor.h"
#include "MetadataExtractor.h"
#include "Ranger3LineMetadata.h"

#include "ChunkAdapter.h"
#include "DeviceSelector.h"
#include "SampleUtils.h"

#include <GenApi/ChunkAdapterGEV.h>

#include <chrono>
#include <string>


float percent(const std::chrono::microseconds duration,
              const std::chrono::microseconds totalDuration)
{
  return (100.f * duration.count()) / totalDuration.count();
}


void printMeasurement(const std::chrono::microseconds duration,
                      const std::chrono::microseconds totalDuration)
{
  std::cout << "The extraction took " << duration.count() << " us out of "
            << totalDuration.count()
            << " us = " << percent(duration, totalDuration) << "%" << std::endl;
}

void printMetadata(const Ranger3LineMetadata& metadata)
{
  std::cout << "  Timestamp: " << metadata.timestamp << std::endl;
  std::cout << "  EncoderValue: " << metadata.encoderValue << std::endl;
  std::cout << "  EncoderA: " << metadata.encoderA << std::endl;
  std::cout << "  EncoderB: " << metadata.encoderB << std::endl;
  std::cout << "  FrameTriggerActive: " << metadata.frameTriggerActive
            << std::endl;
  std::cout << "  LineTriggerActive: " << metadata.lineTriggerActive
            << std::endl;
  std::cout << "  OvertriggerCount: "
            << static_cast<int>(metadata.overtriggerCount) << std::endl;
}

void printMetadata(const std::vector<Ranger3LineMetadata>& metadata,
                   size_t linesToPrint)
{
  for (size_t i = 0; i < linesToPrint; ++i)
  {
    std::cout << "Metadata from line " << i << ":" << std::endl;
    printMetadata(metadata[i]);
  }
}

/**
   This sample demonstrates how to attach to chunk metadata carried in
   each buffer. It also shows how to access the metadata through the
   device node map after attaching to the buffer.
*/
int main(int argc, char* argv[])
{
  InputWaiter waiter;
  common::DeviceSelector selector;

  if (!common::parseDeviceOptionsHelper(argc, argv, selector))
  {
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

  // Enable chunk metadata
  GenApi::CBooleanPtr chunkModeActive = device._GetNode("ChunkModeActive");
  *chunkModeActive = true;

  // Switch to 3D
  GenApi::CEnumerationPtr deviceType = device._GetNode("DeviceScanType");
  *deviceType = "Linescan3D";

  // Fetch buffer width and number of lines in buffer
  GenApi::CEnumerationPtr regionSelector = device._GetNode("RegionSelector");
  *regionSelector = "Scan3dExtraction1";

  const size_t lineCount = 1000;
  GenApi::CIntegerPtr height = device._GetNode("Height");
  height->SetValue(lineCount);

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

  std::cout << "Grabbing buffer...";

  // Wait for buffer to be received
  GenTL::EVENT_NEW_BUFFER_DATA bufferData;
  size_t eventSize = sizeof(bufferData);
  CC(tl, tl->EventGetData(hEvent, &bufferData, &eventSize, -1));

  std::cout << " OK" << std::endl << std::endl;

  // See inside the common::ChunkAdapter class to understand how GenTL
  // is used to attach the chunk adapter.
  common::ChunkAdapter chunkAdapter(tl, dataStreamHandle);
  chunkAdapter.attachNodeMap(device._Ptr);
  chunkAdapter.attachBuffer(hBuffer, pBuffer);

  using namespace std::chrono;

  MetadataExtractor standardExtractor(device);
  FastMetadataExtractor fastExtractor(device);

  const std::chrono::seconds simulatedTestDuration{10};
  const size_t lineFrequencyHz = 46000;
  const size_t numIterations =
    lineFrequencyHz * simulatedTestDuration.count() / lineCount;
  const size_t extractedMetadataCount = lineCount * numIterations;

  std::cout
    << "Measure the time required to extract metadata from "
    << extractedMetadataCount
    << " lines.\n\nThis corresponds to running the Ranger3 at full speed "
       "during "
    << simulatedTestDuration.count()
    << " seconds. The metadata extraction should require only a fraction of "
    << simulatedTestDuration.count()
    << " seconds since there must be CPU resources available for other "
    << "processing\n"
    << std::endl;

  auto t1 = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < numIterations; i++)
  {
    standardExtractor.extract();
  }

  auto t2 = high_resolution_clock::now();
  microseconds duration1 = duration_cast<microseconds>(t2 - t1);
  std::cout << "Standard method:" << std::endl;
  printMeasurement(duration1, simulatedTestDuration);

  auto fastT1 = high_resolution_clock::now();
  for (int i = 0; i < numIterations; i++)
  {
    fastExtractor.extract();
  }

  auto fastT2 = high_resolution_clock::now();

  auto duration2 = duration_cast<microseconds>(fastT2 - fastT1);
  std::cout << "Fast method:" << std::endl;
  printMeasurement(duration2, simulatedTestDuration);

  std::cout << "Speed up factor: ";
  if (duration2.count() > 0)
  {
    std::cout << (duration1.count() / duration2.count());
  }
  else
  {
    std::cout << "infinite";
  }
  std::cout << std::endl;

  auto results1 = standardExtractor.extract();
  auto results2 = fastExtractor.extract();

  // Verify that both methods give the same results
  assert(results1 == results2);

  chunkAdapter.detachBuffer();
  chunkAdapter.detachNodeMap();

  printMetadata(results1, 5);

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

  delete[] pBuffer;
  CC(tl, tl->DSClose(dataStreamHandle));
  consumer.closeDevice(deviceHandle);
  consumer.closeInterface(interfaceHandle);
  consumer.close();

  return 0;
}
