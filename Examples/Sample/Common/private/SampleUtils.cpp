// Copyright 2016-2019 SICK AG. All rights reserved.

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif

#include "SampleUtils.h"

#include "Consumer.h"
#include "DeviceSelector.h"

#include <cctype>
#include <conio.h>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

namespace common {

bool fileExists(const std::string& pathToFile)
{
  std::ifstream file(pathToFile.c_str());
  return file.good();
}

std::string getPathToProducer()
{
  char pathToExe[FILENAME_MAX];

  // First we want to verify that the user has placed the .cti-file in the same
  // folder as the built executable
  if (!GetModuleFileName(nullptr, pathToExe, sizeof(pathToExe)))
  {
    std::cout << "Could not verify existence of .cti-file" << std::endl;
    return "";
  }
  std::string path = std::string(pathToExe);
  // Find position that will denote the directory path
  std::size_t pos = path.find_last_of("\\");
  if (pos != std::string::npos)
  {
    path = path.substr(0, pos);
  }
  std::string pathToCti = std::string(path) + "\\SICKGigEVisionTL.cti";
  if (!fileExists(pathToCti))
  {
    std::cout
      << "Could not locate SICKGigEVisionTL.cti." << std::endl
      << "Please make sure to place the .cti-file in the same directory "
      << "as the built sample executable." << std::endl
      << pathToCti << std::endl;
    return "";
  }

  return pathToCti;
}

int32_t getNumericInput()
{
  char input;
  do
  {
    input = _getch();
    if (isdigit(input))
    {
      return (int32_t)(input - '0');
    }
    // ESC aborts input
    if (input == 27)
    {
      break;
    }
  } while (true);

  return -1;
}

std::string currentDateTime()
{
  return formatTime(time(0));
}

std::string formatTime(time_t time)
{
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&time);
  strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S", &tstruct);
  return std::string(buf);
}

static bool handleArgument(int& argc,
                           char* argv[],
                           int idx,
                           const std::string& argName,
                           std::string& result,
                           bool& failure)
{
  std::string currentArgument = argv[idx];
  if (currentArgument == argName)
  {
    if (idx + 1 >= argc)
    {
      std::cerr << "ERROR: No argument given for " + argName << std::endl;
      failure = true;
      return false;
    }
    result = argv[idx + 1];
    for (int j = idx; j < argc - 2; j++)
    {
      argv[j] = argv[j + 2];
    }
    argc = argc - 2;
    return true;
  }
  return false;
}

bool parseDeviceOptionsHelper(int& argc, char* argv[], DeviceSelector& selector)
{
  bool failure = false;
  int idx = 1;
  while (idx < argc)
  {
    std::string macAddr;
    std::string ipAddr;
    if (handleArgument(argc, argv, idx, "--mac", macAddr, failure))
    {
      if (selector.isSelected())
      {
        std::cerr << "ERROR: A device has already been specified" << std::endl;
        failure = true;
      }
      int a, b, c, d, e, f;
      char
        dummy; // To make sure there is nothing after the last part of the MAC
      if (sscanf(macAddr.c_str(),
                 "%x:%x:%x:%x:%x:%x%c",
                 &a,
                 &b,
                 &c,
                 &d,
                 &e,
                 &f,
                 &dummy)
          == 6)
      {
        int sanityCheck = (a | b | c | d | e | f);
        if (sanityCheck != (sanityCheck & 0xff))
        {
          std::cerr << "ERROR: " << macAddr << " is not a legal MAC address"
                    << std::endl;
          failure = true;
        }
        uint64_t mac = static_cast<uint64_t>(a) << 40ULL;
        mac = (static_cast<uint64_t>(b) << 32ULL) | mac;
        mac = (static_cast<uint64_t>(c) << 24ULL) | mac;
        mac = (static_cast<uint64_t>(d) << 16ULL) | mac;
        mac = (static_cast<uint64_t>(e) << 8ULL) | mac;
        mac = (static_cast<uint64_t>(f) << 0ULL) | mac;
        selector.selectDeviceMac(mac);
      }
      else
      {
        std::cerr << "ERROR: \"" << macAddr << "\" is not a valid MAC address"
                  << std::endl;
        failure = true;
      }
    }

    else if (handleArgument(argc, argv, idx, "--ip", ipAddr, failure))
    {
      uint32_t ip;
      if (selector.isSelected())
      {
        std::cerr << "ERROR: A device has already been specified" << std::endl;
        failure = true;
      }
      else if (InetPton(AF_INET, ipAddr.c_str(), &ip) != 1)
      {
        std::cerr << "ERROR: " << ipAddr << " is not a valid IP address"
                  << std::endl;
        failure = true;
      }
      else
      {
        selector.selectDeviceIp(ntohl(ip));
      }
    }
    else
    {
      idx++;
    }
  }
  return !failure;
}

