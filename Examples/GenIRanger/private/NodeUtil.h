// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

#include "GenICam.h"
#include <string>

namespace GenIRanger { namespace NodeUtil {
bool isInteger(const GenApi::CNodePtr& node);

bool isEnumeration(const GenApi::CNodePtr& node);

bool isConfigNode(const GenApi::CNodePtr& node);

std::string getValueAsString(const GenApi::CNodePtr& node);
}}
