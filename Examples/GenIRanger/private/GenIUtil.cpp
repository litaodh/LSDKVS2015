// Copyright 2016-2019 SICK AG. All rights reserved.

#include "GenIUtil.h"

#include "Exceptions.h"

#include <iostream>

namespace GenIRanger { namespace GenIUtil {

void log(std::string message)
{
#ifdef LOG_ENABLED
  std::cout << message;
#endif
}

void throwAndLog(std::string message)
{
#ifdef LOG_ENABLED
  std::cout << message << std::endl;
#endif
  throw GenIRanger::GenIRangerException(message);
}

}}
