// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

#include <exception>
#include <string>

namespace GenIRanger {
/** Generic GenIRanger Exception class. Thrown whenever an error occurs either
    in GenIRanger or GenICam. The messages from GenICam exceptions are wrapped
    into GenIRanger Exceptions.
*/
class GenIRangerException : public std::exception
{
public:
  GenIRangerException(const std::string& message);
};

/** Indicates that something went wrong during export configuration from device.
 */
class ExportException : public GenIRangerException
{
public:
  ExportException(const std::string& message);
};

/** Indicates that something went wrong while writing device log to disk. */
class DeviceLogException : public GenIRangerException
{
public:
  DeviceLogException(const std::string& message);
};

/** Indicates that there is some problem when saving data to disk. */
class SaveException : public GenIRangerException
{
public:
  SaveException(const std::string& message);
};

}
