// Copyright 2016-2019 SICK AG. All rights reserved.
#pragma once

#include "GenICam.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace GenIRanger {
/** Outputs a GenICam node structure as CSV. Values are as <node-name>,<value>.
    If the node is dependent on selectors the output will be
    <node-name>,<value>,<selector-1-name>,<value>,<selector-2-name>,<value> etc.
*/
class ConfigWriter
{
public:
  ConfigWriter(std::ostream& outputStream);

  /** Outputs the category name with a leading '#' indicating that it's ignored
      during import.
  */
  void outputCategory(std::string& name);
  /** Outputs the a node and its value in the format <node name>,<value>. If
      it's dependent on selectors they are also included in the key, i.e.
      <node name>_<selector name 1>_<selector value>_(more selectors), <value>.
      The selectors are alphabetically sorted.
  */
  void outputLeaf(std::string& name, std::string& value);

  /** Indents debug output */
  void pushCategory();
  /** Reduces indentation for debug output */
  void popCategory();

  /** Pushes node to list of selectors */
  void pushSelector(const GenApi::CNodePtr& node);
  /** Removes the latest added selector */
  void popSelector();

private:
  std::vector<GenApi::CNodePtr> mSelectors;
  std::ostream& mOutputStream;
  int mIndent;

  void indent();
  void unindent();
};

}
