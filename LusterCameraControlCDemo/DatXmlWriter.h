// Copyright 2016-2017 SICK AG. All rights reserved.
#ifndef GENIRANGER_DATXMLWRITER_H
#define GENIRANGER_DATXMLWRITER_H


#include <string>
#include <sstream>
#include <stdint.h>

class DatXmlWriter
{
public:
  DatXmlWriter();
  ~DatXmlWriter() {};

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
  void closeSensorRangeTraits();
  void openSubComponent(std::string valueType, std::string name);
  void closeSubComponent();
  std::string toString();

private:
  void indent();
  void unIndent();

  uint32_t mIndent;
  std::stringstream mXml;
};


#endif