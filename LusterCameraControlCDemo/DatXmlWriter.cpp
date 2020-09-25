// Copyright 2016-2017 SICK AG. All rights reserved.
#include "stdafx.h"
#include "DatXmlWriter.h"
//#include "Exceptions.h"


DatXmlWriter::DatXmlWriter() : mIndent(0)
{
};

void DatXmlWriter::open(std::string valueType, std::string name)
{
  mXml << std::string(mIndent, ' ');
  mXml << "<icon_data_format valuetype=\"" << valueType << "\" name=\""
       << name << "\">" << std::endl;
  indent();
}

void DatXmlWriter::close()
{
  mXml << "</icon_data_format>";
}

void DatXmlWriter::indent()
{
  mIndent += 2;
}

void DatXmlWriter::unIndent()
{
  mIndent -= 2;
  if (mIndent < 0)
  {
    //throw GenIRangerException("Indentation cannot be less than 0");
  }
}

void DatXmlWriter::addParameter(std::string name, std::string value)
{
  mXml << std::string(mIndent, ' ');
  mXml << "<parameter name=\"" << name << "\">" << value << "</parameter>"
       << std::endl;
}

void DatXmlWriter::openComponent(std::string valueType, std::string name)
{
  mXml << std::string(mIndent, ' ');
  mXml << "<component valuetype=\"" << valueType << "\" name=\""
       << name << "\">" << std::endl;
  indent();
}

void DatXmlWriter::closeComponent()
{
  unIndent();
  mXml << std::string(mIndent, ' ');
  mXml << "</component>" << std::endl;
}

void DatXmlWriter::openImageTraits()
{
  mXml << std::string(mIndent, ' ');
  mXml << "<imagetraits>" << std::endl;
  indent();
}

void DatXmlWriter::closeImageTraits()
{
  unIndent();
  mXml << std::string(mIndent, ' ');
  mXml << "</imagetraits>" << std::endl;
}

void DatXmlWriter::openSensorRangeTraits()
{
  mXml << std::string(mIndent, ' ');
  mXml << "<sensorrangetraits>" << std::endl;
  indent();
}

void DatXmlWriter::closeSensorRangeTraits()
{
  unIndent();
  mXml << std::string(mIndent, ' ');
  mXml << "</sensorrangetraits>" << std::endl;
}

void DatXmlWriter::openSubComponent(std::string valueType, std::string name)
{
  mXml << std::string(mIndent, ' ');
  mXml << "<subcomponent valuetype=\"" << valueType << "\" name=\""
       << name << "\">" << std::endl;
  indent();
}

void DatXmlWriter::closeSubComponent()
{
  unIndent();
  mXml << std::string(mIndent, ' ');
  mXml << "</subcomponent>" << std::endl;
}

std::string DatXmlWriter::toString()
{
  return mXml.str();
}


