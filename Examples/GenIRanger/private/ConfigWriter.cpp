// Copyright 2016-2019 SICK AG. All rights reserved.

#include "ConfigWriter.h"

#include "Exceptions.h"
#include "GenICam.h"
#include "GenIUtil.h"
#include "NodeUtil.h"

#include <algorithm>

using namespace GenApi;
using namespace GenICam;

namespace GenIRanger {

ConfigWriter::ConfigWriter(std::ostream& outputStream)
  : mOutputStream(outputStream)
  , mIndent(0)
{
  mOutputStream << "#Version,1" << std::endl;
}

void ConfigWriter::outputCategory(std::string& name)
{
  if (name.empty())
  {
    return;
  }

  // Output category
  GenIUtil::log(std::string(mIndent, ' ').append("#" + name + '\n'));
  mOutputStream << "#" << name << std::endl;
}

static bool compare(CNodePtr lh, CNodePtr rh)
{
  return lh->GetName() < rh->GetName();
}

void ConfigWriter::outputLeaf(std::string& name, std::string& value)
{
  if (name.empty() || value.empty())
  {
    return;
  }

  // Output node name
  GenIUtil::log(std::string(mIndent, ' ').append(name));
  mOutputStream << name;

  // If the value is dependent on selectors, output the selectors and their
  // values. This will be used as the key value when importing the node value.
  if (mSelectors.size() > 0)
  {
    std::stringstream ss;
    // Copy selector list so that it can be sorted.
    // The original is treated like a stack and should not be altered.
    auto sorted = mSelectors;
    std::sort(sorted.begin(), sorted.end(), compare);
    for (auto it = sorted.begin(); it != sorted.end(); it++)
    {
      auto selector = *it;
      auto name = selector->GetName();
      try
      {
        ss << "_";
        if (NodeUtil::isInteger(selector))
        {
          CIntegerPtr intSelector = static_cast<CIntegerPtr>(selector);
          ss << name << "_" << intSelector->GetValue();
        }
        else if (NodeUtil::isEnumeration(selector))
        {
          CEnumerationPtr enumSelector = static_cast<CEnumerationPtr>(selector);
          ss << name << "_" << enumSelector->GetCurrentEntry()->GetSymbolic();
        }
        else
        {
          std::stringstream ss;
          ss << "Unsupported selector: " << name;

          GenIUtil::throwAndLog(ss.str());
        }
      }
      catch (GenericException& e)
      {
        std::stringstream ss;
        ss << "Cannot read " << name
           << ". Library exception: " << e.GetDescription();
        GenIUtil::throwAndLog(ss.str());
      }
    }
    GenIUtil::log(ss.str());
    mOutputStream << ss.str();
  }
  // Lastly output the node value
  GenIUtil::log("," + value + '\n');
  mOutputStream << "," << value << '\n';
}

void ConfigWriter::indent()
{
  mIndent = mIndent + 2;
}

void ConfigWriter::unindent()
{
  mIndent = mIndent - 2;
  // TODO test that this shouldn't happen
  mIndent = mIndent < 0 ? 0 : mIndent;
}

void ConfigWriter::pushCategory()
{
  indent();
}

void ConfigWriter::popCategory()
{
  unindent();
}

void ConfigWriter::pushSelector(const CNodePtr& node)
{
  mSelectors.push_back(node);
}

void ConfigWriter::popSelector()
{
  mSelectors.pop_back();
}

}
