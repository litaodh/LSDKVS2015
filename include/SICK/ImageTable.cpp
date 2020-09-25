/**	@file	ImageTable.cpp
*
*	@brief	Class ImgT Head File.
*
*	@attention
*	All supported image date formats are listed in DataNames.
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/


#include "ImageTable.h"

namespace SickCam 
{

const std::map<DN, cStr>
	ImgT::DN_StrName = {
		{ DN::INV		, "INVALID" },
		{ DN::SEN		, "SENSOR" },
		{ DN::RAN		, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1"			},
		{ DN::REF		, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1"		},
		{ DN::SCA		, "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1"			},
		{ DN::RAN_CAL	, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL"		},
		{ DN::REF_CAL	, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL"	},
		{ DN::REF_CAL_F	, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL_F"},
		{ DN::SCA_CAL	, "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL"		},
		{ DN::RAN2		, "ComponentSelector_Range_RegionSelector_Scan3dExtraction2"			},
		{ DN::REF2		, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2"		},
		{ DN::SCA2		, "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2"			},
		{ DN::RAN2_CAL	, "ComponentSelector_Range_RegionSelector_Scan3dExtraction2_CAL"		},
		{ DN::REF2_CAL	, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2_CAL"	},
		{ DN::SCA2_CAL	, "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL"		}
};

const std::map<cStr, DN>
	ImgT::StrName_DN = {
		{ "INVALID"																, DN::INV		},
		{ "SENSOR"																, DN::SEN		},
		{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction1"			, DN::RAN		},
		{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1"		, DN::REF		},
		{ "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1"			, DN::SCA		},
		{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL"		, DN::RAN_CAL	},
		{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL"	, DN::REF_CAL	},
		{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL_F", DN::REF_CAL_F },
		{ "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL"		, DN::SCA_CAL	},
		{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction2"			, DN::RAN2		},
		{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2"		, DN::REF2		},
		{ "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2"			, DN::SCA2		},
		{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction2_CAL"		, DN::RAN2_CAL	},
		{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2_CAL"	, DN::REF2_CAL	},
		{ "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL"		, DN::SCA2_CAL	}
};

cStr 
ImgT::DN2Str(const DN & _dn)
{
	if (DN_StrName.count(_dn) == 1)
		return DN_StrName.at(_dn);
	return cStr();
}

DN 
ImgT::Str2DN(cStr & _dn)
{
	if (StrName_DN.count(_dn) == 1)
		return StrName_DN.at(_dn);
	return DN::INV;
}

cStr 
ImgT::PtrType(const DN & _dn)
{
	switch (_dn)
	{
	case SickCam::INV:			return "";
	case SickCam::SEN:			return "UINT8_T	";
	case SickCam::RAN:			return "UINT16_T";
	case SickCam::REF:			return "UINT8_T	";
	case SickCam::SCA:			return "UINT8_T	";
	case SickCam::RAN_CAL:		return "FLOAT	";
	case SickCam::REF_CAL:		return "UINT8_T	";
	case SickCam::REF_CAL_F:	return "FLOAT	";
	case SickCam::SCA_CAL:		return "UINT8_T	";
	case SickCam::RAN2:			return "UINT16_T";
	case SickCam::REF2:			return "UINT8_T	";
	case SickCam::SCA2:			return "UINT8_T	";
	case SickCam::RAN2_CAL:		return "FLOAT	";
	case SickCam::REF2_CAL:		return "UINT8_T	";
	case SickCam::SCA2_CAL:		return "UINT8_T	";
	}
	return "";
}

//////////////////////////////// ImgT function definitions ///////////////////////////////////////

ImgT::ImgT()
	: m_Data(ImgTData())
{
	OutputDebugString("ImgT()\n");
	this->RangX = nullptr;
	this->RangXRaw = nullptr;
}

ImgT::ImgT(const ImgT & img)
	: m_Data(ImgTData())
{
	OutputDebugString("__doapl()\n");
	__doapl(this, img);
}

ImgT &
ImgT::operator=(const ImgT & img)
{
	if (*this == img)
		return *this;
	OutputDebugString("__doapl()====\n");
	return __doapl(this, img);
}

bool
ImgT::operator==(const ImgT & v)
{
	bool isSame(this->m_info == v.m_info);
	if (!isSame)
		return false;

	for (auto sub : this->m_Data)
	{
		if (v.m_Data.count(sub.first) == 0)
			return false;
		
		isSame &= _compareBuffer(v, sub.first);
	}
	return  isSame;
}

ImgT::~ImgT()
{
	OutputDebugString("~ImgT()\n");
	bool isOK= _clearBuffer(*this);	
}

void*
ImgT::getData(const DN &  _dn) const
{
	if (m_Data.count(_dn) == 1)
		return m_Data.at(_dn);
	return nullptr;
}

Vec<DN>
ImgT::getAvalibleDataNames() const
{
	Vec<DN> vec;
	for (auto sub : getData())		vec.push_back(sub.first);
	return vec;
}

const size_t & 
ImgT::get_cols(const DN & _dn) const
{
	switch (_dn)
	{
	case SickCam::SEN:			return getS_cols();	
	case SickCam::RAN:			return getR_cols();	
	case SickCam::REF:			return getR_cols();	
	case SickCam::SCA:			return getR_cols();	
	case SickCam::RAN_CAL:		return getC_cols();	
	case SickCam::REF_CAL:		return getC_cols();	
	case SickCam::REF_CAL_F:	return getC_cols();	
	case SickCam::SCA_CAL:		return getC_cols();	
	case SickCam::RAN2:			return getR_cols();	
	case SickCam::REF2:			return getR_cols();	
	case SickCam::SCA2:			return getR_cols();	
	case SickCam::RAN2_CAL:		return getC_cols();	
	case SickCam::REF2_CAL:		return getC_cols();	
	case SickCam::SCA2_CAL:		return getC_cols();	
	}
	return getR_cols();
}

const size_t & 
ImgT::get_rows(const DN & _dn) const
{
	switch (_dn)
	{
	case SickCam::SEN:			return getS_rows();
	case SickCam::RAN:			return getR_rows();
	case SickCam::REF:			return getR_rows();
	case SickCam::SCA:			return getR_rows();
	case SickCam::RAN_CAL:		return getC_rows();
	case SickCam::REF_CAL:		return getC_rows();
	case SickCam::REF_CAL_F:	return getC_rows();
	case SickCam::SCA_CAL:		return getC_rows();
	case SickCam::RAN2:			return getR_rows();
	case SickCam::REF2:			return getR_rows();
	case SickCam::SCA2:			return getR_rows();
	case SickCam::RAN2_CAL:		return getC_rows();
	case SickCam::REF2_CAL:		return getC_rows();
	case SickCam::SCA2_CAL:		return getC_rows();
	}
	return getR_rows();
}

bool
ImgT::insertDataCopy(	const DN		& _name,
						const uint8_t	* _pointer,
						const uint64_t	& _id)
{
	if (_pointer == nullptr)	return false;

	size_t byteSize(0), pixelSizes(0);
	switch (_name)
	{
	case SickCam::INV:			return false;
	case SickCam::SEN:			pixelSizes = getS_cols() * getS_rows(); byteSize = pixelSizes * sizeof(uint8_t);	break;
	case SickCam::RAN:			pixelSizes = getR_cols() * getR_rows(); byteSize = pixelSizes * sizeof(uint16_t);	break;
	case SickCam::REF:			pixelSizes = getR_cols() * getR_rows(); byteSize = pixelSizes * sizeof(uint8_t);	break;
	case SickCam::SCA:			pixelSizes = getR_cols() * getR_rows(); byteSize = pixelSizes * sizeof(uint8_t);	break;
	case SickCam::RAN_CAL:		pixelSizes = getC_cols() * getC_rows(); byteSize = pixelSizes * sizeof(float);		break;
	case SickCam::REF_CAL:		pixelSizes = getC_cols() * getC_rows(); byteSize = pixelSizes * sizeof(uint8_t);	break;
	case SickCam::REF_CAL_F:	pixelSizes = getC_cols() * getC_rows(); byteSize = pixelSizes * sizeof(float);		break;
	case SickCam::SCA_CAL:		pixelSizes = getC_cols() * getC_rows(); byteSize = pixelSizes * sizeof(uint8_t);	break;
	case SickCam::RAN2:			pixelSizes = getR_cols() * getR_rows(); byteSize = pixelSizes * sizeof(uint16_t);	break;
	case SickCam::REF2:			pixelSizes = getR_cols() * getR_rows(); byteSize = pixelSizes * sizeof(uint8_t);	break;
	case SickCam::SCA2:			pixelSizes = getR_cols() * getR_rows(); byteSize = pixelSizes * sizeof(uint8_t);	break;
	case SickCam::RAN2_CAL:		pixelSizes = getC_cols() * getC_rows(); byteSize = pixelSizes * sizeof(float);		break;
	case SickCam::REF2_CAL:		pixelSizes = getC_cols() * getC_rows(); byteSize = pixelSizes * sizeof(uint8_t);	break;
	case SickCam::SCA2_CAL:		pixelSizes = getC_cols() * getC_rows(); byteSize = pixelSizes * sizeof(uint8_t);	break;
	}

	if (m_Data.count(_name) > 1)	return false;

	if (m_Data.count(_name) == 1)
	{
		delete[](uint8_t*)m_Data.at(_name);
		m_Data.erase(_name);
	}

	if (m_Data.count(_name) == 0)
	{
		uint8_t* pd = nullptr;
		pd = new uint8_t[byteSize];
		std::memcpy(pd, _pointer, byteSize);
		m_Data.insert({ _name, pd });
		m_info.m_id = _id;
		
		//std::cout << "ImgT - " << get_ID() << " - new - " << _name << std::endl; // todo: delete it later ------
	}

	return true;
}

bool
ImgT::setSensorInfo(const size_t  & _c,
					const size_t  & _r,
					const int64_t & _ox,
					const int64_t & _oy)
{
	m_info.m_SI.cols = _c; m_info.m_SI.rows = _r;
	m_info.m_SI.senOffsetX = _ox; m_info.m_SI.senOffsetY = _oy;
	return true;
}

bool
ImgT::setRangeInfo(	const size_t  & _c,
					const size_t  & _r,
					const int64_t & _aox,
					const int64_t & _aoy,
					const int64_t & _ah,
					const int64_t & _aw,
					const RA      & _rAxis)
{
	m_info.m_RI.cols = _c; m_info.m_RI.rows = _r;
	m_info.m_RI.aoiOffsetX = _aox;
	m_info.m_RI.aoiOffsetY = _aoy;
	m_info.m_RI.aoiHeight = _ah;
	m_info.m_RI.aoiWidth = _aw;
	m_info.m_RI.m_RangeAxis = _rAxis;
	return true;
}

bool
ImgT::setCaliInfo(	const size_t & _c,
					const size_t & _r,
					const double & _offsetX,
					const double & _offsetY,
					const double & _scaleX,
					const double & _scaleY,
					const double & _lower_bound_x,
					const double & _upper_bound_x,
					const double & _lower_bound_r,
					const double & _upper_bound_r)
{
	m_info.m_CI.cols = _c; m_info.m_CI.rows = _r;
	m_info.m_CI.offsetX = _offsetX;
	m_info.m_CI.offsetY = _offsetY;
	m_info.m_CI.scaleX = _scaleX;
	m_info.m_CI.scaleY = _scaleY;
	m_info.m_CI.lower_bound_x = _lower_bound_x;
	m_info.m_CI.upper_bound_x = _upper_bound_x;
	m_info.m_CI.lower_bound_r = _lower_bound_r;
	m_info.m_CI.upper_bound_r = _upper_bound_r;
	return true;
}

bool
ImgT::setChunkInfo(const ChunkData & _Chunk)
{
	if (_Chunk.empty()) return false;

	m_info.m_ChunkData = _Chunk;
	return true;
}

void
ImgT::printInfo() const
{
	std::cout << collectInfo().c_str();
}

Str
ImgT::collectInfo() const
{
	std::stringstream ss;
	ss << "\r\n\r\n";
	ss << "Image_isEmpty____: " << (isEmpty() ? "1" : "0") << "\r\n";
	ss << "Image_Number_____: " << m_Data.size() << "\r\n";
	for (auto sub : m_Data)
	{
		ss << "*_____Image_Name_: " << DN2Str(sub.first) << "\r\n";
	}
	ss << "Sensor_Image_____: " << (has(DN::SEN) ? "1" : "0") << "\r\n";
	ss << "Chunk_Data_______: " << (has_ChunkInfo() ? "1" : "0") << "\r\n";
	ss << "Size_Sen(c,_r)___: " << getS_cols() << ", " << getS_rows() << "\r\n";
	ss << "Size_Ran(c,_r)___: " << getR_cols() << ", " << getR_rows() << "\r\n";
	ss << "Size_Cal(c,_r)___: " << getC_cols() << ", " << getC_rows() << "\r\n";
	ss << "offsetX__________: " << getC_offsetX() << "\r\n";
	ss << "offsetY__________: " << getC_offsetY() << "\r\n";
	ss << "scaleX___________: " << getC_scaleX() << "\r\n";
	ss << "scaleY___________: " << getC_scaleY() << "\r\n";
	ss << "lower_bound_x____: " << getC_lower_bound_x() << "\r\n";
	ss << "upper_bound_x____: " << getC_upper_bound_x() << "\r\n";
	ss << "lower_bound_r____: " << getC_lower_bound_r() << "\r\n";
	ss << "upper_bound_r____: " << getC_upper_bound_r() << "\r\n";
	if (has_ChunkInfo())
	{
		// only print first line.
		auto metadata = m_info.m_ChunkData[0];
		ss << "Timestamp_________: " << metadata.timestamp << "\r\n";
		ss << "EncoderValue______: " << metadata.encoderValue << "\r\n";
		ss << "EncoderA__________: " << metadata.encoderA << "\r\n";
		ss << "EncoderB__________: " << metadata.encoderB << "\r\n";
		ss << "FrameTriggerActive: " << metadata.frameTriggerActive << "\r\n";
		ss << "LineTriggerActive_: " << metadata.lineTriggerActive << "\r\n";
		ss << "OvertriggerCount__: " << static_cast<int>(metadata.overtriggerCount) << "\r\n";
	}
	return ss.str();
}

bool
ImgT::loadFromIconFile(cStr path)
{
	// Image size
	int col(0), row(0);
	bool is_calibrated(false), has_reflectance(false), is_sensor(false);
	size_t RanDSizeBytes(0), RefDSizeBytes(0), SenDSizeBytes(0);

	/// range params
	int64_t	aox(0), aoy(0), ah(0), aw(0), rAxis(1);
	int64_t fov___x0(0),
		fov___x1(0),
		fov___x2(0),
		fov___x3(0),
		fov___z0(0),
		fov___z1(0),
		fov___z2(0),
		fov___z3(0),
		origin_x(0),
		scale__x(0);
	double	origin_z(0.0),
		scale__z(0.0);

	/// calibration params
	double
		ox(0.0), oy(0.0), sx(1.0), sy(1.0),
		lx(0.0), ux(0.0), lr(0.0), ur(0.0);

	/// sensor params
	int64_t
		size___x(0),
		size___y(0),
		offset_x(0),
		offset_y(0);

	/// check path format 
	Str path_name_only("");
	if (path.size() < 4)
		path_name_only = path;
	else
		path_name_only = path.substr(path.size() - 3, 3); // suffix

	if (path_name_only == "xml" || path_name_only == "XML" || path_name_only == "dat" || path_name_only == "DAT")
		path_name_only = path.substr(0, path.size() - 4);
	else
		path_name_only = path;


	// Read xml
	{
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err = doc.LoadFile((path_name_only + ".xml").c_str());
		if (err != tinyxml2::XML_SUCCESS) return FALSE;

		tinyxml2::XMLElement* root = doc.FirstChildElement("icon_data_format");
		if (root == nullptr) return FALSE;

		tinyxml2::XMLElement* parameter1 = root->FirstChildElement();
		tinyxml2::XMLElement* parameter2 = parameter1->NextSiblingElement();
		tinyxml2::XMLElement* parameter3 = parameter2->NextSiblingElement();
		tinyxml2::XMLElement* component = parameter3->NextSiblingElement();
		tinyxml2::XMLElement* parameterC1 = component->FirstChildElement();
		tinyxml2::XMLElement* parameterC2 = parameterC1->NextSiblingElement();

		tinyxml2::XMLElement* XXXXrangetraits = parameterC2->NextSiblingElement();
		std::string name(XXXXrangetraits->Value());
		if (name == "worldrangetraits") // calibrated
		{
			is_calibrated = true;

			tinyxml2::XMLElement* paramXX = XXXXrangetraits->FirstChildElement();

			paramXX->QueryDoubleText(&lx);		paramXX = paramXX->NextSiblingElement();
			paramXX->QueryDoubleText(&lr);		paramXX = paramXX->NextSiblingElement();
			paramXX->QueryDoubleText(&ux);		paramXX = paramXX->NextSiblingElement();
			paramXX->QueryDoubleText(&ur);
		}
		else if (name == "sensorrangetraits")
		{
			is_calibrated = false;

			tinyxml2::XMLElement* paramXX = XXXXrangetraits->FirstChildElement();

			paramXX->QueryInt64Text(&fov___x0); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___x1); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___x2); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___x3); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___z0); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___z1); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___z2); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___z3); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&origin_x); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&scale__x); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryDoubleText(&origin_z); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryDoubleText(&scale__z); paramXX = paramXX->NextSiblingElement();

			rAxis = scale__z > 0 ? 1 : 0;
		}
		else if (name == "imagetraits")
		{
			is_sensor = true;

			tinyxml2::XMLElement* paramXX = XXXXrangetraits->FirstChildElement();

			paramXX->QueryInt64Text(&size___x); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&size___y); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&offset_x); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&offset_y); paramXX = paramXX->NextSiblingElement();
		}
		else
			return false;

		tinyxml2::XMLElement* subcomponent1 = XXXXrangetraits->NextSiblingElement();
		if (!is_sensor)
			RanDSizeBytes = is_calibrated ? sizeof(float) : sizeof(uint16_t);
		else
			SenDSizeBytes = sizeof(uint8_t);
		tinyxml2::XMLElement* size = subcomponent1->FirstChildElement();
		tinyxml2::XMLElement* width = size->NextSiblingElement();
		width->QueryIntText(&col);

		tinyxml2::XMLElement* subcomponent2 = subcomponent1->NextSiblingElement();

		if (subcomponent2)
		{
			has_reflectance = true;
			RefDSizeBytes = std::string(subcomponent2->Attribute("valuetype")) == "FLOAT" ? sizeof(float) : sizeof(uint8_t);
		}
	}

	if (col == 0) return false;

	// Read dat
	std::ifstream inFile;
	inFile.open(path_name_only + ".dat", std::ios_base::in | std::ios_base::binary);
	if (inFile.is_open())
	{
		// binary file size info
		inFile.seekg(0, inFile.end);
		int nLength = inFile.tellg();
		row = nLength / col / (RanDSizeBytes + RefDSizeBytes + SenDSizeBytes);

		// set ImgT data
		uint8_t *pBuffer = new uint8_t[nLength];
		inFile.seekg(0, inFile.beg);
		inFile.read((char *)&pBuffer[0], nLength);

		if (is_sensor)
		{
			setSensorInfo(col, row, offset_x, offset_y);
			insertDataCopy(DN::SEN, pBuffer, 0);
		}
		else
		{
			// set ImgT info
			if (is_calibrated)
			{
				ox = lx;
				oy = 0.0;
				sx = (ux-lx)/col;
				sy = 1.0;
				setCaliInfo(col, row, ox, oy, sx, sy, lx, ux, lr, ur);
			}
			else
			{
				aox = fov___x0;
				aw = fov___x1 - aox;

				ah = (fov___z2 + 1) / 16;	/// 16 bits data with sub-pixel accuracy -- see SaveBuffer.cpp
				aoy = (rAxis == 1 ? origin_z : origin_z + ah); ///  -- see SaveBuffer.cpp

				setRangeInfo(col, row, aox, aoy, ah, aw, (rAxis == 1 ? RA::STANDARD : RA::REVERSED));
			}

			// insert Data
			insertDataCopy(is_calibrated ? DN::RAN_CAL : DN::RAN, pBuffer, 0);
			if (has_reflectance)
			{
				if (RefDSizeBytes == sizeof(float))
				{
					uint8_t* pRef = new uint8_t[row*col];
					uint8_t* pByte = new uint8_t[row*col * sizeof(float)];
					memcpy(pByte, (uint8_t*)(pBuffer + row*col*RanDSizeBytes), row*col * sizeof(float));
					float* pFloat = (float *)pByte;
					for (size_t i = 0; i < row*col; ++i)
						pRef[i] = static_cast<uint8_t>(pFloat[i]);

					insertDataCopy(is_calibrated ? DN::REF_CAL : DN::REF, pRef, 0);

					delete[] pByte;
					delete[] pRef;
				}
				else
					insertDataCopy(is_calibrated ? DN::REF_CAL : DN::REF, (uint8_t*)(pBuffer + row*col*RanDSizeBytes), 0);

			}
		}

		delete[] pBuffer;
		return true;

	}
	return false;
}


bool
ImgT::SaveToIconFile(cStr path, DN whichOne) const
{
	if (path.empty()) return false;

	/// check path format 
	Str path_name_only("");
	if (path.size() < 4)
		path_name_only = path;
	else
		path_name_only = path.substr(path.size() - 3, 3); // suffix

	if (path_name_only == "xml" || path_name_only == "XML" || path_name_only == "dat" || path_name_only == "DAT")
		path_name_only = path.substr(0, path.size() - 4);
	else
		path_name_only = path;

	bool save_all(whichOne == INV);
	bool isSaved(false);
	if (has(DN::SEN) && (save_all || whichOne == SEN))
	{
		GenIRanger::saveBuffer8(
			(uint8_t*)getData(DN::SEN),
			getS_cols(),
			getS_rows(),
			getS_OffsetX(),
			getS_OffsetY(),
			save_all ? path_name_only + "_SEN" : path_name_only,
			"");
		isSaved = true;
	}

	if (has(DN::RAN) && (save_all || whichOne == RAN))
	{
		GenIRanger::saveMultiPartBuffer(
			(uint8_t*)getData(DN::RAN), // RAN | RAN2
			(uint8_t*)getData(DN::REF), // REF | REF2
			(uint8_t*)getData(DN::SCA), // SCA | SCA2
			getR_cols(),
			getR_rows(),
			getR_aoiHeight(),
			getR_aoiOffsetX(),
			getR_aoiOffsetY(),
			getR_RangeAxis(),
			path,
			"");
		isSaved = true;
	}
	if (has(DN::RAN_CAL) && (save_all || whichOne == RAN_CAL))
	{
		GenIRanger::saveBufferFloat(
		{
			(float*)getData(DN::RAN_CAL),	// RAN_CAL | RAN2_CAL
			(uint8_t*)getData(DN::REF_CAL),	// REF_CAL | REF2_CAL
			(uint8_t*)getData(DN::SCA_CAL),	// SCA_CAL | SCA2_CAL
		},
		getC_cols(),
		getC_rows(),
		getR_aoiHeight(),
		getR_aoiOffsetX(),
		getR_aoiOffsetY(),
		getR_RangeAxis(),
		getC_lower_bound_x(),
		getC_lower_bound_r(),
		getC_upper_bound_x(),
		getC_upper_bound_r(),
		save_all ? path_name_only + "_CAL" : path_name_only,
		"");
		isSaved = true;
	}

	if (has(DN::RAN2) && (save_all || whichOne == RAN2))
	{
		GenIRanger::saveMultiPartBuffer(
			(uint8_t*)getData(DN::RAN2), // RAN2
			(uint8_t*)getData(DN::REF2), // REF2
			(uint8_t*)getData(DN::SCA2), // SCA2
			getR_cols(),
			getR_rows(),
			getR_aoiHeight(),
			getR_aoiOffsetX(),
			getR_aoiOffsetY(),
			getR_RangeAxis(),
			path + "_2",
			"");
		isSaved = true;
	}
	if (has(DN::RAN2_CAL) && (save_all || whichOne == RAN2_CAL))
	{
		GenIRanger::saveBufferFloat(
		{
			(float*)getData(DN::RAN2_CAL),		// RAN2_CAL
			(uint8_t*)getData(DN::REF2_CAL),	// REF2_CAL
			(uint8_t*)getData(DN::SCA2_CAL),	// SCA2_CAL
		},
		getC_cols(),
		getC_rows(),
		getR_aoiHeight(),
		getR_aoiOffsetX(),
		getR_aoiOffsetY(),
		getR_RangeAxis(),
		getC_lower_bound_x(),
		getC_lower_bound_r(),
		getC_upper_bound_x(),
		getC_upper_bound_r(),
		save_all ? path_name_only + "_CAL2" : path_name_only,
		"");
		isSaved = true;
	}
	return isSaved;
}

bool
ImgT::SaveCalibratedDataToPointCloud(cStr _path) const
{
	if (_path.empty() || !has(DN::RAN_CAL)) return false;

	std::string path(_path);
	if (path.substr(path.size() - 4, 3) == "txt")
		path += ".txt";

	std::ofstream ofile(path);
	if (ofile.is_open())
	{
		double
			xResolution(getC_scaleX()), yResolution(getC_scaleY()),
			xOffset(getC_offsetX()), yOffset(0);

		//bool hasRef = has(DN::REF_CAL);
		float * pd = (float*)getData(DN::RAN_CAL);

#pragma omp parallel for
		for (int j = 0; j < getC_rows(); ++j)
		{
			for (int i = 0; i < getC_cols(); ++i)
			{
				if (pd[i + j*getC_cols()] == -1e+06)	continue;
				if (isnan(pd[i + j*getC_cols()]))		continue;
#pragma omp critical
				{
				// make its coordinate value right-hand, so reverse y! 
				ofile << (i*xResolution + xOffset) << "," << -(j*yResolution + yOffset) << "," << (pd[i + j*getC_cols()]) << std::endl;
				}
			}
		}
		ofile.close();
	}

	return true;
}

ImgT &
ImgT::__doapl(ImgT * ths, const ImgT & img)
{
	if (!img.isEmpty())
	{
		// data
		if (_clearBuffer(*ths))
		{
			ths->m_info = img.m_info; // todo, check assign ---------------------
			for (auto sub : img.m_Data)
				ths->insertDataCopy(sub.first, (uint8_t*)sub.second, img.m_info.m_id);

			//todo:copy x
			if (img.has(DN::RAN_CAL) && img.RangX != nullptr)
			{
				size_t xSize = img.getC_cols()*img.getC_rows();
				this->RangX = new float[xSize];
				memcpy(this->RangX, img.RangX, xSize*sizeof(float));
			}
			else if (img.has(DN::RAN) && img.RangXRaw != nullptr)
			{
				size_t xSize = img.getR_cols()*img.getR_rows();
				this->RangXRaw = new uint16_t[xSize];
				memcpy(this->RangXRaw, img.RangXRaw, xSize*sizeof(uint16_t));
			}
		}
	}
	return *ths;
}

bool 
ImgT::_clearBuffer(ImgT & img)
{
	OutputDebugString("_clearBuffer()\n");
	for (auto sub : img.m_Data)
	{
		if (sub.second != nullptr)
		{
			delete[](uint8_t*)sub.second;
			sub.second = nullptr;
			//std::cout << "ImgT - " << get_ID() << " - delete [] - " << sub.first << std::endl; // todo: delete it later ------
		}
	}
	img.m_Data.clear();

	//todo:clear X
	if (this->RangX != nullptr)
	{
		delete[] this->RangX;
		this->RangX = nullptr;
	}

	if (this->RangXRaw != nullptr)
	{
		delete[] this->RangXRaw;
		this->RangXRaw = nullptr;
	}


	return img.m_Data.empty();
}

bool 
ImgT::_compareBuffer(const ImgT & img, const DN dn)
{
	bool isSame(false);
	if (has(dn))
	{
		size_t tSizeThis = get_cols(dn) * get_rows(dn);
		size_t tSize = img.get_cols(dn) * img.get_rows(dn);
		isSame = tSizeThis == tSize;
		if (!isSame)
			return false;

		uint8_t* pdThis = (uint8_t*)getData(dn);
		uint8_t* pd = (uint8_t*)img.getData(dn);

		for (size_t i=0; i<tSize; ++i)
			isSame &= (pdThis[i] == pd[i]);
	}

	return isSame;
}

}
