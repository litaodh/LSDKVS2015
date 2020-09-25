// Copyright 2019 SICK AG. All rights reserved.

#include "PartInfoCollection.h"

#include "EnumSelectorEntries.h"
#include "PFNC.h"
#include <algorithm>

PartInfoCollection
PartInfoCollection::fromBuffer(GenTLApi* tl,
                               GenTL::DS_HANDLE dataStreamHandle,
                               GenTL::BUFFER_HANDLE bufferHandle,
                               uint32_t bufferPartCount)
{
  PartInfoCollection object;
  for (uint32_t partIndex = 0; partIndex < bufferPartCount; ++partIndex)
  {
    object.mParts.push_back(
      common::BufferPartInfo(tl, dataStreamHandle, bufferHandle, partIndex));
  }
  return object;
}


const common::BufferPartInfo&
PartInfoCollection::findPart(size_t regionId, size_t componentId) const
{
  auto iter = findPartIterator(regionId, componentId);
  assert(iter != mParts.end());
  return *iter;
}


bool PartInfoCollection::hasPart(size_t regionId, size_t componentId) const
{
  auto iter = findPartIterator(regionId, componentId);
  return iter != mParts.end();
}


void PartInfoCollection::print(const EnumSelectorEntries& regions,
                               const EnumSelectorEntries& components) const
{
  std::cout << "Parts received:" << std::endl;
  for (const auto& part : mParts)
  {
    std::cout << "Part number " << part.mPartNumber << std::endl;
    std::cout << "  Region: " << regions.name(part.mRegionId) << std::endl;
    std::cout << "  Component: " << components.name(part.mPurposeId)
              << std::endl;
    std::cout << "  Pixel format: " << pixelFormatToString(part.mPartDataFormat)
              << std::endl;
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

std::vector<common::BufferPartInfo>::const_iterator
PartInfoCollection::findPartIterator(size_t regionId, size_t componentId) const
{
  return std::find_if(
    mParts.begin(), mParts.end(), [=](const common::BufferPartInfo& part) {
      return part.mRegionId == regionId && part.mPurposeId == componentId;
    });
}
