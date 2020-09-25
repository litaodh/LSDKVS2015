// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

#include <string>

namespace GenIRanger { namespace GenIUtil {

/** Writes the message to std::out if LOG_ENABLED is defined. Does not flush. */
void log(std::string message);

/** Throws a GenIRangerException is the provided message and writes it to
    std::out if LOG_ENABLED is defined. Appends endl and flushes.
*/
void throwAndLog(std::string message);

}}
