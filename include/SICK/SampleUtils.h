// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

#include "DeviceSelector.h"

#include <cstdint>
#include <iostream>
#include <string>

// Avoid name collision with macro in Windows.h
#undef max

class __declspec(dllexport) InputWaiter
{
public:
  ~InputWaiter()
  {
    // Flush cin to get rid of any newline characters left from using cin
    // to input parameters earlier.
    if (std::cin.rdbuf()->in_avail() > 0)
    {
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cout << "Press enter to continue..." << std::endl;
    std::getchar();
  }
};

namespace common {
/**  Returns the absolute path to the SICKGigEVisionTL.cti-file which
     should be located next to the built executable.
*/
std::string __declspec(dllexport) getPathToProducer();

/** Waits for user to input a numerical value from 0-9, pressing ESC aborts */
int32_t __declspec(dllexport) getNumericInput();

/** Get current date/time, format is YYYYMMDD-HHmmss */
std::string __declspec(dllexport) currentDateTime();

/** Format time parameter to to YYYYMMDD-HHmmss */
std::string __declspec(dllexport) formatTime(time_t time);

/** Helper function to parse a single argument on the command line.

    If no argument is given on the command line the user is prompted
    for a value and argumentValue is updated with the user's input. If
    argumentValue is not empty the user will be given the option of
    using this as the default value.

    If a single argument is given on the command line argumentValue
    will be set to this value.

    If any option on the command line contains -h or --help false is
    returned. If more than one argument is given on the command line
    false is returned. Otherwise true is returned.
*/
bool __declspec(dllexport) parseArgumentHelper(int argc,
                         char* argv[],
                         const std::string& argumentName,
                         std::string& argumentValue);

/** Look for --mac or --ip on the command line and initialize the
    specified DeviceSelector if found. If a --mac or --ip option is
    detected it is removed from argc and argv.

    Returns false if an invalid MAC or IP address has been specified
    or if more than one --mac or --ip options is given. Returns true
    otherwise.
*/
bool __declspec(dllexport) parseDeviceOptionsHelper(int& argc,
                              char* argv[],
                              DeviceSelector& selector);

/** Prints a help message describing the --mac and --ip options. */
void __declspec(dllexport) printDeviceOptionsHelp();

/** Parses an IPv4 address in dotted representation.
    For example:
    ip4FromString("192.168.0.1") -> 0xc0a80001
*/
uint32_t __declspec(dllexport) ip4FromString(const std::string& address);

/** Given an IPV4 address returns a string in dotted
    representation.
*/
std::string __declspec(dllexport) stringFromIp4(uint32_t address);

/** Creates a colon-separated hex representation of the given MAC
    address.
*/
std::string __declspec(dllexport) stringFromMac(uint64_t address);

/** Prints a list to stdout of the devices that the producer has
    found. The list includes GigEVision specifics, such as IP and MAC
    addresses.
*/
void __declspec(dllexport) showDeviceList(GenApi::CNodeMapRef& interfaceNodeMap);

/** Allow the user to interactively select a device to connect to. The
    consumer must have been opened before this call.

    Returns a device handle to the selected device, or
    GENTL_INVALID_HANDLE if an error occurred. If a valid device
    handle is returned, interfaceHandle will be updated with a handle
    to the interface the device is connected to.
*/
GenTL::DEV_HANDLE __declspec(dllexport) openDeviceInteractively(common::Consumer& consumer,
                                          GenTL::IF_HANDLE& interfaceHandle);


/** Sets the heartbeat timeout in us for the given local device.
 */
void __declspec(dllexport) setHeartbeatTimeout(const GenTL::DEV_HANDLE deviceHandle,
                         GenTLApi* tl,
                         common::Consumer& consumer,
                         float timeout);
}
