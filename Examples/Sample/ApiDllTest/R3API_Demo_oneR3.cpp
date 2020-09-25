/**	@file	R3API_Demo_oneR3.cpp
*
*	@brief	Demo of showing how to call SICK 3D Camera APIs.
*
*	@attention
*	This Demo program is only used to show how to call SICK 3D Camera APIs.
*			SICK will not guarantee this file is correct!
*			This file is open to customers.
*			Any programs based on this demo, please keep it correct by yourself.
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/


#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "ImageGabber.h"
#include "InterfaceOpenCV.h"
#include <Ranger3.h>
#include <Ranger3Callback.h>
#include <iostream>

#include <opencv2/opencv.hpp>
#include "DatXmlWriter.h"



template<typename T> using Sptr = std::shared_ptr<T>;
template<typename T> using Uptr = std::unique_ptr<T>;

using STR = std::string;

typedef SickCam::Ranger3Shared		R3S;
typedef SickCam::Ranger3			R3;

typedef SickCam::CAM_STATUS			Ecode;
typedef SickCam::DataNames			DN;
typedef SickCam::ImgT				IMG;

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void SaveBuffer(
	const float* xBuffer,
	const float* zBuffer,
	const uint8_t* iBuffer,
	const int64_t bufferWidth,
	const int64_t bufferHeight,
	const int64_t lowerBoundX,
	const int64_t lowerBoundR,
	const int64_t upperBoundX,
	const int64_t upperBoundR,
	const std::string& filePath)
{
	std::string datFilename = filePath + ".dat";
	std::string xmlFilename = filePath + ".xml";

	std::ofstream dataStream(datFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	dataStream.exceptions(std::ios::failbit | std::ios::badbit);

	
	dataStream.write((const char*)xBuffer, bufferWidth*bufferHeight*sizeof(float));	
	dataStream.write((const char*)zBuffer, bufferWidth*bufferHeight*sizeof(float));
	
	bool isSaveInstensity = false;
	if (iBuffer != nullptr)
	{
		dataStream.write((const char*)iBuffer, bufferWidth*bufferHeight);
		isSaveInstensity = true;
	}

	dataStream.close();

	DatXmlWriter xml;
	xml.open();
	{
		xml.addParameter("size", std::to_string(isSaveInstensity ? bufferWidth * (sizeof(float) * 2 + 1) : bufferWidth*sizeof(float) * 2));
		xml.addParameter("version", "1");
		xml.addParameter("layout", "SUBCOMPONENT");
		xml.openComponent("", "Ranger3Range");
		{
			xml.addParameter("size", std::to_string(isSaveInstensity ? bufferWidth * (sizeof(float) * 2 + 1) : bufferWidth*sizeof(float) * 2));
			xml.addParameter("height", "1");
			xml.openWorldRangeTraits();
			{
				// From where on the sensor the data comes from can be calculated by
				// using the origin and scale factors below

				xml.addParameter("lower bound x", std::to_string(lowerBoundX));
				xml.addParameter("lower bound r", std::to_string(lowerBoundR));
				xml.addParameter("upper bound x", std::to_string(upperBoundX));
				xml.addParameter("upper bound r", std::to_string(upperBoundR));
				xml.addParameter("coordinate unit", "");
			}
			xml.closeWorldRangeTraits();
			xml.openSubComponent("FLOAT", "Range X");
			{
				xml.addParameter("size", std::to_string(bufferWidth * sizeof(float)));
				xml.addParameter("width", std::to_string(bufferWidth));
			}
			xml.closeSubComponent();
			xml.openSubComponent("FLOAT", "Range R");
			{
				xml.addParameter("size", std::to_string(bufferWidth * sizeof(float)));
				xml.addParameter("width", std::to_string(bufferWidth));
			}
			if (isSaveInstensity == true)
			{
				xml.openSubComponent("BYTE", "Intensity");
				{
					xml.addParameter("size", std::to_string(bufferWidth));
					xml.addParameter("width", std::to_string(bufferWidth));
				}
				xml.closeSubComponent();
			}
			xml.closeSubComponent();
		}
		xml.closeComponent();
	}
	xml.close();

	std::ofstream xmlStream(xmlFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	xmlStream.exceptions(std::ios::failbit | std::ios::badbit);
	xmlStream << xml.toString();
	xmlStream.close();

}

bool test_one_camera3_old(const DN & imgType=DN::SEN, const int & nImg=-1)
{
	// --- Init Ranger3 environment.
	std::shared_ptr<R3S> pR3S = std::make_shared<R3S>("");

	// --- Check
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevList();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
		{
			std::cout << "---- " << sub.first.c_str() << std::endl;
		}
	}
	else
	{
		printf("Error: scan camera error. \n");
		return 0;
	}

	// --- Parameters for one Ranger3.
	///std::string para_csv_path = "D:\\support\\zzz.csv";
	///std::string calib_xml_path = "D:\\support\\litao\\SN19040073_calibration.xml";
	std::string para_csv_path = "D:/CameraConfigFiles/SickConfig3.csv";
	std::string calib_xml_path = "D:/CameraConfigFiles/sick-2d-120_Calibration_calibration.xml";


	std::string para_csv_path2 = "D:/CameraConfigFiles/SickConfig4.csv";
	std::string calib_xml_path2 = "D:/CameraConfigFiles/sick-3d-20_Calibration_calibration.xml";

	//std::string para_csv_path3 = "D:/CameraConfigFiles/SickConfig3.csv";
	//std::string calib_xml_path3 = "D:/CameraConfigFiles/sick-3d-20_Calibration_calibration.xml";

	//  --- Init ImageGabber for one ranger3.
	// Only one camera, you can open it directly without mac.
	// But you need to gurantee the IP requirement.
	std::unique_ptr<ImageGabber> pIG = std::make_unique<ImageGabber>(pR3S, "0006772c2388", false);
	std::unique_ptr<ImageGabber> pIG2 = std::make_unique<ImageGabber>(pR3S, "0006770af0e3", false);
	//std::unique_ptr<ImageGabber> pIG3 = std::make_unique<ImageGabber>(pR3S, "000677277ff4", false);

	std::unique_ptr<InterfaceOpenCV> pIF = std::make_unique<InterfaceOpenCV>();

	

	if (false == pIG->connect()) // 0.02 is y scale, should be given by user. 
	{
		std::cerr << "Init camera fail." << std::endl;
		return false;
	}

	if (false == pIG2->connect()) // 0.02 is y scale, should be given by user. 
	{
		std::cerr << "Init camera fail." << std::endl;
		return false;
	}




	//if (false == pIG3->connect()) // 0.02 is y scale, should be given by user. 
	//{
	//	std::cerr << "Init camera fail." << std::endl;
	//	return false;
	//}


	//  --- Init ImageGabber for one ranger3.
	pIG->setUserParameter(0.02, para_csv_path, calib_xml_path, SiCaliWrapper::RectMethod::Top, 4000, NAN);
	pIG->getR3()->setBufferCount(100);
	pIG->getR3()->setDoRectify(false);

	pIG2->setUserParameter(0.02, para_csv_path2, calib_xml_path2, SiCaliWrapper::RectMethod::Top, 4000, NAN);
	pIG2->getR3()->setBufferCount(100);
	pIG2->getR3()->setDoRectify(false);

	/*pIG3->setUserParameter(0.02, para_csv_path3, calib_xml_path3, SiCaliWrapper::RectMethod::Top, 4000, NAN);
	pIG3->getR3()->setBufferCount(20);
	pIG3->getR3()->setDoRectify(false);*/

	bool ok(false);
	bool to_get_sensor(imgType == DN::SEN);
	
	if (to_get_sensor)
	{
		ok = pIG->setParameter("DeviceScanType", "Areascan"); printf("%d, \n", ok); // Sensor image
	}
	else
	{
		ok = pIG->setParameter("DeviceScanType", "Linescan3D"); printf("%d, \n", ok); // 3D Range image
		
		// --- Set parameter examples
		ok = pIG->setParameter("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "1");	printf("%d, \n", ok);

		ok = pIG2->setParameter("DeviceScanType", "Linescan3D"); printf("%d, \n", ok); // 3D Range image

																					  // --- Set parameter examples
		ok = pIG2->setParameter("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "1");	printf("%d, \n", ok);

		//ok = pIG3->setParameter("DeviceScanType", "Linescan3D"); printf("%d, \n", ok); // 3D Range image

		//																			  // --- Set parameter examples
		//ok = pIG3->setParameter("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "1");	printf("%d, \n", ok);
		//ok = pIG->setParameter("Height_RegionSelector_Scan3dExtraction1", "3000");	printf("%d, \n", ok);

		//ok = pIG2->setParameter("DeviceScanType", "Linescan3D"); printf("%d, \n", ok); // 3D Range image

		//																			  // --- Set parameter examples
		//ok = pIG2->setParameter("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "1");	printf("%d, \n", ok);
		//ok = pIG2->setParameter("Height_RegionSelector_Scan3dExtraction1", "3000");	printf("%d, \n", ok);
	}

	//  --- Start a thread for one ranger3.
	std::thread* thread_pImgGabber = new std::thread(&ImageGabber::run, pIG.get());
	thread_pImgGabber->detach();
	delete thread_pImgGabber;

	std::thread* thread_pImgGabber2 = new std::thread(&ImageGabber::run, pIG2.get());
	thread_pImgGabber2->detach();
	delete thread_pImgGabber2;

	//std::thread* thread_pImgGabber3 = new std::thread(&ImageGabber::run, pIG3.get());
	//thread_pImgGabber3->detach();
	//delete thread_pImgGabber3;

	// --- Prepare to show image
	mouse_para mp, mp2;// , mp3;

	mp.winName_ran = "Range";

	cv::namedWindow(mp.winName_ran, CV_WINDOW_KEEPRATIO);
	cv::setMouseCallback(mp.winName_ran, &(on_mouse_ran), &mp);

	mp2.winName_ran = "Range2";

	cv::namedWindow(mp2.winName_ran, CV_WINDOW_KEEPRATIO);
	cv::setMouseCallback(mp2.winName_ran, &(on_mouse_ran), &mp2);

	//mp3.winName_ran = "Range3";

	//cv::namedWindow(mp3.winName_ran, CV_WINDOW_KEEPRATIO);
	//cv::setMouseCallback(mp3.winName_ran, &(on_mouse_ran), &mp3);

	std::cout << "Press 'Q' or 'q' to quit! " << std::endl;
	std::cout << "Press 'C' or 'c' to switch sensor/range! " << std::endl;
	std::cout << "Press 'P' or 'p' to set parameters! " << std::endl;

	if (false == pIG->requireStartAcquition())
	{
		std::cerr << "Start camera fail." << std::endl;
		return false;
	}
	while (!pIG->isStartAcquition());
	
	if (false == pIG2->requireStartAcquition())
	{
		std::cerr << "Start camera fail." << std::endl;
		return false;
	}
	while (!pIG2->isStartAcquition());

	/*if (false == pIG3->requireStartAcquition())
	{
		std::cerr << "Start camera fail." << std::endl;
		return false;
	}
	while (!pIG3->isStartAcquition());*/

	int img_n(1);
	bool is_setParameter(false);
	int number = 0;//nImg < 1 ? 100 : nImg;
	int number2 = 0;
	int number3 = 0;
	while (true)
	{
		char key = cv::waitKey(10);
		if (key == 'q' || key == 'Q')
		{
			pIG->requireStopAcquition();
			while (pIG->isStartAcquition());
			std::cout << "cam 1 StopAcquition!" << std::endl;

			pIG2->requireStopAcquition();
			while (pIG2->isStartAcquition());
			std::cout << "cam 2 StopAcquition!" << std::endl;

			pIG->requireDisconnect();
			while (!pIG->isDisconnected());
			std::cout << "cam 1 Disconnected!" << std::endl;

			pIG2->requireDisconnect();
			while (!pIG2->isDisconnected());
			std::cout << "cam 2 Disconnected!" << std::endl;

			break;
		}

		//if (key == 't' || key == 'T')
		//{
		//	pIG->requireStopAcquition();
		//	pIG2->requireStopAcquition();
		//	//while (!pIG->isGrabbed());
		//
		//	std::cout << "Grabbed!" << std::endl;
		//}

		//if (key == 's' || key == 'S')
		//{
		//	pIG->requireStartAcquition();
		//	pIG->requireStartAcquition();
		//	//while (!pIG->isGrabbed());

		//	std::cout << "Grabbed!" << std::endl;
		//}
#if 1
		if (number % 100 == 0)
		{
			Sleep(100);

			pIG->requireStopAcquition();
			Sleep(100);
			pIG2->requireStopAcquition();

			Sleep(100);
			pIG->requireStartAcquition();
			Sleep(100);
			pIG->requireStartAcquition();

			//std::cout << "stop and start" << std::endl;
		}
#endif // 0


		//if ((number % 20)==0)
		//{
		//	/*pIG->requireStopAcquition();
		//	while (!pIG->isGrabbed());
		//	pIG->requireStartAcquition();*/
		//	//pIG->requireRestart();
		//	//std::cout << "Grabbed!" << std::endl;
		//}
				

		if (key == 'c' || key == 'C')
		{
			pIG->requireChangeOutputMode();
			pIG2->requireChangeOutputMode();
			continue;
		}

		if (pIG->isGrabbed())
		{
		}

		if (pIG2->isGrabbed())
		{
		}

		//if (!pIG3->isGrabbed())		continue;

		// --- Get Image.
		mp.pImgD = std::make_unique<SickCam::ImgT>(pIG->getImgT());

		mp2.pImgD = std::make_unique<SickCam::ImgT>(pIG2->getImgT());

		//mp3.pImgD = std::make_unique<SickCam::ImgT>(pIG3->getImgT());

		/*if (mp.pImgD->isEmpty())		continue;

		if (mp2.pImgD->isEmpty())		continue;*/

		//if (mp3.pImgD->isEmpty())		continue;

		//IMG imgs = mp.pImgD->getData();
		bool hasRef = mp.pImgD->has(DN::REF);
		bool isSens = mp.pImgD->has(DN::SEN);
		bool hasChu = mp.pImgD->has_ChunkInfo();

		bool hasRef2 = mp2.pImgD->has(DN::REF);
		bool isSens2 = mp2.pImgD->has(DN::SEN);
		bool hasChu2 = mp2.pImgD->has_ChunkInfo();

	/*	bool hasRef3 = mp2.pImgD->has(DN::REF);
		bool isSens3 = mp2.pImgD->has(DN::SEN);
		bool hasChu3 = mp2.pImgD->has_ChunkInfo();*/

		DN RefName = isSens ? DN::SEN : DN::REF;

		uint8_t * pRef = (uint8_t *)mp.pImgD->getData(RefName);

		mp.setI(pIF->uint8_tPtrToMat(pRef, mp.pImgD->get_rows(RefName), mp.pImgD->get_cols(RefName)));

		mp.setR(pIF->uint16_tPtrToMat(
			(uint16_t *)mp.pImgD->getData(DN::RAN),
			mp.pImgD->get_rows(DN::RAN), mp.pImgD->get_cols(DN::RAN)));

		mp.setC(pIF->floatPtrToMat(
			(float *)mp.pImgD->getData(DN::RAN_CAL),
			mp.pImgD->get_rows(DN::RAN_CAL), mp.pImgD->get_cols(DN::RAN_CAL)));

		/*int cols = mp.pImgD->get_cols(DN::RAN_CAL);
		int rows = mp.pImgD->get_rows(DN::RAN_CAL);
		uint16_t* singleRowXs = new uint16_t[cols];
		double lowerBoundX = mp.pImgD->getC_lower_bound_x();
		double scaleX = mp.pImgD->getC_scaleX();
		float* X = new float[rows*cols];
		for (int i = 0; i < cols; i++)
		{
			singleRowXs[i] = lowerBoundX + i*scaleX;
		}

		for (int i = 0; i < rows; i++)
		{
			memcpy(X + i*cols, singleRowXs, cols*sizeof(uint16_t));
		}

		floatPtrToMat(X, rows, cols);

		delete[] X;
		X = nullptr;*/
		uint8_t * pRef2 = (uint8_t *)mp2.pImgD->getData(RefName);

		mp2.setI(pIF->uint8_tPtrToMat(pRef2, mp2.pImgD->get_rows(RefName), mp2.pImgD->get_cols(RefName)));

		mp2.setR(pIF->uint16_tPtrToMat(
			(uint16_t *)mp2.pImgD->getData(DN::RAN),
			mp2.pImgD->get_rows(DN::RAN), mp2.pImgD->get_cols(DN::RAN)));

		mp2.setC(pIF->floatPtrToMat(
			(float *)mp2.pImgD->getData(DN::RAN_CAL),
			mp2.pImgD->get_rows(DN::RAN_CAL), mp2.pImgD->get_cols(DN::RAN_CAL)));

	/*	uint8_t * pRef3 = (uint8_t *)mp3.pImgD->getData(RefName);

		mp3.setI(pIF->uint8_tPtrToMat(pRef3, mp3.pImgD->get_rows(RefName), mp3.pImgD->get_cols(RefName)));

		mp3.setR(pIF->uint16_tPtrToMat(
			(uint16_t *)mp3.pImgD->getData(DN::RAN),
			mp3.pImgD->get_rows(DN::RAN), mp3.pImgD->get_cols(DN::RAN)));

		mp3.setC(pIF->floatPtrToMat(
			(float *)mp3.pImgD->getData(DN::RAN_CAL),
			mp3.pImgD->get_rows(DN::RAN_CAL), mp3.pImgD->get_cols(DN::RAN_CAL)));*/

		if (!isSens)
		{
			if (!mp.org_ran.empty())
			{
				// Prepare to show.
				cv::Mat src = mp.cal_ran.empty() ? mp.org_ran.clone() : mp.cal_ran.clone();
				if (!mp.cal_ran.empty())
					pIF->removeNAN(src);

				cv::normalize(src, src, 0.0, 255.0, cv::NORM_MINMAX);
				src.convertTo(src, CV_8UC1);
				cv::imshow(mp.winName_ran, src);

				number++;
				
				//number3++;
				std::cout << "-    Get Image1 : " << number << std::endl;
				

			/*	char savePath[255] = { 0 };
				sprintf_s(savePath, "D:/1-%d", number);*/

			/*	SaveBuffer(mp.pImgD->RangX, static_cast<float*>(mp.pImgD->getData(DN::RAN_CAL)),
					nullptr, mp.pImgD->get_cols(DN::RAN_CAL), mp.pImgD->get_rows(DN::RAN_CAL),
					mp.pImgD->getC_lower_bound_x(), mp.pImgD->getC_lower_bound_r(),
					mp.pImgD->getC_upper_bound_x(), mp.pImgD->getC_upper_bound_r(), savePath);*/
			}

			if (!mp2.org_ran.empty())
			{
				// Prepare to show.
				cv::Mat src = mp2.cal_ran.empty() ? mp2.org_ran.clone() : mp2.cal_ran.clone();
				if (!mp2.cal_ran.empty())
					pIF->removeNAN(src);

				cv::normalize(src, src, 0.0, 255.0, cv::NORM_MINMAX);
				src.convertTo(src, CV_8UC1);
				cv::imshow(mp2.winName_ran, src);

				
				number2++;
				//number3++;
				
				std::cout << "-    Get Image2 : " << number2 << std::endl;

				//char savePath[255] = { 0 };
				//sprintf_s(savePath, "D:/2-%d", number);

			/*	SaveBuffer(mp2.pImgD->RangX, static_cast<float*>(mp2.pImgD->getData(DN::RAN_CAL)),
					nullptr, mp2.pImgD->get_cols(DN::RAN_CAL), mp2.pImgD->get_rows(DN::RAN_CAL),
					mp2.pImgD->getC_lower_bound_x(), mp2.pImgD->getC_lower_bound_r(),
					mp2.pImgD->getC_upper_bound_x(), mp2.pImgD->getC_upper_bound_r(), savePath);*/
			}

			//if (!mp3.org_ran.empty())
			//{
			//	// Prepare to show.
			//	cv::Mat src = mp3.cal_ran.empty() ? mp3.org_ran.clone() : mp3.cal_ran.clone();
			//	if (!mp3.cal_ran.empty())
			//		pIF->removeNAN(src);

			//	cv::normalize(src, src, 0.0, 255.0, cv::NORM_MINMAX);
			//	src.convertTo(src, CV_8UC1);
			//	cv::imshow(mp3.winName_ran, src);

			//	char savePath[255] = { 0 };
			//	sprintf_s(savePath, "D:/3-%d", number);

			///*	SaveBuffer(mp3.pImgD->RangX, static_cast<float*>(mp3.pImgD->getData(DN::RAN_CAL)),
			//		nullptr, mp3.pImgD->get_cols(DN::RAN_CAL), mp3.pImgD->get_rows(DN::RAN_CAL),
			//		mp3.pImgD->getC_lower_bound_x(), mp3.pImgD->getC_lower_bound_r(),
			//		mp3.pImgD->getC_upper_bound_x(), mp3.pImgD->getC_upper_bound_r(), savePath);*/
			//}
		}
		else
		{
			if (!mp.org_int.empty())
			{
				cv::imshow(mp.winName_ran, mp.org_int);
			}

			if (!mp2.org_int.empty())
			{
				cv::imshow(mp2.winName_ran, mp2.org_int);
			}

			/*if (!mp3.org_int.empty())
			{
				cv::imshow(mp3.winName_ran, mp3.org_int);
			}*/
		}
		//nImg < 1 ? number : --number;
		//number++;
		//number2++;
		////number3++;
		//std::cout << "-    Get Image1 : " << number << std::endl;
		//std::cout << "-    Get Image2 : " << number2 << std::endl;
		//std::cout << "-    Get Image3 : " << number3 << std::endl;
		///mp.pImgD->printInfo();
	}
	//getchar();
	return true;
}

bool test_one_camera3(const DN & imgType = DN::SEN, const int & nImg = -1)
{
	// --- Init Ranger3 environment.
	std::shared_ptr<R3S> pR3S = std::make_shared<R3S>("");

	// --- Check
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevList();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
		{
			std::cout << "---- " << sub.first.c_str() << std::endl;
		}
	}
	else
	{
		printf("Error: scan camera error. \n");
		return 0;
	}

	// --- Parameters for one Ranger3.
	///std::string para_csv_path = "D:\\support\\zzz.csv";
	///std::string calib_xml_path = "D:\\support\\litao\\SN19040073_calibration.xml";
	std::string para_csv_path = "D:/CameraConfigFiles/SickConfigOn2.csv";
	std::string calib_xml_path = "D:/CameraConfigFiles/sick-2d-120_Calibration_calibration.xml";


	std::string para_csv_path2 = "D:/CameraConfigFiles/SickConfigOn1.csv";
	std::string calib_xml_path2 = "D:/CameraConfigFiles/sick-3d-20_Calibration_calibration.xml";

	std::string para_csv_path3 = "D:/CameraConfigFiles/SickConfigOn3.csv";
	std::string calib_xml_path3 = "D:/CameraConfigFiles/sick-3d-20_Calibration_calibration.xml";

	//std::string para_csv_path3 = "D:/CameraConfigFiles/SickConfig3.csv";
	//std::string calib_xml_path3 = "D:/CameraConfigFiles/sick-3d-20_Calibration_calibration.xml";

	//  --- Init ImageGabber for one ranger3.
	// Only one camera, you can open it directly without mac.
	// But you need to gurantee the IP requirement.
	std::unique_ptr<ImageGabber> pIG = std::make_unique<ImageGabber>(pR3S, "0006772c2388", false);
	std::unique_ptr<ImageGabber> pIG2 = std::make_unique<ImageGabber>(pR3S, "0006770af0e3", false);
	std::unique_ptr<ImageGabber> pIG3 = std::make_unique<ImageGabber>(pR3S, "0006772b0397", false);
	//std::unique_ptr<ImageGabber> pIG3 = std::make_unique<ImageGabber>(pR3S, "000677277ff4", false);

	std::unique_ptr<InterfaceOpenCV> pIF = std::make_unique<InterfaceOpenCV>();



	if (false == pIG->connect()) // 0.02 is y scale, should be given by user. 
	{
		std::cerr << "Init camera fail." << std::endl;
		return false;
	}

	if (false == pIG2->connect()) // 0.02 is y scale, should be given by user. 
	{
		std::cerr << "Init camera fail." << std::endl;
		return false;
	}


	if (false == pIG3->connect()) // 0.02 is y scale, should be given by user. 
	{
		std::cerr << "Init camera fail." << std::endl;
		return false;
	}

	//  --- Init ImageGabber for one ranger3.
	pIG->setUserParameter(0.02, para_csv_path, calib_xml_path, SiCaliWrapper::RectMethod::Top, 4000, NAN);
	pIG->getR3()->setBufferCount(100);
	pIG->getR3()->setDoRectify(false);

	pIG2->setUserParameter(0.02, para_csv_path2, calib_xml_path2, SiCaliWrapper::RectMethod::Top, 4000, NAN);
	pIG2->getR3()->setBufferCount(100);
	pIG2->getR3()->setDoRectify(false);

	pIG3->setUserParameter(0.02, para_csv_path3, calib_xml_path3, SiCaliWrapper::RectMethod::Top, 4000, NAN);
	pIG3->getR3()->setBufferCount(100);
	pIG3->getR3()->setDoRectify(false);

	bool ok(false);
	bool to_get_sensor(imgType == DN::SEN);

	if (to_get_sensor)
	{
		ok = pIG->setParameter("DeviceScanType", "Areascan"); printf("%d, \n", ok); // Sensor image
	}
	else
	{
		ok = pIG->setParameter("DeviceScanType", "Linescan3D"); printf("%d, \n", ok); // 3D Range image

																					  // --- Set parameter examples
		ok = pIG->setParameter("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "1");	printf("%d, \n", ok);

		ok = pIG2->setParameter("DeviceScanType", "Linescan3D"); printf("%d, \n", ok); // 3D Range image

																					   // --- Set parameter examples
		ok = pIG2->setParameter("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "1");	printf("%d, \n", ok);

		ok = pIG3->setParameter("DeviceScanType", "Linescan3D"); printf("%d, \n", ok); // 3D Range image

																					   // --- Set parameter examples
		ok = pIG3->setParameter("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "1");	printf("%d, \n", ok);

		//ok = pIG3->setParameter("DeviceScanType", "Linescan3D"); printf("%d, \n", ok); // 3D Range image

		//																			  // --- Set parameter examples
		//ok = pIG3->setParameter("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "1");	printf("%d, \n", ok);
		//ok = pIG->setParameter("Height_RegionSelector_Scan3dExtraction1", "3000");	printf("%d, \n", ok);

		//ok = pIG2->setParameter("DeviceScanType", "Linescan3D"); printf("%d, \n", ok); // 3D Range image

		//																			  // --- Set parameter examples
		//ok = pIG2->setParameter("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "1");	printf("%d, \n", ok);
		//ok = pIG2->setParameter("Height_RegionSelector_Scan3dExtraction1", "3000");	printf("%d, \n", ok);
	}

	//  --- Start a thread for one ranger3.
	std::thread* thread_pImgGabber = new std::thread(&ImageGabber::run, pIG.get());
	thread_pImgGabber->detach();
	delete thread_pImgGabber;

	std::thread* thread_pImgGabber2 = new std::thread(&ImageGabber::run, pIG2.get());
	thread_pImgGabber2->detach();
	delete thread_pImgGabber2;

	std::thread* thread_pImgGabber3 = new std::thread(&ImageGabber::run, pIG3.get());
	thread_pImgGabber3->detach();
	delete thread_pImgGabber3;

	//std::thread* thread_pImgGabber3 = new std::thread(&ImageGabber::run, pIG3.get());
	//thread_pImgGabber3->detach();
	//delete thread_pImgGabber3;

	// --- Prepare to show image
	mouse_para mp, mp2, mp3;

	mp.winName_ran = "Range";

	cv::namedWindow(mp.winName_ran, CV_WINDOW_KEEPRATIO);
	cv::setMouseCallback(mp.winName_ran, &(on_mouse_ran), &mp);

	mp2.winName_ran = "Range2";

	cv::namedWindow(mp2.winName_ran, CV_WINDOW_KEEPRATIO);
	cv::setMouseCallback(mp2.winName_ran, &(on_mouse_ran), &mp2);

	mp3.winName_ran = "Range3";

	cv::namedWindow(mp3.winName_ran, CV_WINDOW_KEEPRATIO);
	cv::setMouseCallback(mp3.winName_ran, &(on_mouse_ran), &mp3);

	//mp3.winName_ran = "Range3";

	//cv::namedWindow(mp3.winName_ran, CV_WINDOW_KEEPRATIO);
	//cv::setMouseCallback(mp3.winName_ran, &(on_mouse_ran), &mp3);

	std::cout << "Press 'Q' or 'q' to quit! " << std::endl;
	std::cout << "Press 'C' or 'c' to switch sensor/range! " << std::endl;
	std::cout << "Press 'P' or 'p' to set parameters! " << std::endl;

	if (false == pIG->requireStartAcquition())
	{
		std::cerr << "Start camera 1 fail." << std::endl;
		return false;
	}
	while (!pIG->isStartAcquition());

	if (false == pIG2->requireStartAcquition())
	{
		std::cerr << "Start camera 2 fail." << std::endl;
		return false;
	}
	while (!pIG2->isStartAcquition());

	if (false == pIG3->requireStartAcquition())
	{
		std::cerr << "Start camera 3 fail." << std::endl;
		return false;
	}
	while (!pIG3->isStartAcquition());


	/*if (false == pIG3->requireStartAcquition())
	{
	std::cerr << "Start camera fail." << std::endl;
	return false;
	}
	while (!pIG3->isStartAcquition());*/

	int img_n(1);
	bool is_setParameter(false);
	int number1 = 0;//nImg < 1 ? 100 : nImg;
	int number2 = 0;
	int number3 = 0;
	int times = 0;
	while (true)
	{
		char key = cv::waitKey(10);
		if (key == 'q' || key == 'Q')
		{
			pIG->requireStopAcquition();
			while (pIG->isStartAcquition());
			std::cout << "cam 1 StopAcquition!" << std::endl;

			pIG2->requireStopAcquition();
			while (pIG2->isStartAcquition());
			std::cout << "cam 2 StopAcquition!" << std::endl;

			pIG->requireDisconnect();
			while (!pIG->isDisconnected());
			std::cout << "cam 1 Disconnected!" << std::endl;

			pIG2->requireDisconnect();
			while (!pIG2->isDisconnected());
			std::cout << "cam 2 Disconnected!" << std::endl;

			break;
		}
		++times;
#if 1
		if (times % 100 == 0)
		{
			pIG->requireStopAcquition();
			while (pIG->isStartAcquition());
			std::cout << "cam 1 StopAcquition!" << std::endl;

			pIG2->requireStopAcquition();
			while (pIG2->isStartAcquition());
			std::cout << "cam 2 StopAcquition!" << std::endl;

			pIG3->requireStopAcquition();
			while (pIG3->isStartAcquition());
			std::cout << "cam 3 StopAcquition!" << std::endl;

			//Sleep(10000);
			time_t start, end;
			start = clock();
			if (false == pIG->requireStartAcquition())
			{
				std::cerr << "Start camera fail." << std::endl;
				return false;
			}
			while (!pIG->isStartAcquition());

			if (false == pIG2->requireStartAcquition())
			{
				std::cerr << "Start camera fail." << std::endl;
				return false;
			}
			while (!pIG2->isStartAcquition());

			if (false == pIG3->requireStartAcquition())
			{
				std::cerr << "Start camera fail." << std::endl;
				return false;
			}
			while (!pIG3->isStartAcquition());
			end = clock() - start;
			std::cout << "start times is:" << double(end) / CLOCKS_PER_SEC << std::endl;
			wchar_t cc[255] = { 0 };
			swprintf_s(cc, L"start times is:%f \n\n", double(end) / CLOCKS_PER_SEC);
			OutputDebugString(cc);
			//std::cout << "stop and start" << std::endl;
		}
#endif // 0


		//if ((number % 20)==0)
		//{
		//	/*pIG->requireStopAcquition();
		//	while (!pIG->isGrabbed());
		//	pIG->requireStartAcquition();*/
		//	//pIG->requireRestart();
		//	//std::cout << "Grabbed!" << std::endl;
		//}


		if (key == 'c' || key == 'C')
		{
			pIG->requireChangeOutputMode();
			pIG2->requireChangeOutputMode();
			continue;
		}

		if (pIG->isGrabbed())
		{
			mp.pImgD = std::make_unique<SickCam::ImgT>(pIG->getImgT());
			++number1;
			std::cout << "cam 1 " << mp.pImgD->get_ID() << "image count:"<<number1<<std::endl;
		}

		if (pIG2->isGrabbed())
		{
			mp2.pImgD = std::make_unique<SickCam::ImgT>(pIG2->getImgT());
			++number2;
			std::cout << "cam 2 " << mp2.pImgD->get_ID() << "image count:"<<number2<<std::endl;
		}

		if (pIG3->isGrabbed())
		{
			mp3.pImgD = std::make_unique<SickCam::ImgT>(pIG3->getImgT());
			++number3;
			std::cout << "cam 3 " << mp3.pImgD->get_ID() << "image count:" << number3 << std::endl;
		}
	}
	//getchar();
	return true;
}


bool test_one_camera(const DN & imgType = DN::SEN, const int & nImg = -1)
{
	// --- Init Ranger3 environment.
	std::shared_ptr<R3S> pR3S = std::make_shared<R3S>("");

	// --- Check
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevList();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
		{
			std::cout << "---- " << sub.first.c_str() << std::endl;
		}
	}
	else
	{
		printf("Error: scan camera error. \n");
		return 0;
	}

	// --- Parameters for one Ranger3.
	///std::string para_csv_path = "D:\\support\\zzz.csv";
	///std::string calib_xml_path = "D:\\support\\litao\\SN19040073_calibration.xml";
	std::string para_csv_path = "D:/CameraConfigFiles/SickConfig3.csv";
	std::string calib_xml_path = "D:/CameraConfigFiles/sick-2d-120_Calibration_calibration.xml";
	
	
	//  --- Init ImageGabber for one ranger3.
	// Only one camera, you can open it directly without mac.
	// But you need to gurantee the IP requirement.
	std::unique_ptr<ImageGabber> pIG = std::make_unique<ImageGabber>(pR3S, "000677277ff4", false);
	
	std::unique_ptr<InterfaceOpenCV> pIF = std::make_unique<InterfaceOpenCV>();



	if (false == pIG->connect()) // 0.02 is y scale, should be given by user. 
	{
		std::cerr << "Init camera fail." << std::endl;
		return false;
	}

	

	//  --- Init ImageGabber for one ranger3.
	pIG->setUserParameter(0.02, para_csv_path, calib_xml_path, SiCaliWrapper::RectMethod::Top, 4000, NAN);
	pIG->getR3()->setBufferCount(500);
	pIG->getR3()->setDoRectify(false);

	

	bool ok(false);
	bool to_get_sensor(imgType == DN::SEN);

	if (to_get_sensor)
	{
		ok = pIG->setParameter("DeviceScanType", "Areascan"); printf("%d, \n", ok); // Sensor image
	}
	else
	{
		//ok = pIG->setParameter("DeviceScanType", "Linescan3D"); printf("%d, \n", ok); // 3D Range image

																					  // --- Set parameter examples
		//ok = pIG->setParameter("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "1");	printf("%d, \n", ok);

	
		//ok = pIG->setParameter("Height_RegionSelector_Scan3dExtraction1", "3000");	printf("%d, \n", ok);

		
	}

	//  --- Start a thread for one ranger3.
	std::thread* thread_pImgGabber = new std::thread(&ImageGabber::run, pIG.get());
	thread_pImgGabber->detach();
	delete thread_pImgGabber;



	// --- Prepare to show image
	mouse_para mp;

	mp.winName_ran = "Range";

	cv::namedWindow(mp.winName_ran, CV_WINDOW_KEEPRATIO);
	cv::setMouseCallback(mp.winName_ran, &(on_mouse_ran), &mp);


	std::cout << "Press 'Q' or 'q' to quit! " << std::endl;
	std::cout << "Press 'C' or 'c' to switch sensor/range! " << std::endl;
	std::cout << "Press 'P' or 'p' to set parameters! " << std::endl;

	if (false == pIG->requireStartAcquition())
	{
		std::cerr << "Start camera fail." << std::endl;
		return false;
	}
	while (!pIG->isStartAcquition());

	
	int img_n(1);
	bool is_setParameter(false);
	int number = 0;//nImg < 1 ? 100 : nImg;
	time_t start, end;
	while (true)
	{
		start = clock();
		char key = cv::waitKey(10);
		if (key == 'q' || key == 'Q')
		{
			pIG->requireDisconnect();
			while (!pIG->isDisconnected());
			std::cout << "Disconnected!" << std::endl;
			

			break;
		}

		if (key == 't' || key == 'T')
		{
			pIG->requireStopAcquition();
			//while (!pIG->isGrabbed());

			std::cout << "Grabbed!" << std::endl;
		}

		if (key == 's' || key == 'S')
		{
			pIG->requireStartAcquition();
			//while (!pIG->isGrabbed());

			std::cout << "Grabbed!" << std::endl;
		}

		

		if (key == 'c' || key == 'C')
		{
			pIG->requireChangeOutputMode();
			
			continue;
		}

		if (!pIG->isGrabbed())		continue;

	

		// --- Get Image.
		mp.pImgD = std::make_unique<SickCam::ImgT>(pIG->getImgT());

	
		if (mp.pImgD->isEmpty())		continue;


#if 0
		//IMG imgs = mp.pImgD->getData();
		bool hasRef = mp.pImgD->has(DN::REF);
		bool isSens = mp.pImgD->has(DN::SEN);
		bool hasChu = mp.pImgD->has_ChunkInfo();


		DN RefName = isSens ? DN::SEN : DN::REF;

		uint8_t * pRef = (uint8_t *)mp.pImgD->getData(RefName);

		mp.setI(pIF->uint8_tPtrToMat(pRef, mp.pImgD->get_rows(RefName), mp.pImgD->get_cols(RefName)));

		mp.setR(pIF->uint16_tPtrToMat(
			(uint16_t *)mp.pImgD->getData(DN::RAN),
			mp.pImgD->get_rows(DN::RAN), mp.pImgD->get_cols(DN::RAN)));

		mp.setC(pIF->floatPtrToMat(
			(float *)mp.pImgD->getData(DN::RAN_CAL),
			mp.pImgD->get_rows(DN::RAN_CAL), mp.pImgD->get_cols(DN::RAN_CAL)));


		if (!isSens)
		{
			if (!mp.org_ran.empty())
			{
				// Prepare to show.
				cv::Mat src = mp.cal_ran.empty() ? mp.org_ran.clone() : mp.cal_ran.clone();
				if (!mp.cal_ran.empty())
					pIF->removeNAN(src);

				cv::normalize(src, src, 0.0, 255.0, cv::NORM_MINMAX);
				src.convertTo(src, CV_8UC1);
				cv::imshow(mp.winName_ran, src);

				char savePath[255] = { 0 };
				sprintf_s(savePath, "D:/1-%d", number);

				/*	SaveBuffer(mp.pImgD->RangX, static_cast<float*>(mp.pImgD->getData(DN::RAN_CAL)),
				nullptr, mp.pImgD->get_cols(DN::RAN_CAL), mp.pImgD->get_rows(DN::RAN_CAL),
				mp.pImgD->getC_lower_bound_x(), mp.pImgD->getC_lower_bound_r(),
				mp.pImgD->getC_upper_bound_x(), mp.pImgD->getC_upper_bound_r(), savePath);*/
			}


		}
		else
		{
			if (!mp.org_int.empty())
			{
				cv::imshow(mp.winName_ran, mp.org_int);
			}


		}
#endif // 0

		//nImg < 1 ? number : --number;
		end = clock() - start;
		std::cout << "getImgT run time: " << double(end) / CLOCKS_PER_SEC << " sec" << std::endl;
		number++;
	
		std::cout << "-    Get Image1 : " << number << std::endl;
		
		///mp.pImgD->printInfo();
	}
	getchar();
	return true;
}

////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	try
	{
		std::cout << SickCam::VER::__version__().c_str() << std::endl;

#ifdef UNIT_TEST
		protected_unit_test_ip_setting();
#endif // UNIT_TEST
		
		// Êä³ö sensor Í¼Ïñ£¬È¡ 500 ÕÅÍ¼Ïñ¡£
		//test_one_camera(DN::SEN, 500);

		// Êä³ö range Í¼Ïñ£¬È¡ 10 ÕÅÍ¼Ïñ¡£
		//test_one_camera(DN::RAN, 10);
		//test_one_camera(DN::RAN, 10);
		test_one_camera3(DN::RAN, 10);

		//test_one_camera_callback();
		//test_one_camera_mac();
		//test_one_camera_ip();
		//test_two_cameras_with_mMacAddress();
		//test_two_cameras_with_mIpAddress();
	}
	catch (const char* msg)
	{
		std::cerr << msg <<std::endl;
		return -1;
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return -2;
	}
	catch (...)
	{
		std::cerr << "unknown exception!" << std::endl;
		return -3;
	}

	return 0;
}

