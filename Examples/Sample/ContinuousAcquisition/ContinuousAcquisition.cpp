// Copyright 2016-2019 SICK AG. All rights reserved.

#include "BufferInfo.h"
#include "ChunkAdapter.h"
#include "Consumer.h"
#include "GenIRanger.h"
#include "SampleUtils.h"

#include <cassert>
#include <conio.h>
#include <ctime>
#include <fstream>
#include <string>

// From GenICam Pixel Format Naming Convention we get the
// pixel format values, imported from "PFNC.h" available emva.org
#define PFNC_Mono8 0x01080001 // Monochrome 8-bit
#define PFNC_Mono12p 0x010C0047 // Monochrome 12-bit packed
#define PFNC_Mono16 0x01100007 // Monochrome 16-bit
#define PFNC_Coord3D_C12p 0x010C00DA // 3D coordinate C 12-bit packed
#define PFNC_Coord3D_C16 0x011000B8 // 3D coordinate C 16-bit

void usage(int, char* argv[])
{
  std::cout << "Usage:" << std::endl
            << argv[0] << "<directory to save buffers in>" << std::endl;
  common::printDeviceOptionsHelp();
  std::cout << std::endl;
  std::cout << "Path to the directory where buffers should be saved"
            << std::endl
            << "E.g. C:\\MyBuffers\\buffer" << std::endl
            << std::endl;
}

// Globally declared IDs for the receiver components.
// See logging for usage and mapping of parts to components
int64_t gComponentIdReflectance;
int64_t gComponentIdRange;

