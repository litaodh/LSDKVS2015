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
* @brief ���ͼ������͡� \n\n
* Names of output Datas. 
*/
typedef enum DataNames
{
	INV=0,			///< INVALID	*	| INVALID		| �Ƿ�ͼ�� | INVALID
	SEN,			///< uint8_t	*	| [---]			| ������ͼ��| SENSOR
	RAN,			///< uint16_t	*	| [Raw]			| �߶�ͼ��| ComponentSelector_Range_______RegionSelector_Scan3dExtraction1
	REF,			///< uint8_t	*	| [Raw]			| ����ǿ��ͼ�� | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1
	SCA,			///< uint8_t	*	| [Raw]			| ��������ֵͼ�� | ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1
	RAN_CAL,		///< float		*	| [Calibrated]	| �궨��ĸ߶�ͼ�� | ComponentSelector_Range_______RegionSelector_Scan3dExtraction1
	REF_CAL,		///< uint8_t	*	| [Calibrated]	| �궨��ļ���ǿ��ͼ�� | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1
	REF_CAL_F,		///< float		*	| [Calibrated]	| �ɰ汾�ı궨��ļ���ǿ��ͼ�� | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1
	SCA_CAL,		///< not yet.		| [Calibrated]	| �궨��ļ�������ֵͼ�� | ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1
	RAN2,			///< uint16_t	*	| [Raw]			| ����ˮ���߶�ͼ�� | ComponentSelector_Range_______RegionSelector_Scan3dExtraction2 
	REF2,			///< uint8_t	*	| [Raw]			| ����ˮ������ǿ��ͼ�� | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2
	SCA2,			///< uint8_t	*	| [Raw]			| ����ˮ����������ֵͼ�� | ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2
	RAN2_CAL,		///< float		*	| [Calibrated]	| ����ˮ���궨��ĸ߶�ͼ�� | ComponentSelector_Range_______RegionSelector_Scan3dExtraction2
	REF2_CAL,		///< float		*	| [Calibrated]	| ����ˮ���궨��ļ���ǿ��ͼ�� | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2
	SCA2_CAL		///< not yet.		| [Calibrated]	| ����ˮ���궨��ļ�������ֵͼ�� | ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2
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
	ChunkData	m_ChunkData;		///< ��������Ϣ��ChunkInfo
	uint64_t	m_id;				///< Image ID
};


