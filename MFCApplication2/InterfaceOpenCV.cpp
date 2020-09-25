
#include "InterfaceOpenCV.h"

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)


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

