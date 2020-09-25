// Copyright 2016-2019 SICK AG. All rights reserved.

#include "DatAndXmlFiles.h"
#include "DatXmlWriter.h"
#include "Exceptions.h"
#include "GenIRanger.h"
#include "SaveBuffer.h"

using namespace GenApi;

namespace {

size_t bytesPerPixel(GenIRanger::PixelWidth pixelWidth)
{
  return static_cast<size_t>(pixelWidth) / 8;
}

std::string valueTypeFromPixelWidth(GenIRanger::PixelWidth pixelWidth)
{
  switch (pixelWidth)
  {
  case GenIRanger::PixelWidth::PW8: return "BYTE"; break;
  case GenIRanger::PixelWidth::PW16: return "WORD"; break;
  default: throw GenIRanger::GenIRangerException("Unexpected pixel format");
  }
}

}

namespace GenIRanger {

void writeSensorRangeTraits(DatXmlWriter& xml,
                            const int64_t bufferWidth,
                            const int64_t aoiHeight,
                            const int64_t aoiOffsetX,
                            const int64_t aoiOffsetY,
                            const RangeAxis rangeAxis)
{
  // XML file write use { } brackets to indicate element structure
  xml.openSensorRangeTraits();
  {
    // From where on the sensor the data comes from can be calculated by
    // using the origin and scale factors below

    // Value range of data in X-direction, 0-indexed
    xml.addParameter("fov x0", "0");
    xml.addParameter("fov x1", std::to_string(bufferWidth - 1));
    xml.addParameter("fov x2", "0");
    xml.addParameter("fov x3", std::to_string(bufferWidth - 1));
    // Value range of data in Z-direction (height), 0-indexed. Starts at 1
    // because 0 indicates missing data
	xml.addParameter("fov z0", "1");
	xml.addParameter("fov z1", "1");

    // 16 bits data with sub-pixel accuracy
    xml.addParameter("fov z2", std::to_string(aoiHeight * 16 - 1));
    xml.addParameter("fov z3", std::to_string(aoiHeight * 16 - 1));
    // Where on the sensor is the data extracted from
    xml.addParameter("origin x", std::to_string(aoiOffsetX));
    xml.addParameter("scale x", "1");

    auto originZ =
      SaveBuffer::calculateOriginZ(aoiOffsetY, aoiHeight, rangeAxis);
    xml.addParameter("origin z", std::to_string(originZ));

    // Sub-pixel accuracy
    auto scaleZ = SaveBuffer::calculateScaleZ(rangeAxis);
    xml.addParameter("scale z", std::to_string(scaleZ));
  }
  xml.closeSensorRangeTraits();
}

void writeSensorRangeTraits(DatXmlWriter& xml, RangeFrame& frame)
{
  writeSensorRangeTraits(xml,
                         frame.aoiSize().x(),
                         frame.aoiSize().y(),
                         frame.aoiOffset().x(),
                         frame.aoiOffset().y(),
                         frame.rangeAxis());
}

/** For save calibrated buffer, data format is float. -- Add by VisionLab Shanghai China */
void writeWorldRangeTraits(DatXmlWriter& xml,
							const float Lx,
							const float Lr,
							const float Ux,
							const float Ur)
{
	// XML file write use { } brackets to indicate element structure
	xml.openWorldRangeTraits();
	{
		xml.addParameter("lower bound x", std::to_string(Lx));
		xml.addParameter("lower bound R", std::to_string(Lr));
		xml.addParameter("upper bound x", std::to_string(Ux));
		xml.addParameter("upper bound R", std::to_string(Ur));
		xml.addParameter("coordinate unit", "");
	}
	xml.closeWorldRangeTraits();
}

void writeSubComponent(DatXmlWriter& xml,
                       const std::string& name,
                       PixelWidth pixelWidth,
                       int64_t bufferWidth)
{
  // XML file write use { } brackets to indicate element structure
  xml.openSubComponent(valueTypeFromPixelWidth(pixelWidth), name);
  {
    xml.addParameter("size",
                     std::to_string(bufferWidth * bytesPerPixel(pixelWidth)));
    xml.addParameter("width", std::to_string(bufferWidth));
  }
  xml.closeSubComponent();
}

/**  Add by VisionLab Shanghai China*/
void writeSubComponent(DatXmlWriter& xml,
					   const std::string& type,
					   const std::string& name,
					   int64_t sizeBytes,
					   int64_t widthBytes)
{
	// XML file write use { } brackets to indicate element structure
	xml.openSubComponent(type, name);
	{
		xml.addParameter("size", std::to_string(sizeBytes));
		xml.addParameter("width", std::to_string(widthBytes));
	}
	xml.closeSubComponent();
}

void writeSubComponentMarkData(DatXmlWriter& xml, const int64_t bufferWidth)
{
  xml.openSubComponent("INT", "Mark");
  {
    xml.addParameter("size", "20");
    xml.addParameter("width", "5");
  }
  xml.closeSubComponent();
}

GENIRANGER_API void saveBuffer16(const uint8_t* buffer,
                                 const int64_t bufferWidth,
                                 const int64_t bufferHeight,
                                 const int64_t aoiHeight,
                                 const int64_t aoiOffsetX,
                                 const int64_t aoiOffsetY,
                                 const RangeAxis rangeAxis,
                                 const std::string& filePath,
                                 const std::string& arbitraryXml)
{
  saveMultiPartBuffer(buffer,
                      nullptr,
                      nullptr,
                      bufferWidth,
                      bufferHeight,
                      aoiHeight,
                      aoiOffsetX,
                      aoiOffsetY,
                      rangeAxis,
                      filePath,
                      arbitraryXml);
}

GENIRANGER_API void saveBufferFloat(const std::vector<void*> buffers, 
									const int64_t bufferWidth, 
									const int64_t bufferHeight, 
									const int64_t aoiHeight, 
									const int64_t aoiOffsetX, 
									const int64_t aoiOffsetY, 
									const RangeAxis rangeAxis, 
									const float Lx, 
									const float Lr, 
									const float Ux, 
									const float Ur, 
									const std::string & filePath, 
									const std::string & arbitraryXml)
{
	if (buffers.empty() || buffers[0] == nullptr) return;

	// write data
	const size_t aoiWidth = bufferWidth;
	DatAndXmlFiles files(filePath);

	files.writeData(buffers[0], sizeof(float) * aoiWidth * bufferHeight, true);
	size_t totalSize = sizeof(float) * aoiWidth;

	if (buffers.size() >= 2 && buffers[1] != nullptr)
	{
		files.writeData(buffers[1], sizeof(uint8_t) * aoiWidth * bufferHeight, true);
		totalSize += sizeof(uint8_t) * aoiWidth;
	}
	if (buffers.size() >= 3 && buffers[2] != nullptr)
	{
		files.writeData(buffers[2], sizeof(uint8_t) * aoiWidth * bufferHeight, true);
		totalSize += sizeof(uint8_t) * aoiWidth;
	}

	// write xml
	DatXmlWriter xml;
	xml.open();
	{
		xml.addParameter("size", std::to_string(totalSize));
		xml.addParameter("version", "1");
		xml.addParameter("layout", "SUBCOMPONENT");
		xml.openComponent("Hi3D", "Ranger3Range");
		{
			xml.addParameter("size", std::to_string(totalSize));
			xml.addParameter("height", "1");
			writeWorldRangeTraits(xml,	Lx,	Lr,	Ux,	Ur);

			writeSubComponent(xml, "FLOAT", "Range", aoiWidth * sizeof(float), aoiWidth);
			if (buffers.size() >= 2 && buffers[1] != nullptr)
			{
				writeSubComponent(xml, "BYTE", "Intensity", aoiWidth, aoiWidth);
			}
			if (buffers.size() >= 3 && buffers[2] != nullptr)
			{
				writeSubComponent(xml, "BYTE", "Scatter", aoiWidth, aoiWidth);
			}
		}
		xml.closeComponent();
		xml.addArbitraryXml(arbitraryXml);
	}
	xml.close();

	files.writeXml(xml.toString());
	return ;
}

GENIRANGER_API void saveMultiPartBuffer(const uint8_t* bufferRange16,
                                        const uint8_t* bufferReflectance8,
                                        const uint8_t* bufferScatter8,
                                        const int64_t bufferWidth,
                                        const int64_t bufferHeight,
                                        const int64_t aoiHeight,
                                        const int64_t aoiOffsetX,
                                        const int64_t aoiOffsetY,
                                        const RangeAxis rangeAxis,
                                        const std::string& filePath,
                                        const std::string& arbitraryXml)
{
  size_t bufferSize = (size_t)(bufferWidth * bufferHeight);
  RangeFrame frame;
  frame.aoiOffset((size_t)aoiOffsetX, (size_t)aoiOffsetY)
    .aoiSize((size_t)bufferWidth, (size_t)aoiHeight)
    .rangeAxis(rangeAxis);

  frame.createRange(bufferSize * 2, PixelWidth::PW16);
  frame.range()->data().assign(bufferRange16, bufferRange16 + 2 * bufferSize);

  if (bufferReflectance8 != nullptr)
  {
    frame.createReflectance(bufferSize, PixelWidth::PW8);
    frame.reflectance()->data().assign(bufferReflectance8,
                                       bufferReflectance8 + bufferSize);
  }
  if (bufferScatter8 != nullptr)
  {
    frame.createScatter(bufferSize, PixelWidth::PW8);
    frame.scatter()->data().assign(bufferScatter8, bufferScatter8 + bufferSize);
  }
  saveMultipartRangeFrame(frame, filePath, arbitraryXml);
}

GENIRANGER_API void saveBuffer8(const uint8_t* buffer,
                                const int64_t bufferWidth,
                                const int64_t bufferHeight,
                                const int64_t offsetX,
                                const int64_t offsetY,
                                const std::string& filePath,
                                const std::string& arbitraryXml)
{
  DatXmlWriter xml;
  xml.open("IMAGE", "IMAGE");
  {
    xml.addParameter("size", std::to_string(bufferWidth * bufferHeight));
    xml.addParameter("version", "1");
    xml.addParameter("layout", "SCAN");
    xml.openComponent("image", "Ranger3Image");
    {
      xml.addParameter("size", std::to_string(bufferWidth));
      xml.addParameter("height", std::to_string(bufferHeight));
      xml.openImageTraits();
      {
        xml.addParameter("size x", std::to_string(bufferWidth));
        xml.addParameter("size y", std::to_string(bufferHeight));
        xml.addParameter("offset x", std::to_string(offsetX));
        xml.addParameter("offset y", std::to_string(offsetY));
      }
      xml.closeImageTraits();
      xml.openSubComponent("BYTE", "Image");
      {
        xml.addParameter("size", std::to_string(bufferWidth));
        xml.addParameter("width", std::to_string(bufferWidth));
      }
      xml.closeSubComponent();
    }
    xml.closeComponent();
    xml.addArbitraryXml(arbitraryXml);
  }
  xml.close();

  DatAndXmlFiles files(filePath);
  files.writeData(buffer, bufferWidth * bufferHeight);
  files.writeXml(xml.toString());
}


GENIRANGER_API void saveMultipartRangeFrame(RangeFrame& frame,
                                            const std::string& filePath,
                                            const std::string& arbitraryXml)
{
  ComponentPtr range = frame.range();
  ComponentPtr reflectance = frame.reflectance();
  ComponentPtr scatter = frame.scatter();
  LineMetadataPtr metadata = frame.lineMetadata();

  if (!range)
  {
    throw SaveException("Range component must exist, when saving.");
  }
  if (range->pixelWidth() != PixelWidth::PW16)
  {
    throw SaveException(
      "Range component must have pixel width 16, when saving.");
  }
  if (scatter
      && !(scatter->pixelWidth() == PixelWidth::PW8
           || scatter->pixelWidth() == PixelWidth::PW16))
  {
    throw SaveException(
      "Scatter component must have pixel width 8, when saving.");
  }
  if (reflectance && reflectance->pixelWidth() != PixelWidth::PW8)
  {
    throw SaveException(
      "Reflectance component must have pixel width 8, when saving.");
  }

  const size_t aoiWidth = frame.aoiSize().x();
  DatAndXmlFiles files(filePath);

  files.writeData(range);
  size_t totalSize = sizeof(uint16_t) * aoiWidth;

  if (reflectance)
  {
    files.writeData(reflectance);
    totalSize += sizeof(uint8_t) * aoiWidth;
  }
  if (scatter)
  {
    files.writeData(scatter);
    totalSize += bytesPerPixel(scatter->pixelWidth()) * aoiWidth;
  }
  if (!metadata->empty())
  {
    files.writeMarkData(*metadata);
    totalSize += sizeof(DatAndXmlFiles::MarkdataWord) * 5;
  }

  DatXmlWriter xml;
  xml.open();
  {
    xml.addParameter("size", std::to_string(totalSize));
    xml.addParameter("version", "1");
    xml.addParameter("layout", "SUBCOMPONENT");
    xml.openComponent("Hi3D", "Ranger3Range");
    {
      xml.addParameter("size", std::to_string(totalSize));
      xml.addParameter("height", "1");
      writeSensorRangeTraits(xml, frame);

      writeSubComponent(xml, "Range", range->pixelWidth(), aoiWidth);
      if (reflectance)
      {
        writeSubComponent(
          xml, "Intensity", reflectance->pixelWidth(), aoiWidth);
      }
      if (scatter)
      {
        writeSubComponent(xml, "Scatter", scatter->pixelWidth(), aoiWidth);
      }
      if (!metadata->empty())
      {
        writeSubComponentMarkData(xml, aoiWidth);
      }
    }
    xml.closeComponent();
    xml.addArbitraryXml(arbitraryXml);
  }
  xml.close();

  files.writeXml(xml.toString());
}

}
