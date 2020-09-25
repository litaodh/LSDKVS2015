// Copyright 2016-2019 SICK AG. All rights reserved.

#include "Consumer.h"
#include "GenIRanger.h"
#include "SampleUtils.h"

#include <conio.h>
#include <ctime>
#include <direct.h>
#include <fstream>
#include <set>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

// Global variables
std::string gSavePath;

// Temp memory used for pixel format conversion for saving buffers to disk
size_t gTempBufferSize;
uint8_t* gTempBuffer;

void usage(int, char* argv[])
{
  std::cout << "Usage:" << std::endl
            << argv[0] << std::endl
            << "<file path for buffers>" << std::endl
            << "Path to the directory where buffers should be saved"
            << std::endl
            << "E.g. C:\\MyBuffers\\buffer" << std::endl
            << std::endl;
}

bool directoryExists(const std::string& path)
{
  struct stat info;
  return (stat(path.c_str(), &info) == 0 && info.st_mode & S_IFDIR);
}

/** Represents the Area of interest on the sensor, in pixel
    coordinates. This information is required to be able to perform
    proper calibration
*/
struct Aoi
{
  Aoi(uint16_t offsetX, uint16_t offsetY, uint16_t width, uint16_t height)
    : mOffsetX(offsetX)
    , mOffsetY(offsetY)
    , mWidth(width)
    , mHeight(height)
  {
  }

  uint16_t mOffsetX;
  uint16_t mOffsetY;
  uint16_t mWidth;
  uint16_t mHeight;
};

/** Helper class to keep track of all connected devices */
class DeviceConnection
{
public:
  DeviceConnection(GenTLApi* tl,
                   GenTL::DEV_HANDLE deviceHandle,
                   GenTL::DS_HANDLE dataStreamHandle,
                   std::string deviceName)
    : mTl(tl)
    , mAcquisitionRunning(false)
    , mDeviceHandle(deviceHandle)
    , mDataStreamHandle(dataStreamHandle)
    , mDeviceName(deviceName)
    , mAoi(0, 0, 0, 0)
  {
    // Create log file for saving buffer information
    std::string logDir = gSavePath + "\\acquisition_log-" + mDeviceName;
    CreateDirectoryA(logDir.c_str(), nullptr);
    mLog.exceptions(std::ios::failbit | std::ios::badbit);

    std::string logFilename =
      logDir + "\\log-" + common::currentDateTime() + ".txt";
    mLog.open(logFilename, std::ios_base::app);
    mLog << "Buffer acquisition statistics Start" << std::endl
         << "Acquisition num; Buffer frame id; Incomplete; Bytes filled; "
         << "Buffer size" << std::endl;
  }

  void createDeviceNodeMap(common::Consumer& consumer);
  void createDataStreamNodeMap(common::Consumer& consumer);

  void initializeBuffers(size_t buffersCount, size_t payloadSize);
  void teardownBuffers();

  void startAcquisition();
  void stopAcquisition();
  bool isAcquisitionRunning() const;

  void registerNewBufferEvent();
  void unregisterNewBufferEvent();

private:
  std::vector<GenTL::BUFFER_HANDLE> mBufferHandles;
  std::vector<void*> mBufferData;

  GenTLApi* mTl;
  std::unique_ptr<common::GenTLPort> mDevicePort;
  std::unique_ptr<common::GenTLPort> mDataStreamPort;
  bool mAcquisitionRunning;

public:
  GenTL::DEV_HANDLE mDeviceHandle;
  GenTL::DS_HANDLE mDataStreamHandle;
  GenTL::EVENT_HANDLE mNewBufferEventHandle;

  std::string mDeviceName;
  std::ofstream mLog;
  Aoi mAoi;

  GenApi::CNodeMapRef mDeviceNodeMap;
  GenApi::CNodeMapRef mDataStreamNodeMap;
};

void DeviceConnection::createDeviceNodeMap(common::Consumer& consumer)
{
  GenTL::PORT_HANDLE devicePort;
  CC(mTl, mTl->DevGetPort(mDeviceHandle, &devicePort));
  mDevicePort.reset(new common::GenTLPort(devicePort, mTl));
  mDeviceNodeMap = consumer.getNodeMap(mDevicePort.get(), "Device");
}

