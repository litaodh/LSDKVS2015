/**	@file	ImageTable.h
*
*	@brief	Class ImgT Head File.
*
*	@attention
*	All supported image date formats are listed in DataNames.
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/

#pragma once

#include "Typedef.h"
#include "tinyxml2.h"

namespace SickCam 
{

////////////////////////////////////////////////////////////////////////////////


/**
* @brief 输出图像的类型。 \n\n
* Names of output Datas. 
*/
typedef enum DataNames
{
	INV=0,			///< INVALID	*	| INVALID		| 非法图像 | INVALID
	SEN,			///< uint8_t	*	| [---]			| 激光线图像| SENSOR
	RAN,			///< uint16_t	*	| [Raw]			| 高度图像| ComponentSelector_Range_______RegionSelector_Scan3dExtraction1
	REF,			///< uint8_t	*	| [Raw]			| 激光强度图像 | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1
	SCA,			///< uint8_t	*	| [Raw]			| 激光置信值图像 | ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1
	RAN_CAL,		///< float		*	| [Calibrated]	| 标定后的高度图像 | ComponentSelector_Range_______RegionSelector_Scan3dExtraction1
	REF_CAL,		///< uint8_t	*	| [Calibrated]	| 标定后的激光强度图像 | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1
	REF_CAL_F,		///< float		*	| [Calibrated]	| 旧版本的标定后的激光强度图像 | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1
	SCA_CAL,		///< not yet.		| [Calibrated]	| 标定后的激光置信值图像 | ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1
	RAN2,			///< uint16_t	*	| [Raw]			| （胶水）高度图像 | ComponentSelector_Range_______RegionSelector_Scan3dExtraction2 
	REF2,			///< uint8_t	*	| [Raw]			| （胶水）激光强度图像 | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2
	SCA2,			///< uint8_t	*	| [Raw]			| （胶水）激光置信值图像 | ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2
	RAN2_CAL,		///< float		*	| [Calibrated]	| （胶水）标定后的高度图像 | ComponentSelector_Range_______RegionSelector_Scan3dExtraction2
	REF2_CAL,		///< float		*	| [Calibrated]	| （胶水）标定后的激光强度图像 | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2
	SCA2_CAL		///< not yet.		| [Calibrated]	| （胶水）标定后的激光置信值图像 | ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2
} DN;
typedef std::map<DN, void *>		ImgTData;

struct EXPORT_TO_DLL SensoInfo
{
	SensoInfo()
		: cols(0), rows(0), senOffsetX(0), senOffsetY(0)
	{}

	bool operator == (const SensoInfo& sen)
	{
		return this->cols == sen.cols
			&& this->rows == sen.rows
			&& this->senOffsetX == sen.senOffsetX
			&& this->senOffsetY == sen.senOffsetY;
	}

	size_t		cols;				///< Width of sensor image. 
	size_t		rows;				///< Height of sensor image. 
	int64_t		senOffsetX;			///< The default is zero. 
	int64_t		senOffsetY;			///< The default is zero.
};
struct EXPORT_TO_DLL RangeInfo
{
	RangeInfo()
		: cols(0), rows(0), aoiOffsetX(0), aoiOffsetY(0), aoiHeight(0), aoiWidth(0), m_RangeAxis(RA::STANDARD)
	{}

	bool operator == (const RangeInfo& v)
	{
		return this->cols == v.cols
			&& this->rows == v.rows
			&& this->aoiOffsetX == v.aoiOffsetX
			&& this->aoiOffsetY == v.aoiOffsetY
			&& this->aoiHeight == v.aoiHeight
			&& this->aoiWidth == v.aoiWidth
			&& this->m_RangeAxis == v.m_RangeAxis;
	}

	size_t		cols;				///< Width of range image. 
	size_t		rows;				///< Height of range image.
	int64_t		aoiOffsetX;			///<
	int64_t		aoiOffsetY;			///<
	int64_t		aoiHeight;			///<
	int64_t		aoiWidth;			///<
	RA			m_RangeAxis;		///< Z direction.
};
struct EXPORT_TO_DLL CalibInfo
{
	CalibInfo()
		: cols(0), rows(0), offsetX(0.0), offsetY(0.0), scaleX(1.0), scaleY(1.0),
		lower_bound_x(0.0), upper_bound_x(0.0), lower_bound_r(0.0), upper_bound_r(0.0)
	{}

