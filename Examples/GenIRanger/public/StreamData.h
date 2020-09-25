// Copyright 2018-2019 SICK AG. All rights reserved.

#pragma once

#include <map>
#include <memory>
#include <stdint.h>
#include <vector>

namespace GenIRanger {
/** A positive (1st quadrant) point in cartesian 2-dimensional plane. */
class Size2D
{
private:
  size_t mX;
  size_t mY;

public:
  Size2D(size_t x, size_t y)
    : mX(x)
    , mY(y)
  {
  }

  size_t x() const { return mX; }
  size_t y() const { return mY; }

  bool operator==(const Size2D& other) const
  {
    return mX == other.x() && mY == other.y();
  };
};

/** Width in bits for a pixel. */
enum class PixelWidth
{
  PW8 = 8,
  PW12 = 12,
  PW16 = 16
};

/** Range axis direction. */
enum class RangeAxis
{
  REVERSED,
  STANDARD
};

/** Container for raw payload data (bytes). */
typedef std::vector<uint8_t> DataVector;

/** Data payload of specific component type. */
class Component
{
private:
  DataVector mData;
  PixelWidth mPixelWidth;

public:
  Component(size_t sizeInBytes, PixelWidth pw)
    : mData(sizeInBytes)
    , mPixelWidth(pw)
  {
  }

  Component(const DataVector& data, PixelWidth pw)
    : mData(data)
    , mPixelWidth(pw)
  {
  }

  Component(size_t sizeInBytes)
    : Component(sizeInBytes, PixelWidth::PW8)
  {
  }

  Component()
    : Component(0)
  {
  }

  Component& data(DataVector& data)
  {
    mData = data;
    return *this;
  }
  DataVector& data() { return mData; }

  PixelWidth pixelWidth() const { return mPixelWidth; }
  Component& pixelWidth(PixelWidth pw)
  {
    mPixelWidth = pw;
    return *this;
  }
};

/** Container for metadata for a series of lines. */
struct LineMetadata
{
public:
  LineMetadata() {}

  bool empty()
  {
    return encoderValues.empty() && timestamps.empty()
           && overtriggerCounts.empty() && frameTriggerActive.empty()
           && lineTriggerActive.empty() && encoderResetActive.empty()
           && encoderA.empty() && encoderB.empty();
  }

public:
  typedef std::vector<uint32_t> EncoderValues;
  typedef std::vector<int64_t> Timestamps;
  typedef std::vector<uint8_t> OverTriggers;
  typedef std::vector<bool> InputSignals;

  EncoderValues encoderValues;
  Timestamps timestamps;
  OverTriggers overtriggerCounts;
  InputSignals frameTriggerActive;
  InputSignals lineTriggerActive;
  InputSignals encoderResetActive;
  InputSignals encoderA;
  InputSignals encoderB;
};

/** Pointer to LineMetadata. */
typedef std::shared_ptr<LineMetadata> LineMetadataPtr;

/** Pointer to Component. */
typedef std::shared_ptr<Component> ComponentPtr;

/** A range data frame containing some AOI information and some data Component.
 */
class RangeFrame
{
private:
  Size2D mAoiSize;
  Size2D mAoiOffset;
  RangeAxis mRangeAxis;

  LineMetadataPtr mLineMetadata;

  ComponentPtr mRange;
  ComponentPtr mReflectance;
  ComponentPtr mScatter;

public:
  RangeFrame()
    : mAoiSize(Size2D(0, 0))
    , mAoiOffset(Size2D(0, 0))
    , mRangeAxis(RangeAxis::REVERSED)
    , mLineMetadata(std::make_shared<LineMetadata>())
    , mRange()
    , mReflectance()
    , mScatter()
  {
  }

  Size2D aoiOffset() const { return mAoiOffset; }
  RangeFrame& aoiOffset(Size2D offset)
  {
    mAoiOffset = offset;
    return *this;
  }
  RangeFrame& aoiOffset(size_t x, size_t y) { return aoiOffset(Size2D(x, y)); }

  Size2D aoiSize() const { return mAoiSize; }
  RangeFrame& aoiSize(Size2D size)
  {
    mAoiSize = size;
    return *this;
  }
  RangeFrame& aoiSize(size_t x, size_t y) { return aoiSize(Size2D(x, y)); }

  RangeAxis rangeAxis() const { return mRangeAxis; }
  RangeFrame& rangeAxis(RangeAxis axis)
  {
    mRangeAxis = axis;
    return *this;
  }

  LineMetadataPtr lineMetadata() { return mLineMetadata; }
  RangeFrame& lineMetadata(LineMetadataPtr& lineMetadata)
  {
    mLineMetadata = lineMetadata;
    return *this;
  }

  ComponentPtr& range() { return mRange; }
  ComponentPtr& reflectance() { return mReflectance; }
  ComponentPtr& scatter() { return mScatter; }

  RangeFrame& createRange(size_t bytes, PixelWidth pw)
  {
    mRange = std::make_shared<Component>(bytes, pw);
    return *this;
  }

  RangeFrame& createRange(const DataVector& data, PixelWidth pw)
  {
    mRange = std::make_shared<Component>(data, pw);
    return *this;
  }

  RangeFrame& createReflectance(size_t bytes, PixelWidth pw)
  {
    mReflectance = std::make_shared<Component>(bytes, pw);
    return *this;
  }

  RangeFrame& createReflectance(const DataVector& data, PixelWidth pw)
  {
    mReflectance = std::make_shared<Component>(data, pw);
    return *this;
  }

  RangeFrame& createScatter(size_t bytes, PixelWidth pw)
  {
    mScatter = std::make_shared<Component>(bytes, pw);
    return *this;
  }

  RangeFrame& createScatter(const DataVector& data, PixelWidth pw)
  {
    mScatter = std::make_shared<Component>(data, pw);
    return *this;
  }
};

}
