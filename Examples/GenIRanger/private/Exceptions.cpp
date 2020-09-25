// Copyright 2016-2019 SICK AG. All rights reserved.

#include "Exceptions.h"

namespace GenIRanger {

GenIRangerException::GenIRangerException(const std::string& message)
  : std::exception(message.c_str())
{
  // Empty
}

ExportException::ExportException(const std::string& message)
  : GenIRangerException(message)
{
  // Empty
}

DeviceLogException::DeviceLogException(const std::string& message)
  : GenIRangerException(message)
{
  // Empty
}

SaveException::SaveException(const std::string& message)
  : GenIRangerException(message)
{
  // Empty
}

}