/**
   This sample demonstrates how to configure and continuously acquire
   range data buffers from the Ranger3. It also logs various
   information about each buffer to a timestamped file in a folder
   next to the buffers.

   This sample program takes one command line argument: Absolute path
   to the directory where buffers should be saved. This folder must
   exist!

   After selecting interface and device it provides a couple of options:
   - Number of buffers to acquire
   - To save the buffers to disk or just throw them away
   - Name (acquisition ID will be appended)
   - The maximum number of buffers on disk, when this number is reached old
     buffers will be overwritten

     * Acquisition can be aborted by pressing the escape key.
*/
int main(int argc, char* argv[])
{
  InputWaiter waiter;
  common::DeviceSelector selector;


  if (!common::parseDeviceOptionsHelper(argc, argv, selector))
  {
    return 1;
  }

  std::string savePath = "continuous_saved_buffers";
  if (!common::parseArgumentHelper(
        argc, argv, "directory to save buffers in", savePath))
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

  GenTL::DS_HANDLE dataStreamHandle = consumer.openDataStream(deviceHandle);
  if (dataStreamHandle == GENTL_INVALID_HANDLE)
  {
    return 1;
  }

  int64_t numToSave = 0;
  std::cout << "Enter the number of buffers acquire: " << std::endl;
  std::cin >> numToSave;

  std::cout << "Save buffers to disk? (y/n)" << std::endl;
  std::string answer;
  std::cin >> answer;
  bool saveToDisk;
  if (answer == "y")
  {
    saveToDisk = true;
  }
  else if (answer == "n")
  {
    saveToDisk = false;
  }
  else
  {
    std::cerr << "Unknown answer \"" << answer << "\" exiting" << std::endl;
    exit(1);
  }

  std::string bufferName("buffer");
  int64_t bufferRotationNum = 0;
  if (saveToDisk)
  {
    std::cout << "Enter name of buffer: " << std::endl;
    std::cin >> bufferName;
    std::cout << "Enter the maximum number of buffers on disk (0 = no limit):"
              << std::endl;
    std::cin >> bufferRotationNum;
  }

  // Function pointer to GenTL methods
  GenTLApi* tl = consumer.tl();

  // Set the heartbeat timeout for the device to 300 seconds if debug, else the
  // default 3s.
  if (IsDebuggerPresent())
  {
    common::setHeartbeatTimeout(
      deviceHandle, tl, consumer, 300.f * 1000 * 1000);
  }
  else
  {
    common::setHeartbeatTimeout(deviceHandle, tl, consumer, 3.f * 1000 * 1000);
  }

  // Get node map for data stream module to boost receiver thread priority
  common::GenTLPort dataStreamPort = common::GenTLPort(dataStreamHandle, tl);
  GenApi::CNodeMapRef datastreamNodeMap =
    consumer.getNodeMap(&dataStreamPort, "StreamPort");
  // For logging of receiver statistics after test run, complete list
  // of statistics
  GenApi::CIntegerPtr seenPacketCount =
    datastreamNodeMap._GetNode("GevStreamSeenPacketCount");
  GenApi::CIntegerPtr lostPacketCount =
    datastreamNodeMap._GetNode("GevStreamLostPacketCount");
  GenApi::CIntegerPtr resendPacketCount =
    datastreamNodeMap._GetNode("GevStreamResendPacketCount");
  GenApi::CIntegerPtr deliveredPacketCount =
    datastreamNodeMap._GetNode("GevStreamDeliveredPacketCount");
  GenApi::CIntegerPtr unavailablePacketCount =
    datastreamNodeMap._GetNode("GevStreamUnavailablePacketCount");
  GenApi::CIntegerPtr duplicatePacketCount =
    datastreamNodeMap._GetNode("GevStreamDuplicatePacketCount");
  GenApi::CIntegerPtr skippedBlockCount =
    datastreamNodeMap._GetNode("GevStreamSkippedBlockCount");
  GenApi::CIntegerPtr engineUnderrunCount =
    datastreamNodeMap._GetNode("GevStreamEngineUnderrunCount");
  GenApi::CIntegerPtr discardedBlockCount =
    datastreamNodeMap._GetNode("GevStreamDiscardedBlockCount");
  GenApi::CIntegerPtr incompleteBlockCount =
    datastreamNodeMap._GetNode("GevStreamIncompleteBlockCount");
  GenApi::CIntegerPtr oversizedBlockCount =
    datastreamNodeMap._GetNode("GevStreamOversizedBlockCount");

  // Connect device node map to GenTL
  GenTL::PORT_HANDLE devicePort;
  CC(tl, tl->DevGetPort(deviceHandle, &devicePort));
  common::GenTLPort port = common::GenTLPort(devicePort, tl);
  // Node map for reading/setting device parameters
  GenApi::CNodeMapRef device = consumer.getNodeMap(&port);

  // Wrapper to be able to access chunk metadata
  std::unique_ptr<common::ChunkAdapter> chunkAdapter(
    new common::ChunkAdapter(tl, dataStreamHandle));
  chunkAdapter->attachNodeMap(device._Ptr);
  GenApi::CIntegerPtr chunkHeight = device._GetNode("ChunkHeight");
  GenApi::CIntegerPtr chunkWidth = device._GetNode("ChunkWidth");

  // Enable chunk metadata
  GenApi::CBooleanPtr chunkModeActive = device._GetNode("ChunkModeActive");
  *chunkModeActive = true;

  // Switch to 3D
  GenApi::CEnumerationPtr deviceType = device._GetNode("DeviceScanType");
  *deviceType = "Linescan3D";

  // Switch to Continuous Acquisition
  GenApi::CEnumerationPtr aquisitionMode = device._GetNode("AcquisitionMode");
  *aquisitionMode = "Continuous";

  // Set AcquisitionLineRate
  GenApi::CFloatPtr acquisitionLineRate =
    device._GetNode("AcquisitionLineRate");
  acquisitionLineRate->SetValue(1000.f);

  // Enable Reflectance in addition to Range data
  GenApi::CEnumerationPtr regionSelector = device._GetNode("RegionSelector");
  *regionSelector = "Scan3dExtraction1";
  GenApi::CEnumerationPtr componentSelector =
    device._GetNode("ComponentSelector");
  *componentSelector = "Range";
  GenApi::CIntegerPtr componentIdValue = device._GetNode("ComponentIDValue");
  gComponentIdRange = componentIdValue->GetValue();
  *componentSelector = "Reflectance";
  gComponentIdReflectance = componentIdValue->GetValue();
  GenApi::CBooleanPtr reflectanceEnable = device._GetNode("ComponentEnable");

  *reflectanceEnable = true;

  // Set the number of lines in buffer
  GenApi::CIntegerPtr height = device._GetNode("Height");
  int64_t bufferHeight = 500;
  height->SetValue(bufferHeight);

  // Set AOI height
  *regionSelector = "Region1";
  int64_t aoiHeight = 200;
  height->SetValue(aoiHeight);
  // Set AOI offset
  GenApi::CIntegerPtr offsetY = device._GetNode("OffsetY");
  offsetY->SetValue(200);

  // Set exposure time for 3D acquisition
  GenApi::CFloatPtr exposuretime = device._GetNode("ExposureTime");
  exposuretime->SetValue(25.f);


  GenApi::CIntegerPtr width = device._GetNode("Width");
  GenApi::CIntegerPtr offsetX = device._GetNode("OffsetX");
  int64_t aoiOffsetX = offsetX->GetValue();
  int64_t aoiOffsetY = offsetY->GetValue();

  int64_t bufferWidth = width->GetValue();
  // 16-bit range, 8-bit reflectance
  int64_t buffer16Size = bufferWidth * bufferHeight * (2 + 1);

  // Setup a sufficient amount of buffers
  GenApi::CIntegerPtr payload = device._GetNode("PayloadSize");
  const size_t payloadSize = static_cast<size_t>(payload->GetValue());
  const size_t buffersCount = 20;

  // Two different sized buffers are used below. One for the 12-bit data which
  // is sent from the device, and one for converting it to buffer with a
  // 16-bit representation. The latter is only used for saving it to disk in
  // the .dat/.xml format.
  uint8_t* pBuffer[buffersCount];
  uint8_t* pBuffer16;
  GenTL::BUFFER_HANDLE bufferHandles[buffersCount];

  // Announce and queue buffers to producer, we keep track of the buffers by
  // their index in the array
  pBuffer16 = new uint8_t[buffer16Size];
  for (size_t i = 0; i < buffersCount; i++)
  {
    pBuffer[i] = new uint8_t[payloadSize];
    CC(tl,
       tl->DSAnnounceBuffer(dataStreamHandle,
                            pBuffer[i],
                            payloadSize,
                            reinterpret_cast<void*>(i),
                            &bufferHandles[i]));
    CC(tl, tl->DSQueueBuffer(dataStreamHandle, bufferHandles[i]));
  }

  // Register event
  GenTL::EVENT_HANDLE hEvent;
  CC(tl,
     tl->GCRegisterEvent(dataStreamHandle, GenTL::EVENT_NEW_BUFFER, &hEvent));

  // Create log file for saving buffer information
  std::string filePath = savePath + "\\acquisition_log";
  // Create directories if they do not already exist
  CreateDirectoryA(savePath.c_str(), nullptr);
  CreateDirectoryA(filePath.c_str(), nullptr);
  std::ofstream logFile;
  std::string filename =
    filePath + "\\log-" + common::currentDateTime() + ".txt";
  logFile.exceptions(std::ios::failbit | std::ios::badbit);
  logFile.open(filename, std::ios_base::app);

  // Get range axis
  GenApi::CEnumerationPtr rangeAxisEnum = device._GetNode("RangeAxis");
  GenIRanger::RangeAxis rangeAxis = ("Reverse" == rangeAxisEnum->ToString())
                                      ? GenIRanger::RangeAxis::REVERSED
                                      : GenIRanger::RangeAxis::STANDARD;

  // Lock all parameters before starting
  GenApi::CIntegerPtr paramLock = device._GetNode("TLParamsLocked");
  paramLock->SetValue(1);
  // Start acquisition
  CC(tl,
     tl->DSStartAcquisition(
       dataStreamHandle, GenTL::ACQ_START_FLAGS_DEFAULT, GENTL_INFINITE));
  GenApi::CCommandPtr command = device._GetNode("AcquisitionStart");
  command->Execute();

  intptr_t bufferId = 0;
  GenTL::EVENT_NEW_BUFFER_DATA bufferData;
  std::cout << "Acquiring buffers..." << std::endl;

  int returnCode = 0;
  try
  {
    // Must provide the size of the event buffer
    size_t eventSize = sizeof(bufferData);
    // Timeout in milliseconds
    const uint64_t timeout = 5000;
    for (int64_t i = 1; i < numToSave + 1; i++)
    {
      std::cout << "Acquiring buffer " << i << "...";
      try
      {
        // Wait for buffer to be received
        CC(tl, tl->EventGetData(hEvent, &bufferData, &eventSize, timeout));

        bufferId = reinterpret_cast<intptr_t>(bufferData.pUserPointer);

        common::BufferInfo bufferInfo(
          tl, dataStreamHandle, bufferHandles[bufferId]);
        assert(bufferInfo.mBufferPayloadType == GenTL::PAYLOAD_TYPE_MULTI_PART);
        assert(bufferInfo.mPartCount == 2);

        common::BufferPartInfo part0Info(
          tl, dataStreamHandle, bufferHandles[bufferId], 0);
        assert(part0Info.mPartDataWidth == bufferWidth);
        assert(part0Info.mPartDataHeight == bufferHeight);
        // Part 0 should be the 3D Range data
        assert(part0Info.mPartDataType == GenTL::PART_DATATYPE_3D_IMAGE);

        common::BufferPartInfo part1Info(
          tl, dataStreamHandle, bufferHandles[bufferId], 1);
        assert(part1Info.mPartDataWidth == bufferWidth);
        assert(part1Info.mPartDataHeight == bufferHeight);
        // Part 1 should be the Reflectance data
        assert(part1Info.mPartDataType == GenTL::PART_DATATYPE_2D_IMAGE);
        assert(part1Info.mPartDataFormat == PFNC_Mono8);

        // Attach the chunk adapter to this buffer to be able to access metadata
        chunkAdapter->attachBuffer(bufferHandles[bufferId], pBuffer[bufferId]);

        // Log information about the received buffer and some chunk metadata
        logFile << std::endl << "Buffer: " << i << std::endl;
        bufferInfo.log(logFile);
        part0Info.log(logFile);
        part1Info.log(logFile);
        logFile << "Chunk Height: " << chunkHeight->GetValue()
                << ", Width: " << chunkWidth->GetValue() << std::endl;

        if (saveToDisk)
        {
          // Convert Range data to 16-bit if needed
          if (part0Info.mPartDataFormat == PFNC_Coord3D_C12p
              || part0Info.mPartDataFormat == PFNC_Mono12p)
          {
            GenIRanger::convert12pTo16(part0Info.mPartDataPointer,
                                       part0Info.mPartDataSize,
                                       pBuffer16,
                                       &buffer16Size);
          }
          else
          {
            // Just copy the data to buffer16 pointer
            assert(part0Info.mPartDataSize
                   <= static_cast<uint64_t>(buffer16Size));
            std::memcpy(
              pBuffer16, part0Info.mPartDataPointer, part0Info.mPartDataSize);
          }


          std::stringstream bufferPath;
          // Append loop index to buffer name
          bufferPath << savePath << "\\" << bufferName << "-"
                     << (bufferRotationNum == 0
                           ? i
                           : (i % bufferRotationNum + 1 == 0
                                ? 1
                                : i % bufferRotationNum + 1));

          // Save buffer to disk
          // To save buffers with a single part, look at saveBuffer16
          // and saveBuffer8
          GenIRanger::saveMultiPartBuffer(pBuffer16,
                                          part1Info.mPartDataPointer,
                                          nullptr,
                                          bufferWidth,
                                          bufferHeight,
                                          aoiHeight,
                                          aoiOffsetX,
                                          aoiOffsetY,
                                          rangeAxis,
                                          bufferPath.str());
        }

        chunkAdapter->detachBuffer();
        // Re-queue buffer
        CC(tl, tl->DSQueueBuffer(dataStreamHandle, bufferHandles[bufferId]));
      }
      catch (const std::exception& e)
      {
        std::cout << e.what() << std::endl;
        returnCode = 1;
      }
      std::cout << std::endl;
    }
    if (saveToDisk)
    {
      std::cout << "Saved buffers to directory " << savePath << std::endl;
    }

    // Stop acquisition
    command = device._GetNode("AcquisitionStop");
    command->Execute();
    CC(tl, tl->DSStopAcquisition(dataStreamHandle, GenTL::ACQ_STOP_FLAGS_KILL));
    paramLock->SetValue(0);

    // Log receiver statistics.
    logFile << std::endl << "Image acquisition stopped" << std::endl;
    logFile << std::endl
            << "Stream receiver statistics Start" << std::endl
            << "Seen Packet Count: " << seenPacketCount->GetValue() << std::endl
            << "Lost Packet Count: " << lostPacketCount->GetValue() << std::endl
            << "Delivered Packet Count: " << deliveredPacketCount->GetValue()
            << std::endl
            << "Unavailable Packet Count: "
            << unavailablePacketCount->GetValue() << std::endl
            << "Duplicate Packet Count: " << duplicatePacketCount->GetValue()
            << std::endl
            << "Resend Packet Count: " << resendPacketCount->GetValue()
            << std::endl
            << "Skipped Block Count: " << skippedBlockCount->GetValue()
            << std::endl
            << "Engine Underrun Count: " << engineUnderrunCount->GetValue()
            << std::endl
            << "Discarded Block Count: " << discardedBlockCount->GetValue()
            << std::endl
            << "Incomplete Block Count: " << incompleteBlockCount->GetValue()
            << std::endl
            << "Oversized Block Count: " << oversizedBlockCount->GetValue()
            << std::endl
            << "Stream receiver statistics End" << std::endl;

    // Discard all buffers so that they can be revoked from stream
    CC(tl, tl->DSFlushQueue(dataStreamHandle, GenTL::ACQ_QUEUE_ALL_DISCARD));
    CC(tl, tl->GCUnregisterEvent(dataStreamHandle, GenTL::EVENT_NEW_BUFFER));

    for (size_t i = 0; i < buffersCount; i++)
    {
      // Revoke buffer from stream so that memory can be deleted safely
      CC(tl,
         tl->DSRevokeBuffer(dataStreamHandle,
                            bufferHandles[i],
                            reinterpret_cast<void**>(&pBuffer[i]),
                            nullptr));
      delete[] pBuffer[i];
    }
    delete[] pBuffer16;
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
    returnCode = 1;
  }

  logFile.close();

  chunkAdapter->detachNodeMap();
  chunkAdapter.reset();
  consumer.closeDataStream(dataStreamHandle);
  consumer.closeDevice(deviceHandle);
  consumer.closeInterface(interfaceHandle);
  consumer.close();

  return returnCode;
}
