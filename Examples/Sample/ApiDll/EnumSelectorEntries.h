// Copyright 2019 SICK AG. All rights reserved.

#pragma once

#include <GenApi/GenApi.h>
#include <map>

/** A class to map selector values to names and vice versa. */
class EnumSelectorEntries
{
public:
  /** Finds all the name, value pair entries for an enum selector and
      keeps them for easy look up.
   */
  EnumSelectorEntries(GenApi::CNodeMapRef& device,
                      const std::string& selectorName);

  /** Get value from enumerator name. */
  uint64_t value(const std::string& name) const;

  /** Get enumerator name from value. */
  const std::string& name(uint64_t value) const;

private:
  std::map<std::string, uint64_t> mValueFromName;
};
