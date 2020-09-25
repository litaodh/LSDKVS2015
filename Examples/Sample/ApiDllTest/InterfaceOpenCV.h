/** ------------------------------------------------------------------------------------------
*	@ File Name		:	InterfaceOpenCV.h
*
*	@ Description	:
*		Class InterfaceOpenCV.
*		Show how to convert SICK Image Date to OpenCV
*
*	@ Copyright		:	Copyright 2016-2020 SICK AG. All rights reserved.
*	@ Author		:	SICK GCN Vision Lab.
*	@ Version		:	2.0.0.0
*	@ Time			:	2019.10.15
* ------------------------------------------------------------------------------------------ */


#pragma once


#include "Ranger3.h"
#include <opencv2\opencv.hpp>


typedef SickCam::ImgT			ImgT;
typedef std::unique_ptr<ImgT>	ImgTPtr;
typedef SickCam::DN				DN;
typedef Vec<DN>					ImgNames;


////////////////////////////////////////////////////////////////////////////////


/** @brifef InterfaceOpenCV
*	This class shows how to manage threads of Ranger3.
*	It is just a sample, you should modify it in your project.
*/
class InterfaceOpenCV
{
public:
	InterfaceOpenCV();
	~InterfaceOpenCV();

	// ----------------------------------------------------------------------------
	// Image Grabber
	cv::Mat		floatPtrToMat			(float * pD, size_t rows, size_t cols);
	cv::Mat		uint8_tPtrToMat			(uint8_t * pD, size_t rows, size_t cols);
	cv::Mat		uint16_tPtrToMat		(uint16_t * pD, size_t rows, size_t cols);
	double		Xmm						(const uint16_t vpixel, double offsetX, double scaleX);
	double		Ymm						(const uint16_t vpixel, double offsetY, double scaleY);
	void		removeNAN				(cv::Mat & img);

	// ----------------------------------------------------------------------------
	// API of ImgT to cv::Mat
	cv::Mat		getMat					(const ImgT & _img, const DN & _dn) const;
	bool		SaveImgT2OpenCVXML		(const ImgT & _img, cStr & path = "file.xml")				const;
	ImgT		LoadImgTFromOpenCVXML	(cStr & path = "file.xml", const ImgNames & _names=ImgNames())	const;

public:
	static const std::map<DN, cStr> xmlNames;

private:

};


////////////////////////////////////////////////////////////////////////////////


/** @brifef mouse_para
*	This class shows how to manage threads of Ranger3.
*	It is just a sample, you should modify it in your project.
*/
struct mouse_para
{
	mouse_para()
		: org_int(cv::Mat()), org_ran(cv::Mat()), cal_ran(cv::Mat()),
		pImgD(nullptr), winName_ran("")
	{}

	void setI(cv::Mat V)	{	org_int = V; }
	void setR(cv::Mat V)	{	org_ran = V; }
	void setC(cv::Mat V)	{	cal_ran = V; }


	cv::Mat			org_int;		// !< Intensitt/Reflactance/Sensor image
	cv::Mat			org_ran;		// !< Range image
	cv::Mat			cal_ran;		// !< Range image calibrated
	std::string		winName_ran;
	ImgTPtr			pImgD;
	InterfaceOpenCV	IfCV;
};


////////////////////////////////////////////////////////////////////////////////


/** @brifef on_mouse_ran			
*									
*	OpenCV mouse callback function.	
*									
*	Show Range(Extraction1) image.	
*/
void on_mouse_ran(int event, int x, int y, int flags, void * _ustc);