void printDeviceOptionsHelp()
{
  std::cout << "Device selection options:" << std::endl << std::endl;
  std::cout << "   --mac xx:xx:xx:xx:xx:xx   Connect to the device "
               "with the specified mac"
            << std::endl;
  std::cout << "   --ip x.x.x.x              Connect to the device "
               "with the specified ip"
            << std::endl;
}

bool parseArgumentHelper(int argc,
                         char* argv[],
                         const std::string& argumentName,
                         std::string& argumentValue)
{
  for (int i = 0; i < argc; i++)
  {
    if (std::string(argv[i]) == "--help")
    {
      return false;
    }
    if (std::string(argv[i]) == "-h")
    {
      return false;
    }
  }

  if (argc == 2)
  {
    argumentValue = argv[1];
    return true;
  }

  if (argc > 2)
  {
    std::cerr << "ERROR: Too many arguments" << std::endl;
    return false;
  }

  std::cout << "No argument given for the \"" << argumentName
            << "\" parameter ";
  std::cout << " on the command line.";
  std::cout << " Please enter a value for this parameter";
  if (!argumentValue.empty())
  {
    std::cout << " (default: " << argumentValue << ")";
  }
  std::cout << ": ";
  std::cout.flush();
  std::string tmp;
  getline(std::cin, tmp);
  if (!tmp.empty())
  {
    argumentValue = tmp;
    return true;
  }
  else
  {
    if (!argumentValue.empty())
    {
      // No change in the argument value
      return true;
    }
    else
    {
      std::cerr << "ERROR: Empty argument given" << std::endl;
      return false;
    }
  }
}

uint32_t ip4FromString(const std::string& address)
{
  uint32_t ipAddress;
  int result = InetPton(AF_INET, address.c_str(), &ipAddress);
  if (result != 1)
  {
    std::cerr << "Invalid ip address: \"" << address << "\"" << std::endl;
    exit(2);
  }
  return ntohl(ipAddress);
}

std::string stringFromIp4(uint32_t address)
{
  char str[INET_ADDRSTRLEN];
  uint32_t addressNetworkByteOrder = htonl(address);
  InetNtop(AF_INET, &addressNetworkByteOrder, str, sizeof(str));
  return str;
}

/* Creates a colon-separated hex representation of the given MAC address */
std::string stringFromMac(uint64_t address)
{
  std::stringstream ss;
  const size_t bytesInMac = 6;
  for (size_t i = 0; i < bytesInMac; ++i)
  {
    ss << std::setfill('0') << std::setw(2) << std::hex
       << ((address >> (bytesInMac - i - 1) * 8) & 0xff);
    if (i != bytesInMac - 1)
    {
      ss << ":";
    }
  }
  return ss.str();
}

void showDeviceList(GenApi::CNodeMapRef& interfaceNodeMap)
{
  GenApi::CIntegerPtr deviceSelector =
    interfaceNodeMap._GetNode("DeviceSelector");

  GenApi::CStringPtr deviceModelName =
    interfaceNodeMap._GetNode("DeviceModelName");
  GenApi::CStringPtr deviceUserId = interfaceNodeMap._GetNode("DeviceUserID");
  GenApi::CStringPtr deviceSerialNumber =
    interfaceNodeMap._GetNode("DeviceSerialNumber");
  GenApi::CStringPtr deviceId = interfaceNodeMap._GetNode("DeviceID");
  GenApi::CIntegerPtr gevDeviceIpAddress =
    interfaceNodeMap._GetNode("GevDeviceIPAddress");
  GenApi::CIntegerPtr gevDeviceMacAddress =
    interfaceNodeMap._GetNode("GevDeviceMACAddress");

  for (int64_t i = 0; i <= deviceSelector->GetMax(); ++i)
  {
    deviceSelector->SetValue(i);
    uint32_t currentIp = static_cast<uint32_t>(gevDeviceIpAddress->GetValue());
    uint64_t currentMac =
      static_cast<uint64_t>(gevDeviceMacAddress->GetValue());


    std::cout << i << ". DeviceModelName: " << deviceModelName->GetValue()
              << std::endl;
    std::cout << "   DeviceUserID: " << deviceUserId->GetValue() << std::endl;
    std::cout << "   DeviceSerialNumber: " << deviceSerialNumber->GetValue()
              << std::endl;
    std::cout << "   DeviceIPAddress: " << stringFromIp4(currentIp)
              << std::endl;
    std::cout << "   DeviceMACAddress: " << stringFromMac(currentMac)
              << std::endl;
    std::cout << "   DeviceID: " << deviceId->GetValue() << std::endl;
    std::cout << std::endl;
  }
}