/**
* @brief ���ͼ�� \n\n
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
	static cStr ImgT::DN2Str(const DN & _dn);


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
	* @brief ���ݸ���������ö������������ݵ�ָ�� \n\n
	* Get data pointer according to DN. 
	*
	* @return
	* - uint8_t *   ������ͼ�񡢼���ǿ��ͼ�񡢼�������ֵͼ�񡢱궨��ļ���ǿ��ͼ�񡢱궨��ļ�������ֵͼ�� \n
	*				Sensor, Reflectance, Scatter, Calibrated Reflectance, Calibrated Scatter.\n
	*				DN::SEN, DN::REF, DN::SCA, DN::REF_CAL, DN::SCA_CAL.
	* - uint16_t *  �߶�ͼ��\n  Range.\n  DN::RAN
 	* - float *		�궨��ĸ߶�ͼ�񡢾ɰ汾�ı궨��ļ���ǿ��ͼ��  \n
	*				Calibrated Range, Old version Calibrated Reflectance \n
	*				DN::RAN_CAL, DN::REF_CAL_F, 
  	*/
	void *		getData					(const DN &  _dn)	const;

	/** 
	* @brief ������е�����ָ�롣 \n\n
	* Get all data pointers. 
	*
	* @return
	* - std::map<DN, void *>
	*/
	const ImgTData &	getData			()	const	{	return m_Data;					}
	
	/**
	* @brief ��õ�ǰ������Ч���ݵ�ö�����б�\n\n
	* Get all avaliable DataNames.
	*/
	Vec<DN>		getAvalibleDataNames	()	const;
	
	/**
	* @brief ���ָ����ͼ�����ݿ�ȡ�\n\n
	* Get width of given DataName.
	*/
	const size_t	& get_cols			(const DN &  _dn)	const;
	
	/**
	* @brief ���ָ����ͼ�����ݸ߶ȡ�\n\n
	* Get height of given DataName.
	*/
	const size_t	& get_rows			(const DN &  _dn)	const;


	////////////////////////////////////////////////////////////////////////////////


	/**
	* @brief ��ü�����ͼ������ݿ�ȡ�\n\n
	* Get width of sensor image.
	*/
	const size_t	& getS_cols			()	const	{	return m_info.m_SI.cols;		}
	
	/**
	* @brief ��ü�����ͼ������ݸ߶ȡ�\n\n
	* Get height of sensor image.
	*/
	const size_t	& getS_rows			()	const	{	return m_info.m_SI.rows;		}
	
	/**
	* @brief ���趨��Ĭ��Ϊ0����λ���ء�\n\n
	* To be zero always, uint is pixel.
	*/
	const int64_t	& getS_OffsetX		()	const	{	return m_info.m_SI.senOffsetX;	}
	
	/**
	* @brief ���趨��Ĭ��Ϊ0����λ���ء�\n\n
	* To be zero always, uint is pixel.
	*/
	const int64_t	& getS_OffsetY		()	const	{	return m_info.m_SI.senOffsetY;	}
	
	
	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief ��ø߶�ͼ������ݿ�ȡ�\n\n
	* Get width of range image.
	*/
	const size_t	& getR_cols			()	const	{	return m_info.m_RI.cols;		}
	
	/**
	* @brief ��ø߶�ͼ������ݸ߶ȡ�\n\n
	* Get height of range image.
	*/
	const size_t	& getR_rows			()	const	{	return m_info.m_RI.rows;		}
	
	/**
	* @brief ��� ROI ������ X ���ꡣ\n\n
	* Get X of ROI.
	*/
	const int64_t	& getR_aoiOffsetX	()	const	{	return m_info.m_RI.aoiOffsetX;	}
	
	/**
	* @brief ��� ROI ������ Y ���ꡣ\n\n
	* Get Y of ROI.
	*/
	const int64_t	& getR_aoiOffsetY	()	const	{	return m_info.m_RI.aoiOffsetY;	}
	
	/**
	* @brief ��� ROI �ĸ߶ȡ�\n\n
	* Get Height of ROI.
	*/
	const int64_t	& getR_aoiHeight	()	const	{	return m_info.m_RI.aoiHeight;	}
	
	/**
	* @brief ��� ROI �Ŀ�ȡ�\n\n
	* Get Width of ROI.
	*/
	const int64_t	& getR_aoiWidth		()	const	{	return m_info.m_RI.aoiWidth;	}
	
	/**
	* @brief ��� R �᷽��\n\n
	* Get Axis of Range.
	*/
	const RA		& getR_RangeAxis	()	const	{	return m_info.m_RI.m_RangeAxis;	}

	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief ��ñ궨��ͼ������ݿ�ȡ�\n\n
	* Get width of calibrated image.
	*/
	const size_t & getC_cols			()	const	{	return m_info.m_CI.cols;	}
	
	/**
	* @brief ��ñ궨��ͼ������ݸ߶ȡ�\n\n
	* Get height of calibrated image.
	*/
	const size_t & getC_rows			()	const	{	return m_info.m_CI.rows;	}
	
	/**
	* @brief ��ñ궨��ͼ��� X ��ʼֵ����λ���ס� \n\n
	* Get the offset in X axis of calibrated image, uint is millimeter.
	* @note ���� X_mm = scaleX * X_pixel + offsetX�� \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX. 
	*/
	const double & getC_offsetX			()	const	{	return m_info.m_CI.offsetX;	} 
	
	/**
	* @brief ��ñ궨��ͼ��� Y ��ʼֵ����λ���ס� \n\n
	* Get the offset in Y axis of calibrated image, uint is millimeter.
	* @note ���� Y_mm = scaleY * Y_pixel + offsetY�� \n   Y of cloud point is Y_mm = scaleY * Y_pixel + offsetY.
	*/
	const double & getC_offsetY			()	const	{	return m_info.m_CI.offsetY;	} 
	
	/**
	* @brief ��ñ궨��ͼ��� X �ֱ��ʣ���λ ����/���ء� \n\n
	* Get the scale in X axis of calibrated image, uint is millimeter/pixel.
	* @note ���� X_mm = scaleX * X_pixel + offsetX�� \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX.
	*/
	const double & getC_scaleX			()	const	{	return m_info.m_CI.scaleX;	} 
	
	/**
	* @brief ��ñ궨��ͼ��� Y �ֱ��ʣ���λ ����/���ء����û����롣 \n\n
	* Get the scale in Y axis of calibrated image, uint is millimeter/pixel. Input by user.
	* @note ���� Y_mm = scaleY * Y_pixel + offsetY�� \n   Y of cloud point is Y_mm = scaleY * Y_pixel + offsetY.
	*/
	const double & getC_scaleY			()	const	{	return m_info.m_CI.scaleY;	} 
	
	/**
	* @brief ��ñ궨��ͼ����ɵ��ƺ� X ����Сֵ����λ ���ס��� getC_offsetX() ��ȡ� \n\n
	* Get the minimum X in cloud points, uint is millimeter. Equal to getC_offsetX().
	* @note ���� X_mm = scaleX * X_pixel + offsetX�� \n   X of cloud point is X_mm = scaleX * X_pixel + offsetX.
	*/
	const double & getC_lower_bound_x	()	const	{	return m_info.m_CI.lower_bound_x;} 
	
	/**
	* @brief ��ñ궨��ͼ����ɵ��ƺ� X �����ֵ����λ ���ס� \n\n
	* Get the maximum X in cloud points, uint is millimeter.
	*/
	const double & getC_upper_bound_x	()	const	{	return m_info.m_CI.upper_bound_x;} 
	
	/**
	* @brief ��ñ궨��ͼ����ɵ��ƺ� Z ����Сֵ����λ ���ס� \n\n
	* Get the minimum Z in cloud points, uint is millimeter. 
	*/
	const double & getC_lower_bound_r	()	const	{	return m_info.m_CI.lower_bound_r;} 
	
	/**
	* @brief ��ñ궨��ͼ����ɵ��ƺ� Z �����ֵ����λ ���ס� \n\n
	* Get the maximum Z in cloud points, uint is millimeter.
	*/
	const double & getC_upper_bound_r	()	const	{	return m_info.m_CI.upper_bound_r;} 

	////////////////////////////////////////////////////////////////////////////////
	
	/**
	* @brief ���ÿ������ı�������Ϣ�� \n\n
	* Get encoder information of each profile.
	*/
	const ChunkData	& get_ChunkInfo		()	const	{	return m_info.m_ChunkData;	}
	
	/**
	* @brief 0��ʼ�����ֹͣ�����á� \n\n
	* Start from 0. Reset when camera stop.
	*/
	const uint64_t	& get_ID			()	const	{	return m_info.m_id;			}
	
	////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief ������ָ�뷵�� false. \n\n
	* Return false if no data pointer.
	*/
	bool	isEmpty				()	const	{	return m_Data.empty();				}
	
	/**
	* @brief ��ָ�������ݷ��� false. \n\n
	* Return false if no such data pointer.
	*/
	bool	has	(const DN _name)	const	{	return (m_Data.count(_name) == 1);	}
	
	/**
	* @brief �ޱ�������Ϣ���� false. \n\n
	* Return false if no encoder information.
	*/
	bool	has_ChunkInfo		()	const	{	return !m_info.m_ChunkData.empty();	}


	////////////////////////////////////////////////////////////////////////////////


	/**
	* @brief �������ݡ�����ָ������� uint8_t* ����ʽ���档���Ͳ�ͬ������ִ��ǿ��ת����nullptr�����ԡ� \n\n
	* Any data pointers type need to be uint8_t*, nullptr will be ignored.
	*
	* @param [in] _name ͼ�����ݵ�ö��ֵ��  DataName of inserting data.
	* @param [in] _pointer ����ָ�롣 Data pointer.
	* @param [in] _id	ͼ�����  Image id.
	*
	* @note �˺�������������ͼ�񲢷��ء�\n
	* ͼ�������Ϣ���ñ����ڵ���֮ǰ��ɡ�\n
	* ����������������ݡ� \n\n
	* �˺�����������ڴ��������������ͷš� \n\n
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
	* @brief ��ȡ ICON ͼ���ļ��� \n\n
	* load image data from Icon file.
	*
	* @param [in] path	������չ����  Please do not provide the suffix;
	* @note				�ɰ汾�� Reflectance �ᱻ����Ϊ <uint8_t*> ���͵����ݡ�\n\n 
	*					If Reflectance data format is < float* >, it will be converted to < uint8_t* >.
	*/
	bool	loadFromIconFile	(cStr path = "file");
	

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
	bool	SaveToIconFile		(cStr path = "file", DN whichOne=INV)	const;
	

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

