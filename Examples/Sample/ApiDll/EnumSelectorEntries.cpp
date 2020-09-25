// Copyright 2019 SICK AG. All rights reserved.

#include "EnumSelectorEntries.h"
#include <algorithm>
#include <cassert>

EnumSelectorEntries::EnumSelectorEntries(GenApi::CNodeMapRef& device,
                                         const std::string& selectorName)
{
  GenApi::CEnumerationPtr selector = device._GetNode(selectorName.c_str());
  GenApi::NodeList_t entries;
  selector->GetEntries(entries);
  for (GenApi::CEnumEntryPtr entry : entries)
  {
    GenApi::CEnumEntryPtr enumEntry = entry;
    mValueFromName[enumEntry->GetSymbolic().c_str()] = enumEntry->GetValue();
  }
}

uint64_t EnumSelectorEntries::value(const std::string& name) const
{
  return mValueFromName.at(name);
}

const std::string& EnumSelectorEntries::name(uint64_t value) const
{
  auto iter = std::find_if(
    mValueFromName.begin(),
    mValueFromName.end(),
    [=](std::pair<std::string, uint64_t> p) { return p.second == value; });
  assert(iter != mValueFromName.end());
  return iter->first;
}
