// Copyright 2018-2019 SICK AG. All rights reserved.

#pragma once

#include <cstdint>

/** Container for metadata per scan line. */
struct Ranger3LineMetadata
{
  uint64_t timestamp;
  uint32_t encoderValue;
  uint8_t overtriggerCount;
  bool frameTriggerActive;
  bool lineTriggerActive;
  bool encoderResetActive;
  bool encoderA;
  bool encoderB;

  bool operator==(const Ranger3LineMetadata& rhs) const
  {
    return timestamp == rhs.timestamp && encoderValue == rhs.encoderValue
           && overtriggerCount == rhs.overtriggerCount
           && frameTriggerActive == rhs.frameTriggerActive
           && lineTriggerActive == rhs.lineTriggerActive
           && encoderResetActive == rhs.encoderResetActive
           && encoderA == rhs.encoderA && encoderB == rhs.encoderB;
  }
};
