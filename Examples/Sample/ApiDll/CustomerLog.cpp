/**	@file	CustomerLog.cpp
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

#include "CustomerLog.h"


namespace SickCam 
{

CustomerLog::CustomerLog(const std::string path, const bool enShow, const bool enWrite)
	:mEnShow(enShow), mEnWrite(enWrite)
{
	if (mEnWrite)
	{
		mPath = path;
		//mFile.open(mPath, std::fstream::app);
	}
}

CustomerLog::~CustomerLog()
{
	/*if (mEnWrite)
		mFile.close();*/
}

std::string
CustomerLog::time()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	struct tm ltm = { 0 };
	localtime_s(&ltm, &tt);
	std::stringstream stm;
	stm << std::setfill('0');
	stm << std::setw(4) << (ltm.tm_year + 1900) << "-";
	stm << std::setw(2) << (ltm.tm_mon + 1) << "-";
	stm << std::setw(2) << (ltm.tm_mday) << " ";
	stm << std::setw(2) << (ltm.tm_hour) << ":";
	stm << std::setw(2) << (ltm.tm_min) << ":";
	stm << std::setw(2) << (ltm.tm_sec);

	return stm.str();
}

void
CustomerLog::_open()
{
	/*if (mEnWrite)
	{
		if (!mFile.is_open())
			mFile.open(mPath, std::fstream::app);
	}*/
}

void
CustomerLog::_close()
{
	/*if (mEnWrite)
	{
		if (mFile.is_open())
			mFile.close();
	}*/
}

CustomerLog &
CustomerLog::operator << (StandardEndLine manip)
{
	/*if (manip == static_cast<StandardEndLine>(endl))
	{
		if (mEnShow)
		{
			cout << time() << "\n\n\n-----------------------------\n" << endl;
		}

		if (mEnWrite)
		{
			_open();
			mFile << time() << "\n\n\n-----------------------------\n" << endl;
			_close();
		}
	}
	/// call the function, but we cannot return it's value
	manip(cout);*/
	return *this;
}




}

