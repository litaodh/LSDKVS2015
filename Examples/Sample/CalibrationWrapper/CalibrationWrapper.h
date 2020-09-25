#pragma once

#if defined(DLL_EXPORTS) // inside DLL
#   define EXPORT_DLL   __declspec(dllexport)
#else // outside DLL
#   define EXPORT_DLL   __declspec(dllimport)
#endif

#include "CalibrationFilter.h"
#include "RectificationFilter.h"

#include <vector>
#include <map>
#include <memory>

template<class T> using Ve = std::vector<T>;


#ifdef __cplusplus
namespace SiCaliWrapper{
	extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////


/**	Names of calibrated data pointers. */
typedef enum PointerNames
{
	RAN_C,		//! < float		* | Normal   | [Calibrated]	| ComponentSelector_Range_______RegionSelector_Scan3dExtraction1
	REF_C,		//! < uint8_t	* | Normal   | [Calibrated]	| ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1
	SCA_C		//! < not yet.	  |	Normal	 | [Calibrated]	| ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1
} PtrN;

//typedef std::map<PtrN, void *>					PtrData;
//typedef ranger3calibration::RectificationMethod RM;

enum RectMethod
{
	Top,
	Mean,
	Bottom
};


/** @brief calibratedData													 
*																			 
*	Calibrated data Class													 
*																		
*/
class EXPORT_DLL calibratedData
{
public:
	calibratedData();
	~calibratedData();

	void *		getData		(PtrN _PN);
	bool		insertData	(Ve<PtrN> _names, Ve<void*> _pointers);

	size_t		Width;
	size_t		Height;

	double		scaleX;
	double		offsetX;

	double		lower_bound_x;
	double		upper_bound_x;
	double		lower_bound_r;
	double		upper_bound_r;

	ranger3calibration::RectificationMethod			rm;
	bool		doRectify;

private:
	std::map<PtrN, void *>		m_Data;

};


/** Interface for calling calibration functions	*/
class CalibrationWrapper final
{
public:
	EXPORT_DLL CalibrationWrapper();
	EXPORT_DLL ~CalibrationWrapper();

	EXPORT_DLL bool initCalibrationFilter(
		const std::string filePath, const int bufferWidth, const int bufferHeight,
		const int aoiHeight, const float scaleZ, const float originZ,
		const RectMethod method, const size_t rectificationWidth = 2560,
		const float missingDataValue = NAN, const bool _doRectify = true);

	EXPORT_DLL bool calibrate(uint8_t * range, uint8_t * reflectance, bool doRectify = true);
	EXPORT_DLL std::shared_ptr<calibratedData> getData	() {	return m_pData;	};
	
	// todo : 1 add a API to pass RangeX
	EXPORT_DLL float* XXXX() { return m_pCalibrationData->rangeX.get(); };


private:

	std::unique_ptr<ranger3calibration::CalibrationFilter>		m_pCalFilter;
	std::unique_ptr<ranger3calibration::RectificationFilter>	m_pRectFilter;
	std::unique_ptr<ranger3calibration::CalibrationData>		m_pCalibrationData;
	std::unique_ptr<ranger3calibration::RectificationData>		m_pRectificationData;
	std::unique_ptr<ranger3calibration::SensorTraits>			m_pSensorTraits;
	std::unique_ptr<ranger3calibration::CalibrationParameters>	m_params; //! < Calibration Parameters
	std::shared_ptr<calibratedData>								m_pData;

};



#ifdef __cplusplus
	};
}
#endif

typedef SiCaliWrapper::PtrN PN;