	bool operator == (const CalibInfo& v)
	{
		return this->cols == v.cols
			&& this->rows == v.rows
			&& this->offsetX	== v.offsetX	
			&& this->offsetY	== v.offsetY	
			&& this->scaleX		== v.scaleX	
			&& this->scaleY		== v.scaleY	
			&& this->lower_bound_x == v.lower_bound_x
			&& this->upper_bound_x == v.upper_bound_x
			&& this->lower_bound_r == v.lower_bound_r
			&& this->upper_bound_r == v.upper_bound_r
			;
	}

	size_t		cols;				///< Width of range image. 
	size_t		rows;				///< Height of range image.
	double		offsetX			;	///< Unit millimeter, minimum x in FOV
	double		offsetY			;	///< Unit millimeter, minimum y in FOV
	double		scaleX			;	///< Unit millimeter/pixel, x resolution
	double		scaleY			;	///< Unit millimeter/pixel, y resolution
	double		lower_bound_x;		///< Unit millimeter, minimum x in FOV
	double		upper_bound_x;		///< Unit millimeter, maximum x in FOV
	double		lower_bound_r;		///< Unit millimeter, minimum r in FOV, when set lazer vertically, same as z
	double		upper_bound_r;		///< Unit millimeter, maximum r in FOV, when set lazer vertically, same as z

};

/** struct of output images info. */
struct EXPORT_TO_DLL ImgInfo
{
	ImgInfo()
		: m_id(0)
	{}
	bool operator == (const ImgInfo& v)
	{
		return this->m_RI == v.m_RI
			&& this->m_SI == v.m_SI
			&& this->m_CI == v.m_CI
			&& this->m_ChunkData == v.m_ChunkData
			&& this->m_id == v.m_id
			;
	}

	RangeInfo	m_RI;				///< Range Information
	SensoInfo	m_SI;				///< Sensor Information
	CalibInfo	m_CI;				///< Calibration Information
	ChunkData	m_ChunkData;		///< 编码器信息。ChunkInfo
	uint64_t	m_id;				///< Image ID
};


/**
* @brief 输出图像 \n\n
* Class of output images.
*/
class EXPORT_TO_DLL ImgT
{
public:
	ImgT();
	ImgT(const ImgT& img);
	ImgT& operator = (const ImgT& img);
	bool operator == (const ImgT& img);

	~ImgT();


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
	static cStr ImgT::DN2Str(const DN & _dn);


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
	static DN ImgT::Str2DN(cStr & _dn);
	

	/** convert DataNames to its corresponding data type. 
	* @param [in] _dn DataNames
	* @return Type string name
	* - "UINT8_T"
	* - "UINT16_T"
	* - "FLOAT"
	*/
	static cStr ImgT::PtrType(const DN & _dn);
	

	////////////////////////////////////////////////////////////////////////////////
	

	/** 
	* @brief 根据给定的数据枚举名，获得数据的指针 \n\n
	* Get data pointer according to DN. 
	*
	* @return
	* - uint8_t *   激光线图像、激光强度图像、激光置信值图像、标定后的激光强度图像、标定后的激光置信值图像 \n
	*				Sensor, Reflectance, Scatter, Calibrated Reflectance, Calibrated Scatter.\n
	*				DN::SEN, DN::REF, DN::SCA, DN::REF_CAL, DN::SCA_CAL.
	* - uint16_t *  高度图象。\n  Range.\n  DN::RAN
 	* - float *		标定后的高度图像、旧版本的标定后的激光强度图像。  \n
	*				Calibrated Range, Old version Calibrated Reflectance \n
	*				DN::RAN_CAL, DN::REF_CAL_F, 
  	*/
	void *		getData					(const DN &  _dn)	const;

	/** 
	* @brief 获得所有的数据指针。 \n\n
	* Get all data pointers. 
	*
	* @return
	* - std::map<DN, void *>
	*/
	const ImgTData &	getData			()	const	{	return m_Data;					}
	
	/**
	* @brief 获得当前所有有效数据的枚举名列表。\n\n
	* Get all avaliable DataNames.
	*/
	Vec<DN>		getAvalibleDataNames	()	const;
	
	/**
	* @brief 获得指定的图像数据宽度。\n\n
	* Get width of given DataName.
	*/
	const size_t	& get_cols			(const DN &  _dn)	const;
	
	/**
	* @brief 获得指定的图像数据高度。\n\n
	* Get height of given DataName.
	*/
	const size_t	& get_rows			(const DN &  _dn)	const;


	////////////////////////////////////////////////////////////////////////////////


