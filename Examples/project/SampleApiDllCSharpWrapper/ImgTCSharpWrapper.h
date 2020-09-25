/**	@file	SampleApiDllCSharpWrapper.h
*
*	@brief	C# Wrapper Class of Ranger3.
*
*	@attention
*	This is the C# API wrapper of C++ Class Ranger3 and Ranger3Shared in ranger3.cpp|h.
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/


#pragma once

#include "CommonFunctions.h"
#include <Ranger3.h>
#include <cliext\list>
#include <cliext\map>
#include <vcclr.h>

#include <string>
#include <sstream>

#include <windows.h>

using namespace System;

namespace SICK3D
{

////////////////////////////////////////////////////////////////////////////////

/**
* @brief ���ͼ������͡� \n\n
* Names of output Datas.
*/
public enum class DataNames
{
	INV=0,			
	SEN,			
	RAN,			
	REF,			
	SCA,			
	RAN_CAL,		
	REF_CAL,		
	REF_CAL_F,		
	SCA_CAL,		
	RAN2,			
	REF2,			
	SCA2,			
	RAN2_CAL,		
	REF2_CAL,		
	SCA2_CAL		
};
typedef DataNames DN;

/**
* @brief ���ͼ�� \n\n
* Class of output images.
*/
public ref class CSImgT
{
public:
	CSImgT();
	~CSImgT();
	CSImgT(const CSImgT^ img);

	CSImgT^ operator = (const CSImgT^ img);
	bool operator == (const CSImgT^ img);
	bool operator != (const CSImgT^ img);



	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief �����ݵ�ö����ת��Ϊ��Ӧ���ַ�����\n\n
	* Convert DataNames to its corresponding string name.
	*
	* @param [in] _dn   ���ݵ�ö����   DataNames
	*
	* @par ����ö�����Ͷ�Ӧ�ַ����Ķ�Ӧ��    The list of enum DataNames and corresponding string name.
	* @verbatim
	{ DN::INV		, "INVALID" },
	{ DN::SEN		, "SENSOR" },
	{ DN::RAN		, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1"			},
	{ DN::REF		, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1"		},
	{ DN::SCA		, "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1"			},
	{ DN::RAN_CAL	, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL"		},
	{ DN::REF_CAL	, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL"	},
	{ DN::REF_CAL_F	, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL_F"},
	{ DN::SCA_CAL	, "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL"		},
	{ DN::RAN2		, "ComponentSelector_Range_RegionSelector_Scan3dExtraction2"			},
	{ DN::REF2		, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2"		},
	{ DN::SCA2		, "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2"			},
	{ DN::RAN2_CAL	, "ComponentSelector_Range_RegionSelector_Scan3dExtraction2_CAL"		},
	{ DN::REF2_CAL	, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2_CAL"	},
	{ DN::SCA2_CAL	, "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL"		}
	@endverbatim
	*/
	String^ DN2Str(const DN _dn);

	/**
	* @brief ����Ӧ���ַ���ת��Ϊ���ݵ�ö������\n\n
	* Convert string name to its corresponding DataNames.
	*
	* @param [in] _dn   ���ݵ�ö�����Ķ�Ӧ�ַ���   DataNames string name.
	*
	* @par ��Ӧ�ַ���������ö�����Ķ�Ӧ��    The list of string name and corresponding enum DataNames.
	* @param [in] _dn
	* @verbatim
	{ "INVALID"																, DN::INV		},
	{ "SENSOR"																, DN::SEN		},
	{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction1"			, DN::RAN		},
	{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1"		, DN::REF		},
	{ "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1"			, DN::SCA		},
	{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL"		, DN::RAN_CAL	},
	{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL"	, DN::REF_CAL	},
	{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL_F", DN::REF_CAL_F },
	{ "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL"		, DN::SCA_CAL	},
	{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction2"			, DN::RAN2		},
	{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2"		, DN::REF2		},
	{ "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2"			, DN::SCA2		},
	{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction2_CAL"		, DN::RAN2_CAL	},
	{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2_CAL"	, DN::REF2_CAL	},
	{ "ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL"		, DN::SCA2_CAL	}
	@endverbatim
	*/
	DN Str2DN(String^ _str);


	////////////////////////////////////////////////////////////////////////////////


	array<uint8_t>^		getDataByte(const DN _dn);
	array<uint16_t>^	getDataWord(const DN _dn);
	array<float>^		getDataFloat(const DN _dn);
	array<DN>^			getAvalibleDataNames();
	

	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief ���ָ����ͼ�����ݿ�ȡ�\n\n
	* Get width of given DataName.
	*/
	const int	get_cols (const DN _dn) { return static_cast<int>(mp_ImgT->get_cols(static_cast<SickCam::DN>(_dn))); }
	
	/**
	* @brief ���ָ����ͼ�����ݸ߶ȡ�\n\n
	* Get height of given DataName.
	*/
	const int	get_rows (const DN _dn) { return static_cast<int>(mp_ImgT->get_rows(static_cast<SickCam::DN>(_dn))); }


	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief ��ü�����ͼ������ݿ�ȡ�\n\n
	* Get width of sensor image.
	*/
	const int	getS_cols		()	{ return static_cast<int>(mp_ImgT->getS_cols	()); }
	
	/**
	* @brief ��ü�����ͼ������ݸ߶ȡ�\n\n
	* Get height of sensor image.
	*/
	const int	getS_rows		()	{ return static_cast<int>(mp_ImgT->getS_rows	()); }
	
	/**
	* @brief ���趨��Ĭ��Ϊ0����λ���ء�\n\n
	* To be zero always, uint is pixel.
	*/
	const int	getS_OffsetX	()	{ return static_cast<int>(mp_ImgT->getS_OffsetX	()); }
	
	/**
	* @brief ���趨��Ĭ��Ϊ0����λ���ء�\n\n
	* To be zero always, uint is pixel.
	*/
	const int	getS_OffsetY	()	{ return static_cast<int>(mp_ImgT->getS_OffsetY	()); }
	
	
	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief ��ø߶�ͼ������ݿ�ȡ�\n\n
	* Get width of range image.
	*/
	const int	getR_cols		()	{ return static_cast<int>(mp_ImgT->getR_cols		());}
	
	/**
	* @brief ��ø߶�ͼ������ݸ߶ȡ�\n\n
	* Get height of range image.
	*/
	const int	getR_rows		()	{ return static_cast<int>(mp_ImgT->getR_rows		());}
	
	/**
	* @brief ��� ROI ������ X ���ꡣ\n\n
	* Get X of ROI.
	*/
	const int	getR_AoiOffsetX	()	{ return static_cast<int>(mp_ImgT->getR_aoiOffsetX	());}
	
	/**
	* @brief ��� ROI ������ Y ���ꡣ\n\n
	* Get Y of ROI.
	*/
	const int	getR_AoiOffsetY	()	{ return static_cast<int>(mp_ImgT->getR_aoiOffsetY	());}
	
	/**
	* @brief ��� ROI �ĸ߶ȡ�\n\n
	* Get Height of ROI.
	*/
	const int	getR_AoiHeight	()	{ return static_cast<int>(mp_ImgT->getR_aoiHeight	());}
	
	/**
	* @brief ��� ROI �Ŀ�ȡ�\n\n
	* Get Width of ROI.
	*/
	const int	getR_AoiWidth	()	{ return static_cast<int>(mp_ImgT->getR_aoiWidth	());}
	
	/**
	* @brief ��� R �᷽��\n\n
	* Get Axis of Range.
	*/
	const bool	getR_RangeAxis	()	{ return RA::STANDARD == mp_ImgT->getR_RangeAxis();}

	
	////////////////////////////////////////////////////////////////////////////////
	
	/**
	* @brief ��ñ궨��ͼ������ݿ�ȡ�\n\n
	* Get width of calibrated image.
	*/
	const int	 getC_cols			()	{ return static_cast<int>	(mp_ImgT->getC_cols());}
	
	/**
	* @brief ��ñ궨��ͼ������ݸ߶ȡ�\n\n
	* Get height of calibrated image.
	*/
	const int	 getC_rows			()	{ return static_cast<int>	(mp_ImgT->getC_rows());}
	
	/**
	* @brief ��ñ궨��ͼ��� X ��ʼֵ����λ���ס� \n\n
	* Get the offset in X axis of calibrated image, uint is millimeter.
	* @note ���� X_mm = scaleX * X_pixel + offsetX�� \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX.
	*/
	const double getC_offsetX		()	{ return static_cast<double>(mp_ImgT->getC_offsetX		());}
	
	/**
	* @brief ��ñ궨��ͼ��� Y ��ʼֵ����λ���ס� \n\n
	* Get the offset in Y axis of calibrated image, uint is millimeter.
	* @note ���� Y_mm = scaleY * Y_pixel + offsetY�� \n   Y of cloud point is Y_mm = scaleY * Y_pixel + offsetY.
	*/
	const double getC_offsetY		()	{ return static_cast<double>(mp_ImgT->getC_offsetY		());}
	
	/**
	* @brief ��ñ궨��ͼ��� X �ֱ��ʣ���λ ����/���ء� \n\n
	* Get the scale in X axis of calibrated image, uint is millimeter/pixel.
	* @note ���� X_mm = scaleX * X_pixel + offsetX�� \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX.
	*/
	const double getC_scaleX		()	{ return static_cast<double>(mp_ImgT->getC_scaleX		());}
	
	/**
	* @brief ��ñ궨��ͼ��� Y �ֱ��ʣ���λ ����/���ء����û����롣 \n\n
	* Get the scale in Y axis of calibrated image, uint is millimeter/pixel. Input by user.
	* @note ���� Y_mm = scaleY * Y_pixel + offsetY�� \n   Y of cloud point is Y_mm = scaleY * Y_pixel + offsetY.
	*/
	const double getC_scaleY		()	{ return static_cast<double>(mp_ImgT->getC_scaleY		());}
	
	/**
	* @brief ��ñ궨��ͼ����ɵ��ƺ� X ����Сֵ����λ ���ס��� getC_offsetX() ��ȡ� \n\n
	* Get the minimum X in cloud points, uint is millimeter. Equal to getC_offsetX().
	* @note ���� X_mm = scaleX * X_pixel + offsetX�� \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX.
	*/
	const double getC_lower_bound_x	()	{ return static_cast<double>(mp_ImgT->getC_lower_bound_x());}
	
	/**
	* @brief ��ñ궨��ͼ����ɵ��ƺ� X �����ֵ����λ ���ס� \n\n
	* Get the maximum X in cloud points, uint is millimeter.
	*/
	const double getC_upper_bound_x	()	{ return static_cast<double>(mp_ImgT->getC_upper_bound_x());}
	
	/**
	* @brief ��ñ궨��ͼ����ɵ��ƺ� Z ����Сֵ����λ ���ס� \n\n
	* Get the minimum Z in cloud points, uint is millimeter.
	*/
	const double getC_lower_bound_r	()	{ return static_cast<double>(mp_ImgT->getC_lower_bound_r());}
	
	/**
	* @brief ��ñ궨��ͼ����ɵ��ƺ� Z �����ֵ����λ ���ס� \n\n
	* Get the maximum Z in cloud points, uint is millimeter.
	*/
	const double getC_upper_bound_r	()	{ return static_cast<double>(mp_ImgT->getC_upper_bound_r());}


	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief 0��ʼ�����ֹͣ�����á� \n\n
	* Start from 0. Reset when camera stop.
	*/
	const int	get_ID	()				{ return static_cast<int>(mp_ImgT->get_ID()); }


	////////////////////////////////////////////////////////////////////////////////
	
	/**
	* @brief ������ָ�뷵�� false. \n\n
	* Return false if no data pointer.
	*/
	bool		isEmpty	()				{ return mp_ImgT->isEmpty(); }
	
	/**
	* @brief ��ָ�������ݷ��� false. \n\n
	* Return false if no such data pointer.
	*/
	bool		has	(const DN _name)	{ return mp_ImgT->has(static_cast<SickCam::DN>(_name)); }

	/**
	* @brief �ޱ�������Ϣ���� false. \n\n
	* Return false if no encoder information.
	*/
	bool		has_ChunkInfo()			{ return mp_ImgT->has_ChunkInfo(); }

	
	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief ���� uint8_t* ���ͣ�Byte[]�������ݡ� \n\n
	* Insert uint8_t* (Byte[] ).
	*
	* @param [in] _name ͼ�����ݵ�ö��ֵ��  DataName of inserting data.
	* @param [in] _pointer ����ָ�롣 Data pointer.
	* @param [in] _id	ͼ�����  Image id.
	*
	* @note �˺�������������ͼ�񲢷��ء�\n
	* ͼ�������Ϣ���ñ����ڵ���֮ǰ��ɡ�\n
	* ����������������ݡ� \n\n
	* It is used in passing data from device to user.\n
	* Set image information before call it.\n
	* It will allocate new memory. It is deep copy!
	*/
	bool		insertDataByteClone(const DN _name, array<uint8_t>^ _pointer, const uint64_t _id);

	/**
	* @brief ���� uint16_t* ���ͣ�int[]�������ݡ� \n\n
	* Insert uint16_t* (int[] ).
	*
	* @param [in] _name ͼ�����ݵ�ö��ֵ��  DataName of inserting data.
	* @param [in] _pointer ����ָ�롣 Data pointer.
	* @param [in] _id	ͼ�����  Image id.
	*
	* @note �˺�������������ͼ�񲢷��ء�\n
	* ͼ�������Ϣ���ñ����ڵ���֮ǰ��ɡ�\n
	* ����������������ݡ� \n\n
	* It is used in passing data from device to user.\n
	* Set image information before call it.\n
	* It will allocate new memory. It is deep copy!
	*/
	bool		insertDataWordClone(const DN _name, array<uint16_t>^ _pointer, const uint64_t _id);

	/**
	* @brief ���� float* ���ͣ�float[]�������ݡ� \n\n
	* Insert float* (float[] ).
	*
	* @param [in] _name ͼ�����ݵ�ö��ֵ��  DataName of inserting data.
	* @param [in] _pointer ����ָ�롣 Data pointer.
	* @param [in] _id	ͼ�����  Image id.
	*
	* @note �˺�������������ͼ�񲢷��ء�\n
	* ͼ�������Ϣ���ñ����ڵ���֮ǰ��ɡ�\n
	* ����������������ݡ� \n\n
	* It is used in passing data from device to user.\n
	* Set image information before call it.\n
	* It will allocate new memory. It is deep copy!
	*/
	bool		insertDataFloatClone(const DN _name, array<float>^ _pointer, const uint64_t _id);

	////////////////////////////////////////////////////////////////////////////////
	
	
	bool		setSensorInfo(const int _c, const int _r, const int _ox, const int _oy)
											   { return mp_ImgT->setSensorInfo(
																		 static_cast<int>(_c ), 
																		 static_cast<int>(_r ), 
																		 static_cast<int>(_ox), 
																		 static_cast<int>(_oy));}
	void		setSen_cols		(const int _v) { mp_ImgT->setS_cols	(static_cast<int>(_v ));}
	void		setSen_rows		(const int _v) { mp_ImgT->setS_rows	(static_cast<int>(_v ));}
	void		setSen_OffsetX	(const int _v) { mp_ImgT->setS_OffsetX(static_cast<int>(_v ));}
	void		setSen_OffsetY	(const int _v) { mp_ImgT->setS_OffsetY(static_cast<int>(_v ));}


	////////////////////////////////////////////////////////////////////////////////
	
	
	bool		setRangeInfo	(	const int _c,
									const int _r,
									const int _aox,
									const int _aoy,
									const int _ah,
									const int _aw,
									const bool  _rAxis)
									{ return mp_ImgT->setRangeInfo(
																	static_cast<size_t>	(_c  ),
																	static_cast<size_t>	(_r	 ),
																	static_cast<int64_t>(_aox),
																	static_cast<int64_t>(_aoy),
																	static_cast<int64_t>(_ah ),
																	static_cast<int64_t>(_aw ),	
																	_rAxis == true ? RA::STANDARD : RA::REVERSED);}
	void		setRan_cols			(const int  _v) { mp_ImgT->setR_cols		(static_cast<size_t>(_v));}
	void		setRan_rows			(const int  _v) { mp_ImgT->setR_rows		(static_cast<size_t>(_v));}
	void		setRan_aoiOffsetX	(const int  _v) { mp_ImgT->setR_aoiOffsetX(static_cast<int64_t>(_v));}
	void		setRan_aoiOffsetY	(const int  _v) { mp_ImgT->setR_aoiOffsetY(static_cast<int64_t>(_v));}
	void		setRan_aoiHeight	(const int  _v) { mp_ImgT->setR_aoiHeight	(static_cast<int64_t>(_v));}
	void		setRan_aoiWidth		(const int  _v) { mp_ImgT->setR_aoiWidth	(static_cast<int64_t>(_v));}
	void		setRan_RangeAxis	(const bool _v) { mp_ImgT->setR_RangeAxis	(_v==true?RA::STANDARD:RA::REVERSED);}

	
	////////////////////////////////////////////////////////////////////////////////
	
	
	bool		setCaliInfo(
							const int _c,
							const int _r,
							const double _offsetX,
							const double _offsetY,
							const double _scaleX,
							const double _scaleY,
							const double _lower_bound_x,
							const double _upper_bound_x,
							const double _lower_bound_r,
							const double _upper_bound_r)
							{ return mp_ImgT->setCaliInfo(	static_cast<size_t>	(_c),
															static_cast<size_t>	(_r),
															_offsetX,
															_offsetY,
															_scaleX,
															_scaleY,
															_lower_bound_x,
															_upper_bound_x,
															_lower_bound_r,
															_upper_bound_r);}

	void		setCali_cols			(const int _v)	  { mp_ImgT->setC_cols		  (_v);}
	void		setCali_rows			(const int _v)	  { mp_ImgT->setC_rows		  (_v);}
	void		setCali_offsetX			(const double _v) { mp_ImgT->setC_offsetX	  (_v);}
	void		setCali_offsetY			(const double _v) { mp_ImgT->setC_offsetY	  (_v);}
	void		setCali_scaleX			(const double _v) { mp_ImgT->setC_scaleX		  (_v);}
	void		setCali_scaleY			(const double _v) { mp_ImgT->setC_scaleY		  (_v);}
	void		setCali_lower_bound_x	(const double _v) { mp_ImgT->setC_lower_bound_x(_v);}
	void		setCali_upper_bound_x	(const double _v) { mp_ImgT->setC_upper_bound_x(_v);}
	void		setCali_lower_bound_r	(const double _v) { mp_ImgT->setC_lower_bound_r(_v);}
	void		setCali_upper_bound_r	(const double _v) { mp_ImgT->setC_upper_bound_r(_v);}
	
	
	////////////////////////////////////////////////////////////////////////////////


	String^		collectInfo	() { return gcnew String(string2wstring(mp_ImgT->collectInfo()).c_str()); }
	

	////////////////////////////////////////////////////////////////////////////////
	
	/**
	* @brief ��ȡ ICON ͼ���ļ��� \n\n
	* load image data from Icon file.
	*
	* @param [in] path	������չ����  Please do not provide the suffix;
	* @note				�ɰ汾�� Reflectance �ᱻ����Ϊ <uint8_t*> ���͵����ݡ�\n\n
	*					If Reflectance data format is < float* >, it will be converted to < uint8_t* >.
	*/
	bool		loadFromIconFile(const String^ path);
	
	/**
	* @brief ����Ϊ ICON ��ʽ��ͼ�����ݡ� \n\n
	* Save data as SICK icon format.
	*
	* @verbatim
	�� icon ��ʽ�У�ͼ�񱻷�Ϊ��ͬ��ͼ���顣���磺
	Group 1 "Sensor_____Data" : SEN
	Group 2 "Range______Data" : RAN		+ @REF		+ @SCA
	Group 3 "Calibrated_Data" : RAN_CAL	+ @REF_CAL	+ @SCA_CAL

	__ ���� "@" ��ʾ����һ�����ڣ����ھͻᱻ���档
	ÿ�� xml|dat �ļ�ֻ�ܱ�������һ�� group �����ݡ�

	���� ImgT ֧�ִ洢����һ�� group ��ͼ�����ݡ���˵����Ҫ����һ�� ImgT �е��������ݣ���������Ҫ��ֹһ�� xml|dat ���ļ���
	@endverbatim
	*
	* @verbatim
	In icon format, images are seperated into different groups. For instance:
	Group 1 "Sensor_____Data" : SEN
	Group 2 "Range______Data" : RAN		+ @REF		+ @SCA
	Group 3 "Calibrated_Data" : RAN_CAL	+ @REF_CAL	+ @SCA_CAL

	__ Sign "@" means if it exists it will be saved as well.
	Each xml|dat file can only save ONE group.

	While, ImgT can hold more than one image group. It means there might need more than
	one xml|dat to save entire ImgT data.
	@endverbatim
	*
	* @param [in] path			·����  Path;
	* @param [in] whichOne		ѡ�񱣴����͡����÷�ʽ���±�˵����   Saving type. See note for details.
	*
	* @note
	*
	* "whichOne" Options | ����
	* ------ | --------
	* INV | �������е����ݡ����� sensor ���ݣ��������ļ�������ȫΪ��${path} + "_SEN"������ range ���ݣ��������ļ�������ȫΪ��${path}������ calibrated ���ݣ��������ļ�������ȫΪ��${path} + "_CAL"��
	* SEN | ������ sensor ���ݡ��������ļ�����������ͬ��
	* RAN_CAL | ������ calibrated ���ݡ� �������ļ�����������ͬ��
	*
	*
	* "whichOne" Options | Meanings
	* ------ | --------
	* INV | Saving all, for sensor data the file name will be ${path} + "_SEN", for range data the file name will be ${path}, for calibrated data the file name will be ${path} + "_CAL".
	* SEN | Saving sensor only, path is unchanged.
	* RAN_CAL | Saving calibrated data only, path is unchanged.
	*
	*/
	bool		SaveToIconFile(const String^ path, DN whichOne);
	
	/**
	* @brief ����Ϊ txt �ĵ��Ƹ�ʽ�� \n\n
	* Save data as pointCloud format, txt.
	*
	* @verbatim
	// data example:
	xValue1, yValue1, zValue1,
	xValue2, yValue2, zValue2,
	...
	@endverbatim
	*
	* @note �ò�����֧�ֱ궨������ݡ� \n\n
	* Calibrated Data only!
	*/
	bool		SaveCalibratedDataToPointCloud(const String^ path);


public:
	bool setData(const SickCam::ImgT & p);
	void _deepCopy(const CSImgT^ img);

private:
	SickCam::ImgT * mp_ImgT;
};


}
