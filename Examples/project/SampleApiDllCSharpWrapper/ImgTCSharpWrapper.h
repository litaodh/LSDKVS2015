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
* @brief 输出图像的类型。 \n\n
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
* @brief 输出图像 \n\n
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
	* @brief 将数据的枚举名转化为对应的字符串。\n\n
	* Convert DataNames to its corresponding string name.
	*
	* @param [in] _dn   数据的枚举名   DataNames
	*
	* @par 数据枚举名和对应字符串的对应表    The list of enum DataNames and corresponding string name.
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
	* @brief 将相应的字符串转化为数据的枚举名。\n\n
	* Convert string name to its corresponding DataNames.
	*
	* @param [in] _dn   数据的枚举名的对应字符串   DataNames string name.
	*
	* @par 对应字符串和数据枚举名的对应表    The list of string name and corresponding enum DataNames.
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
	* @brief 获得指定的图像数据宽度。\n\n
	* Get width of given DataName.
	*/
	const int	get_cols (const DN _dn) { return static_cast<int>(mp_ImgT->get_cols(static_cast<SickCam::DN>(_dn))); }
	
	/**
	* @brief 获得指定的图像数据高度。\n\n
	* Get height of given DataName.
	*/
	const int	get_rows (const DN _dn) { return static_cast<int>(mp_ImgT->get_rows(static_cast<SickCam::DN>(_dn))); }


	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief 获得激光线图像的数据宽度。\n\n
	* Get width of sensor image.
	*/
	const int	getS_cols		()	{ return static_cast<int>(mp_ImgT->getS_cols	()); }
	
	/**
	* @brief 获得激光线图像的数据高度。\n\n
	* Get height of sensor image.
	*/
	const int	getS_rows		()	{ return static_cast<int>(mp_ImgT->getS_rows	()); }
	
	/**
	* @brief 不设定，默认为0，单位像素。\n\n
	* To be zero always, uint is pixel.
	*/
	const int	getS_OffsetX	()	{ return static_cast<int>(mp_ImgT->getS_OffsetX	()); }
	
	/**
	* @brief 不设定，默认为0，单位像素。\n\n
	* To be zero always, uint is pixel.
	*/
	const int	getS_OffsetY	()	{ return static_cast<int>(mp_ImgT->getS_OffsetY	()); }
	
	
	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief 获得高度图像的数据宽度。\n\n
	* Get width of range image.
	*/
	const int	getR_cols		()	{ return static_cast<int>(mp_ImgT->getR_cols		());}
	
	/**
	* @brief 获得高度图像的数据高度。\n\n
	* Get height of range image.
	*/
	const int	getR_rows		()	{ return static_cast<int>(mp_ImgT->getR_rows		());}
	
	/**
	* @brief 获得 ROI 的左上 X 坐标。\n\n
	* Get X of ROI.
	*/
	const int	getR_AoiOffsetX	()	{ return static_cast<int>(mp_ImgT->getR_aoiOffsetX	());}
	
	/**
	* @brief 获得 ROI 的左上 Y 坐标。\n\n
	* Get Y of ROI.
	*/
	const int	getR_AoiOffsetY	()	{ return static_cast<int>(mp_ImgT->getR_aoiOffsetY	());}
	
	/**
	* @brief 获得 ROI 的高度。\n\n
	* Get Height of ROI.
	*/
	const int	getR_AoiHeight	()	{ return static_cast<int>(mp_ImgT->getR_aoiHeight	());}
	
	/**
	* @brief 获得 ROI 的宽度。\n\n
	* Get Width of ROI.
	*/
	const int	getR_AoiWidth	()	{ return static_cast<int>(mp_ImgT->getR_aoiWidth	());}
	
	/**
	* @brief 获得 R 轴方向。\n\n
	* Get Axis of Range.
	*/
	const bool	getR_RangeAxis	()	{ return RA::STANDARD == mp_ImgT->getR_RangeAxis();}

	
	////////////////////////////////////////////////////////////////////////////////
	
	/**
	* @brief 获得标定后图像的数据宽度。\n\n
	* Get width of calibrated image.
	*/
	const int	 getC_cols			()	{ return static_cast<int>	(mp_ImgT->getC_cols());}
	
	/**
	* @brief 获得标定后图像的数据高度。\n\n
	* Get height of calibrated image.
	*/
	const int	 getC_rows			()	{ return static_cast<int>	(mp_ImgT->getC_rows());}
	
	/**
	* @brief 获得标定后图像的 X 起始值，单位毫米。 \n\n
	* Get the offset in X axis of calibrated image, uint is millimeter.
	* @note 点云 X_mm = scaleX * X_pixel + offsetX。 \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX.
	*/
	const double getC_offsetX		()	{ return static_cast<double>(mp_ImgT->getC_offsetX		());}
	
	/**
	* @brief 获得标定后图像的 Y 起始值，单位毫米。 \n\n
	* Get the offset in Y axis of calibrated image, uint is millimeter.
	* @note 点云 Y_mm = scaleY * Y_pixel + offsetY。 \n   Y of cloud point is Y_mm = scaleY * Y_pixel + offsetY.
	*/
	const double getC_offsetY		()	{ return static_cast<double>(mp_ImgT->getC_offsetY		());}
	
	/**
	* @brief 获得标定后图像的 X 分辨率，单位 毫米/像素。 \n\n
	* Get the scale in X axis of calibrated image, uint is millimeter/pixel.
	* @note 点云 X_mm = scaleX * X_pixel + offsetX。 \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX.
	*/
	const double getC_scaleX		()	{ return static_cast<double>(mp_ImgT->getC_scaleX		());}
	
	/**
	* @brief 获得标定后图像的 Y 分辨率，单位 毫米/像素。由用户输入。 \n\n
	* Get the scale in Y axis of calibrated image, uint is millimeter/pixel. Input by user.
	* @note 点云 Y_mm = scaleY * Y_pixel + offsetY。 \n   Y of cloud point is Y_mm = scaleY * Y_pixel + offsetY.
	*/
	const double getC_scaleY		()	{ return static_cast<double>(mp_ImgT->getC_scaleY		());}
	
	/**
	* @brief 获得标定后图像换算成点云后， X 的最小值，单位 毫米。和 getC_offsetX() 相等。 \n\n
	* Get the minimum X in cloud points, uint is millimeter. Equal to getC_offsetX().
	* @note 点云 X_mm = scaleX * X_pixel + offsetX。 \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX.
	*/
	const double getC_lower_bound_x	()	{ return static_cast<double>(mp_ImgT->getC_lower_bound_x());}
	
	/**
	* @brief 获得标定后图像换算成点云后， X 的最大值，单位 毫米。 \n\n
	* Get the maximum X in cloud points, uint is millimeter.
	*/
	const double getC_upper_bound_x	()	{ return static_cast<double>(mp_ImgT->getC_upper_bound_x());}
	
	/**
	* @brief 获得标定后图像换算成点云后， Z 的最小值，单位 毫米。 \n\n
	* Get the minimum Z in cloud points, uint is millimeter.
	*/
	const double getC_lower_bound_r	()	{ return static_cast<double>(mp_ImgT->getC_lower_bound_r());}
	
	/**
	* @brief 获得标定后图像换算成点云后， Z 的最大值，单位 毫米。 \n\n
	* Get the maximum Z in cloud points, uint is millimeter.
	*/
	const double getC_upper_bound_r	()	{ return static_cast<double>(mp_ImgT->getC_upper_bound_r());}


	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief 0初始。相机停止后重置。 \n\n
	* Start from 0. Reset when camera stop.
	*/
	const int	get_ID	()				{ return static_cast<int>(mp_ImgT->get_ID()); }


	////////////////////////////////////////////////////////////////////////////////
	
	/**
	* @brief 无数据指针返回 false. \n\n
	* Return false if no data pointer.
	*/
	bool		isEmpty	()				{ return mp_ImgT->isEmpty(); }
	
	/**
	* @brief 无指定的数据返回 false. \n\n
	* Return false if no such data pointer.
	*/
	bool		has	(const DN _name)	{ return mp_ImgT->has(static_cast<SickCam::DN>(_name)); }

	/**
	* @brief 无编码器信息返回 false. \n\n
	* Return false if no encoder information.
	*/
	bool		has_ChunkInfo()			{ return mp_ImgT->has_ChunkInfo(); }

	
	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief 插入 uint8_t* 类型（Byte[]）的数据。 \n\n
	* Insert uint8_t* (Byte[] ).
	*
	* @param [in] _name 图像数据的枚举值。  DataName of inserting data.
	* @param [in] _pointer 数据指针。 Data pointer.
	* @param [in] _id	图像序号  Image id.
	*
	* @note 此函数用于相机获得图像并返回。\n
	* 图像基本信息设置必须在调用之前完成。\n
	* 深拷贝。复制所有数据。 \n\n
	* It is used in passing data from device to user.\n
	* Set image information before call it.\n
	* It will allocate new memory. It is deep copy!
	*/
	bool		insertDataByteClone(const DN _name, array<uint8_t>^ _pointer, const uint64_t _id);

	/**
	* @brief 插入 uint16_t* 类型（int[]）的数据。 \n\n
	* Insert uint16_t* (int[] ).
	*
	* @param [in] _name 图像数据的枚举值。  DataName of inserting data.
	* @param [in] _pointer 数据指针。 Data pointer.
	* @param [in] _id	图像序号  Image id.
	*
	* @note 此函数用于相机获得图像并返回。\n
	* 图像基本信息设置必须在调用之前完成。\n
	* 深拷贝。复制所有数据。 \n\n
	* It is used in passing data from device to user.\n
	* Set image information before call it.\n
	* It will allocate new memory. It is deep copy!
	*/
	bool		insertDataWordClone(const DN _name, array<uint16_t>^ _pointer, const uint64_t _id);

	/**
	* @brief 插入 float* 类型（float[]）的数据。 \n\n
	* Insert float* (float[] ).
	*
	* @param [in] _name 图像数据的枚举值。  DataName of inserting data.
	* @param [in] _pointer 数据指针。 Data pointer.
	* @param [in] _id	图像序号  Image id.
	*
	* @note 此函数用于相机获得图像并返回。\n
	* 图像基本信息设置必须在调用之前完成。\n
	* 深拷贝。复制所有数据。 \n\n
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
	* @brief 读取 ICON 图像文件。 \n\n
	* load image data from Icon file.
	*
	* @param [in] path	无需拓展名。  Please do not provide the suffix;
	* @note				旧版本的 Reflectance 会被处理为 <uint8_t*> 类型的数据。\n\n
	*					If Reflectance data format is < float* >, it will be converted to < uint8_t* >.
	*/
	bool		loadFromIconFile(const String^ path);
	
	/**
	* @brief 保存为 ICON 格式的图像数据。 \n\n
	* Save data as SICK icon format.
	*
	* @verbatim
	在 icon 格式中，图像被分为不同的图像组。比如：
	Group 1 "Sensor_____Data" : SEN
	Group 2 "Range______Data" : RAN		+ @REF		+ @SCA
	Group 3 "Calibrated_Data" : RAN_CAL	+ @REF_CAL	+ @SCA_CAL

	__ 符号 "@" 表示并非一定存在，存在就会被保存。
	每组 xml|dat 文件只能保存以上一个 group 的数据。

	但是 ImgT 支持存储超过一个 group 的图像数据。这说明，要保存一个 ImgT 中的完整数据，将可能需要不止一对 xml|dat 的文件。
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
	* @param [in] path			路径。  Path;
	* @param [in] whichOne		选择保存类型。设置方式见下表说明。   Saving type. See note for details.
	*
	* @note
	*
	* "whichOne" Options | 意义
	* ------ | --------
	* INV | 保存所有的数据。对于 sensor 数据，完整的文件名将补全为：${path} + "_SEN"；对于 range 数据，完整的文件名将补全为：${path}；对于 calibrated 数据，完整的文件名将补全为：${path} + "_CAL"。
	* SEN | 仅保存 sensor 数据。完整的文件名与设置相同。
	* RAN_CAL | 仅保存 calibrated 数据。 完整的文件名与设置相同。
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
	* @brief 保存为 txt 的点云格式。 \n\n
	* Save data as pointCloud format, txt.
	*
	* @verbatim
	// data example:
	xValue1, yValue1, zValue1,
	xValue2, yValue2, zValue2,
	...
	@endverbatim
	*
	* @note 该操作仅支持标定后的数据。 \n\n
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
