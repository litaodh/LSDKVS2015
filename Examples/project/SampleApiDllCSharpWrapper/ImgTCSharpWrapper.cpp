/**	@file	SampleApiDllCSharpWrapper.h
*
*	@brief	C# Wrapper Class of Ranger3.
*
*	@attention
*	This is the C# API wrapper of C++ Class Ranger3 and Ranger3Shared in ranger3.cpp|h.
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/

#include "stdafx.h"

#include "ImgTCSharpWrapper.h"

namespace SICK3D
{


// ----------------------------------------------------------------------------
// Wrapper for C# ImgT

	
CSImgT::CSImgT()
{
	mp_ImgT = new SickCam::ImgT();
}


CSImgT::~CSImgT()
{
	//if (mp_ImgT != nullptr)
	delete mp_ImgT;
}

CSImgT::CSImgT(const CSImgT ^ img)
{
	_deepCopy(img);
}

CSImgT^
CSImgT::operator=(const CSImgT^ img)
{
	if (*this == img)
		return this;

	_deepCopy(img);
	return this;
}

bool CSImgT::operator==(const CSImgT^ img)
{
	return *this->mp_ImgT == *img->mp_ImgT;
}

bool CSImgT::operator!=(const CSImgT^ img)
{
	return !(*this->mp_ImgT == *img->mp_ImgT);
}


String^ 
CSImgT::DN2Str(const DN _dn)
{
	auto str = mp_ImgT->DN2Str(static_cast<SickCam::DN>(_dn));
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

DN 
CSImgT::Str2DN(String^ _str)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(_str);
	auto str = wchart2string(wpStr);
	auto dn = mp_ImgT->Str2DN(str);
	return static_cast<DN>(dn);
}

array<uint8_t>^ 
CSImgT::getDataByte(const DN _dn)
{
	auto dn = static_cast<SickCam::DN>(_dn);
	uint8_t* data = (uint8_t*)mp_ImgT->getData(dn);
	auto cols = mp_ImgT->get_cols(dn);
	auto rows = mp_ImgT->get_rows(dn);
	auto dataType = mp_ImgT->PtrType(dn);

	array<uint8_t>^ res = gcnew array<uint8_t>(cols * rows);

	for (size_t i = 0; i < cols * rows; ++i)
	{
		res[i] = data[i];
	}
	return res;
}

array<uint16_t>^
CSImgT::getDataWord(const DN _dn)
{
	auto dn = static_cast<SickCam::DN>(_dn);
	uint16_t* data = (uint16_t*)mp_ImgT->getData(dn);
	auto cols = mp_ImgT->get_cols(dn);
	auto rows = mp_ImgT->get_rows(dn);
	auto dataType = mp_ImgT->PtrType(dn);

	array<uint16_t>^ res = gcnew array<uint16_t>(cols * rows);

	for (size_t i = 0; i < cols * rows; ++i)
	{
		res[i] = data[i];
	}
	return res;
}

array<float>^
CSImgT::getDataFloat(const DN _dn)
{
	auto dn = static_cast<SickCam::DN>(_dn);
	float* data = (float*)mp_ImgT->getData(dn);
	auto cols = mp_ImgT->get_cols(dn);
	auto rows = mp_ImgT->get_rows(dn);
	auto dataType = mp_ImgT->PtrType(dn);

	array<float>^ res = gcnew array<float>(cols * rows);

	for (size_t i = 0; i < cols * rows; ++i)
	{
		res[i] = data[i];
	}
	return res;
}

array<DN>^ 
CSImgT::getAvalibleDataNames()
{
	auto adn = mp_ImgT->getAvalibleDataNames();
	array<DN>^ res = gcnew array<DN>(adn.size());
	for (size_t i=0; i<adn.size(); ++i)
	{
		res[i] = static_cast<DN>(adn[i]);
	}
	return res;
}

bool 
CSImgT::insertDataByteClone(const DN _name, array<uint8_t>^ _pointer, const uint64_t _id)
{
	pin_ptr<uint8_t> p = &_pointer[0];
	return mp_ImgT->insertDataCopy(static_cast<SickCam::DN>(_name), p, _id);
}

bool 
CSImgT::insertDataWordClone(const DN _name, array<uint16_t>^ _pointer, const uint64_t _id)
{
	pin_ptr<uint16_t> p = &_pointer[0];
	return mp_ImgT->insertDataCopy(static_cast<SickCam::DN>(_name), (uint8_t*)p, _id);
}

bool 
CSImgT::insertDataFloatClone(const DN _name, array<float>^ _pointer, const uint64_t _id)
{
	pin_ptr<float> p = &_pointer[0];
	return mp_ImgT->insertDataCopy(static_cast<SickCam::DN>(_name), (uint8_t*)p, _id);
}

bool 
CSImgT::loadFromIconFile(const String ^ path)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto spath = wchart2string(wpStr);
	return mp_ImgT->loadFromIconFile(spath);
}

bool 
CSImgT::SaveToIconFile(const String ^ path, DN whichOne)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto spath = wchart2string(wpStr);
	return mp_ImgT->SaveToIconFile(spath, static_cast<SickCam::DN>(whichOne));
}

bool 
CSImgT::SaveCalibratedDataToPointCloud(const String ^ path)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto spath = wchart2string(wpStr);
	return mp_ImgT->SaveCalibratedDataToPointCloud(spath);
}

bool
CSImgT::setData(const SickCam::ImgT & p)
{
	*mp_ImgT = p;
	return true;
}

void 
CSImgT::_deepCopy(const CSImgT^ img)
{
	//if(!this->mp_ImgT->isEmpty())
	delete this->mp_ImgT;

	this->mp_ImgT = new SickCam::ImgT();
	*this->mp_ImgT = *img->mp_ImgT;
}




}
