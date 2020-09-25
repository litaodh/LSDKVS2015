// Copyright 2018-2019 SICK AG. All rights reserved.

#pragma once

#include <string>

namespace GenIRanger {
/** Represents the result of a configuration file import.

    The enum values are set explicitly and should not be changed, in order to
    simplify customer support.
 */
enum class ConfigurationStatus
{
  /** Successfully imported */
  OK = 0,

  // Warnings
  /** A partial CSV file was successfully imported */
  PARTIAL_IMPORT = 100,

  // Errors
  /** Generic error */
  GENERIC_ERROR = 200,
  /** Bad version in the parameter file */
  BAD_VERSION = 201,
  /** The parameter file is empty */
  EMPTY_PARAMETER_FILE = 202,
  /** One or more parameters are duplicated in the parameter file. */
  DUPLICATE_PARAMETER = 203,
  /** One or more parameters are not valid */
  INVALID_CONFIGURATION = 204,
};

struct ConfigurationResult
{
public:
  ConfigurationResult(ConfigurationStatus status, std::string message)
    : status(status)
    , message(message)
  {
    // Empty
  }

public:
  ConfigurationStatus status;

  /** Descriptive string describing result of import */
  std::string message;
};

}
