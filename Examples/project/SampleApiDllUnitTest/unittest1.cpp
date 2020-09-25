/**	@file	unittest1.cpp
*
*	@brief	Class Ranger3 Head File.
*
*	@attention
*	Hide from user, developer only!!
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/

//#include "stdafx.h"
#include "CppUnitTest.h"

#include <Ranger3.h>
#include <ImageTable.h>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SampleApiDllUnitTest
{	

TEST_CLASS(UT_ImagTable)
{
public:
	TEST_METHOD(t00_SensoInfo_assign)
	{
		SickCam::SensoInfo v1;
		v1.cols			= rand();
		v1.rows			= rand();
		v1.senOffsetX	= rand();
		v1.senOffsetY	= rand();

		SickCam::SensoInfo v2 = v1;

		Assert::AreEqual(v1.cols,		v2.cols		);
		Assert::AreEqual(v1.rows,		v2.rows		);
		Assert::IsTrue(v1.senOffsetX == v2.senOffsetX);
		Assert::IsTrue(v1.senOffsetY == v2.senOffsetY);

	}

	TEST_METHOD(t10_RangeInfo_assign)
	{
		SickCam::RangeInfo v1;
		v1.cols			= rand();
		v1.rows			= rand();
		v1.aoiHeight	= rand();
		v1.aoiWidth		= rand();
		v1.aoiOffsetX	= rand();
		v1.aoiOffsetY	= rand();
		v1.m_RangeAxis = RA::REVERSED;

		SickCam::RangeInfo v2 = v1;

		Assert::AreEqual(v1.cols		,v2.cols);
		Assert::AreEqual(v1.rows		,v2.rows);
		Assert::IsTrue(v1.aoiHeight		== v2.aoiHeight	);
		Assert::IsTrue(v1.aoiWidth		== v2.aoiWidth	);
		Assert::IsTrue(v1.aoiOffsetX	== v2.aoiOffsetX);
		Assert::IsTrue(v1.aoiOffsetY	== v2.aoiOffsetY);
		Assert::IsTrue(v1.m_RangeAxis == v2.m_RangeAxis);
	}

	TEST_METHOD(t20_CalibInfo_assign)
	{
		SickCam::CalibInfo v1;
		v1.cols = rand();
		v1.rows = rand();
		v1.offsetX			= (double)rand();
		v1.offsetY			= (double)rand();
		v1.scaleX			= (double)rand();
		v1.scaleY			= (double)rand();
		v1.lower_bound_x	= (double)rand();
		v1.upper_bound_x	= (double)rand();
		v1.lower_bound_r	= (double)rand();
		v1.upper_bound_r	= (double)rand();

		SickCam::CalibInfo v2 = v1;

		Assert::AreEqual(v1.cols, v2.cols);
		Assert::AreEqual(v1.rows, v2.rows);
		Assert::IsTrue(v1.offsetX		== v2.offsetX		);
		Assert::IsTrue(v1.offsetY		== v2.offsetY		);
		Assert::IsTrue(v1.scaleX		== v2.scaleX		);
		Assert::IsTrue(v1.scaleY		== v2.scaleY		);
		Assert::IsTrue(v1.lower_bound_x	== v2.lower_bound_x);
		Assert::IsTrue(v1.upper_bound_x	== v2.upper_bound_x);
		Assert::IsTrue(v1.lower_bound_r	== v2.lower_bound_r);
		Assert::IsTrue(v1.upper_bound_r	== v2.upper_bound_r);
	}

	TEST_METHOD(t30_ImgInfo_assign)
	{
		SickCam::ImgInfo v1;

		v1.m_id = rand();

		v1.m_SI.cols = rand();
		v1.m_SI.rows = rand();
		v1.m_SI.senOffsetX = rand();
		v1.m_SI.senOffsetY = rand();

		v1.m_RI.cols = rand();
		v1.m_RI.rows = rand();
		v1.m_RI.aoiHeight = rand();
		v1.m_RI.aoiWidth = rand();
		v1.m_RI.aoiOffsetX = rand();
		v1.m_RI.aoiOffsetY = rand();
		v1.m_RI.m_RangeAxis = RA::REVERSED;

		v1.m_CI.cols = rand();
		v1.m_CI.rows = rand();
		v1.m_CI.offsetX = (double)rand();
		v1.m_CI.offsetY = (double)rand();
		v1.m_CI.scaleX = (double)rand();
		v1.m_CI.scaleY = (double)rand();
		v1.m_CI.lower_bound_x = (double)rand();
		v1.m_CI.upper_bound_x = (double)rand();
		v1.m_CI.lower_bound_r = (double)rand();
		v1.m_CI.upper_bound_r = (double)rand();


		v1.m_ChunkData = std::vector<Ranger3LineMetadata>(5);
		for (auto s : v1.m_ChunkData)
		{
			s.encoderA = rand() > 15000;
			s.encoderB = rand() > 15000;
			s.encoderResetActive = rand() > 15000;
			s.encoderValue = (uint32_t)rand();
			s.frameTriggerActive = rand() > 15000;
			s.overtriggerCount = (uint8_t)rand();
			s.timestamp = (uint64_t)rand();
		}


		SickCam::ImgInfo v2 = v1;

		Assert::IsTrue(v1.m_id == v2.m_id);
		Assert::IsTrue(v1.m_SI == v2.m_SI);
		Assert::IsTrue(v1.m_RI == v2.m_RI);
		Assert::IsTrue(v1.m_CI == v2.m_CI);
		Assert::IsTrue(v1.m_ChunkData == v2.m_ChunkData);
	}

	TEST_METHOD(t35_ImgT_assign)
	{
		SickCam::ImgT img;
		Assert::IsTrue(img.loadFromIconFile("D:\\support\\test\\sen\\img"));

		SickCam::ImgT img2;
		Assert::IsTrue(img2.loadFromIconFile("D:\\support\\test\\sen\\img"));

		Assert::IsTrue(img == img2);
	}

	TEST_METHOD(t36_ImgT_assign)
	{
		SickCam::ImgT img;
		Assert::IsTrue(img.loadFromIconFile("D:\\support\\test\\sen\\img"));

		SickCam::ImgT img2;
		Assert::IsTrue(img2.loadFromIconFile("D:\\support\\test\\sen\\sensor"));

		Assert::IsFalse(img == img2);
	}

	TEST_METHOD(t40_loadFromIconFile_sen)
	{
		SickCam::ImgT img;
		Assert::IsTrue(img.loadFromIconFile("D:\\support\\test\\sen\\img"));

		Assert::IsTrue(img.has(SickCam::DN::SEN));
		Assert::IsTrue(!img.isEmpty());
		Logger::WriteMessage(img.collectInfo().c_str());
	}

	TEST_METHOD(t50_loadFromIconFile_ran_ref)
	{
		SickCam::ImgT img;
		Assert::IsTrue(img.loadFromIconFile("D:\\support\\test\\ran_ref\\img"));
		Assert::IsTrue(img.has(SickCam::DN::RAN));
		Assert::IsTrue(img.has(SickCam::DN::REF));
		Assert::IsTrue(!img.isEmpty());
		Logger::WriteMessage(img.collectInfo().c_str());
	}

	TEST_METHOD(t60_loadFromIconFile_ran_ref_onlyR)
	{
		SickCam::ImgT img;
		Assert::IsTrue(img.loadFromIconFile("D:\\support\\test\\ran_ref\\imgOnlyR"));
		Assert::IsTrue(img.has(SickCam::DN::RAN));
		Assert::IsTrue(!img.isEmpty());
		Logger::WriteMessage(img.collectInfo().c_str());
	}

	TEST_METHOD(t70_loadFromIconFile_ran_ref_cal)
	{
		SickCam::ImgT img;
		Assert::IsTrue(img.loadFromIconFile("D:\\support\\test\\ran_ref_cal\\img"));

		Assert::IsTrue(img.has(SickCam::DN::RAN_CAL));
		Assert::IsTrue(img.has(SickCam::DN::REF_CAL));
		Assert::IsTrue(!img.isEmpty());
		Logger::WriteMessage(img.collectInfo().c_str());
	}

	TEST_METHOD(t80_loadFromIconFile_ran_ref_cal_onlyR)
	{
		SickCam::ImgT img;
		Assert::IsTrue(img.loadFromIconFile("D:\\support\\test\\ran_ref_cal\\imgOnlyR"));

		Assert::IsTrue(img.has(SickCam::DN::RAN_CAL));
		Assert::IsTrue(!img.isEmpty());
		Logger::WriteMessage(img.collectInfo().c_str());
	}

	TEST_METHOD(t90_SaveToIconFile_sen)
	{
		SickCam::ImgT img;
		Assert::IsTrue(img.loadFromIconFile("D:\\support\\test\\sen\\img"));
		Assert::IsTrue(img.SaveToIconFile("D:\\support\\test\\sen\\img_new", SickCam::SEN));

		SickCam::ImgT img2;
		Assert::IsTrue(img2.loadFromIconFile("D:\\support\\test\\sen\\img_new"));

		Assert::IsTrue(img.has(SickCam::SEN));
		Assert::IsTrue(img2.has(SickCam::SEN));
		Assert::IsTrue(img == img2);

		Logger::WriteMessage(img.collectInfo().c_str());
		Logger::WriteMessage(img2.collectInfo().c_str());
	}

	TEST_METHOD(t100_SaveCalibratedDataToPointCloud)
	{
		SickCam::ImgT img;
		Assert::IsTrue(img.loadFromIconFile("D:\\support\\test\\ran_ref_cal\\img"));
		Logger::WriteMessage(img.collectInfo().c_str());
		img.setC_scaleY(0.04);
		Assert::IsTrue(img.SaveCalibratedDataToPointCloud("D:\\support\\test\\pcltxt\\img.txt"));
	}
};


}