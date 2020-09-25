#include "CalibrationWrapper.h"
#include <fstream>


namespace SiCaliWrapper
{


// ----------------------------------------------------------------------------
// calibratedData


calibratedData::calibratedData()
{
}

calibratedData::~calibratedData()
{
}

void * calibratedData::getData(PtrN _PN)
{
	if (m_Data.count(_PN) == 1)
	{
		return m_Data[_PN];
	}

	return nullptr;
}

bool calibratedData::insertData(Ve<PtrN> _names, Ve<void*> _pointers)
{
	if (_names.size() != _pointers.size()) return false;

	for (size_t i = 0; i < _names.size(); ++i)
	{
		m_Data.insert({_names[i], _pointers[i]});
	}

	return true;
}


// ----------------------------------------------------------------------------
// CalibrationWrapper


CalibrationWrapper::CalibrationWrapper()
{
}

CalibrationWrapper::~CalibrationWrapper()
{
}

bool CalibrationWrapper::initCalibrationFilter(
	const std::string filePath, const int bufferWidth, 
	const int bufferHeight, const int aoiHeight, const float scaleZ, const float originZ, 
	const RectMethod method, const size_t rectificationWidth, float missingDataValue, const bool _doRectify)
{
	// init sensor trait
	m_pSensorTraits				= std::make_unique<ranger3calibration::SensorTraits>();
	m_pSensorTraits->width		= bufferWidth;
	m_pSensorTraits->height		= bufferHeight;
	m_pSensorTraits->aoiHeight	= aoiHeight;
	m_pSensorTraits->scaleZ		= scaleZ;
	m_pSensorTraits->scaleX		= 1.0f;
	m_pSensorTraits->xOffset	= 0.0f;
	m_pSensorTraits->zOffset	= originZ;
	
	m_pData = std::make_shared<calibratedData>(); 
	m_pData->doRectify			= _doRectify;
	

	m_params = std::make_unique<ranger3calibration::CalibrationParameters>();
	std::ifstream inputStream(filePath);
	if (inputStream.good())
		ranger3calibration::loadCalibrationParameters(filePath.c_str(), m_params.get());
	else
		return false;

	m_pCalFilter = std::make_unique<ranger3calibration::CalibrationFilter>(*m_params, *(m_pSensorTraits.get()), missingDataValue);
	m_pRectFilter = std::make_unique<ranger3calibration::RectificationFilter>(*m_pCalFilter, rectificationWidth);

	//Get container for data
	m_pCalibrationData = std::make_unique<ranger3calibration::CalibrationData>(m_pCalFilter->generateOutputContainer());
	m_pRectificationData = std::make_unique<ranger3calibration::RectificationData>(m_pRectFilter->generateOutputContainer());

	switch (method)
	{
	case SiCaliWrapper::Top:
		m_pData->rm = ranger3calibration::RectificationMethod::Top;
		break;
	case SiCaliWrapper::Bottom:
		m_pData->rm = ranger3calibration::RectificationMethod::Bottom;
		break;
	case SiCaliWrapper::Mean:
		m_pData->rm = ranger3calibration::RectificationMethod::Mean;
		break;
	default:
		m_pData->rm = ranger3calibration::RectificationMethod::Top;
		break;
	}

	return true;
}

bool CalibrationWrapper::calibrate(uint8_t * range, uint8_t * reflectance, bool doRectify)
{
	if(range == nullptr)
		return false;

	m_pCalFilter->applyCalibration(range, *m_pCalibrationData, m_params->SSE);
	

	//if (doRectify)
	if(m_pData->doRectify)
	{
		m_pRectFilter->applyRectification(reflectance, *m_pCalibrationData, *m_pRectificationData, m_pData->rm);

		// convert data
		//m_pData->vRange = m_pRectificationData->range.get();
		//m_pData->vReflectance = m_pRectificationData->reflectance.get();
		m_pData->insertData({ PtrN::RAN_C, PtrN::REF_C }, { m_pRectificationData->range.get(), m_pRectificationData->reflectanceByte.get() });
		m_pData->Height = m_pRectificationData->height;
		m_pData->Width = m_pRectificationData->width;
	}
	else
	{	
		if (reflectance != nullptr && m_pRectificationData->reflectanceByte != nullptr)
		{
			int h = m_pCalibrationData->height;
			int w = m_pCalibrationData->width;
			memcpy(m_pRectificationData->reflectanceByte.get(), reflectance, h*w);
		}
		
		m_pData->insertData({ PtrN::RAN_C, PtrN::REF_C }, { m_pCalibrationData->rangeR.get(),  m_pRectificationData->reflectanceByte.get() });
		//m_pData->Width = m_pCalibrationData->width;
		m_pData->Height = m_pCalibrationData->height;
		m_pData->Width = m_pCalibrationData->width;
	}

	m_pData->offsetX = m_pCalFilter->worldTraits.lowerBoundX;
	m_pData->scaleX = (m_pCalFilter->worldTraits.upperBoundX - m_pCalFilter->worldTraits.lowerBoundX) / m_pData->Width;
	m_pData->lower_bound_x = m_pCalFilter->worldTraits.lowerBoundX;
	m_pData->upper_bound_x = m_pCalFilter->worldTraits.upperBoundX;
	m_pData->lower_bound_r = m_pCalFilter->worldTraits.lowerBoundR;
	m_pData->upper_bound_r = m_pCalFilter->worldTraits.upperBoundR;

	return true;
}

}
