// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

#include "GenIRanger.h"
#include <sstream>
#include <string>

namespace GenIRanger {

/** A very basic XML writer for .dat-files

    Always start by calling open() and end with close(), the rest of
    parameters and components needs to be added in the following order:

    Parameter: size, <size>
    Parameter: version, 1
    Parameter: layout, SUBCOMPONENT
    Component 1: HorThr, <name>
      Parameter: size, <size>
      Parameter: height, <height>
      SensorRangeTraits
        Parameter: coordinates, ""
        Parameter: fov x0, <x0>
        Parameter: fov x1, <x1>
        Parameter: fov x2, <x2>
        Parameter: fov x3, <x3>
        Parameter: fov z0, <z0>
        Parameter: fov z1, <z1>
        Parameter: fov z2, <z2>
        Parameter: fov z3, <z3>
        Parameter: origin x, <origin-x>
        Parameter: scale x, <scale-x>
        Parameter: origin z, <origin z>
        Parameter: scale z, <scale z>
      Subcomponent 1: WORD, Range
        Parameter: size, <size>
        Parameter: width, <width>
    Component N ...

*/
class DatXmlWriter
{
public:
  DatXmlWriter();
  ~DatXmlWriter(){};

  /** Opens the top-level icon_data_format tag.
      The valueType and name parameters should be empty when saving range data.
      For sensor image both valueType and name should be set to 'IMAGE'.
  */
  void open(std::string valueType = "", std::string name = "");
  void close();
  void addParameter(std::string name, std::string value);
  void openComponent(std::string valueType, std::string name);
  void closeComponent();
  void openImageTraits();
  void closeImageTraits();
  void openSensorRangeTraits();
  /** Add by VisionLab Shanghai China*/
  void openWorldRangeTraits();
  void closeSensorRangeTraits();
  /** Add by VisionLab Shanghai China*/
  void closeWorldRangeTraits();
  void openSubComponent(std::string valueType, std::string name);
  void closeSubComponent();
  void addArbitraryXml(std::string arbitraryXml);
  std::string toString();

private:
  void indent();
  void unIndent();

  uint32_t mIndent;
  std::stringstream mXml;
};

}