void DeviceConnection::createDataStreamNodeMap(common::Consumer& consumer)
{
  mDataStreamPort.reset(new common::GenTLPort(mDataStreamHandle, mTl));
  mDataStreamNodeMap = consumer.getNodeMap(mDataStreamPort.get(), "StreamPort");
}

/** Allocates, announces and queues a number of buffers to the data stream */
void DeviceConnection::initializeBuffers(size_t buffersCount,
                                         size_t payloadSize)
{
  mBufferHandles.resize(buffersCount, GENTL_INVALID_HANDLE);
  mBufferData.resize(buffersCount, nullptr);

  for (size_t i = 0; i < buffersCount; i++)
  {
    uint8_t* bufferData = new uint8_t[payloadSize];
    if (bufferData == nullptr)
    {
      throw std::exception("Could not allocated enough memory for buffers");
    }
    GenTL::BUFFER_HANDLE bufferHandle;
    // Store the pointer to the raw memory in the user data for sake of
    // simplicity. This makes it easy to access the memory when a buffer has
    // been received.
    void* userData = mBufferData[i];
    CC(mTl,
       mTl->DSAnnounceBuffer(
         mDataStreamHandle, bufferData, payloadSize, userData, &bufferHandle));
    CC(mTl, mTl->DSQueueBuffer(mDataStreamHandle, bufferHandle));

    mBufferHandles[i] = bufferHandle;
    mBufferData[i] = bufferData;
  }
}

void DeviceConnection::teardownBuffers()
{
  for (size_t i = 0; i < mBufferHandles.size(); i++)
  {
    // Revoke buffer from stream so that memory can be deleted safely
    CC(mTl,
       mTl->DSRevokeBuffer(
         mDataStreamHandle, mBufferHandles[i], &mBufferData[i], nullptr));
    delete[] mBufferData[i];
  }
}

void DeviceConnection::startAcquisition()
{
  GenApi::CCommandPtr acquisitionStart =
    mDeviceNodeMap._GetNode("AcquisitionStart");
  acquisitionStart->Execute();
  mAcquisitionRunning = true;
}

void DeviceConnection::stopAcquisition()
{
  if (mAcquisitionRunning)
  {
    std::cout << "Stopping acquisition of " << mDeviceName << std::endl;

    GenApi::CCommandPtr acquisitionStop =
      mDeviceNodeMap._GetNode("AcquisitionStop");
    acquisitionStop->Execute();
    mAcquisitionRunning = false;
  }
}

bool DeviceConnection::isAcquisitionRunning() const
{
  return mAcquisitionRunning;
}

void DeviceConnection::registerNewBufferEvent()
{
  CC(mTl,
     mTl->GCRegisterEvent(
       mDataStreamHandle, GenTL::EVENT_NEW_BUFFER, &mNewBufferEventHandle));
}

void DeviceConnection::unregisterNewBufferEvent()
{
  CC(mTl, mTl->GCUnregisterEvent(mDataStreamHandle, GenTL::EVENT_NEW_BUFFER));
}

void logBufferInformation(GenTLApi* tl,
                          std::ostream& logFile,
                          GenTL::DS_HANDLE dataStreamHandle,
                          size_t bufferNum,
                          GenTL::BUFFER_HANDLE bufferHandle)
{
  GenTL::INFO_DATATYPE bufferInfoType = GenTL::INFO_DATATYPE_UNKNOWN;
  bool8_t bufferIncomplete;
  size_t bufferFilledSize;
  size_t bufferDataSize;
  size_t bufferInfoSize;
  uint64_t bufferFrameID;

  bufferInfoSize = sizeof(bufferIncomplete);

  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_IS_INCOMPLETE,
                         &bufferInfoType,
                         &bufferIncomplete,
                         &bufferInfoSize));
  bufferInfoSize = sizeof(bufferFilledSize);
  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_SIZE_FILLED,
                         &bufferInfoType,
                         &bufferFilledSize,
                         &bufferInfoSize));
  bufferInfoSize = sizeof(bufferDataSize);
  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_DATA_SIZE,
                         &bufferInfoType,
                         &bufferDataSize,
                         &bufferInfoSize));
  bufferInfoSize = sizeof(bufferFrameID);
  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_FRAMEID,
                         &bufferInfoType,
                         &bufferFrameID,
                         &bufferInfoSize));

  logFile << bufferNum << ";" << bufferFrameID << ";" << bufferIncomplete << ";"
          << bufferFilledSize << ";" << bufferDataSize << "\n";
}

