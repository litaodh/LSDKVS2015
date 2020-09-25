// Copyright 2016-2019 SICK AG. All rights reserved.

#include "NodeExporter.h"

#include "ConfigWriter.h"
#include "Exceptions.h"
#include "GenIUtil.h"
#include "NodeUtil.h"

#include <sstream>

using namespace GenApi;
using namespace GenICam;

namespace GenIRanger {

NodeExporter::NodeExporter(ConfigWriter& formatter)
  : mFormat(formatter)
{
  // Empty
}

NodeExporter::~NodeExporter()
{
  // Empty
}

const std::vector<std::string>& NodeExporter::getErrors() const
{
  return mErrors;
}

void NodeExporter::enterCategory(CCategoryPtr node)
{
  mFormat.outputCategory(std::string(node->GetNode()->GetName()));
  mFormat.pushCategory();
}

void NodeExporter::leaveCategory(CCategoryPtr category)
{
  mFormat.popCategory();
}

void NodeExporter::enterSelector(CNodePtr selector)
{
  mFormat.pushSelector(selector);
}

void NodeExporter::leaveSelector(CNodePtr node)
{
  mFormat.popSelector();
}

void NodeExporter::onLeaf(CNodePtr node)
{
  std::string name(node->GetName().c_str());
  std::stringstream valueStream;
  try
  {
    if (NodeUtil::isConfigNode(node))
    {
      valueStream << NodeUtil::getValueAsString(node);
    }
    else
    {
      // Do nothing, the other types aren't interesting
    }
    mFormat.outputLeaf(name, valueStream.str());
  }
  catch (GenericException& e)
  {
    std::stringstream ss;
    ss << "Cannot read " << name
       << ". Library exception: " << e.GetDescription();
    mErrors.push_back(ss.str());
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "Cannot read " << name << ". " << e.what();
    mErrors.push_back(ss.str());
  }
}

}
