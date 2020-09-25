/**	@file	Typdef.h
*
*	@brief	typdef and #define set for STL \ GenICam \ SICK GenIRanger \ SICK common
*
*	@attention
*	This is the Ranger3 GenIcam API wrapper.
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/


#pragma once

#include "Version.h"

#include "FastMetadataExtractor.h"
#include "ChunkAdapter.h"
#include "Consumer.h"
#include "GenIRanger.h"
#include "SampleUtils.h"
#include "StreamData.h"

#include <map>
#include <set>
#include <string>
#include <vector>
#include <memory>


#ifdef DLL_EXPORTS
#   define EXPORT_TO_DLL   __declspec(dllexport)
#else
#   define EXPORT_TO_DLL   __declspec(dllimport)
#endif  

using std::cout;
using std::endl;



#ifdef NAMESPACE_SAMPLE

#define SiGetPathToProducer Sample::getPathToProducer
#define SiShowDeviceList Sample::showDeviceList
#define SiStringFromIp4 Sample::stringFromIp4
#define SiStringFromMac Sample::stringFromMac
#define SiIp4FromString Sample::ip4FromString

typedef Sample::Consumer SiConsumer;
typedef Sample::GenTLPort SiGenTLPort;
typedef Sample::ChunkAdapter SiChunkAdapter;
typedef Sample::InterfaceList SiInterfaceList;
typedef Sample::InterfaceId SiInterfaceId;

typedef Sample::DeviceList SiDeviceList;
typedef Sample::InterfaceList SiInterfaceList;
typedef Sample::InterfaceList SiInterfaceList;

#endif


#ifdef NAMESPACE_COMMON

#define SiGetPathToProducer common::getPathToProducer
#define SiShowDeviceList common::showDeviceList
#define SiStringFromIp4 common::stringFromIp4
#define SiStringFromMac common::stringFromMac
#define SiIp4FromString common::ip4FromString

typedef common::Consumer SiConsumer;
typedef common::GenTLPort SiGenTLPort;
typedef common::ChunkAdapter SiChunkAdapter;
typedef common::InterfaceList SiInterfaceList;
typedef common::InterfaceId SiInterfaceId;

typedef common::DeviceList SiDeviceList;
typedef common::InterfaceList SiInterfaceList;
typedef common::InterfaceList SiInterfaceList;

#endif


template <class T> using SPtr = std::shared_ptr<T>;
template <class T> using UPtr = std::unique_ptr<T>;
template <class T> using Vec  = std::vector<T>;

typedef GenApi::CNodeMapRef			GNodeMap;
typedef GenTL::BUFFER_HANDLE		GBufferH;
typedef GenTL::DEV_HANDLE			GDeviceH;
typedef GenTL::TL_HANDLE			GTL_H;
typedef GenTLApi *					GTl;
typedef GenIRanger::RangeAxis		RA;

typedef const std::string			cStr;
typedef std::string					Str;
typedef SPtr<SiConsumer>			ConsumerPtr;
typedef SPtr<GenTLApi>				GenTLPtr;
typedef Vec<GBufferH>				VecBufferHandle;
typedef Vec<Ranger3LineMetadata>	ChunkData;
typedef UPtr<SiChunkAdapter>		ChunkPtr;
