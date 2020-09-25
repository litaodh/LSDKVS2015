/**	@file	Parameters.cpp
*
*	@brief	Define Class Selector, Parameter and ParameterVector.
*
*	@attention
*	These classes are used to parameter setting and getting.
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/


#include "Parameters.h"


#include<cstdio>

namespace SickCam
{

////////////////////////////////////////////////////////////////////////////////

	
Selector::Selector()
	:	m_key(""), m_value(""), m_parentKey("")
{
}

Selector::~Selector()
{

}

std::map<std::string, SelecPtr> 
Selector::mapSelectors = std::map<std::string, SelecPtr>();

bool 
Selector::intiSelector(const std::string & _line, const std::string & son_key_value)
{
	if (_line.empty())	return false;
	
	// prase string a line, format NodeName_Selector1Key_Selector1Vlaue_Selector2Key_Selector2Vlaue_...------------------------
	std::stringstream csvName(_line);
	std::string tt;
	std::vector<std::string> values;

	while (std::getline(csvName, tt, '_'))
	{
		tt.erase(std::remove(tt.begin(), tt.end(), '\n'), tt.end());
		values.push_back(tt);
	}

	if (values.size()<2) return false;
	
	size_t begin(0);
	if (values.size() % 2 == 1)
	{
		m_key = values[1];
		m_value = values[2];
		begin = values[0].size() + values[1].size() + values[2].size() + 3;
	}
	else
	{
		m_key = values[0];
		m_value = values[1];
		begin = values[0].size() + values[1].size() + 2;
	}

	if (_line.size() <= begin)
	{
		m_parentKey = "";
		return false;
	}

	if (values.size() >= 2)
	{
		m_parentKey = _line.substr(begin, _line.size() - begin);

		if (!m_parentKey.empty())
		{
			if (mapSelectors.count(m_parentKey) == 0)
			{
				std::shared_ptr<Selector> parent = std::make_shared<Selector>();
				mapSelectors.insert({ m_parentKey, parent });
				std::string sonOfNext(m_key + "_" + m_value);
				mapSelectors[m_parentKey]->intiSelector(m_parentKey, sonOfNext);
			}
		}
		else
			m_parentKey = "";

		return true;
	}
	else
	{
		m_parentKey = "";
		return false;
	}
}

bool 
Selector::hasParent() const
{
	return !m_parentKey.empty();
}

SelecPtr 
Selector::getParent()
{
	if (m_parentKey.empty())
		return nullptr;

	return mapSelectors[m_parentKey];
};


////////////////////////////////////////////////////////////////////////////////


void 
GetKeyValue(const std::string& line, 
			std::string& key, 
			std::string& value, 
			std::string& options, 
			std::string& RWMode)
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
	if (values.size() == 3)
	{
		key = values.at(0);
		value = values.at(1);
		options = values.at(2);
	}
	if (values.size() == 4)
	{
		key = values.at(0);
		value = values.at(1);
		options = values.at(2);
		RWMode = values.at(3);
	}
}


////////////////////////////////////////////////////////////////////////////////


bool 
Parameter::initValueFromDevice(const std::string & _aLine, const std::string & _category)
{
	// prase string a line, format NodeName_Selector1Key_Selector1Vlaue_Selector2Key_Selector2Vlaue_...------------------------
	if (_aLine.empty() || _category.empty()) return false;

	std::string opts, rw;
	m_category = _category;
	GetKeyValue(_aLine, m_csvName, m_strValue, opts, rw);

	m_readOnly = rw == "R";

	std::stringstream stream(m_csvName);
	std::getline(stream, m_name, '_');

	if (!opts.empty())
	{
		std::stringstream ssOpts(opts);
		std::string opt;
		while (std::getline(ssOpts, opt, ' '))
		{
			opt.erase(std::remove(opt.begin(), opt.end(), '\n'), opt.end());
			m_options.push_back(opt);
		}
	}

	if (m_csvName.empty()) return false;
	
	m_selecPtr = std::make_shared<Selector>();
	m_selecPtr->intiSelector(m_csvName, std::string(""));

	
	return true;
}

bool 
Parameter::setValueToDevice(const GenApi::CNodeMapRef & _nodeMap, const std::string & _v)
{
	if (isReadOnly()) return false;

	if (!modifyValue(_v)) return false;

	if (false == moveSelector(_nodeMap)) return false;
	
	// get type by node name, set parameter by string.
	auto node = _nodeMap._GetNode(m_name.c_str());
	if (node == nullptr)
	{
		std::cout << "node == nullptr" << std::endl;
		return false;
	}

	auto type = node->GetPrincipalInterfaceType();
	if (type == intfIInteger)
	{
		CIntegerPtr value = static_cast<CIntegerPtr>(node);
		value->FromString(gcstring(_v.c_str()), false);
	}
	else if (type == intfIFloat)
	{
		CFloatPtr value = static_cast<CFloatPtr>(node);
		value->FromString(gcstring(_v.c_str()), false);
	}
	else if (type == intfIString)
	{
		CStringPtr value = static_cast<CStringPtr>(node);
		value->SetValue(gcstring(_v.c_str()), false);
	}
	else if (type == intfIBoolean)
	{
		CBooleanPtr value = static_cast<CBooleanPtr>(node);
		value->FromString(gcstring(_v.c_str()), false);
	}
	else if (type == intfIEnumeration)
	{
		CEnumerationPtr value = static_cast<CEnumerationPtr>(node);
		value->FromString(gcstring(_v.c_str()), false);
	}
	else
	{
		// Do nothing, the other types aren't interesting
		return false;
	}

	m_strValue = _v;
	return true;
}

bool 
Parameter::getValueFromDevice(const GenApi::CNodeMapRef & _nodeMap, std::string & _v)
{
	// move selectors
	if (false == moveSelector(_nodeMap)) return false;

	// get type by node name, set parameter by string.
	auto node = _nodeMap._GetNode(m_name.c_str());

	if (node == nullptr)
	{
		std::cout << "node == nullptr" << std::endl;
		return false;
	}
	
	auto type = node->GetPrincipalInterfaceType();
	if (type == intfIInteger || type == intfIString || 
		type == intfIBoolean || type == intfIEnumeration)
	{
		CValuePtr value = static_cast<CValuePtr>(node);
		_v = std::string(value->ToString().c_str());
	}
	else if (type == intfIFloat)
	{
		CFloatPtr value = static_cast<CFloatPtr>(node);
		_v = std::string(value->ToString().c_str());
	}
	else
	{
		// Do nothing, the other types aren't interesting
		return false;
	}

	if (_v.empty()) return false;

	return modifyValue(_v);
}

bool Parameter::getValueConstraintFromDevice(const GenApi::CNodeMapRef & _nodeMap, std::string & _minV, std::string & _maxV, std::string & _incV)
{
	// move selectors
	if (false == moveSelector(_nodeMap)) return false;

	// get type by node name, set parameter by string.
	auto node = _nodeMap._GetNode(m_name.c_str());

	if (node == nullptr)
	{
		std::cout << "node == nullptr" << std::endl;
		return false;
	}

	auto type = node->GetPrincipalInterfaceType();
	if (type == intfIInteger || type == intfIString ||
		type == intfIBoolean || type == intfIEnumeration)
	{
		CIntegerPtr value = static_cast<CValuePtr>(node);
		_minV = std::to_string(value->GetMax());
		_maxV = std::to_string(value->GetMin());
		_incV = std::to_string(value->GetInc());

		if (_minV.empty() || _maxV.empty() || _incV.empty())
			return false;
	}
	else if (type == intfIFloat)
	{
		CFloatPtr value = static_cast<CFloatPtr>(node);
		_minV = std::to_string(value->GetMax());
		_maxV = std::to_string(value->GetMin());
		
		if (_minV.empty() || _maxV.empty())
			return false;
	}
	else
	{
		// Do nothing, the other types aren't interesting
		return false;
	}

	return true;
}

bool 
Parameter::modifyValue(const std::string & _v)
{
	if (_v.empty()) return false;
	m_strValue = _v;
	return true;
}

bool 
Parameter::isWithInOptitons(const std::string & _v) const
{
	if (m_options.empty())	return false;

	return (std::find(m_options.begin(), m_options.end(), _v) == m_options.end());
}

bool 
Parameter::moveSelector(const GenApi::CNodeMapRef & _nodeMap)
{
	GenApi::CEnumerationPtr selectorPtr;
	SelecPtr nextSel = this->getSeletor();
	if (nextSel->isValid())
	{
		std::vector<std::string> vKeys, vValues;
		while (true)
		{
			vKeys.push_back(nextSel->getKey());
			vValues.push_back(nextSel->getValue());
			if (nextSel->hasParent())
				nextSel = nextSel->getParent();
			else
				break;
		}

		if (vKeys.empty() || vValues.empty() || (vKeys.size() != vValues.size())) return false;

		for (int i = vKeys.size() - 1; i>=0; --i)
		{
			selectorPtr = _nodeMap._GetNode(vKeys[i].c_str());
			*selectorPtr = vValues[i].c_str();
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////


ParameterVector::ParameterVector() : 
	m_RectMethod		(RectMethod::Top	),
	m_scaleY			(1.0				),
	m_missingData		(NAN				),
	m_CaliPath			(""					),
	m_ParaPath			(""					),
	m_timeOut			(DEFAULT_TIMEOUT	),
	m_doRectify			(true				),
	m_buffersCount		(DEFAULT_BUF_COT	),
	m_TmpPath			("parameter.tmp"	)
{}

ParameterVector::~ParameterVector()
{}

bool 
ParameterVector::ScanDeviceParameters(INodeMap * const nodeMap, const std::string path)
{
	// save parameter.tmp, Overload NodeExporter
	std::ofstream file(m_TmpPath);
	GenIRanger::exportDeviceParametersVLB(nodeMap, file);

	// prase and init parameters, Overload NodeImporter
	std::ifstream ifile(m_TmpPath);
	if (!ifile.is_open())	return false;
	
	// clear class variables before set new values;
	m_AllParams.clear();
	m_Params.clear();
	m_Categories.clear();
	m_Categories.shrink_to_fit();
	m_Categories.resize(0);

	// parse file
	std::string versionLine;
	std::getline(ifile, versionLine, '\n');

	if (!versionLine.empty())
	{
		std::string versionStr;
		std::string versionNumber;
		GetKeyValue(versionLine, versionStr, versionNumber, std::string(), std::string());

		if (std::stoi(versionNumber) == VERSION)
		{
			std::string line;
			std::string key;
			std::string value;
			while (std::getline(ifile, line, '\n'))
			{
				// # is Category Name
				if (line.find('#') != std::string::npos)
				{
					m_Categories.push_back(line.substr(1, line.size() - 1));
					continue;
				}

				ParaPtr para = std::make_shared<Parameter>();
				para->initValueFromDevice(line, m_Categories[m_Categories.size()-1]);

				if (para->isValid() && !m_Categories.empty())
				{
					std::string cate = m_Categories[m_Categories.size() - 1];
					if (0 == m_Params.count(cate))
						m_Params.insert({ cate, std::vector<ParaPtr>{ para } });
					else if (1 == m_Params.count(cate))
						m_Params[cate].push_back(para);
					else
						continue;
				}
			}
				
		}
	}

	ifile.close();

	// init m_AllParams
	for (auto subVec : m_Params)
	{
		for (auto sub : subVec.second)
			m_AllParams.insert({sub->getCsvName(), sub});
	}

	return true;
}

bool 
ParameterVector::getParameter(	const GenApi::CNodeMapRef & nodeMap, 
								const std::string & ParamterName, 
								std::string & value)
{
	if (m_AllParams.count(ParamterName) == 1)
	{
		return m_AllParams[ParamterName]->getValueFromDevice(nodeMap, value);
	}
	
	return false;
}

bool ParameterVector::getConstraintParameter(const GenApi::CNodeMapRef	& nodeMap,
	                                         const std::string			& ParamterName,
	                                         std::string                 & minV,
	                                         std::string                 & maxV,
	                                         std::string                 & incV)
{
	if (m_AllParams.count(ParamterName) == 1)
	{
		return m_AllParams[ParamterName]->getValueConstraintFromDevice(nodeMap, minV,maxV,incV);
	}

	return false;
}


std::string 
ParameterVector::getIP(		const GenApi::CNodeMapRef	&	nodeMap,
							const GenTL::IF_HANDLE		&	interfaceHandle,
							const GenTLApi				*	tl,
							const int64_t				&	id,
							const bool						isPersistent)
{		
	bool8_t changed = false;
	CC(tl, tl->IFUpdateDeviceList(interfaceHandle, &changed, 500));
	uint32_t current(0);
	if (isPersistent)
	{
		GenApi::CIntegerPtr gevDevice= nodeMap._GetNode("GevPersistentIPAddress");
		current = static_cast<uint32_t>(gevDevice->GetValue());
	}
	else
	{
		GenApi::CIntegerPtr deviceSelector = nodeMap._GetNode("DeviceSelector");
		deviceSelector->SetValue(id); 
		GenApi::CIntegerPtr gevDevice = nodeMap._GetNode("GevDeviceIPAddress"); 
		current = static_cast<uint32_t>(gevDevice->GetValue());
	}
	return SiStringFromIp4(current);
}

std::string 
ParameterVector::getSubnet(	const GenApi::CNodeMapRef	& nodeMap,
							const GenTL::IF_HANDLE		& interfaceHandle,
							const GenTLApi				* tl,
							const int64_t				& id,
							const bool					  isPersistent)
{
	bool8_t changed = false;
	CC(tl, tl->IFUpdateDeviceList(interfaceHandle, &changed, 500));
	uint32_t current(0);

	if (isPersistent)
	{
		GenApi::CIntegerPtr gevDevice = nodeMap._GetNode("GevPersistentSubnetMask");
		current = static_cast<uint32_t>(gevDevice->GetValue());
	}
	else
	{
		GenApi::CIntegerPtr deviceSelector = nodeMap._GetNode("DeviceSelector");
		deviceSelector->SetValue(id);
		GenApi::CIntegerPtr gevDevice = nodeMap._GetNode("GevDeviceSubnetMask");
		current = static_cast<uint32_t>(gevDevice->GetValue());
	}
	return SiStringFromIp4(current);
}

bool 
ParameterVector::setParameter(
							const GenApi::CNodeMapRef	& nodeMap, 
							const std::string			& ParamterName, 
							const std::string			& value)
{
	if (m_AllParams.count(ParamterName) == 1)
	{
		try
		{
			return m_AllParams[ParamterName]->setValueToDevice(nodeMap, value);
		}
		catch (...)
		{
			std::cout << "set " << ParamterName << " as " << value << " failed!" << std::endl;
			return false;
		}
	}
	return false;
}

bool 
ParameterVector::setIP(		const GenApi::CNodeMapRef	& nodeMap,
							const GenTL::IF_HANDLE		& interfaceHandle,
							const GenTLApi				* tl,
							const int64_t				& id,
							const std::string			& _IP,
							const bool					isPersistent)
{
	if (isPersistent)
	{
		GenApi::CIntegerPtr gev = nodeMap._GetNode("GevPersistentIPAddress");
		gev->SetValue(SiIp4FromString(_IP));
		auto m_IP = getIP(nodeMap, interfaceHandle, tl, id, true);
		return m_IP == _IP;
	}
	else
	{
		return _forceIP(nodeMap, interfaceHandle, tl, id, _IP, getSubnet(nodeMap, interfaceHandle, tl, id, false));
	}
}

bool 
ParameterVector::setSubnet(	const GenApi::CNodeMapRef	& nodeMap,
							const GenTL::IF_HANDLE		& interfaceHandle,
							const GenTLApi				* tl,
							const int64_t				& id,
							const std::string			& subNet,
							const bool					isPersistent)
{
	if (isPersistent)
	{
		GenApi::CIntegerPtr gev = nodeMap._GetNode("GevPersistentSubnetMask");
		gev->SetValue(SiIp4FromString(subNet));
		auto m_SubNet = getSubnet(nodeMap, interfaceHandle, tl, id, true);
		return m_SubNet == subNet;
	}
	else
	{
		return _forceIP(nodeMap, interfaceHandle, tl, id, getIP(nodeMap, interfaceHandle, tl, id, false), subNet);
	}
}

bool 
ParameterVector::_forceIP(	const GenApi::CNodeMapRef	& nodeMap,
							const GenTL::IF_HANDLE		& interfaceHandle,
							const GenTLApi				* tl,
							const int64_t				& id,
							const std::string			& IP,
							const std::string			& subNet)
{
	GenApi::CIntegerPtr deviceSelector = nodeMap._GetNode("DeviceSelector");
	deviceSelector->SetValue(static_cast<int32_t>(id));

	GenApi::CIntegerPtr gevIpAddress = nodeMap._GetNode("GevDeviceForceIPAddress");
	gevIpAddress->SetValue(SiIp4FromString(IP));


	GenApi::CIntegerPtr gevSubnetMask = nodeMap._GetNode("GevDeviceForceSubnetMask");
	gevSubnetMask->SetValue(SiIp4FromString(subNet));

	GenApi::CCommandPtr gevIp = nodeMap._GetNode("GevDeviceForceIP");
	gevIp->Execute();
	Sleep(2000);

	auto m_IP = getIP(nodeMap, interfaceHandle, tl, id, false);
	auto m_SubNet = getSubnet(nodeMap, interfaceHandle, tl, id, false);

	return m_IP == IP && m_SubNet == subNet;
}

bool 
ParameterVector::_persistentIP(
							const GenApi::CNodeMapRef	& nodeMap, 
							const GenTL::IF_HANDLE		& interfaceHandle,
							const GenTLApi				* tl,
							const int64_t				& id,
							const std::string			& IP, 
							const std::string			& subNet)
{
	GenApi::CIntegerPtr gevIpAddress = nodeMap._GetNode("GevPersistentIPAddress");
	gevIpAddress->SetValue(SiIp4FromString(IP));

	GenApi::CIntegerPtr gevForceSubnetMask = nodeMap._GetNode("GevPersistentSubnetMask");
	gevForceSubnetMask->SetValue(SiIp4FromString(subNet));

	auto m_IP = getIP(nodeMap, interfaceHandle, tl, id, true);
	auto m_SubNet = getSubnet(nodeMap, interfaceHandle, tl, id, true);

	return m_IP == IP && m_SubNet == subNet;
}


}