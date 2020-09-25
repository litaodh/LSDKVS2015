// Copyright 2016-2019 SICK AG. All rights reserved.

#include "NodeUtil.h"

#include "Exceptions.h"
#include "GenIUtil.h"

#include <iomanip>
#include <sstream>

namespace GenIRanger {
using namespace GenApi;

bool NodeUtil::isInteger(const CNodePtr& node)
{
  auto type = node->GetPrincipalInterfaceType();
  return type == intfIInteger;
}

bool NodeUtil::isEnumeration(const CNodePtr& node)
{
  auto type = node->GetPrincipalInterfaceType();
  return type == intfIEnumeration;
}

bool NodeUtil::isConfigNode(const CNodePtr& node)
{
  return node->IsStreamable() && GenApi::IsWritable(node);
}

std::string NodeUtil::getValueAsString(const CNodePtr& node)
{
  try
  {
    auto type = node->GetPrincipalInterfaceType();
    switch (type)
    {
    case intfIInteger:
    case intfIString:
    case intfIBoolean:
    case intfIEnumeration:
    {
      CValuePtr value = static_cast<CValuePtr>(node);
      return std::string(value->ToString().c_str());
    }
    case intfIFloat:
    {
      CFloatPtr value = static_cast<CFloatPtr>(node);
      // Output floats with high precision, so that they can be read back into
      // exactly the same value.
      // https://randomascii.wordpress.com/2013/02/07/float-precision-revisited-nine-digit-float-portability/
      std::stringstream ss;
      ss << std::setprecision(9);
      ss << value->GetValue();
      return ss.str();
    }
    default:
    {
      std::stringstream ss;
      ss << "Unsupported node: " << node->GetName();
      GenIUtil::throwAndLog(ss.str());
      return std::string();
    }
    }
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "Cannot get string value of: " << node->GetName() << " " << e.what();
    throw GenIRanger::GenIRangerException(ss.str());
  }
  catch (GenICam::GenericException& e)
  {
    std::stringstream ss;
    ss << "Cannot get string value of: " << node->GetName() << " " << e.what();
    throw GenIRanger::GenIRangerException(ss.str());
  }
}

}
