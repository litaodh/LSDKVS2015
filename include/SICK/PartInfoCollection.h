// Copyright 2019 SICK AG. All rights reserved.

#pragma once

#include "BufferInfoVLB.h"
#include <vector>

class EnumSelectorEntries;

class PartInfoCollection
{
public:
  /** Create a BufferPartInfoCollection with the parts in a buffer.
      \param tl The GenTLApi
      \param dataStreamHandle Handle to the data stream
      \param bufferHandle Handle to the buffer to get the parts from
      \param bufferPartCount The number of parts available in the buffer
  */
  static PartInfoCollection fromBuffer(GenTLApi* tl,
                                       GenTL::DS_HANDLE dataStreamHandle,
                                       GenTL::BUFFER_HANDLE bufferHandle,
                                       uint32_t bufferPartCount);

  /** Find the part with a specific component in a specific region.
      \param regionId The Region which the Component belongs to
      \param componentId The Component to find the part for
  */
  const common::BufferPartInfo& findPart(size_t regionId,
                                         size_t componentId) const;

  /** Check if there is a part with a specific component in a specific region.
      \param regionId The Region which the Component belongs to
      \param componentId The Component to look for
  */
  bool hasPart(size_t regionId, size_t componentId) const;

  /** Print information on the parts to stdout. */
  void print(const EnumSelectorEntries& regions,
             const EnumSelectorEntries& components) const;

private:
  PartInfoCollection() {}

  std::vector<common::BufferPartInfo>::const_iterator
  findPartIterator(size_t regionId, size_t componentId) const;

private:
  std::vector<common::BufferPartInfo> mParts;
};