void logBufferInformation(GenTLApi* tl,
                          DeviceConnection& device,
                          size_t bufferNum,
                          GenTL::BUFFER_HANDLE bufferHandle)
{
  logBufferInformation(
    tl, device.mLog, device.mDataStreamHandle, bufferNum, bufferHandle);
}

void configureAcquisition(GenApi::CNodeMapRef& device, int64_t bufferHeight)
{
  // Switch to Continuous Acquisition
  GenApi::CEnumerationPtr aquisitionMode = device._GetNode("AcquisitionMode");
  *aquisitionMode = "Continuous";

  // Switch to 3D
  GenApi::CEnumerationPtr deviceType = device._GetNode("DeviceScanType");
  *deviceType = "Linescan3D";

  // Set the number of lines in buffer
  GenApi::CEnumerationPtr regionSelector = device._GetNode("RegionSelector");
  *regionSelector = "Scan3dExtraction1";

  // This sample is not updated to handle multipart acquisition yet, disable
  // reflectance to ensure that no multipart data arrives
  GenApi::CEnumerationPtr componentSelector =
    device._GetNode("ComponentSelector");
  *componentSelector = "Reflectance";
  GenApi::CBooleanPtr reflectanceEnable = device._GetNode("ComponentEnable");
  *reflectanceEnable = false;

  GenApi::CIntegerPtr height = device._GetNode("Height");
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

  // Set AcquisitionLineRate
  GenApi::CFloatPtr acquisitionLineRate =
    device._GetNode("AcquisitionLineRate");
  acquisitionLineRate->SetValue(30000.f);
}

int64_t getValueOrDefault(GenApi::CIntegerPtr node, int64_t defaultValue)
{
  if (node.IsValid())
  {
    return node->GetValue();
  }
  return defaultValue;
}

void printStatistics(GenApi::CNodeMapRef& dataStreamNodeMap, std::ostream& out)
{
  // For logging of receiver statistics after test run, complete list
  // of statistics
  GenApi::CIntegerPtr seenPacketCount =
    dataStreamNodeMap._GetNode("GevStreamSeenPacketCount");
  GenApi::CIntegerPtr lostPacketCount =
    dataStreamNodeMap._GetNode("GevStreamLostPacketCount");
  GenApi::CIntegerPtr resendPacketCount =
    dataStreamNodeMap._GetNode("GevStreamResendPacketCount");
  GenApi::CIntegerPtr deliveredPacketCount =
    dataStreamNodeMap._GetNode("GevStreamDeliveredPacketCount");
  GenApi::CIntegerPtr unavailablePacketCount =
    dataStreamNodeMap._GetNode("GevStreamUnavailablePacketCount");
  GenApi::CIntegerPtr duplicatePacketCount =
    dataStreamNodeMap._GetNode("GevStreamDuplicatePacketCount");
  GenApi::CIntegerPtr skippedBlockCount =
    dataStreamNodeMap._GetNode("GevStreamSkippedBlockCount");
  GenApi::CIntegerPtr engineUnderrunCount =
    dataStreamNodeMap._GetNode("GevStreamEngineUnderrunCount");
  GenApi::CIntegerPtr discaardeBlockCount =
    dataStreamNodeMap._GetNode("GevStreamDiscardedBlockCount");
  GenApi::CIntegerPtr incompleteBlockCount =
    dataStreamNodeMap._GetNode("GevStreamIncompleteBlockCount");
  GenApi::CIntegerPtr oversizedBlockCount =
    dataStreamNodeMap._GetNode("GevStreamOversizedBlockCount");

  if (!seenPacketCount.IsValid())
  {
    // No statistics available
    return;
  }

  // Log receiver statistics.
  out << "Buffer acquisition statistics End" << std::endl
      << "Stream receiver statistics Start" << std::endl
      << "Seen Packet Count: " << seenPacketCount->GetValue() << std::endl
      << "Lost Packet Count: " << lostPacketCount->GetValue() << std::endl
      << "Delivered Packet Count: " << deliveredPacketCount->GetValue()
      << std::endl
      << "Unavailable Packet Count: " << unavailablePacketCount->GetValue()
      << std::endl
      << "Duplicate Packet Count: " << duplicatePacketCount->GetValue()
      << std::endl
      << "Resend Packet Count: " << resendPacketCount->GetValue() << std::endl
      << "Skipped Block Count: " << skippedBlockCount->GetValue() << std::endl
      << "Engine Underrun Count: " << engineUnderrunCount->GetValue()
      << std::endl
      << "Discarded Block Count: " << discaardeBlockCount->GetValue()
      << std::endl
      << "Incomplete Block Count: " << incompleteBlockCount->GetValue()
      << std::endl
      << "Oversized Block Count: " << oversizedBlockCount->GetValue()
      << std::endl
      //<< "Stream Thread Prio: "
      //<< threadPriority->GetValue() << std::endl
      << "Stream receiver statistics End" << std::endl;
}