GenTL::DEV_HANDLE openDeviceInteractively(common::Consumer& consumer,
                                          GenTL::IF_HANDLE& interfaceHandle)
{
  auto tlHandle = consumer.tlHandle();
  auto tl = consumer.tl();

  std::cout << std::endl;
  std::cout << "Interface list:" << std::endl;
  std::cout << std::endl;

  // Will print out a list of all available interfaces
  auto interfaces = consumer.getInterfaces(tlHandle);
  int i = 0;
  for (auto& theInterface : interfaces)
  {
    auto interfaceId = theInterface.first;
    auto displayName = theInterface.second;
    std::cout << "  " << i << ". " << interfaceId << std::endl;
    std::cout << "    " << displayName << std::endl;
    i++;
  }
  std::cout << std::endl;
  std::cout << "Select interface by its index: ";
  std::cout.flush();

  int32_t input = -1;
  std::cin >> input;
  if (input < 0)
  {
    std::cerr << "Invalid interface selection" << std::endl;
    return GENTL_INVALID_HANDLE;
  }

  common::InterfaceId interfaceId =
    consumer.findInterfaceByIndex(interfaces, input);
  if (interfaceId.empty())
  {
    return GENTL_INVALID_HANDLE;
  }

  interfaceHandle = consumer.openInterfaceById(interfaceId);
  if (interfaceHandle == GENTL_INVALID_HANDLE)
  {
    return GENTL_INVALID_HANDLE;
  }

  // Create a NodeMap of the GenTL Producer interface module
  // This allows access to GigEVision specific functionality, such as ForceIP,
  // that is not part of the GenTL C api.
  common::GenTLPort interfacePort = common::GenTLPort(interfaceHandle, tl);
  GenApi::CNodeMapRef interfaceNodeMap =
    consumer.getNodeMap(&interfacePort, "InterfacePort");
  // Perform a scan, to find devices on the interface
  bool8_t changed = false;
  CC(tl, tl->IFUpdateDeviceList(interfaceHandle, &changed, 500));
  uint32_t deviceCount = 0;
  CC(tl, tl->IFGetNumDevices(interfaceHandle, &deviceCount));
  if (deviceCount == 0)
  {
    std::cerr << "No device found" << std::endl;
    return GENTL_INVALID_HANDLE;
  }
  showDeviceList(interfaceNodeMap);

  std::cout << std::endl;
  std::cout << "Select device by its index: ";
  std::cout.flush();

  input = -1;
  std::cin >> input;

  GenApi::CIntegerPtr deviceSelector =
    interfaceNodeMap._GetNode("DeviceSelector");
  if (input < 0)
  {
    std::cerr << "Invalid device selection" << std::endl;
    return GENTL_INVALID_HANDLE;
  }
  if (input > deviceSelector->GetMax())
  {
    std::cerr << "Invalid device selection" << std::endl;
    return GENTL_INVALID_HANDLE;
  }

  deviceSelector->SetValue(input);
  GenApi::CStringPtr deviceId = interfaceNodeMap._GetNode("DeviceID");
  if (deviceId->GetValue().empty())
  {
    return GENTL_INVALID_HANDLE;
  }

  DeviceId deviceIdString = deviceId->GetValue();

  GenTL::DEV_HANDLE deviceHandle =
    consumer.openDeviceById(interfaceHandle, deviceIdString);
  return deviceHandle;
}

void setHeartbeatTimeout(const GenTL::DEV_HANDLE deviceHandle,
                         GenTLApi* tl,
                         common::Consumer& consumer,
                         float timeout)
{
  std::vector<char> portName;
  size_t portNameSize = 0;
  GenTL::INFO_DATATYPE type;
  CC(tl,
     tl->GCGetPortInfo(deviceHandle,
                       GenTL::PORT_INFO_CMD_LIST::PORT_INFO_PORTNAME,
                       &type,
                       NULL,
                       &portNameSize));
  portName.resize(portNameSize);
  CC(tl,
     tl->GCGetPortInfo(deviceHandle,
                       GenTL::PORT_INFO_CMD_LIST::PORT_INFO_PORTNAME,
                       &type,
                       portName.data(),
                       &portNameSize));

  GenTLPort localDevicePort = GenTLPort(deviceHandle, tl);
  GenApi::CNodeMapRef localDeviceNodeMap =
    consumer.getNodeMap(&localDevicePort, portName.data());

  GenApi::CFloatPtr heartbeatTimeout =
    localDeviceNodeMap._GetNode("DeviceLinkHeartbeatTimeout");
  *heartbeatTimeout = timeout;
}

}
