// Copyright 2016-2019 SICK AG. All rights reserved.

#include "Consumer.h"
#include "DeviceLogWriter.h"
#include "GenIRanger.h"
#include "SampleUtils.h"

#include <GenApi/EventAdapterGEV.h>

#include <conio.h>
#include <string>

void usage(int, char* argv[])
{
  std::cout << "Usage:" << std::endl
            << argv[0] << " <file path for log>" << std::endl;
  common::printDeviceOptionsHelp();
  std::cout << std::endl;
  std::cout << "File path should include the name of the file to save."
            << std::endl
            << "E.g. C:\\DeviceLogs\\log" << std::endl;
}

void provokeErrorMessage(GenApi::CNodeMapRef& device)
{
  try
  {
    // Set exposure time to illegal value to provoke a log message from device.
    GenApi::CFloatPtr exposureTime = device._GetNode("ExposureTime");
    exposureTime->SetValue(-1, false);
  }
  catch (std::runtime_error& e)
  {
    std::cout << std::endl
              << "Writing negative exposure time to provoke error event: "
              << e.what() << std::endl;
  }
}

int main(int argc, char* argv[])
{
  InputWaiter waiter;
  common::DeviceSelector selector;

  if (!common::parseDeviceOptionsHelper(argc, argv, selector))
  {
    return 1;
  }

  std::string filePath = "saved_events.log";
  if (!common::parseArgumentHelper(
        argc, argv, "file path for event log", filePath))
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

  // Setup logging to file, listens to events sent from the device
  GenTL::EVENT_HANDLE hEvent;
  size_t eventSizeMax;
  size_t eventSizeMaxSize = sizeof(eventSizeMaxSize);
  GenTL::INFO_DATATYPE dataType;
  // Maximum size of the event
  CC(tl,
     tl->GCRegisterEvent(deviceHandle, GenTL::EVENT_REMOTE_DEVICE, &hEvent));
  CC(tl,
     tl->EventGetInfo(hEvent,
                      GenTL::EVENT_SIZE_MAX,
                      &dataType,
                      &eventSizeMax,
                      &eventSizeMaxSize));
  // Allocate buffer that will hold the event
  uint8_t* eventBuffer = new uint8_t[eventSizeMax];

  // The adapter takes care of updating the node map from an event buffer
  GenApi::CEventAdapterGEV adapter;
  adapter.AttachNodeMap(device._Ptr);
  // New scope to dispose of DeviceLogWriter before detaching the node map
  {
    GenIRanger::DeviceLogWriter log(device._Ptr, filePath, 1000000);

    std::cout << "Logging device events to " << filePath.c_str() << std::endl;
    while (true)
    {
      std::cout << "Press <enter> for next event or <q> to exit." << std::endl;
      int c = getchar();
      if (c == EOF || c == 'q' || c == 'Q')
      {
        break;
      }

      provokeErrorMessage(device);

      // Wait for device to send event
      CC(tl,
         tl->EventGetData(hEvent, eventBuffer, &eventSizeMax, GENTL_INFINITE));
      // Push event data into node map, it has the knowledge of how the event
      // structure looks like and will trigger the callback registered in
      // DeviceLogWriter
      adapter.DeliverMessage(reinterpret_cast<uint8_t*>(eventBuffer),
                             static_cast<uint32_t>(eventSizeMax));
      std::cout << "Event logged!" << std::endl;
    }
  }
  adapter.DetachNodeMap();

  delete[] eventBuffer;

  consumer.closeDevice(deviceHandle);
  consumer.closeInterface(interfaceHandle);
  consumer.close();

  return 0;
}
