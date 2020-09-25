// Copyright 2016-2019 SICK AG. All rights reserved.

#include "ConfigReader.h"

#include "Exceptions.h"
#include "GenIUtil.h"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace GenIRanger {
const int VERSION = 1;

void GetKeyValue(std::string line, std::string& key, std::string& value)
{
  std::string element;
  std::stringstream stream(line);
  std::vector<std::string> values;

  while (std::getline(stream, element, ','))
  {
    element.erase(std::remove(element.begin(), element.end(), '\n'),
                  element.end());
    values.push_back(element);
  }
  if (values.size() == 2)
  {
    key = values.at(0);
    value = values.at(1);
  }
  else
  {
    std::string error("No <key>,<value> pair found");
    GenIUtil::throwAndLog(error);
  }
}
ConfigReader::ConfigReader()
{
  // Empty
}

ConfigurationResult ConfigReader::parse(std::istream& csv)
{
  // First row contains the format version
  std::string versionLine;
  std::getline(csv, versionLine, '\n');

  if (!versionLine.empty())
  {
    std::string versionStr;
    std::string versionNumber;
    GetKeyValue(versionLine, versionStr, versionNumber);

    if (std::stoi(versionNumber) == VERSION)
    {
      std::string line;
      std::string key;
      std::string value;
      while (std::getline(csv, line, '\n'))
      {
        // Skip lines starting with #
        if (line.find('#') != std::string::npos)
        {
          continue;
        }

        GetKeyValue(line, key, value);
        if (mNodeValueMap.find(key) != mNodeValueMap.end())
        {
          ConfigurationResult result(ConfigurationStatus::DUPLICATE_PARAMETER,
                                     "Duplicate parameter found: " + key);
          GenIUtil::log(result.message);
          return result;
        }
        mNodeValueMap.insert(NodeValueMap::value_type(key, value));
      }
      return ConfigurationResult(ConfigurationStatus::OK,
                                 "Successfully parsed file");
    }
    else
    {
      ConfigurationResult result(ConfigurationStatus::BAD_VERSION,
                                 "Version number does not match the parser");
      GenIUtil::log(result.message);
      return result;
    }
  }
  else
  {
    ConfigurationResult result(ConfigurationStatus::EMPTY_PARAMETER_FILE,
                               "Empty parameter file");
    GenIUtil::log(result.message);
    return result;
  }
}

ConfigReader::~ConfigReader()
{
  // Empty
}

bool ConfigReader::hasValue(std::string& key)
{
  return mNodeValueMap.find(key) != mNodeValueMap.end();
}

std::string ConfigReader::getValue(std::string& key)
{
  if (hasValue(key))
  {
    return mNodeValueMap.find(key)->second;
  }
  else
  {
    std::stringstream ss;
    ss << key << " cannot be found in configuration file";
    GenIUtil::throwAndLog(ss.str());
    return std::string();
  }
}

std::set<std::string> ConfigReader::getKeys()
{
  std::set<std::string> keys;
  for (auto it = mNodeValueMap.begin(); it != mNodeValueMap.end(); ++it)
  {
    keys.insert(it->first);
  }
  return keys;
}

}
