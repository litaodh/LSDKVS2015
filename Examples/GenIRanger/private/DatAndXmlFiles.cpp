// Copyright 2018-2019 SICK AG. All rights reserved.

#include "DatAndXmlFiles.h"
#include "Exceptions.h"

#include <algorithm>
#include <iterator>
#include <sstream>

namespace {

/** Returns a transformed value of the metadata at index, or zero if there is
    no metadata.
*/
template<typename T>
uint32_t bitsFromMetadata(const std::vector<T>& data,
                          size_t index,
                          std::function<uint32_t(T)> transformer)
{
  return data.empty() ? 0 : transformer(data[index]);
}

/** If available, returns the state of an IO shifted to the wanted position.
    Otherwise, returns 0.
*/
uint32_t bitFromMetadata(std::vector<bool> data, size_t index, size_t bitNum)
{
  return bitsFromMetadata<bool>(data, index, [&](bool value) -> uint32_t {
    return (value & 0x01) << bitNum;
  });
}

template<typename T>
T safeGet(const std::vector<T>& data, size_t index)
{
  return data.empty() ? 0 : data[index];
}

}

namespace GenIRanger {

DatAndXmlFiles::DatAndXmlFiles(std::string filePathWithNoEnding)
  : mDataStream(filePathWithNoEnding + ".dat", openMode)
  , mXmlStream(filePathWithNoEnding + ".xml", openMode)
{
  mDataStream.exceptions(std::ios::failbit | std::ios::badbit);
  mXmlStream.exceptions(std::ios::failbit | std::ios::badbit);
}

DatAndXmlFiles::~DatAndXmlFiles()
{
  mDataStream.close();
  mXmlStream.close();
}

void DatAndXmlFiles::writeData(ComponentPtr& component)
{
  writeData(component->data().data(), component->data().size());
}

void DatAndXmlFiles::writeMarkData(LineMetadata metadata)
{
  const MarkdataWord notUsed = 0;
  std::vector<size_t> sizes = {metadata.encoderValues.size(),
                               metadata.timestamps.size(),
                               metadata.overtriggerCounts.size(),
                               metadata.frameTriggerActive.size(),
                               metadata.lineTriggerActive.size(),
                               metadata.encoderResetActive.size(),
                               metadata.encoderA.size(),
                               metadata.encoderB.size()};
  std::vector<size_t> nonZeroSizes;

  // Collect only metadata that actually have any values
  std::copy_if(sizes.begin(),
               sizes.end(),
               std::back_inserter(nonZeroSizes),
               [](size_t i) { return i > 0; });

  if (nonZeroSizes.empty())
  {
    throw new SaveException("No metadata to write (size = 0)");
  }

  bool hasSameSize = std::equal(
    nonZeroSizes.begin() + 1, nonZeroSizes.end(), nonZeroSizes.begin());
  if (!hasSameSize)
  {
    std::stringstream str;
    str << "Inconsistent metadata length: "
        << "encoder: " << metadata.encoderValues.size() + ", "
        << "timestamp: " << metadata.timestamps.size() + ", "
        << "overtrig: " << metadata.overtriggerCounts.size() + ", "
        << "io1: " << metadata.overtriggerCounts.size() + ", "
        << "encoderA: " << metadata.overtriggerCounts.size() + ", "
        << "encoderB: " << metadata.overtriggerCounts.size() + ", ";
    throw new SaveException(str.str());
  }

  size_t someSize = nonZeroSizes[0];
  for (size_t i = 0; i < someSize; ++i)
  {
    writeSingleMarkValue(safeGet(metadata.encoderValues, i));

    uint32_t statusBits = 0;
    statusBits |= bitsFromMetadata<uint8_t>(
      metadata.overtriggerCounts, i, [&](uint8_t value) {
        return value << STATUS_BIT_OVERTRIG;
      });
    statusBits |=
      bitFromMetadata(metadata.frameTriggerActive, i, STATUS_BIT_ENABLE);
    statusBits |=
      bitFromMetadata(metadata.lineTriggerActive, i, STATUS_BIT_LINE_TRIGGER);
    statusBits |=
      bitFromMetadata(metadata.encoderResetActive, i, STATUS_BIT_ENCODER_RESET);
    statusBits |= bitFromMetadata(metadata.encoderA, i, STATUS_BIT_ENCODER_A);
    statusBits |= bitFromMetadata(metadata.encoderB, i, STATUS_BIT_ENCODER_B);

    writeSingleMarkValue(statusBits);
    // The file format supports only 32-bit timestamps. Keep the lowest bits
    // to keep the precision, and give up the range.
    writeSingleMarkValue(safeGet(metadata.timestamps, i) & 0xffffffff);
    writeSingleMarkValue(notUsed);
    writeSingleMarkValue(notUsed);
  }
}

void DatAndXmlFiles::writeXml(std::string& xml)
{
  mXmlStream << xml;
}

void DatAndXmlFiles::writeSingleMarkValue(const MarkdataWord value)
{
  writeData(reinterpret_cast<const uint8_t*>(&value), sizeof(MarkdataWord));
}

}
