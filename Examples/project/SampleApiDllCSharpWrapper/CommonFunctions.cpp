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

#include "CommonFunctions.h"

namespace SICK3D
{


// ----------------------------------------------------------------------------
// Common functions

std::wstring 
string2wstring(const std::string & str)
{
	std::wstring result;
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];

	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}  

std::string 
wstring2string(const std::wstring & wstr) 
{
	std::string result;
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);      
	char* buffer = new char[len + 1];      
	
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);      
	buffer[len] = '\0';      
	result.append(buffer);      
	delete[] buffer;      
	return result;  
} 

std::string 
wchart2string(const wchar_t* pwc)
{
	size_t wl = wcslen(pwc);
	std::wstring wStr(&pwc[0], wl);
	return wstring2string(wStr);
}

std::string
StringRef2string(const String ^ strRef)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(strRef);
	return wchart2string(wpStr);
}


}
