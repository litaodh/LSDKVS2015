/** ------------------------------------------------------------------------------------------
*	@ File Name		:	Ranger3Callback.cpp
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


#include "InterfaceOpenCV.h"

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

const std::map<DN, cStr> InterfaceOpenCV::xmlNames = {
	{DN::SEN	 , "SEN_INFO"},
	{DN::RAN	 , "RAN_INFO"}, 
	{DN::RAN_CAL , "CAL_INFO"}
};


//====================================================================
// InterfaceOpenCV : constructor/destructor   

InterfaceOpenCV::InterfaceOpenCV()
{
}

InterfaceOpenCV::~InterfaceOpenCV()
{
}


//====================================================================
// InterfaceOpenCV : constructor/destructor   


cv::Mat InterfaceOpenCV::floatPtrToMat(float* pD, size_t rows, size_t cols)
{
	if (pD == nullptr)	return cv::Mat();

	cv::Mat dst = cv::Mat(rows, cols, CV_32FC1, pD);
	return dst;
}

cv::Mat InterfaceOpenCV::uint8_tPtrToMat(uint8_t * pD, size_t rows, size_t cols)
{
	if (pD == nullptr)	return cv::Mat();

	cv::Mat dst = cv::Mat(rows, cols, CV_8UC1, pD);
	return dst;
}

cv::Mat InterfaceOpenCV::uint16_tPtrToMat(uint16_t * pD, size_t rows, size_t cols)
{
	if (pD == nullptr)	return cv::Mat();

	cv::Mat dst = cv::Mat(rows, cols, CV_16SC1, pD);
	return dst;
}


double InterfaceOpenCV::Xmm(const uint16_t vpixel, double offsetX, double scaleX)
{
	return (offsetX + scaleX*vpixel);
}

double InterfaceOpenCV::Ymm(const uint16_t vpixel, double offsetY, double scaleY)
{
	return (offsetY + scaleY*vpixel);
}

void InterfaceOpenCV::removeNAN(cv::Mat & img)
{
	float fmin(0);
	cv::min(fmin, img);
	fmin = fmin * 0.1;

	if (img.type() != CV_32FC1) return;

	for (size_t j = 0; j < img.rows; ++j)
	{
		float* data = img.ptr<float>(j);
		for (size_t i = 0; i < img.cols; ++i)
		{
			data[i] = isnan(data[i]) ? fmin : data[i];
		}
	}
}

cv::Mat InterfaceOpenCV::getMat(const ImgT & _img, const DN & _dn) const
{
	cv::Mat src;
	if (_img.has(_dn))
	{
		switch (_dn)
		{
		case DN::INV:		break;
		case DN::SEN:		src = cv::Mat(_img.getS_rows(), _img.getS_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::RAN:		src = cv::Mat(_img.getR_rows(), _img.getR_cols(), CV_16SC1, (uint16_t	*)_img.getData(_dn));	break;
		case DN::REF:		src = cv::Mat(_img.getR_rows(), _img.getR_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::SCA:		src = cv::Mat(_img.getR_rows(), _img.getR_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::RAN_CAL:	src = cv::Mat(_img.getC_rows(), _img.getC_cols(), CV_32FC1, (float		*)_img.getData(_dn));	break;
		case DN::REF_CAL:	src = cv::Mat(_img.getC_rows(), _img.getC_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::REF_CAL_F:	src = cv::Mat(_img.getC_rows(), _img.getC_cols(), CV_32FC1, (float		*)_img.getData(_dn));	break;
		case DN::SCA_CAL:	src = cv::Mat(_img.getC_rows(), _img.getC_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::RAN2:		src = cv::Mat(_img.getR_rows(), _img.getR_cols(), CV_16SC1, (uint16_t	*)_img.getData(_dn));	break;
		case DN::REF2:		src = cv::Mat(_img.getR_rows(), _img.getR_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::SCA2:		src = cv::Mat(_img.getR_rows(), _img.getR_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::RAN2_CAL:	src = cv::Mat(_img.getC_rows(), _img.getC_cols(), CV_32FC1, (float		*)_img.getData(_dn));	break;
		case DN::REF2_CAL:	src = cv::Mat(_img.getC_rows(), _img.getC_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::SCA2_CAL:	src = cv::Mat(_img.getC_rows(), _img.getC_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		}
	}
	return src;
}

bool InterfaceOpenCV::SaveImgT2OpenCVXML(const ImgT & _img, cStr & path) const
{
	// save data
	auto _names = _img.getAvalibleDataNames();
	cv::FileStorage f;
	f.open(path, cv::FileStorage::WRITE);
	if (f.isOpened())
	{
		for (auto sub : _names)
		{
			if (_img.has(sub))
			{
				cv::Mat src = getMat(_img, sub);
				f << ImgT::DN2Str(sub) << src;
			}
		}
	}
	else
		return false;

	// save info
	cv::Mat
		SEN_info((cv::Mat_<uint16_t>(1, 2) << _img.getS_OffsetX(), _img.getS_OffsetY())),
		CAL_info((cv::Mat_<double> (1, 8) <<
			_img.getC_offsetX(),			_img.getC_offsetY(),			_img.getC_scaleX(),			_img.getC_scaleY(),
			_img.getC_lower_bound_x(),	_img.getC_upper_bound_x(),	_img.getC_lower_bound_r(),	_img.getC_upper_bound_r())),
		RAN_info((cv::Mat_<uint16_t>(1, 5) <<
			_img.getR_aoiOffsetX(), _img.getR_aoiOffsetY(), _img.getR_aoiHeight(), _img.getR_aoiWidth(), 
			(_img.getR_RangeAxis() == RA::STANDARD ? 1 : 0)));

	f << xmlNames.at(DN::SEN) << SEN_info;
	f << xmlNames.at(DN::RAN_CAL) << CAL_info;
	f << xmlNames.at(DN::RAN) << RAN_info;

	return true;
}

ImgT InterfaceOpenCV::LoadImgTFromOpenCVXML(cStr & path, const ImgNames & _names) const
{
	ImgT imgT;
	
	cv::FileStorage f;
	f.open(path, cv::FileStorage::READ);

	if (f.isOpened())
	{
		for (auto sub : _names)
		{
			cv::Mat src;
			f[ImgT::DN2Str(sub)] >> src;
			if (src.empty())	continue;

			imgT.insertDataCopy({ sub }, { src.data }, 0);
			if (DN::SEN == sub)
			{
				cv::Mat info;
				f[xmlNames.at(DN::SEN)] >> info;
				imgT.setSensorInfo(src.cols, src.rows, info.at<int16_t>(0), info.at<int16_t>(1));
			}
			
			if (DN::RAN_CAL == sub)
			{
				cv::Mat info;
				f[xmlNames.at(DN::RAN_CAL)] >> info;
				imgT.setCaliInfo(
					src.cols, src.rows,
					info.at<double>(0), info.at<double>(1), info.at<double>(2), info.at<double>(3), 
					info.at<double>(4), info.at<double>(5), info.at<double>(6), info.at<double>(7)
				);
			}
			
			if(DN::RAN == sub)
			{
				cv::Mat info;
				f[xmlNames.at(DN::RAN)] >> info;
				imgT.setRangeInfo(
					src.cols, src.rows,
					info.at<int16_t>(0), info.at<int16_t>(1), info.at<int16_t>(2), info.at<int16_t>(3),
					(info.at<int16_t>(4) == 1 ? RA::STANDARD : RA::REVERSED)
				);
			}
		
		}

	}

	return imgT;

}


//====================================================================
// on_mouse_ran : constructor/destructor   


void on_mouse_ran(int event, int x, int y, int flags, void *_ustc)
{
	mouse_para *ustc = static_cast<mouse_para*>(_ustc);

	if (event == CV_EVENT_MOUSEMOVE && !ustc->org_ran.empty())
	{
		std::cout << (ustc->org_ran.empty() ? "empty" : "") << std::endl;

		char sss[100] = { 0 };

		if (ustc->cal_ran.empty())
		{
			sprintf_s(sss, 100, "(px = %d, py = %d) = (x = %d, y = %d, z = %d)",
				x, y, x, y,
				ustc->org_ran.at<uint16_t>(y, x));
		}
		else
		{
			sprintf_s(sss, 100, "(px = %d, py = %d) = (x = %fmm, y = %fmm, z = %fmm)",
				x, y,
				ustc->IfCV.Xmm(x, ustc->pImgD->getC_offsetX(), ustc->pImgD->getC_scaleX()),
				ustc->IfCV.Ymm(y, ustc->pImgD->getC_offsetY(), ustc->pImgD->getC_scaleY()),
				ustc->cal_ran.at<float>(y, x));
		}

		std::cout << sss << std::endl;
	}
}