/** Saves a buffer in 12p format as a 16-bit file */
void save12bitBufferIn16bitFormat(GenTLApi* tl,
                                  GenTL::DS_HANDLE dataStreamHandle,
                                  GenTL::BUFFER_HANDLE bufferHandle,
                                  uint8_t* buffer16Data,
                                  int64_t buffer16Size,
                                  const Aoi& aoi,
                                  GenIRanger::RangeAxis rangeAxis,
                                  const std::string& path)
{
  GenTL::INFO_DATATYPE bufferInfoType = GenTL::INFO_DATATYPE_UNKNOWN;
  size_t bufferInfoSize = sizeof(size_t);

  uint8_t* buffer12Data;
  size_t width;
  size_t height;

  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_BASE,
                         &bufferInfoType,
                         &buffer12Data,
                         &bufferInfoSize));

  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_WIDTH,
                         &bufferInfoType,
                         &width,
                         &bufferInfoSize));
  CC(tl,
     tl->DSGetBufferInfo(dataStreamHandle,
                         bufferHandle,
                         GenTL::BUFFER_INFO_HEIGHT,
                         &bufferInfoType,
                         &height,
                         &bufferInfoSize));

  size_t buffer12Size = width * height * 3 / 2;

  GenIRanger::convert12pTo16(
    buffer12Data, buffer12Size, buffer16Data, &buffer16Size);

  // Save buffer to disk
  GenIRanger::saveBuffer16(buffer16Data,
                           width,
                           height,
                           aoi.mHeight,
                           aoi.mOffsetX,
                           aoi.mOffsetY,
                           rangeAxis,
                           path);
}

void clearPartialBuffers(GenTLApi* tl,
                         std::shared_ptr<DeviceConnection> deviceConnection)
{
  // Wait only short time
  const uint64_t timeoutFast = 20;
  for (;;)
  {
    // Wait for buffer to be received
    GenTL::EVENT_NEW_BUFFER_DATA event;
    size_t eventSize = sizeof(event);
    GenTL::GC_ERROR result = tl->EventGetData(
      deviceConnection->mNewBufferEventHandle, &event, &eventSize, timeoutFast);

    GenTL::BUFFER_HANDLE bufferHandle = event.BufferHandle;

    if (result == GenTL::GC_ERR_SUCCESS)
    {
      deviceConnection->mLog << "Flush buffer \n: ";
      logBufferInformation(tl, *deviceConnection, 0, bufferHandle);

      // Re-queue buffer
      CC(tl,
         tl->DSQueueBuffer(deviceConnection->mDataStreamHandle, bufferHandle));
    }
    else if (result == GenTL::GC_ERR_TIMEOUT)
    {
      // No more buffer in queue, we are done
      break;
    }
  }
}

