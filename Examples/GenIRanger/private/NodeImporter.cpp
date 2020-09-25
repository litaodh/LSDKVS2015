// Copyright 2016-2019 SICK AG. All rights reserved.

#include "NodeImporter.h"

#include "Exceptions.h"
#include "GenIUtil.h"
#include "NodeUtil.h"

#include <algorithm>
#include <sstream>

using namespace GenApi;
using namespace GenICam;

namespace GenIRanger {

NodeImporter::NodeImporter(ConfigReader reader)
  : mReader(reader)
  , mRemainingToImport(reader.getKeys())
{
}

NodeImporter::~NodeImporter()
{
}

void NodeImporter::enterSelector(CNodePtr selector)
{
  mSelectors.push_back(selector);
}

void NodeImporter::leaveSelector(CNodePtr node)
{
  mSelectors.pop_back();
}


void NodeImporter::leaveCategory(CCategoryPtr category)
{
  // When all nodes have been traversed
  auto categoryName = category->GetNode()->GetName(true);
  if (categoryName == "Std::Root")
  {
    checkUnimportedParameters();
  }
}

static bool compare(CNodePtr lh, CNodePtr rh)
{
  return lh->GetName() < rh->GetName();
}

void NodeImporter::onLeaf(CNodePtr node)
{
  std::string nodeName = node->GetName();
  std::string key(nodeName);

  if (mSelectors.size() > 0)
  {
    std::stringstream ss;
    // Copy selector vector, so it can be sorted without modifying the original
    auto sorted = mSelectors;
    std::sort(sorted.begin(), sorted.end(), compare);
    for (auto it = sorted.begin(); it != sorted.end(); it++)
    {
      auto selector = *it;
      ss << "_" << selector->GetName();
      ss << "_" << NodeUtil::getValueAsString(selector);
    }
    key.append(ss.str());
  }

  if (NodeUtil::isConfigNode(node))
  {
    if (mReader.hasValue(key))
    {
      std::string readValue = mReader.getValue(key);
      try
      {
        auto type = node->GetPrincipalInterfaceType();
        if (type == intfIInteger)
        {
          CIntegerPtr value = static_cast<CIntegerPtr>(node);
          value->FromString(gcstring(readValue.c_str()), false);
        }
        else if (type == intfIFloat)
        {
          CFloatPtr value = static_cast<CFloatPtr>(node);
          value->FromString(gcstring(readValue.c_str()), false);
        }
        else if (type == intfIString)
        {
          CStringPtr value = static_cast<CStringPtr>(node);
          value->SetValue(gcstring(readValue.c_str()), false);
        }
        else if (type == intfIBoolean)
        {
          CBooleanPtr value = static_cast<CBooleanPtr>(node);
          value->FromString(gcstring(readValue.c_str()), false);
        }
        else if (type == intfIEnumeration)
        {
          CEnumerationPtr value = static_cast<CEnumerationPtr>(node);
          value->FromString(gcstring(readValue.c_str()), false);
        }
        else
        {
          // Do nothing, the other types aren't interesting
        }

        // Mark this key as imported
        mRemainingToImport.erase(key);

        GenIUtil::log("Set " + key + " " + readValue + "\n");
      }
      catch (GenericException& e)
      {
        std::stringstream ss;
        ss << "Cannot set " << key << " = " << readValue
           << ". Library exception: " << e.GetDescription();

        mErrors.push_back(ss.str());
      }
      catch (std::exception& e)
      {
        std::stringstream ss;
        ss << "Cannot set " << key << " = " << readValue << ". " << e.what();

        mErrors.push_back(ss.str());
      }
    }
    else
    {
      std::stringstream ss;
      ss << "Parameter " << key << " cannot be found in configuration file";
      mMissing.push_back(ss.str());
    }
  }
  else
  {
    // Not a configuration node, just ignore
    if (mReader.hasValue(key))
    {
      std::stringstream ss;
      ss << "Parameter " << key << " was found in configuration file, but is "
         << "not a configuration parameter";
      mErrors.push_back(ss.str());
    }
  }
}

void NodeImporter::checkUnimportedParameters()
{
  for (auto it = mRemainingToImport.begin(); it != mRemainingToImport.end();
       ++it)
  {
    std::stringstream ss;
    ss << "Parameter " << *it << " was found in configuration file, but is "
       << "not a configuration parameter";
    mErrors.push_back(ss.str());
  }
}

const std::vector<std::string>& NodeImporter::getErrors() const
{
  return mErrors;
}

const std::vector<std::string>& NodeImporter::getMissing() const
{
  return mMissing;
}

}
