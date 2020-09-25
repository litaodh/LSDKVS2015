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


#pragma once

#include <Ranger3.h>

#include <cliext\list>
#include <cliext\map>
#include <vcclr.h>

#include <string>
#include <sstream>

#include <windows.h>

using namespace System;

namespace SICK3D
{

std::wstring string2wstring(const std::string & str);
std::string wstring2string(const std::wstring & wstr);
std::string wchart2string(const wchar_t* pwc);
std::string StringRef2string(const String^ strRef);

}
