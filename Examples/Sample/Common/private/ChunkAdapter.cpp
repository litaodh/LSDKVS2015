// Copyright 2018-2019 SICK AG. All rights reserved.

#include "ChunkAdapter.h"

namespace common {

ChunkAdapter::ChunkAdapter(GenTLApi* tl, GenTL::DS_HANDLE dataStreamHandle)
  : mTl(tl)
  , mDataStreamHandle(dataStreamHandle)
  , mAdapter(new GenApi::CChunkAdapterGEV())
{
  // Empty
}

void ChunkAdapter::attachNodeMap(GenApi::INodeMap* nodeMap)
{
  mAdapter->AttachNodeMap(nodeMap);
}

void ChunkAdapter::detachNodeMap()
{
  mAdapter->DetachNodeMap();
}

void ChunkAdapter::attachBuffer(GenTL::BUFFER_HANDLE handle, uint8_t* buffer)
{
  GenApi::AttachStatistics_t statistics;
  size_t chunkPayloadSize = getChunkPayloadSize(handle);
  if (!mAdapter->CheckBufferLayout(buffer, chunkPayloadSize))
  {
    throw std::exception("Buffer has unknown chunk layout");
  }
  mAdapter->AttachBuffer(buffer, chunkPayloadSize, &statistics);

  // Ranger3 uses a single chunk port for all metadata.
  if (statistics.NumChunkPorts != 1)
  {
    throw std::exception("A single chunk port was expected");
  }

  // There should be one chunk for the metadata port and one for
  // wrapping the image data.
  if (statistics.NumChunks != 2)
  {
    throw std::exception("Two chunks were expected");
  }

  // Only the metadata chunk should be attached.
  if (statistics.NumAttachedChunks != 1)
  {
    throw std::exception("A single attached chunk was expected");
  }
}

void ChunkAdapter::detachBuffer()
{
  mAdapter->DetachBuffer();
}

/**
   Get the actual chunk payload size from a buffer. This is needed to
   allow the chunk adapter to find the chunk trailer information when
   attaching.
*/
size_t ChunkAdapter::getChunkPayloadSize(GenTL::BUFFER_HANDLE handle)
{
  size_t result;
  size_t size = sizeof(result);
  GenTL::INFO_DATATYPE dataType;
  CC(mTl,
     mTl->DSGetBufferInfo(mDataStreamHandle,
                          handle,
                          GenTL::BUFFER_INFO_DELIVERED_CHUNKPAYLOADSIZE,
                          &dataType,
                          &result,
                          &size));
  return result;
}

}