	/**
	* @brief 获得激光线图像的数据宽度。\n\n
	* Get width of sensor image.
	*/
	const size_t	& getS_cols			()	const	{	return m_info.m_SI.cols;		}
	
	/**
	* @brief 获得激光线图像的数据高度。\n\n
	* Get height of sensor image.
	*/
	const size_t	& getS_rows			()	const	{	return m_info.m_SI.rows;		}
	
	/**
	* @brief 不设定，默认为0，单位像素。\n\n
	* To be zero always, uint is pixel.
	*/
	const int64_t	& getS_OffsetX		()	const	{	return m_info.m_SI.senOffsetX;	}
	
	/**
	* @brief 不设定，默认为0，单位像素。\n\n
	* To be zero always, uint is pixel.
	*/
	const int64_t	& getS_OffsetY		()	const	{	return m_info.m_SI.senOffsetY;	}
	
	
	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief 获得高度图像的数据宽度。\n\n
	* Get width of range image.
	*/
	const size_t	& getR_cols			()	const	{	return m_info.m_RI.cols;		}
	
	/**
	* @brief 获得高度图像的数据高度。\n\n
	* Get height of range image.
	*/
	const size_t	& getR_rows			()	const	{	return m_info.m_RI.rows;		}
	
	/**
	* @brief 获得 ROI 的左上 X 坐标。\n\n
	* Get X of ROI.
	*/
	const int64_t	& getR_aoiOffsetX	()	const	{	return m_info.m_RI.aoiOffsetX;	}
	
	/**
	* @brief 获得 ROI 的左上 Y 坐标。\n\n
	* Get Y of ROI.
	*/
	const int64_t	& getR_aoiOffsetY	()	const	{	return m_info.m_RI.aoiOffsetY;	}
	
	/**
	* @brief 获得 ROI 的高度。\n\n
	* Get Height of ROI.
	*/
	const int64_t	& getR_aoiHeight	()	const	{	return m_info.m_RI.aoiHeight;	}
	
	/**
	* @brief 获得 ROI 的宽度。\n\n
	* Get Width of ROI.
	*/
	const int64_t	& getR_aoiWidth		()	const	{	return m_info.m_RI.aoiWidth;	}
	
	/**
	* @brief 获得 R 轴方向。\n\n
	* Get Axis of Range.
	*/
	const RA		& getR_RangeAxis	()	const	{	return m_info.m_RI.m_RangeAxis;	}

	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief 获得标定后图像的数据宽度。\n\n
	* Get width of calibrated image.
	*/
	const size_t & getC_cols			()	const	{	return m_info.m_CI.cols;	}
	
	/**
	* @brief 获得标定后图像的数据高度。\n\n
	* Get height of calibrated image.
	*/
	const size_t & getC_rows			()	const	{	return m_info.m_CI.rows;	}
	
	/**
	* @brief 获得标定后图像的 X 起始值，单位毫米。 \n\n
	* Get the offset in X axis of calibrated image, uint is millimeter.
	* @note 点云 X_mm = scaleX * X_pixel + offsetX。 \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX. 
	*/
	const double & getC_offsetX			()	const	{	return m_info.m_CI.offsetX;	} 
	
	/**
	* @brief 获得标定后图像的 Y 起始值，单位毫米。 \n\n
	* Get the offset in Y axis of calibrated image, uint is millimeter.
	* @note 点云 Y_mm = scaleY * Y_pixel + offsetY。 \n   Y of cloud point is Y_mm = scaleY * Y_pixel + offsetY.
	*/
	const double & getC_offsetY			()	const	{	return m_info.m_CI.offsetY;	} 
	
	/**
	* @brief 获得标定后图像的 X 分辨率，单位 毫米/像素。 \n\n
	* Get the scale in X axis of calibrated image, uint is millimeter/pixel.
	* @note 点云 X_mm = scaleX * X_pixel + offsetX。 \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX.
	*/
	const double & getC_scaleX			()	const	{	return m_info.m_CI.scaleX;	} 
	
	/**
	* @brief 获得标定后图像的 Y 分辨率，单位 毫米/像素。由用户输入。 \n\n
	* Get the scale in Y axis of calibrated image, uint is millimeter/pixel. Input by user.
	* @note 点云 Y_mm = scaleY * Y_pixel + offsetY。 \n   Y of cloud point is Y_mm = scaleY * Y_pixel + offsetY.
	*/
	const double & getC_scaleY			()	const	{	return m_info.m_CI.scaleY;	} 
	
