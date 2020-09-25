/**	@file	CustomerLog.h
*
*	@brief	Class of output log.
*
*	@attention
*	Log and printf control
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/


#pragma once

#include "Typedef.h"
#include <iostream>
#include <chrono>

namespace SickCam
{

class CustomerLog
{
public:
	CustomerLog(const std::string path, const bool enShow = true, const bool enWrite = true);
	~CustomerLog();

	//void beg();
	std::string time();

	// Overloading << 
	template<typename T>
	friend
		CustomerLog &
		operator << (CustomerLog & ths, const T & ss);


	/// this is the type of std::cout
	typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

	/// this is the function signature of std::endl
	typedef CoutType& (*StandardEndLine)(CoutType&);

	/// define an operator<< to take in std::endl
	CustomerLog &
		operator << (StandardEndLine manip);

protected:
	CustomerLog() = delete;
	void _open();
	void _close();

private:

	std::ofstream	mFile;
	std::string		mPath;
	bool mEnShow;
	bool mEnWrite;
};


/////////////////////////////////////////////////////////////


template<typename T>
inline CustomerLog &
operator << (CustomerLog & ths, const T & ss)
{
	if (ths.mEnShow)
	{
		std::cout << ss;
	}

	if (ths.mEnWrite)
	{
		ths._open();
		ths.mFile << ss;
		ths._close();
	}

	return ths;
}

template < >
inline CustomerLog &
operator << (CustomerLog & ths, const std::stringstream & ss)
{
	if (ths.mEnShow)
	{
		std::cout << ss.str().c_str();
	}

	if (ths.mEnWrite)
	{
		ths._open();
		ths.mFile << ss.str().c_str();
		ths._close();
	}

	return ths;
}


}