bool isEscapeKeyPressed()
{
  return _kbhit() && VK_ESCAPE == _getch();
}

// Make sure this application runs at the highest priority to ensure that
// all buffers can be recorded.
void setProcessPriority(void)
{
  if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
  {
    std::cerr << "Failed to set priority class REALTIME." << std::endl;
    exit(1);
  }

  if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST))
  {
    std::cerr << "Failed to set thread priority TIME CRITICAL." << std::endl;
    exit(1);
  }

  // We have seen situations where it is necessary to set ES_DISPLAY_REQUIRED to
  // make sure that buffer recording is not disturbed. In addition we also need
  // to ensure that the processor does not enter a lower power state.
  if (!SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED
                               | ES_AWAYMODE_REQUIRED | ES_DISPLAY_REQUIRED))
  {
    std::cerr << "Failed to disable various power-saving modes." << std::endl;
    exit(1);
  }
}

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

   Acquisition can be aborted by pressing the escape key.
*/
int main(int argc, char* argv[])
{
  InputWaiter waiter;
  setProcessPriority();
  if (argc != 2)
  {
    usage(argc, argv);
    return 1;
  }

  gSavePath = argv[1];
  if (!directoryExists(gSavePath))
  {
    char cwd[MAX_PATH];
    _getcwd(cwd, MAX_PATH);
    std::cout << "The directory \"" << gSavePath << "\" does not exist.\n"
              << "Current working directory: " << cwd << std::endl;
    return 2;
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

  typedef std::vector<std::shared_ptr<DeviceConnection>> DeviceConnections;
  DeviceConnections connectedDevices;
  // Keep track of interfaces that we open, so that we can close them later
  std::set<GenTL::IF_HANDLE> openInterfaces;

  bool connectToMore = true;
  do
  {
    common::InterfaceList interfaces = consumer.getInterfaces(tlHandle);
    int i = 0;
    for (auto& theInterface : interfaces)
    {
      auto interfaceId = theInterface.first;
      auto displayName = theInterface.second;
      std::cout << "  " << i << ". " << interfaceId << std::endl;
      std::cout << "    " << displayName << std::endl;
      i++;
    }

    std::cout << "Select interface by its index: " << std::endl;
    int32_t input = common::getNumericInput();
    common::InterfaceId interfaceId =
      consumer.findInterfaceByIndex(interfaces, input);
    if (interfaceId.empty())
    {
      return 1;
    }

    GenTL::IF_HANDLE interfaceHandle = consumer.openInterfaceById(interfaceId);
    if (interfaceHandle == GENTL_INVALID_HANDLE)
    {
      return 1;
    }
    openInterfaces.insert(interfaceHandle);

    // Will print out a list of all available devices on interface
    common::DeviceList devices = consumer.getDevices(interfaceHandle);
    i = 0;
    for (auto& device : devices)
    {
      auto deviceId = device.first;
      auto displayName = device.second;
      std::cout << "  " << i << ". " << deviceId << std::endl;
      std::cout << "    " << displayName << std::endl;
      i++;
    }

    std::cout << "Select device by its index: " << std::endl;
    input = common::getNumericInput();
    std::string deviceId = consumer.findDeviceByIndex(devices, input);
    if (deviceId.empty())
    {
      return 1;
    }

    GenTL::DEV_HANDLE deviceHandle =
      consumer.openDeviceById(interfaceHandle, deviceId);
    if (deviceHandle == GENTL_INVALID_HANDLE)
    {
      return 1;
    }

    GenTL::DS_HANDLE dataStreamHandle = consumer.openDataStream(deviceHandle);
    if (dataStreamHandle == GENTL_INVALID_HANDLE)
    {
      return 1;
    }

    GenTL::INFO_DATATYPE dataType;
    char modelName[1024];
    size_t modelNameSize = sizeof(modelName);
    GenTLApi* tl = consumer.tl();
    CC(tl,
       tl->IFGetDeviceInfo(interfaceHandle,
                           deviceId.c_str(),
                           GenTL::DEVICE_INFO_MODEL,
                           &dataType,
                           modelName,
                           &modelNameSize));

    // Append user defined name if possible
    char deviceUserId[1024];
    size_t deviceUserIdSize = sizeof(deviceUserId);
    GenTL::GC_ERROR status =
      tl->IFGetDeviceInfo(interfaceHandle,
                          deviceId.c_str(),
                          GenTL::DEVICE_INFO_USER_DEFINED_NAME,
                          &dataType,
                          deviceUserId,
                          &deviceUserIdSize);


    std::stringstream deviceName;
    deviceName << modelName;
    if (status == GenTL::GC_ERR_SUCCESS)
    {
      deviceName << "_" << deviceUserId;
    }

    connectedDevices.push_back(std::make_shared<DeviceConnection>(
      tl, deviceHandle, dataStreamHandle, deviceName.str()));

    std::cout << "Connect to more devices? [y/N]: " << std::endl;
    connectToMore = _getch() == 'y';
  } while (connectToMore);

  int64_t startStopIterationsCount = 0;
  std::cout << "Number of start-stop iterations: " << std::endl;
  std::cin >> startStopIterationsCount;

  uint64_t numBuffersToAcquire = 0;
  std::cout << "Number of buffers acquire (per start-stop-iteration): "
            << std::endl;
  std::cin >> numBuffersToAcquire;

  size_t buffersCount = 0;
  std::cout << "Number of allocated buffers: ";
  std::cin >> buffersCount;

  std::cout << "Save buffers to disk? [y/N]" << std::endl;
  char answer = _getch();
  bool saveToDisk = answer == 'y';

  std::string bufferName("buffer");
  int64_t rotationBufferCount = 0;
  if (saveToDisk)
  {
    std::cout << "Buffer filename: " << std::endl;
    std::cin >> bufferName;
    std::cout << "Enter the maximum number of buffers on disk (0 = no limit):"
              << std::endl;
    std::cin >> rotationBufferCount;
  }

  // Function pointer to GenTL methods
  GenTLApi* tl = consumer.tl();

  int exitStatus = 0;

  size_t totalAllocatedMemory = 0;

  for (DeviceConnections::iterator it = connectedDevices.begin();
       it != connectedDevices.end();
       ++it)
  {
    std::shared_ptr<DeviceConnection> deviceConnection = *it;

    // Get node map for data stream module
    deviceConnection->createDataStreamNodeMap(consumer);

    // Connect device node map to GenTL
    deviceConnection->createDeviceNodeMap(consumer);
    // Node map for reading/setting device parameters
    GenApi::CNodeMapRef device = deviceConnection->mDeviceNodeMap;

    int64_t bufferHeight = 4000;
    configureAcquisition(device, bufferHeight);

    GenApi::CIntegerPtr width = device._GetNode("Width");
    GenApi::CIntegerPtr offsetX = device._GetNode("OffsetX");
    GenApi::CIntegerPtr offsetY = device._GetNode("OffsetY");
    int16_t aoiOffsetX = static_cast<uint16_t>(getValueOrDefault(offsetX, 0));
    int16_t aoiOffsetY = static_cast<uint16_t>(getValueOrDefault(offsetY, 0));

    int16_t aoiWidth = static_cast<uint16_t>(width->GetValue());
    int16_t aoiHeight = 200;
    deviceConnection->mAoi = Aoi(aoiOffsetX, aoiOffsetY, width, aoiHeight);

    int64_t bufferWidth = width->GetValue();
    size_t buffer16Size = static_cast<size_t>(bufferWidth * bufferHeight * 2);

    // Setup a sufficient amount of buffers
    GenApi::CIntegerPtr payload = device._GetNode("PayloadSize");
    const size_t payloadSize = static_cast<size_t>(payload->GetValue());

    // A temporary buffer used for 12-to-16-bit conversion before saving to
    // disk.
    if (gTempBuffer == nullptr || buffer16Size > gTempBufferSize)
    {
      gTempBufferSize = buffer16Size;
      gTempBuffer = new uint8_t[gTempBufferSize];
    }

    // Announce and queue buffers to producer, we keep track of the buffers by
    // their index in the array. These will contain the 12-bit format sent from
    // the device
    deviceConnection->initializeBuffers(buffersCount, payloadSize);
    totalAllocatedMemory += buffersCount * payloadSize;

    // Register event so that we can be notified when new buffers have
    // been received
    deviceConnection->registerNewBufferEvent();
  }

  std::cout << "Total memory used for buffers: "
            << (totalAllocatedMemory / 1024 / 1024) << " MB" << std::endl;

  bool aborted = false;
  const uint64_t timeout = 1000; // ms

  // Loop index is 1-based since it is printed to the user
  for (int j = 1; j <= startStopIterationsCount; ++j)
  {
    std::cout << "\n\nStarting iteration " << j << std::endl;

    for (DeviceConnections::iterator it = connectedDevices.begin();
         it != connectedDevices.end();
         ++it)
    {
      GenApi::CNodeMapRef device = (*it)->mDeviceNodeMap;
      GenTL::DS_HANDLE dataStreamHandle = (*it)->mDataStreamHandle;

      std::cout << "Starting acquisition of " << (*it)->mDeviceName
                << std::endl;

      // Lock all parameters before starting
      GenApi::CIntegerPtr paramsLocked = device._GetNode("TLParamsLocked");
      paramsLocked->SetValue(1);

      // Start acquisition
      CC(tl,
         tl->DSStartAcquisition(
           dataStreamHandle, GenTL::ACQ_START_FLAGS_DEFAULT, GENTL_INFINITE));

      (*it)->startAcquisition();
    }

    std::cout << "Acquiring buffers..." << std::endl;

    int64_t currentUnderrunCount = 0;
    int64_t previousUnderrunCount = 0;

    for (size_t i = 1; i < numBuffersToAcquire + 1; i++)
    {
      for (DeviceConnections::iterator it = connectedDevices.begin();
           it != connectedDevices.end();
           ++it)
      {
        std::shared_ptr<DeviceConnection> deviceConnection = *it;

        try
        {
          if (isEscapeKeyPressed())
          {
            std::cout << "Aborting acquisition..." << std::endl;
            aborted = true;
            break;
          }

          // Wait for buffer to be received
          GenTL::EVENT_NEW_BUFFER_DATA event;
          size_t eventSize = sizeof(event);
          CC(tl,
             tl->EventGetData(deviceConnection->mNewBufferEventHandle,
                              &event,
                              &eventSize,
                              timeout));

          std::cout << ".";

          GenTL::BUFFER_HANDLE bufferHandle = event.BufferHandle;

          size_t numAwaitingDelivery;
          size_t numAwaitingDeliverySize = sizeof(numAwaitingDelivery);
          GenTL::INFO_DATATYPE dataType;
          CC(tl,
             tl->DSGetInfo(deviceConnection->mDataStreamHandle,
                           GenTL::STREAM_INFO_NUM_AWAIT_DELIVERY,
                           &dataType,
                           &numAwaitingDelivery,
                           &numAwaitingDeliverySize));

          if (i + numAwaitingDelivery >= numBuffersToAcquire
              && deviceConnection->isAcquisitionRunning())
          {
            // The producer has received enough buffers, tell the device to
            // stop. We will continue to process them in our own pace
            std::cout << "\n"
                      << deviceConnection->mDeviceName
                      << " has sent enough buffers" << std::endl;
            deviceConnection->stopAcquisition();
          }

          // Log information about the received buffer
          logBufferInformation(tl, *deviceConnection, i, bufferHandle);

          GenApi::CIntegerPtr engineUnderrunCount =
            deviceConnection->mDataStreamNodeMap._GetNode(
              "GevStreamEngineUnderrunCount");
          if (engineUnderrunCount.IsValid())
          {
            currentUnderrunCount = engineUnderrunCount->GetValue();
            if (currentUnderrunCount != previousUnderrunCount)
            {
              std::cout << "U";
              deviceConnection->mLog
                << "StreamEngineUnderrunCount: " << currentUnderrunCount
                << std::endl;
            }
            previousUnderrunCount = currentUnderrunCount;
          }

          if (saveToDisk)
          {
            // Get range axis
            GenApi::CEnumerationPtr rangeAxisEnum =
              deviceConnection->mDeviceNodeMap._GetNode("RangeAxis");
            GenIRanger::RangeAxis rangeAxis =
              ("Reverse" == rangeAxisEnum->ToString())
                ? GenIRanger::RangeAxis::REVERSED
                : GenIRanger::RangeAxis::STANDARD;

            // Append loop index to buffer name
            size_t fileSuffix = i;
            if (rotationBufferCount != 0)
            {
              fileSuffix = i % rotationBufferCount;
            }

            std::stringstream bufferPath;
            bufferPath << gSavePath << "\\" << bufferName << "-"
                       << deviceConnection->mDeviceName << "-" << fileSuffix;

            uint8_t* bufferData = static_cast<uint8_t*>(event.pUserPointer);
            save12bitBufferIn16bitFormat(tl,
                                         deviceConnection->mDataStreamHandle,
                                         event.BufferHandle,
                                         gTempBuffer,
                                         gTempBufferSize,
                                         deviceConnection->mAoi,
                                         rangeAxis,
                                         bufferPath.str());
          }

          // Re-queue buffer
          CC(tl,
             tl->DSQueueBuffer(deviceConnection->mDataStreamHandle,
                               bufferHandle));
        }
        catch (const std::exception& e)
        {
          std::cout << std::endl
                    << "Error from " << deviceConnection->mDeviceName
                    << std::endl;
          std::cout << e.what() << std::endl;
          exitStatus = 1;
        }
      }
      if (aborted)
      {
        break;
      }
    }

    // Force a newline after previous progress output
    std::cout << std::endl;

    for (DeviceConnections::iterator it = connectedDevices.begin();
         it != connectedDevices.end();
         ++it)
    {
      std::shared_ptr<DeviceConnection> deviceConnection = *it;
      GenApi::CNodeMapRef device = deviceConnection->mDeviceNodeMap;
      GenTL::DS_HANDLE dataStreamHandle = deviceConnection->mDataStreamHandle;

      // Tell the device to stop acquisition
      deviceConnection->stopAcquisition();

      Sleep(500);

      // We might receive a partial buffer after stopping acquisition. Clear any
      // possible "leftover" buffers
      clearPartialBuffers(tl, deviceConnection);

      CC(tl,
         tl->DSStopAcquisition(dataStreamHandle, GenTL::ACQ_STOP_FLAGS_KILL));
      GenApi::CIntegerPtr paramsLocked = device._GetNode("TLParamsLocked");
      paramsLocked->SetValue(0);

      printStatistics(deviceConnection->mDataStreamNodeMap,
                      deviceConnection->mLog);
    }
    if (aborted)
    {
      break;
    }
  }

  if (saveToDisk)
  {
    std::cout << "Saved buffers to " << gSavePath << std::endl;
  }

  for (DeviceConnections::iterator it = connectedDevices.begin();
       it != connectedDevices.end();
       ++it)
  {
    std::shared_ptr<DeviceConnection> deviceConnection = *it;

    GenTL::DS_HANDLE dataStreamHandle = deviceConnection->mDataStreamHandle;

    // Discard all buffers so that they can be revoked from stream
    CC(tl, tl->DSFlushQueue(dataStreamHandle, GenTL::ACQ_QUEUE_ALL_DISCARD));
    deviceConnection->unregisterNewBufferEvent();

    deviceConnection->teardownBuffers();

    consumer.closeDataStream(dataStreamHandle);
    consumer.closeDevice(deviceConnection->mDeviceHandle);
  }

  delete[] gTempBuffer;
  gTempBuffer = nullptr;

  for (std::set<GenTL::IF_HANDLE>::iterator it = openInterfaces.begin();
       it != openInterfaces.end();
       ++it)
  {
    consumer.closeInterface(*it);
  }
  consumer.close();

  return exitStatus;
}