	/**
	* @brief 获得标定后图像换算成点云后， X 的最小值，单位 毫米。和 getC_offsetX() 相等。 \n\n
	* Get the minimum X in cloud points, uint is millimeter. Equal to getC_offsetX().
	* @note 点云 X_mm = scaleX * X_pixel + offsetX。 \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX.
	*/
	const double & getC_lower_bound_x	()	const	{	return m_info.m_CI.lower_bound_x;} 
	
	/**
	* @brief 获得标定后图像换算成点云后， X 的最大值，单位 毫米。 \n\n
	* Get the maximum X in cloud points, uint is millimeter.
	*/
	const double & getC_upper_bound_x	()	const	{	return m_info.m_CI.upper_bound_x;} 
	
	/**
	* @brief 获得标定后图像换算成点云后， Z 的最小值，单位 毫米。 \n\n
	* Get the minimum Z in cloud points, uint is millimeter. 
	*/
	const double & getC_lower_bound_r	()	const	{	return m_info.m_CI.lower_bound_r;} 
	
	/**
	* @brief 获得标定后图像换算成点云后， Z 的最大值，单位 毫米。 \n\n
	* Get the maximum Z in cloud points, uint is millimeter.
	*/
	const double & getC_upper_bound_r	()	const	{	return m_info.m_CI.upper_bound_r;} 

	////////////////////////////////////////////////////////////////////////////////
	
	/**
	* @brief 获得每个剖面的编码器信息。 \n\n
	* Get encoder information of each profile.
	*/
	const ChunkData	& get_ChunkInfo		()	const	{	return m_info.m_ChunkData;	}
	
	/**
	* @brief 0初始。相机停止后重置。 \n\n
	* Start from 0. Reset when camera stop.
	*/
	const uint64_t	& get_ID			()	const	{	return m_info.m_id;			}
	
	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief 无数据指针返回 false. \n\n
	* Return false if no data pointer.
	*/
	bool	isEmpty				()	const	{	return m_Data.empty();				}
	
	/**
	* @brief 无指定的数据返回 false. \n\n
	* Return false if no such data pointer.
	*/
	bool	has	(const DN _name)	const	{	return (m_Data.count(_name) == 1);	}
	
	/**
	* @brief 无编码器信息返回 false. \n\n
	* Return false if no encoder information.
	*/
	bool	has_ChunkInfo		()	const	{	return !m_info.m_ChunkData.empty();	}


	////////////////////////////////////////////////////////////////////////////////


	/**
	* @brief 插入数据。数据指针均按照 uint8_t* 的形式保存。类型不同请事先执行强制转化。nullptr被忽略。 \n\n
	* Any data pointers type need to be uint8_t*, nullptr will be ignored.
	*
	* @param [in] _name 图像数据的枚举值。  DataName of inserting data.
	* @param [in] _pointer 数据指针。 Data pointer.
	* @param [in] _id	图像序号  Image id.
	*
	* @note 此函数用于相机获得图像并返回。\n
	* 图像基本信息设置必须在调用之前完成。\n
	* 深拷贝。复制所有数据。 \n\n
	* 此函数中申请的内存再析构函数中释放。 \n\n
	* It is used in passing data from device to user.\n
	* Set image information before call it.\n
	* It will allocate new memory. It is deep copy!
	* The memory will be deleted when disconstructing ImgT!
	*/
	bool	insertDataCopy	(	const DN		& _name, 
								const uint8_t	* _pointer, 
								const uint64_t	& _id);

	////////////////////////////////////////////////////////////////////////////////

	bool	setSensorInfo	(	const size_t  & _c, 
								const size_t  & _r, 
								const int64_t & _ox, 
								const int64_t & _oy);

	bool	setS_cols		(	const size_t  & _v)		{	m_info.m_SI.cols		= _v; return _v == getS_cols	();}
	bool	setS_rows		(	const size_t  & _v)		{	m_info.m_SI.rows		= _v; return _v == getS_rows	();}
	bool	setS_OffsetX	(	const size_t  & _v)		{	m_info.m_SI.senOffsetX	= _v; return _v == getS_OffsetX();}
	bool	setS_OffsetY	(	const size_t  & _v)		{	m_info.m_SI.senOffsetY	= _v; return _v == getS_OffsetY();}

	// ----------------------------------------------------------------------------
	// Set Range Info

	bool	setRangeInfo	(	const size_t  & _c, 
								const size_t  & _r, 
								const int64_t & _aox, 
								const int64_t & _aoy, 
								const int64_t & _ah, 
								const int64_t & _aw, 
								const RA      & _rAxis);

