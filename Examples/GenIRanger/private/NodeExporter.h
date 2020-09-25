// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

#include "ConfigWriter.h"
#include "NodeTraverser.h"
#include <string>

namespace GenIRanger {

/** Traverses a GenICam node map and exports all node with RW or WO access */
class NodeExporter : public NodeTraverser
{
public:
  NodeExporter(ConfigWriter& formatter);
  ~NodeExporter();

  const std::vector<std::string>& getErrors() const;

protected:
  /** Outputs the visited category */
  virtual void enterCategory(GenApi::CCategoryPtr category) override;
  virtual void leaveCategory(GenApi::CCategoryPtr category) override;

  /** Forwards the visited selector to the ConfigWriter */
  virtual void enterSelector(GenApi::CNodePtr selector) override;
  virtual void leaveSelector(GenApi::CNodePtr selector) override;

  /** Outputs the visited node if it's considered to be a configuration node */
  virtual void onLeaf(GenApi::CNodePtr node) override;

private:
  ConfigWriter& mFormat;

  std::vector<std::string> mErrors;
};

}
