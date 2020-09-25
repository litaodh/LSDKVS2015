// Copyright 2017-2019 SICK AG. All rights reserved.

#include "Exceptions.h"
#include "FileOperation.h"

#include <GenApi/Filestream.h>

#include <fstream>
#include <mutex>
#include <sstream>

// Flag to abort an ongoing file transfer
bool gRequestAbort = false;
// Mutex for making abort request thread-safe,
// protecting access to gRequestAbort
std::mutex abortMutex;

using namespace GenApi;

namespace GenIRanger {

// Local helper functions
namespace {

const char* readFile(const std::string& path, size_t& fileSize)
{
  std::ifstream file(path, std::ios::binary | std::ios::ate | std::ios::in);
  if (!file.good())
  {
    std::stringstream ss;
    ss << "Unable to open file for reading: '" << path << "'";
    throw GenIRangerException(ss.str());
  }
  std::ifstream::pos_type pos = file.tellg();
  if (pos == std::ifstream::pos_type(-1))
  {
    std::stringstream ss;
    ss << "Unable to tell file size: '" << path << "'";
    throw GenIRangerException(ss.str());
  }
  fileSize = static_cast<size_t>(pos);
  file.seekg(0, std::ios::beg);

  char* buffer = new char[fileSize];
  if (file.read(buffer, fileSize))
  {
    return buffer;
  }
  else
  {
    delete buffer;
    std::stringstream ss;
    ss << "Unable to read file '" << path << "' with size " << fileSize << ".";
    throw GenIRangerException(ss.str());
  }
}

void writeFile(char* buffer,
               size_t bufferSize,
               const std::string& localFilePath)
{
  std::ofstream file(localFilePath,
                     std::ios::binary | std::ios::ate | std::ios::out);
  if (!file.good())
  {
    std::stringstream ss;
    ss << "Unable to open file for writing: '" << localFilePath << "'";
    throw GenIRangerException(ss.str());
  }
  file.write(buffer, bufferSize);
}

void tryClosingFile(IFileProtocolAdapter& adapter, const std::string& file)
{
  // Ignore return value
  adapter.closeFile(file.c_str());
}

}

GENIRANGER_API void sendFile(const std::string& localFilePath,
                             const std::string& destinationFile,
                             GenApi::INodeMap* const nodeMap,
                             GenApi::IFileProtocolAdapter& fileProtocolAdapter)
{
  bool success = fileProtocolAdapter.attach(nodeMap);
  if (!success)
  {
    throw GenIRangerException("Unable to attach node map");
  }

  success =
    fileProtocolAdapter.openFile(destinationFile.c_str(), std::ios_base::trunc);
  if (!success)
  {
    std::stringstream ss;
    ss << "Unable to open remote file '" << destinationFile << "'.";
    throw GenIRangerException(ss.str());
  }

  size_t fileSize;
  std::unique_ptr<const char> buffer(readFile(localFilePath, fileSize));

  size_t bytesTransfered = 0;
  bool aborted = false;
  bool failedOrAborted = false;

  // TODO: RNX-3679: Remove this retry logic, when device responds timely during
  // file upload
  int const maxRetries = 10;
  int retryCount = 0;

  do
  {
    size_t remaningBytes = fileSize - bytesTransfered;
    // Transfer chunks to make it possible to cleanly abort the file transfer
    size_t chunk = 4096;
    if (remaningBytes < chunk)
    {
      chunk = remaningBytes;
    }
    size_t writtenBytes = static_cast<size_t>(
      fileProtocolAdapter.write(buffer.get() + bytesTransfered,
                                bytesTransfered,
                                chunk,
                                destinationFile.c_str()));

    if (writtenBytes <= 0)
    {
      std::cout << "Error during file upload. Transferred " << bytesTransfered
                << " bytes so far." << std::endl;
      retryCount++;

      if (retryCount < maxRetries)
      {
        std::cout << "Retrying... attempt " << retryCount << std::endl;
      }
      else
      {
        std::cout << "Reached maximum number of retries" << std::endl;
        tryClosingFile(fileProtocolAdapter, destinationFile);
        throw GenIRangerException("Error while writing file");
      }
    }
    else
    {
      bytesTransfered += writtenBytes;
    }

    std::lock_guard<std::mutex> lock(abortMutex);
    aborted = gRequestAbort;
    failedOrAborted = gRequestAbort;
  } while (bytesTransfered < fileSize && !aborted);

  {
    std::lock_guard<std::mutex> lock(abortMutex);
    gRequestAbort = false;
  }

  std::stringstream ss;
  if (aborted)
  {
    ss << "File transfer of '" << destinationFile << "' was aborted.";
  }
  else if (bytesTransfered != fileSize)
  {
    failedOrAborted = true;
    ss << "Unable to write remote file '" << destinationFile << "'.";
  }

  tryClosingFile(fileProtocolAdapter, destinationFile);
  if (failedOrAborted)
  {
    throw GenIRangerException(ss.str());
  }
}

GENIRANGER_API void abortSendFile()
{
  std::lock_guard<std::mutex> lock(abortMutex);
  gRequestAbort = true;
}

GENIRANGER_API void retrieveFile(const std::string& deviceFile,
                                 const std::string& destinationPath,
                                 GenApi::INodeMap* const nodeMap)
{
  GenApi::IDevFileStreamBuf<char, std::char_traits<char>> remoteFile;
  auto res = remoteFile.open(
    nodeMap, deviceFile.c_str(), std::ios_base::in | std::ios_base::binary);
  if (res == nullptr)
  {
    std::stringstream ss;
    ss << "Unable to open device file '" << deviceFile << "'.";
    throw GenIRangerException(ss.str());
  }

  std::ofstream outputFile;
  outputFile.open(destinationPath.c_str(),
                  std::ios_base::trunc | std::ios_base::binary);
  if (!outputFile.good())
  {
    std::stringstream ss;
    ss << "Unable to open destination file '" << destinationPath << "'.";
    throw GenIRangerException(ss.str());
  }

  outputFile << &remoteFile;

  remoteFile.close();

  if (!outputFile.good())
  {
    std::stringstream ss;
    ss << "Error during retrieval of '" << deviceFile << "'.";
    throw GenIRangerException(ss.str());
  }
}

GENIRANGER_API void
deleteFile(const std::string& destinationFile,
           GenApi::INodeMap* const nodeMap,
           GenApi::IFileProtocolAdapter& fileProtocolAdapter)
{
  bool success = fileProtocolAdapter.attach(nodeMap);
  if (!success)
  {
    throw GenIRangerException("Unable to attach node map");
  }

  success = fileProtocolAdapter.deleteFile(destinationFile.c_str());
  if (!success)
  {
    std::stringstream ss;
    ss << "Unable to delete remote file '" << destinationFile << "'.";
    throw GenIRangerException(ss.str());
  }
}

GENIRANGER_API void updateFirmware(const std::string& sourceFilePath,
                                   uint32_t crc32,
                                   GenApi::INodeMap* const nodeMap)
{
  GenApi::FileProtocolAdapter fileProtocolAdapter;
  const std::string updateFileGeniName("Update");

  sendFile(sourceFilePath, updateFileGeniName, nodeMap, fileProtocolAdapter);

  // Set Checksum register
  CIntegerPtr crcInteger = nodeMap->GetNode("FirmwareChecksum");

  if (crcInteger.IsValid())
  {
    crcInteger->SetValue(crc32);
  }
  else
  {
    throw GenIRangerException("FirmwareChecksum node not found in node map.");
  }

  // Trigger update
  CCommandPtr updateCommand = nodeMap->GetNode("FirmwarePerformUpdate");
  if (updateCommand.IsValid())
  {
    updateCommand->Execute();
  }
  else
  {
    throw GenIRangerException(
      "FirmwarePerformUpdate node not found in node map.");
  }
}

}
