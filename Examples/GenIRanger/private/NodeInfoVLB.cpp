/** @brief NodeInfo.cpp \n \n
|	
|	Overload class of NodeImporterVLB and NodeExporterVLB.
|
|	Add save parameter as "parameter.tmp" into NodeExporterVLB.
|
|
|
*/
// Copyright 2016-2019 SICK AG. All rights reserved.


#include "NodeInfoVLB.h"
#include "Exceptions.h"
#include "GenIUtil.h"
#include "NodeUtil.h"

#include <algorithm>
#include <sstream>

using namespace GenApi;
using namespace GenICam;

namespace GenIRanger {


#pragma region Writer

ConfigWriterVLB::ConfigWriterVLB(std::ostream& outputStream)
	: mOutputStream(outputStream)
	, mIndent(0)
{
	mOutputStream << "#Version,1" << std::endl;
}

void ConfigWriterVLB::outputCategory(std::string& name)
{
	if (name.empty())
	{
		return;
	}

	// Output category
	GenIUtil::log(std::string(mIndent, ' ').append("#" + name + '\n'));
	mOutputStream << "#" << name << std::endl;
}

static bool compareVLB(CNodePtr lh, CNodePtr rh)
{
	return lh->GetName() < rh->GetName();
}

void ConfigWriterVLB::outputLeaf(std::string& name, std::string& value, const GenApi::CNodePtr& node)
{
	if (name.empty() || value.empty())
	{
		return;
	}

	// Output node name
	GenIUtil::log(std::string(mIndent, ' ').append(name));
	mOutputStream << name;

	// Add optional value bu Vision Lab
	// Store all optional values
	StringList_t sysbolics; 

	// If the value is dependent on selectors, output the selectors and their
	// values. This will be used as the key value when importing the node value.
	if (mSelectors.size() > 0)
	{
		std::stringstream ss;
		// Copy selector list so that it can be sorted.
		// The original is treated like a stack and should not be altered.
		auto sorted = mSelectors;
		std::sort(sorted.begin(), sorted.end(), compareVLB);
		for (auto it = sorted.begin(); it != sorted.end(); it++)
		{
			auto selector = *it;
			auto name = selector->GetName();
			try
			{
				ss << "_";
				if (NodeUtil::isInteger(selector))
				{
					CIntegerPtr intSelector = static_cast<CIntegerPtr>(selector);
					ss << name << "_" << intSelector->GetValue();
				}
				else if (NodeUtil::isEnumeration(selector))
				{
					CEnumerationPtr enumSelector = static_cast<CEnumerationPtr>(selector);
					ss << name << "_" << enumSelector->GetCurrentEntry()->GetSymbolic();
				}
				else
				{
					std::stringstream ss;
					ss << "Unsupported selector: " << name;

					GenIUtil::throwAndLog(ss.str());
				}
			}
			catch (GenericException& e)
			{
				std::stringstream ss;
				ss << "Cannot read " << name
					<< ". Library exception: " << e.GetDescription();
				GenIUtil::throwAndLog(ss.str());
			}
		}
		GenIUtil::log(ss.str());
		mOutputStream << ss.str();
	}
	// Lastly output the node value
	GenIUtil::log("," + value + '\n');
	mOutputStream << "," << value;

	// Add optional value by Vision Lab
	std::stringstream ss;
	if (NodeUtil::isEnumeration(node))
	{
		CEnumerationPtr enumSelector = static_cast<CEnumerationPtr>(node);
		enumSelector->GetSymbolics(sysbolics);
		std::vector<std::string> snames;
		for (auto sub : sysbolics)
			ss << sub << " ";
	}
	mOutputStream << "," << ss.str();
	
	// Add ifReadOnly by Vision Lab
	mOutputStream << (NodeUtil::isConfigNode(node) ? ",RW" : ",R");
	mOutputStream << '\n';


}

void ConfigWriterVLB::indent()
{
	mIndent = mIndent + 2;
}

void ConfigWriterVLB::unindent()
{
	mIndent = mIndent - 2;
	// TODO test that this shouldn't happen
	mIndent = mIndent < 0 ? 0 : mIndent;
}

void ConfigWriterVLB::pushCategory()
{
	indent();
}

void ConfigWriterVLB::popCategory()
{
	unindent();
}

void ConfigWriterVLB::pushSelector(const CNodePtr& node)
{
	mSelectors.push_back(node);
}

void ConfigWriterVLB::popSelector()
{
	mSelectors.pop_back();
}

#pragma endregion


#if 0

#pragma region Reader

const int VERSION = 1;

void GetKeyValueVLB(std::string line, std::string& key, std::string& value)
{
	std::string element;
	std::stringstream stream(line);
	std::vector<std::string> values;

	while (std::getline(stream, element, ','))
	{
		element.erase(std::remove(element.begin(), element.end(), '\n'),
			element.end());
		values.push_back(element);
	}
	if (values.size() == 2)
	{
		key = values.at(0);
		value = values.at(1);
	}
	else
	{
		std::string error("No <key>,<value> pair found");
		GenIUtil::throwAndLog(error);
	}
}
ConfigReaderVLB::ConfigReaderVLB()
{
	// Empty
}

#if 0
ConfigurationResult ConfigReaderVLB::parse(std::istream& csv)
{
	// First row contains the format version
	std::string versionLine;
	std::getline(csv, versionLine, '\n');

	if (!versionLine.empty())
	{
		std::string versionStr;
		std::string versionNumber;
		GetKeyValueVLB(versionLine, versionStr, versionNumber);

		if (std::stoi(versionNumber) == VERSION)
		{
			std::string line;
			std::string key;
			std::string value;
			while (std::getline(csv, line, '\n'))
			{
				// Skip lines starting with #
				if (line.find('#') != std::string::npos)
				{
					continue;
				}

				GetKeyValueVLB(line, key, value);
				if (mNodeValueMap.find(key) != mNodeValueMap.end())
				{
					ConfigurationResult result(ConfigurationStatus::DUPLICATE_PARAMETER,
						"Duplicate parameter found: " + key);
					GenIUtil::log(result.message);
					return result;
				}
				mNodeValueMap.insert(NodeValueMap::value_type(key, value));
			}
			return ConfigurationResult(ConfigurationStatus::OK,
				"Successfully parsed file");
		}
		else
		{
			ConfigurationResult result(ConfigurationStatus::BAD_VERSION,
				"Version number does not match the parser");
			GenIUtil::log(result.message);
			return result;
		}
	}
	else
	{
		ConfigurationResult result(ConfigurationStatus::EMPTY_PARAMETER_FILE,
			"Empty parameter file");
		GenIUtil::log(result.message);
		return result;
	}
}

#endif // 0

ConfigReaderVLB::~ConfigReaderVLB()
{
	// Empty
}

bool ConfigReaderVLB::hasValue(std::string& key)
{
	return mNodeValueMap.find(key) != mNodeValueMap.end();
}

std::string ConfigReaderVLB::getValue(std::string& key)
{
	if (hasValue(key))
	{
		return mNodeValueMap.find(key)->second;
	}
	else
	{
		std::stringstream ss;
		ss << key << " cannot be found in configuration file";
		GenIUtil::throwAndLog(ss.str());
		return std::string();
	}
}

std::set<std::string> ConfigReaderVLB::getKeys()
{
	std::set<std::string> keys;
	for (auto it = mNodeValueMap.begin(); it != mNodeValueMap.end(); ++it)
	{
		keys.insert(it->first);
	}
	return keys;
}

#pragma endregion



#pragma region Importer

NodeImporterVLB::NodeImporterVLB(ConfigReaderVLB reader)
	: mReader(reader)
	, mRemainingToImport(reader.getKeys())
{
}

NodeImporterVLB::~NodeImporterVLB()
{
}

void NodeImporterVLB::enterSelector(CNodePtr selector)
{
	mSelectors.push_back(selector);
}

void NodeImporterVLB::leaveSelector(CNodePtr node)
{
	mSelectors.pop_back();
}


void NodeImporterVLB::leaveCategory(CCategoryPtr category)
{
	// When all nodes have been traversed
	auto categoryName = category->GetNode()->GetName(true);
	if (categoryName == "Std::Root")
	{
		checkUnimportedParameters();
	}
}

//static bool compareVLB(CNodePtr lh, CNodePtr rh)
//{
//	return lh->GetName() < rh->GetName();
//}

void NodeImporterVLB::onLeaf(CNodePtr node)
{
	std::string nodeName = node->GetName();
	std::string key(nodeName);

	if (mSelectors.size() > 0)
	{
		std::stringstream ss;
		// Copy selector vector, so it can be sorted without modifying the original
		auto sorted = mSelectors;
		std::sort(sorted.begin(), sorted.end(), compareVLB);
		for (auto it = sorted.begin(); it != sorted.end(); it++)
		{
			auto selector = *it;
			ss << "_" << selector->GetName();
			ss << "_" << NodeUtil::getValueAsString(selector);
		}
		key.append(ss.str());
	}

	if (NodeUtil::isConfigNode(node))
	{
		if (mReader.hasValue(key))
		{
			std::string readValue = mReader.getValue(key);
			try
			{
				auto type = node->GetPrincipalInterfaceType();
				if (type == intfIInteger)
				{
					CIntegerPtr value = static_cast<CIntegerPtr>(node);
					value->FromString(gcstring(readValue.c_str()), false);
				}
				else if (type == intfIFloat)
				{
					CFloatPtr value = static_cast<CFloatPtr>(node);
					value->FromString(gcstring(readValue.c_str()), false);
				}
				else if (type == intfIString)
				{
					CStringPtr value = static_cast<CStringPtr>(node);
					value->SetValue(gcstring(readValue.c_str()), false);
				}
				else if (type == intfIBoolean)
				{
					CBooleanPtr value = static_cast<CBooleanPtr>(node);
					value->FromString(gcstring(readValue.c_str()), false);
				}
				else if (type == intfIEnumeration)
				{
					CEnumerationPtr value = static_cast<CEnumerationPtr>(node);
					value->FromString(gcstring(readValue.c_str()), false);
				}
				else
				{
					// Do nothing, the other types aren't interesting
				}

				// Mark this key as imported
				mRemainingToImport.erase(key);

				GenIUtil::log("Set " + key + " " + readValue + "\n");
			}
			catch (GenericException& e)
			{
				std::stringstream ss;
				ss << "Cannot set " << key << " = " << readValue
					<< ". Library exception: " << e.GetDescription();

				mErrors.push_back(ss.str());
			}
			catch (std::exception& e)
			{
				std::stringstream ss;
				ss << "Cannot set " << key << " = " << readValue << ". " << e.what();

				mErrors.push_back(ss.str());
			}
		}
		else
		{
			std::stringstream ss;
			ss << "Parameter " << key << " cannot be found in configuration file";
			mMissing.push_back(ss.str());
		}
	}
	else
	{
		// Not a configuration node, just ignore
		if (mReader.hasValue(key))
		{
			std::stringstream ss;
			ss << "Parameter " << key << " was found in configuration file, but is "
				<< "not a configuration parameter";
			mErrors.push_back(ss.str());
		}
	}
}

void NodeImporterVLB::checkUnimportedParameters()
{
	for (auto it = mRemainingToImport.begin(); it != mRemainingToImport.end();
		++it)
	{
		std::stringstream ss;
		ss << "Parameter " << *it << " was found in configuration file, but is "
			<< "not a configuration parameter";
		mErrors.push_back(ss.str());
	}
}

const std::vector<std::string>& NodeImporterVLB::getErrors() const
{
	return mErrors;
}

const std::vector<std::string>& NodeImporterVLB::getMissing() const
{
	return mMissing;
}

#pragma endregion

#endif // 0

#pragma region Exporter


NodeExporterVLB::NodeExporterVLB(ConfigWriterVLB& formatter)
	: mFormat(formatter)
{
	// Empty
}

NodeExporterVLB::~NodeExporterVLB()
{
	// Empty
}

const std::vector<std::string>& NodeExporterVLB::getErrors() const
{
	return mErrors;
}

void NodeExporterVLB::enterCategory(CCategoryPtr node)
{
	mFormat.outputCategory(std::string(node->GetNode()->GetName()));
	mFormat.pushCategory();
}

void NodeExporterVLB::leaveCategory(CCategoryPtr category)
{
	mFormat.popCategory();
}

void NodeExporterVLB::enterSelector(CNodePtr selector)
{
	mFormat.pushSelector(selector);
}

void NodeExporterVLB::leaveSelector(CNodePtr node)
{
	mFormat.popSelector();
}

void NodeExporterVLB::onLeaf(CNodePtr node)
{
	std::string name(node->GetName().c_str());
	std::stringstream valueStream;
	try
	{
		if (NodeUtil::isConfigNode(node))
		{
			valueStream << NodeUtil::getValueAsString(node);
		}
		else
		{
			// Do nothing, the other types aren't interesting
		}
		mFormat.outputLeaf(name, valueStream.str(), node);
	}
	catch (GenericException& e)
	{
		std::stringstream ss;
		ss << "Cannot read " << name
			<< ". Library exception: " << e.GetDescription();
		mErrors.push_back(ss.str());
	}
	catch (std::exception& e)
	{
		std::stringstream ss;
		ss << "Cannot read " << name << ". " << e.what();
		mErrors.push_back(ss.str());
	}
}

#pragma endregion





}