#pragma once


#include <opencv2\opencv.hpp>


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

};