	bool	setR_cols		(	const size_t  & _v)		{	m_info.m_RI.cols		= _v; return _v == getR_cols		();}
	bool	setR_rows		(	const size_t  & _v)		{	m_info.m_RI.rows		= _v; return _v == getR_rows		();}
	bool	setR_aoiOffsetX	(	const int64_t & _v)		{	m_info.m_RI.aoiOffsetX	= _v; return _v == getR_aoiOffsetX	();}
	bool	setR_aoiOffsetY	(	const int64_t & _v)		{	m_info.m_RI.aoiOffsetY	= _v; return _v == getR_aoiOffsetY	();}
	bool	setR_aoiHeight	(	const int64_t & _v)		{	m_info.m_RI.aoiHeight	= _v; return _v == getR_aoiHeight	();}
	bool	setR_aoiWidth	(	const int64_t & _v)		{	m_info.m_RI.aoiWidth	= _v; return _v == getR_aoiWidth	();}
	bool	setR_RangeAxis	(	const RA	  & _v)		{	m_info.m_RI.m_RangeAxis	= _v; return _v == getR_RangeAxis	();}

	// ----------------------------------------------------------------------------
	// Set Calibration Info
	bool	setCaliInfo		(	const size_t & _c,
								const size_t & _r,
								const double & _offsetX,
								const double & _offsetY,
								const double & _scaleX,
								const double & _scaleY,
								const double & _lower_bound_x,
								const double & _upper_bound_x,
								const double & _lower_bound_r,
								const double & _upper_bound_r);

	bool	setC_cols		  (	const size_t & _v)	{	m_info.m_CI.cols			= _v; return _v == getC_cols	();	}
	bool	setC_rows		  (	const size_t & _v)	{	m_info.m_CI.rows			= _v; return _v == getC_rows	();	}
	bool	setC_offsetX	  (	const double & _v)	{	m_info.m_CI.offsetX			= _v; return _v == getC_offsetX	();	} 
	bool	setC_offsetY	  (	const double & _v)	{	m_info.m_CI.offsetY			= _v; return _v == getC_offsetY	();	} 
	bool	setC_scaleX		  (	const double & _v)	{	m_info.m_CI.scaleX			= _v; return _v == getC_scaleX	();	} 
	bool	setC_scaleY		  (	const double & _v)	{	m_info.m_CI.scaleY			= _v; return _v == getC_scaleY	();	} 
	bool	setC_lower_bound_x(	const double & _v)	{	m_info.m_CI.lower_bound_x	= _v; return _v == getC_lower_bound_x();} 
	bool	setC_upper_bound_x(	const double & _v)	{	m_info.m_CI.upper_bound_x	= _v; return _v == getC_upper_bound_x();} 
	bool	setC_lower_bound_r(	const double & _v)	{	m_info.m_CI.lower_bound_r	= _v; return _v == getC_lower_bound_r();} 
	bool	setC_upper_bound_r(	const double & _v)	{	m_info.m_CI.upper_bound_r	= _v; return _v == getC_upper_bound_r();} 


	bool	setChunkInfo	(	const ChunkData & _Chunk);
	void	printInfo		()	const;
	Str		collectInfo		()	const;


	////////////////////////////////////////////////////////////////////////////////


	/**
	* @brief 读取 ICON 图像文件。 \n\n
	* load image data from Icon file.
	*
	* @param [in] path	无需拓展名。  Please do not provide the suffix;
	* @note				旧版本的 Reflectance 会被处理为 <uint8_t*> 类型的数据。\n\n 
	*					If Reflectance data format is < float* >, it will be converted to < uint8_t* >.
	*/
	bool	loadFromIconFile	(cStr path = "file");
	

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
	bool	SaveToIconFile		(cStr path = "file", DN whichOne=INV)	const;
	

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
	bool	SaveCalibratedDataToPointCloud	(cStr path = "file.txt")	const;
	//todo:4 x 
	float* RangX=nullptr;
	uint16_t* RangXRaw=nullptr;

protected:
	ImgT&	__doapl			(ImgT* ths, const ImgT& img);
	bool	_clearBuffer	(ImgT & img);
	bool	_compareBuffer	(const ImgT & img, const DN dn);


private:
	ImgTData	m_Data;				///< Map of all data pointers, keys are DataNames.
	ImgInfo		m_info;

public:
	const static std::map<DN, cStr> DN_StrName;
	const static std::map<cStr, DN> StrName_DN;
};


}

