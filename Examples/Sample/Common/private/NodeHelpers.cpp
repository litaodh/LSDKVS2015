// Copyright 2019 SICK AG. All rights reserved.

#include "NodeHelpers.h"

#include <chrono>
#include <thread>

namespace common {

template<class NodeType, typename ValueType>
ValueType getNodeValue(GenApi::CNodeMapRef& nodemap, const char* name)
{
  NodeType node = nodemap._GetNode(name);
  return **node;
}

template<class NodeType, typename ValueType>
void setNodeValue(GenApi::CNodeMapRef& nodemap,
                  const char* name,
                  ValueType value)
{
  NodeType node = nodemap._GetNode(name);
  *node = value;
}

void executeCommand(GenApi::CNodeMapRef& nodemap,
                    const char* name,
                    bool waitForCompletion)
{
  GenApi::CCommandPtr node = nodemap._GetNode(name);
  node->Execute();
  if (waitForCompletion)
  {
    while (!node->IsDone())
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
}

}
