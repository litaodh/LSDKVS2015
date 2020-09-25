// Copyright 2017-2019 SICK AG. All rights reserved.

#include "SelectorSnapshot.h"

#include "GenIUtil.h"
#include "NodeUtil.h"
#include <algorithm>

namespace GenIRanger {

SelectorSnapshot::SelectorSnapshot(GenApi::INodeMap* nodeMap)
  : mNodeMap(nodeMap)
{
  traverse(mNodeMap->GetNode("Root"));
}

SelectorSnapshot::~SelectorSnapshot()
{
  for (SelectorState& state : mState)
  {
    GenApi::INode* selector = mNodeMap->GetNode(state.name);
    if (NodeUtil::isEnumeration(selector))
    {
      GenApi::CEnumerationPtr enumSelector =
        static_cast<GenApi::CEnumerationPtr>(selector);
      GenApi::IEnumEntry* entry = enumSelector->GetEntry(state.value);
      if (entry->GetAccessMode() == GenApi::EAccessMode::NA)
      {
        // This can happen, e.g., if the ComponentSelector has a value that
        // is not available given the value of the RegionSelector when the
        // snapshot is taken.
        std::ostringstream ss;
        ss << "The selector node " << state.name << " cannot be restored since "
           << "the value " << entry->GetSymbolic() << " is not available.";
        GenIUtil::log(ss.str());
      }
      else
      {
        enumSelector->SetIntValue(state.value);
      }
    }
    else if (NodeUtil::isInteger(selector))
    {
      GenApi::CIntegerPtr intSelector =
        static_cast<GenApi::CIntegerPtr>(selector);
      intSelector->SetValue(state.value);
    }
  }
}

void SelectorSnapshot::enterSelector(GenApi::CNodePtr selector)
{
  // This function is called by the NodeTraverser _before_ it starts iterating
  // all values.
  GenICam::gcstring name = selector->GetName();
  // We might visit the same selector several times, if it is part of a nested
  // array. Store the initial state only.
  bool alreadySaved =
    (std::find_if(mState.begin(),
                  mState.end(),
                  [&](SelectorState& state) { return state.name == name; })
     != mState.end());
  if (!alreadySaved)
  {
    if (NodeUtil::isEnumeration(selector))
    {
      GenApi::CEnumerationPtr enumSelector =
        static_cast<GenApi::CEnumerationPtr>(selector);
      mState.push_back({name, enumSelector->GetCurrentEntry()->GetValue()});
    }
    else if (NodeUtil::isInteger(selector))
    {
      GenApi::CIntegerPtr intSelector =
        static_cast<GenApi::CIntegerPtr>(selector);
      mState.push_back({name, intSelector->GetValue()});
    }
    else
    {
      std::stringstream ss;
      ss << "Unsupported selector node: " << name
         << ", of type: " << selector->GetPrincipalInterfaceType();
      GenIUtil::throwAndLog(ss.str());
    }
  }
}

void SelectorSnapshot::leaveSelector(GenApi::CNodePtr selector)
{
  // This function is called by the NodeTraverser _after_ it has iterated all
  // selector values.
}

}
