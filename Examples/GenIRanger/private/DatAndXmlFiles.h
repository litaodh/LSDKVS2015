// Copyright 2018-2019 SICK AG. All rights reserved.

#pragma once

#include "StreamData.h"

#include <fstream>
#include <functional>
#include <string>

namespace GenIRanger {

/** The file format written to requires two files. A DAT-file
 * for binary data and a XML-file for other information. */
class DatAndXmlFiles
{
private:
  std::ofstream mDataStream;
  std::ofstream mXmlStream;
  static const auto openMode =
    std::ios::binary | std::ios::trunc | std::ios::out;

  const size_t STATUS_BIT_ENABLE = 30;
  const size_t STATUS_BIT_ENCODER_A = 28;
  const size_t STATUS_BIT_ENCODER_B = 27;
  const size_t STATUS_BIT_OVERTRIG = 16;
  const size_t STATUS_BIT_LINE_TRIGGER = 25;
  const size_t STATUS_BIT_ENCODER_RESET = 24;

public:
  /** One metadata value. */
  typedef uint32_t MarkdataWord;

public:
  DatAndXmlFiles(std::string filePathWithNoEnding);

  virtual ~DatAndXmlFiles();

  /** Write data to DAT-file. */
  template<class SIZE>
  void writeData(const uint8_t* data, const SIZE size)
  {
    mDataStream.write(reinterpret_cast<const char*>(data),
                      static_cast<const size_t>(size));
  }

  /** All by VisionLab Shanghai China*/
  void writeData(void * data, const size_t size, const bool vlb)
  {
	  mDataStream.write(reinterpret_cast<const char*>(data), size);
  }

  /** Write Component data to DAT-file. */
  void writeData(ComponentPtr& component);

  /** Write line mark data, a.k.a GigEVision line chunk data, to
      DAT-file. Each line mark data consists of 5 values, each 4
      bytes (32 bits) in size. They are ordered in the following
      sequence for each line (profile) in data:

      <ol>
      <li>Encoder value
      <li>Status bits (zero-indexed from lsb):
          <ul>
          <li> Bit 16-23: Overtrig
          <li> Bit 27: Encoder B
          <li> Bit 28: Encoder A
          <li> Bit 30: Enable
         </ul>
      <li>Sample time stamp
      <li>Encoder pulse time stamp
      <li>Camera scan ID
      </ol>
   */
  void writeMarkData(LineMetadata metadata);

  /** Write XML string content to XML-file. */
  void writeXml(std::string& xml);

private:
  void writeSingleMarkValue(const MarkdataWord value);
};

}
