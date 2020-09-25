// Copyright 2016-2019 SICK AG. All rights reserved.

#include "GenIRanger.h"
#include "SampleUtils.h"
#include "SingleDeviceConsumer.h"

#include <fstream>
#include <string>

void usage(int, char* argv[])
{
  std::cout << "Usage:" << std::endl
            << argv[0] << " <file path for parameter file>" << std::endl;
  common::printDeviceOptionsHelp();
  std::cout << std::endl;
  std::cout
    << "File path should include the name of the file to save including "
    << "extension." << std::endl
    << "E.g. C:\\ParameterFiles\\parameters.csv" << std::endl;
}

int main(int argc, char* argv[])
{
  InputWaiter waiter;
  common::DeviceSelector selector;

  if (!common::parseDeviceOptionsHelper(argc, argv, selector))
  {
    return 1;
  }

  std::string filePath = "saved_parameters.csv";
  if (!common::parseArgumentHelper(
        argc,
        argv,
        "file path for parameter file including extension",
        filePath))
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

  std::ofstream file(filePath);
  GenIRanger::exportDeviceParameters(device._Ptr, file);
  file.close();

  std::cout << "Parameter file exported to " << filePath << std::endl;

  consumer.closeDevice(deviceHandle);
  consumer.closeInterface(interfaceHandle);
  consumer.close();

  return 0;
}
