// Copyright 2018-2019 SICK AG. All rights reserved.

#pragma once

#include "StreamData.h"

#include <stdint.h>
#include <string>

namespace GenIRanger {

class SaveBuffer
{
private:
  static const float SIXTEENTH() { return 1.0f / 16; }

public:
  /** Calculates ICon file format sensor trait OriginZ. */
  static double calculateOriginZ(const int64_t aoiOffsetY,
                                 const int64_t aoiHeight,
                                 RangeAxis rangeAxis)
  {
    // Regarding calculation, see comments in RNX-3940
    if (rangeAxis == RangeAxis::REVERSED)
    {
      return 1.0 * aoiOffsetY + aoiHeight;
    }
    else
    {
      return 1.0 * aoiOffsetY;
    }
  }

  /** Calculates ICon file format sensor trait ScaleZ. */
  static double calculateScaleZ(RangeAxis rangeAxis)
  {
    auto sign = rangeAxis == RangeAxis::REVERSED ? -1 : 1;
    return sign * SIXTEENTH();
  }
};

}
