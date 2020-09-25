// Copyright 2018-2019 SICK AG. All rights reserved.

#include "MetadataExtractor.h"

MetadataExtractor::MetadataExtractor(GenApi::CNodeMapRef device)
  : mScanLineSelector(device._GetNode("ChunkScanLineSelector"))
  , mTimestamp(device._GetNode("ChunkTimestamp"))
  , mEncoderValue(device._GetNode("ChunkEncoderValue"))
  , mOvertriggerCount(device._GetNode("ChunkOvertriggerCount"))
  , mFrameTriggerActive(device._GetNode("ChunkFrameTriggerActive"))
  , mLineTriggerActive(device._GetNode("ChunkLineTriggerActive"))
  , mEncoderResetActive(device._GetNode("ChunkEncoderResetActive"))
  , mEncoderA(device._GetNode("ChunkEncoderA"))
  , mEncoderB(device._GetNode("ChunkEncoderB"))
{
}

std::vector<Ranger3LineMetadata> MetadataExtractor::extract()
{
  std::vector<Ranger3LineMetadata> metadata;
  const auto lineMin = mScanLineSelector->GetMin();
  const auto lineMax = mScanLineSelector->GetMax();
  metadata.reserve(lineMax - lineMin + 1);

  for (int64_t i = lineMin; i <= lineMax; ++i)
  {
    mScanLineSelector->SetValue(i);
    Ranger3LineMetadata lineMetadata;
    lineMetadata.timestamp = mTimestamp->GetValue();
    lineMetadata.encoderValue =
      static_cast<uint32_t>(mEncoderValue->GetValue());
    lineMetadata.overtriggerCount =
      static_cast<uint8_t>(mOvertriggerCount->GetValue());
    lineMetadata.frameTriggerActive =
      mFrameTriggerActive->GetValue() ? true : false;
    lineMetadata.lineTriggerActive =
      mLineTriggerActive->GetValue() ? true : false;
    lineMetadata.encoderResetActive =
      mEncoderResetActive->GetValue() ? true : false;
    lineMetadata.encoderA = mEncoderA->GetValue() ? true : false;
    lineMetadata.encoderB = mEncoderB->GetValue() ? true : false;
    metadata.push_back(lineMetadata);
  }
  return metadata;
}
