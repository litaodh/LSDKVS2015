// Copyright 2018-2019 SICK AG. All rights reserved.

#pragma once

#include "Ranger3LineMetadata.h"

#include <GenApi/IInteger.h>
#include <GenApi/NodeMapRef.h>

/** Extracts metadata from chunk features given a NodeMap. This implementation
    uses the GenApi standard way of reading every scan line via the selector.
 */
class MetadataExtractor
{
public:
  MetadataExtractor(GenApi::CNodeMapRef device);
  std::vector<Ranger3LineMetadata> extract();

private:
  GenApi::CIntegerPtr mScanLineSelector;
  GenApi::CIntegerPtr mTimestamp;
  GenApi::CIntegerPtr mEncoderValue;
  GenApi::CIntegerPtr mOvertriggerCount;
  GenApi::CIntegerPtr mFrameTriggerActive;
  GenApi::CIntegerPtr mLineTriggerActive;
  GenApi::CIntegerPtr mEncoderResetActive;
  GenApi::CIntegerPtr mEncoderA;
  GenApi::CIntegerPtr mEncoderB;
};
