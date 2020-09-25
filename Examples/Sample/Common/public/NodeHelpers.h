// Copyright 2019 SICK AG. All rights reserved.

#pragma once

#include "Consumer.h"

namespace common {

// Helper function for getting a nodemap parameter value
template<class NodeType, typename ValueType>
extern ValueType getNodeValue(GenApi::CNodeMapRef& nodemap, const char* name);

// Helper function for setting a nodemap parameter value
template<class NodeType, typename ValueType>
extern void
setNodeValue(GenApi::CNodeMapRef& nodemap, const char* name, ValueType value);

// Function aliases
const auto getInt = getNodeValue<GenApi::CIntegerPtr, int64_t>;
const auto setBool = setNodeValue<GenApi::CBooleanPtr, bool>;
const auto setEnum = setNodeValue<GenApi::CEnumerationPtr, const char*>;
const auto setFloat = setNodeValue<GenApi::CFloatPtr, float>;
const auto setInt = setNodeValue<GenApi::CIntegerPtr, int64_t>;

// Helper function for executing a nodemap command
extern void executeCommand(GenApi::CNodeMapRef& nodemap,
                           const char* name,
                           bool waitForCompletion = true);

}
