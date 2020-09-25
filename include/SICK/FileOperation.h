// Copyright 2017-2019 SICK AG. All rights reserved.

#pragma once

#include "GenICam.h"
#include "GenIRangerDll.h"

namespace GENAPI_NAMESPACE {
struct IFileProtocolAdapter;
}

namespace GenIRanger {

/** Sends a file to the device.

\param sourceFilePath Path to the local file that should be sent to the
device.
\param destinationFile The name of the GenICam file on the device, i.e. the
FileSelector.
\param nodeMap The node map
\param fileProtocolAdapter Adapter between the std::iostreambuf and the SFNC
Features representing the device file system
*/
GENIRANGER_API void sendFile(const std::string& sourceFilePath,
                             const std::string& deviceFile,
                             GenApi::INodeMap* const nodeMap,
                             GenApi::IFileProtocolAdapter& fileProtocolAdapter);

/** Aborts ongoing sendFile() operation */
GENIRANGER_API void abortSendFile();

/** Retrieves a file from the device.

\param sourceFile The name of the GenICam file on the device, i.e. the
FileSelector.
\param destinationFilePath Path to the local destination file
\param nodeMap The node map
*/
GENIRANGER_API void retrieveFile(const std::string& deviceFile,
                                 const std::string& destinationFilePath,
                                 GenApi::INodeMap* const nodeMap);

/** Removes a file on the device.

\param destinationFile The name of the target file on the device.
\param nodeMap The node map
\param fileProtocolAdapter Adapter between the std::iostreambuf and the SFNC
Features representing the device file system
*/
GENIRANGER_API void
deleteFile(const std::string& destinationFile,
           GenApi::INodeMap* const nodeMap,
           GenApi::IFileProtocolAdapter& fileProtocolAdapter);

/** Updates firmware with the specified package.

\param firmwarePackagePath Path to the firmware package
\param crc32 The CRC checksum of the firmware package
\param nodeMap The node map
*/
GENIRANGER_API void updateFirmware(const std::string& sourceFilePath,
                                   uint32_t crc32,
                                   GenApi::INodeMap* const nodeMap);
}
