// Copyright 2018-2019 SICK AG. All rights reserved.

#pragma once

#include <GenApi/ChunkAdapterGEV.h>
#include <GenTLApi.h>

#include <memory>

namespace common {

/**
   Wrapper class for a GigE Vision chunk adapter. Creating an instance
   of this class and attaching it to a node map and a buffer will make
   sure you can access chunk meta data via the node map.
 */
class __declspec(dllexport) ChunkAdapter
{
public:
  ChunkAdapter(GenTLApi* tl, GenTL::DS_HANDLE dataStreamHandle);

  /** Attach a nodemap to the adapter to allow accessing the metadata
      via the node map.
  */
  void attachNodeMap(GenApi::INodeMap* nodeMap);

  /** Detach the nodemap from the adapter. */
  void detachNodeMap();

  /** Attach the chunk adapter to a specific buffer. This allows
      access to the chunk metadata carried in the buffer.
   */
  void attachBuffer(GenTL::BUFFER_HANDLE handle, uint8_t* buffer);
  /** Detach the from the buffer when done with it. */
  void detachBuffer();

private:
  size_t getChunkPayloadSize(GenTL::BUFFER_HANDLE handle);

private:
  GenTLApi* mTl;
  GenTL::DS_HANDLE mDataStreamHandle;
  std::unique_ptr<GenApi::CChunkAdapterGEV> mAdapter;
};

}
