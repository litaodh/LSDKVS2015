/***********************************************************************
* 
* Copyright (c) 2000-2001 Luster LightTech(Beijing) Co.Ltd.
* All Rights Reserved.
* 
*
* FILE NAME     :  LstCommonDef.h
* PROJECT NAME  :  LSDK
* ABSTRACT      :  ��������ͷ�ļ���������Ÿ��㶼����õĹ������ݵĶ��塣
*
*
* VERSION       :  1.1.0
* DATE 		    :  2018/09/29
* AUTHOR 		:  ����
* NOTE 		    :  ��Ӧ�ӿ��ĵ���LSDKͨ�ö���˵��(��׼C����)_v1.0.1��
***********************************************************************/
#pragma once

#ifndef LST_COMMON_DEF_H_
#define LST_COMMON_DEF_H_

//�ַ����鳤��Ϊ128
#define LEN_128 128
//�ַ����鳤��Ϊ256
#define LEN_256 256
//֧�ֵ����ö������
#define MAX_ENUM_NUM 64
//һ����������֧��������������
#define MAX_CAMERA_NUM 64


/*
*enum
*/

//�ڵ�����ö�٣�����Ŀ¼���ͣ��������ͣ�δ֪���͡���Χ0x01000100 ~ 0x0100010F
typedef enum tagEnumNodeType
{
	//Ŀ¼���ͣ�û��ʵ����ֵ
	CATEGORY			= 0x01000100,
	//�������ͣ�����ʵ����ֵ�����Զ�ȡ/д����ֵ
	FEATURE				= 0x01000101,
	//δ֪����	
	NODE_TYPE_UNKNOW	= 0x0100010F
} LE_NODE_TYPE;

//�����������ö�٣���������Int��������Double���ַ�����String��������Bool��ö����Enum����������Uncertain��δ֪����Unknow��
//��Χ0x01000110 ~ 0x0100012F
typedef enum tagEnumFeatureType
{
	//��������
	TYPE_INT			= 0x01000110,
	//�������ͣ��˴���ȡ˫����double������������Ϊfloat����ǿ��ת��
	TYPE_DOUBLE			= 0x01000111,
	//�ַ�������
	TYPE_STRING			= 0x01000112,
	//��������
	TYPE_BOOLEAN		= 0x01000113,
	//ö������
	TYPE_ENUM			= 0x01000114,
	//��������
	TYPE_UNCERTAIN		= 0x01000115,
	//δ֪����
	FEATURE_TYPE_UNKNOW	= 0x0100012F
} LE_FEATURE_TYPE;

//ͼ��洢����ö�٣�����ԭʼ����RAW��λͼBMP����Яʽ����ͼ��PNG����ǩͼ���ļ���ʽTIFF������ѹ����ʽJPEG��δ֪����Unknow��
//ö�ٷ�Χ0x01000150 ~ 0x0100016F
typedef enum tagEnumImageType
{
	//ԭʼ���ݣ�û�о�����������ԭ�ļ�
	RAW					= 0x01000150,
	//λͼ(Bitmap)
	BMP					= 0x01000151,
	//��Яʽ����ͼ��(Portable Network Graphics)
	PNG					= 0x01000152,
	//��ǩͼ���ļ���ʽ(TagImage FileFormat)
	TIFF				= 0x01000153,
	//����ѹ����ʽ��������Ƭר����(Joint Photographic Expert Group)
	JPEG				= 0x01000154,
	//δ֪����
	IMAGE_TYPE_UNKNOW	= 0x0100016F
} LE_IMAGE_TYPE;

//���ظ�ʽö�٣���Pixel Format Naming Convention, PFNC��������243�����ظ�ʽ�����У�����2Dͼ���3D�������ݡ�
//�����ظ�ʽö�ٵ�Ŀ���Ƕ���һ��ͨ��Լ�������������Ӿ���ʹ�õ����ظ�ʽ��
//����Ϊ���������ϵĿ������ṩ���صĶ��壬�����������µ����ظ�ʽʱ�ṩ��ȷ��ָ����
//ö�ٷ�Χ ����0x01010000 ~ ???  (�˴�Ϊ�˱�׼�����ظ�ʽ�����յ���GenICam��ö��ֵ������GenICam��ö��ֵ�ǲ������ģ���������������Χ)
typedef enum tagEnumPfncFormat
{
	Mono1p             = 0x01010037, /* Monochrome 1-bit packed */
	Mono2p             = 0x01020038, /* Monochrome 2-bit packed */
	Mono4p             = 0x01040039, /* Monochrome 4-bit packed */
	Mono8              = 0x01080001, /* Monochrome 8-bit */
	Mono8s             = 0x01080002, /* Monochrome 8-bit signed */
	Mono10             = 0x01100003, /* Monochrome 10-bit unpacked */
	Mono10p            = 0x010A0046, /* Monochrome 10-bit packed */
	Mono12             = 0x01100005, /* Monochrome 12-bit unpacked */
	Mono12p            = 0x010C0047, /* Monochrome 12-bit packed */
	Mono14             = 0x01100025, /* Monochrome 14-bit unpacked */
	Mono16             = 0x01100007, /* Monochrome 16-bit */
	BayerBG8           = 0x0108000B, /* Bayer Blue-Green 8-bit */
	BayerBG10          = 0x0110000F, /* Bayer Blue-Green 10-bit unpacked */
	BayerBG10p         = 0x010A0052, /* Bayer Blue-Green 10-bit packed */
	BayerBG12          = 0x01100013, /* Bayer Blue-Green 12-bit unpacked */
	BayerBG12p         = 0x010C0053, /* Bayer Blue-Green 12-bit packed */
	BayerBG16          = 0x01100031, /* Bayer Blue-Green 16-bit */
	BayerGB8           = 0x0108000A, /* Bayer Green-Blue 8-bit */
	BayerGB10          = 0x0110000E, /* Bayer Green-Blue 10-bit unpacked */
	BayerGB10p         = 0x010A0054, /* Bayer Green-Blue 10-bit packed */
	BayerGB12          = 0x01100012, /* Bayer Green-Blue 12-bit unpacked */
	BayerGB12p         = 0x010C0055, /* Bayer Green-Blue 12-bit packed */
	BayerGB16          = 0x01100030, /* Bayer Green-Blue 16-bit */
	BayerGR8           = 0x01080008, /* Bayer Green-Red 8-bit */
	BayerGR10          = 0x0110000C, /* Bayer Green-Red 10-bit unpacked */
	BayerGR10p         = 0x010A0056, /* Bayer Green-Red 10-bit packed */
	BayerGR12          = 0x01100010, /* Bayer Green-Red 12-bit unpacked */
	BayerGR12p         = 0x010C0057, /* Bayer Green-Red 12-bit packed */
	BayerGR16          = 0x0110002E, /* Bayer Green-Red 16-bit */
	BayerRG8           = 0x01080009, /* Bayer Red-Green 8-bit */
	BayerRG10          = 0x0110000D, /* Bayer Red-Green 10-bit unpacked */
	BayerRG10p         = 0x010A0058, /* Bayer Red-Green 10-bit packed */
	BayerRG12          = 0x01100011, /* Bayer Red-Green 12-bit unpacked */
	BayerRG12p         = 0x010C0059, /* Bayer Red-Green 12-bit packed */
	BayerRG16          = 0x0110002F, /* Bayer Red-Green 16-bit */
	RGBa8              = 0x02200016, /* Red-Green-Blue-alpha 8-bit */
	RGBa10             = 0x0240005F, /* Red-Green-Blue-alpha 10-bit unpacked */
	RGBa10p            = 0x02280060, /* Red-Green-Blue-alpha 10-bit packed */
	RGBa12             = 0x02400061, /* Red-Green-Blue-alpha 12-bit unpacked */
	RGBa12p            = 0x02300062, /* Red-Green-Blue-alpha 12-bit packed */
	RGBa14             = 0x02400063, /* Red-Green-Blue-alpha 14-bit unpacked */
	RGBa16             = 0x02400064, /* Red-Green-Blue-alpha 16-bit */
	RGB8               = 0x02180014, /* Red-Green-Blue 8-bit */
	RGB8_Planar        = 0x02180021, /* Red-Green-Blue 8-bit planar */
	RGB10              = 0x02300018, /* Red-Green-Blue 10-bit unpacked */
	RGB10_Planar       = 0x02300022, /* Red-Green-Blue 10-bit unpacked planar */
	RGB10p             = 0x021E005C, /* Red-Green-Blue 10-bit packed */
	RGB10p32           = 0x0220001D, /* Red-Green-Blue 10-bit packed into 32-bit */
	RGB12              = 0x0230001A, /* Red-Green-Blue 12-bit unpacked */
	RGB12_Planar       = 0x02300023, /* Red-Green-Blue 12-bit unpacked planar */
	RGB12p             = 0x0224005D, /* Red-Green-Blue 12-bit packed   */
	RGB14              = 0x0230005E, /* Red-Green-Blue 14-bit unpacked */
	RGB16              = 0x02300033, /* Red-Green-Blue 16-bit   */
	RGB16_Planar       = 0x02300024, /* Red-Green-Blue 16-bit planar */
	RGB565p            = 0x02100035, /* Red-Green-Blue 5/6/5-bit packed */
	BGRa8              = 0x02200017, /* Blue-Green-Red-alpha 8-bit */
	BGRa10             = 0x0240004C, /* Blue-Green-Red-alpha 10-bit unpacked */
	BGRa10p            = 0x0228004D, /* Blue-Green-Red-alpha 10-bit packed */
	BGRa12             = 0x0240004E, /* Blue-Green-Red-alpha 12-bit unpacked */
	BGRa12p            = 0x0230004F, /* Blue-Green-Red-alpha 12-bit packed */
	BGRa14             = 0x02400050, /* Blue-Green-Red-alpha 14-bit unpacked */
	BGRa16             = 0x02400051, /* Blue-Green-Red-alpha 16-bit */
	BGR8               = 0x02180015, /* Blue-Green-Red 8-bit */
	BGR10              = 0x02300019, /* Blue-Green-Red 10-bit unpacked */
	BGR10p             = 0x021E0048, /* Blue-Green-Red 10-bit packed */
	BGR12              = 0x0230001B, /* Blue-Green-Red 12-bit unpacked */
	BGR12p             = 0x02240049, /* Blue-Green-Red 12-bit packed */
	BGR14              = 0x0230004A, /* Blue-Green-Red 14-bit unpacked */
	BGR16              = 0x0230004B, /* Blue-Green-Red 16-bit */
	BGR565p            = 0x02100036, /* Blue-Green-Red 5/6/5-bit packed */
	R8                 = 0x010800C9, /* Red 8-bit */
	R10                = 0x010A00CA, /* Red 10-bit */
	R12                = 0x010C00CB, /* Red 12-bit */
	R16                = 0x011000CC, /* Red 16-bit */
	G8                 = 0x010800CD, /* Green 8-bit */
	G10                = 0x010A00CE, /* Green 10-bit */
	G12                = 0x010C00CF, /* Green 12-bit */
	G16                = 0x011000D0, /* Green 16-bit */
	B8                 = 0x010800D1, /* Blue 8-bit */
	B10                = 0x010A00D2, /* Blue 10-bit */
	B12                = 0x010C00D3, /* Blue 12-bit */
	B16                = 0x011000D4, /* Blue 16-bit */
	Coord3D_ABC8          = 0x021800B2, /* 3D coordinate A-B-C 8-bit */
	Coord3D_ABC8_Planar   = 0x021800B3, /* 3D coordinate A-B-C 8-bit planar */
	Coord3D_ABC10p        = 0x021E00DB, /* 3D coordinate A-B-C 10-bit packed */
	Coord3D_ABC10p_Planar = 0x021E00DC, /* 3D coordinate A-B-C 10-bit packed planar */
	Coord3D_ABC12p        = 0x022400DE, /* 3D coordinate A-B-C 12-bit packed */
	Coord3D_ABC12p_Planar = 0x022400DF, /* 3D coordinate A-B-C 12-bit packed planar */
	Coord3D_ABC16         = 0x023000B9, /* 3D coordinate A-B-C 16-bit */
	Coord3D_ABC16_Planar  = 0x023000BA, /* 3D coordinate A-B-C 16-bit planar */
	Coord3D_ABC32f        = 0x026000C0, /* 3D coordinate A-B-C 32-bit floating point */
	Coord3D_ABC32f_Planar = 0x026000C1, /* 3D coordinate A-B-C 32-bit floating point planar */
	Coord3D_AC8           = 0x021000B4, /* 3D coordinate A-C 8-bit */
	Coord3D_AC8_Planar    = 0x021000B5, /* 3D coordinate A-C 8-bit planar */
	Coord3D_AC10p         = 0x021400F0, /* 3D coordinate A-C 10-bit packed */
	Coord3D_AC10p_Planar  = 0x021400F1, /* 3D coordinate A-C 10-bit packed planar */
	Coord3D_AC12p         = 0x021800F2, /* 3D coordinate A-C 12-bit packed */
	Coord3D_AC12p_Planar  = 0x021800F3, /* 3D coordinate A-C 12-bit packed planar */
	Coord3D_AC16          = 0x022000BB, /* 3D coordinate A-C 16-bit */
	Coord3D_AC16_Planar   = 0x022000BC, /* 3D coordinate A-C 16-bit planar */
	Coord3D_AC32f         = 0x024000C2, /* 3D coordinate A-C 32-bit floating point */
	Coord3D_AC32f_Planar  = 0x024000C3, /* 3D coordinate A-C 32-bit floating point planar */
	Coord3D_A8            = 0x010800AF, /* 3D coordinate A 8-bit */
	Coord3D_A10p          = 0x010A00D5, /* 3D coordinate A 10-bit packed */
	Coord3D_A12p          = 0x010C00D8, /* 3D coordinate A 12-bit packed */
	Coord3D_A16           = 0x011000B6, /* 3D coordinate A 16-bit */
	Coord3D_A32f          = 0x012000BD, /* 3D coordinate A 32-bit floating point */
	Coord3D_B8            = 0x010800B0, /* 3D coordinate B 8-bit */
	Coord3D_B10p          = 0x010A00D6, /* 3D coordinate B 10-bit packed */
	Coord3D_B12p          = 0x010C00D9, /* 3D coordinate B 12-bit packed */
	Coord3D_B16           = 0x011000B7, /* 3D coordinate B 16-bit */
	Coord3D_B32f          = 0x012000BE, /* 3D coordinate B 32-bit floating point */
	Coord3D_C8            = 0x010800B1, /* 3D coordinate C 8-bit */
	Coord3D_C10p          = 0x010A00D7, /* 3D coordinate C 10-bit packed */
	Coord3D_C12p          = 0x010C00DA, /* 3D coordinate C 12-bit packed */
	Coord3D_C16           = 0x011000B8, /* 3D coordinate C 16-bit */
	Coord3D_C32f          = 0x012000BF, /* 3D coordinate C 32-bit floating point */
	Confidence1           = 0x010800C4, /* Confidence 1-bit unpacked */
	Confidence1p          = 0x010100C5, /* Confidence 1-bit packed */
	Confidence8           = 0x010800C6, /* Confidence 8-bit */
	Confidence16          = 0x011000C7, /* Confidence 16-bit */
	Confidence32f         = 0x012000C8, /* Confidence 32-bit floating point */
	BiColorBGRG8          = 0x021000A6, /* Bi-color Blue/Green - Red/Green 8-bit */
	BiColorBGRG10         = 0x022000A9, /* Bi-color Blue/Green - Red/Green 10-bit unpacked */
	BiColorBGRG10p        = 0x021400AA, /* Bi-color Blue/Green - Red/Green 10-bit packed */
	BiColorBGRG12         = 0x022000AD, /* Bi-color Blue/Green - Red/Green 12-bit unpacked */
	BiColorBGRG12p        = 0x021800AE, /* Bi-color Blue/Green - Red/Green 12-bit packed */
	BiColorRGBG8          = 0x021000A5, /* Bi-color Red/Green - Blue/Green 8-bit */
	BiColorRGBG10         = 0x022000A7, /* Bi-color Red/Green - Blue/Green 10-bit unpacked */
	BiColorRGBG10p        = 0x021400A8, /* Bi-color Red/Green - Blue/Green 10-bit packed */
	BiColorRGBG12         = 0x022000AB, /* Bi-color Red/Green - Blue/Green 12-bit unpacked */
	BiColorRGBG12p        = 0x021800AC, /* Bi-color Red/Green - Blue/Green 12-bit packed */
	SCF1WBWG8             = 0x01080067, /* Sparse Color Filter #1 White-Blue-White-Green 8-bit */
	SCF1WBWG10            = 0x01100068, /* Sparse Color Filter #1 White-Blue-White-Green 10-bit unpacked */
	SCF1WBWG10p           = 0x010A0069, /* Sparse Color Filter #1 White-Blue-White-Green 10-bit packed */
	SCF1WBWG12            = 0x0110006A, /* Sparse Color Filter #1 White-Blue-White-Green 12-bit unpacked */
	SCF1WBWG12p           = 0x010C006B, /* Sparse Color Filter #1 White-Blue-White-Green 12-bit packed */
	SCF1WBWG14            = 0x0110006C, /* Sparse Color Filter #1 White-Blue-White-Green 14-bit unpacked */
	SCF1WBWG16            = 0x0110006D, /* Sparse Color Filter #1 White-Blue-White-Green 16-bit unpacked */
	SCF1WGWB8             = 0x0108006E, /* Sparse Color Filter #1 White-Green-White-Blue 8-bit */
	SCF1WGWB10            = 0x0110006F, /* Sparse Color Filter #1 White-Green-White-Blue 10-bit unpacked */
	SCF1WGWB10p           = 0x010A0070, /* Sparse Color Filter #1 White-Green-White-Blue 10-bit packed */
	SCF1WGWB12            = 0x01100071, /* Sparse Color Filter #1 White-Green-White-Blue 12-bit unpacked */
	SCF1WGWB12p           = 0x010C0072, /* Sparse Color Filter #1 White-Green-White-Blue 12-bit packed */
	SCF1WGWB14            = 0x01100073, /* Sparse Color Filter #1 White-Green-White-Blue 14-bit unpacked */
	SCF1WGWB16            = 0x01100074, /* Sparse Color Filter #1 White-Green-White-Blue 16-bit */
	SCF1WGWR8             = 0x01080075, /* Sparse Color Filter #1 White-Green-White-Red 8-bit */
	SCF1WGWR10            = 0x01100076, /* Sparse Color Filter #1 White-Green-White-Red 10-bit unpacked */
	SCF1WGWR10p           = 0x010A0077, /* Sparse Color Filter #1 White-Green-White-Red 10-bit packed */
	SCF1WGWR12            = 0x01100078, /* Sparse Color Filter #1 White-Green-White-Red 12-bit unpacked */
	SCF1WGWR12p           = 0x010C0079, /* Sparse Color Filter #1 White-Green-White-Red 12-bit packed */
	SCF1WGWR14            = 0x0110007A, /* Sparse Color Filter #1 White-Green-White-Red 14-bit unpacked */
	SCF1WGWR16            = 0x0110007B, /* Sparse Color Filter #1 White-Green-White-Red 16-bit */
	SCF1WRWG8             = 0x0108007C, /* Sparse Color Filter #1 White-Red-White-Green 8-bit */
	SCF1WRWG10            = 0x0110007D, /* Sparse Color Filter #1 White-Red-White-Green 10-bit unpacked */
	SCF1WRWG10p           = 0x010A007E, /* Sparse Color Filter #1 White-Red-White-Green 10-bit packed */
	SCF1WRWG12            = 0x0110007F, /* Sparse Color Filter #1 White-Red-White-Green 12-bit unpacked */
	SCF1WRWG12p           = 0x010C0080, /* Sparse Color Filter #1 White-Red-White-Green 12-bit packed */
	SCF1WRWG14            = 0x01100081, /* Sparse Color Filter #1 White-Red-White-Green 14-bit unpacked */
	SCF1WRWG16            = 0x01100082, /* Sparse Color Filter #1 White-Red-White-Green 16-bit */
	YCbCr8                   = 0x0218005B, /* YCbCr 4:4:4 8-bit */
	YCbCr8_CbYCr             = 0x0218003A, /* YCbCr 4:4:4 8-bit */
	YCbCr10_CbYCr            = 0x02300083, /* YCbCr 4:4:4 10-bit unpacked */
	YCbCr10p_CbYCr           = 0x021E0084, /* YCbCr 4:4:4 10-bit packed */
	YCbCr12_CbYCr            = 0x02300085, /* YCbCr 4:4:4 12-bit unpacked */
	YCbCr12p_CbYCr           = 0x02240086, /* YCbCr 4:4:4 12-bit packed */
	YCbCr411_8               = 0x020C005A, /* YCbCr 4:1:1 8-bit */
	YCbCr411_8_CbYYCrYY      = 0x020C003C, /* YCbCr 4:1:1 8-bit */
	YCbCr422_8               = 0x0210003B, /* YCbCr 4:2:2 8-bit */
	YCbCr422_8_CbYCrY        = 0x02100043, /* YCbCr 4:2:2 8-bit */
	YCbCr422_10              = 0x02200065, /* YCbCr 4:2:2 10-bit unpacked */
	YCbCr422_10_CbYCrY       = 0x02200099, /* YCbCr 4:2:2 10-bit unpacked */
	YCbCr422_10p             = 0x02140087, /* YCbCr 4:2:2 10-bit packed */
	YCbCr422_10p_CbYCrY      = 0x0214009A, /* YCbCr 4:2:2 10-bit packed */
	YCbCr422_12              = 0x02200066, /* YCbCr 4:2:2 12-bit unpacked */
	YCbCr422_12_CbYCrY       = 0x0220009B, /* YCbCr 4:2:2 12-bit unpacked */
	YCbCr422_12p             = 0x02180088, /* YCbCr 4:2:2 12-bit packed */
	YCbCr422_12p_CbYCrY      = 0x0218009C, /* YCbCr 4:2:2 12-bit packed */
	YCbCr601_8_CbYCr         = 0x0218003D, /* YCbCr 4:4:4 8-bit BT.601 */
	YCbCr601_10_CbYCr        = 0x02300089, /* YCbCr 4:4:4 10-bit unpacked BT.601 */
	YCbCr601_10p_CbYCr       = 0x021E008A, /* YCbCr 4:4:4 10-bit packed BT.601 */
	YCbCr601_12_CbYCr        = 0x0230008B, /* YCbCr 4:4:4 12-bit unpacked BT.601 */
	YCbCr601_12p_CbYCr       = 0x0224008C, /* YCbCr 4:4:4 12-bit packed BT.601 */
	YCbCr601_411_8_CbYYCrYY  = 0x020C003F, /* YCbCr 4:1:1 8-bit BT.601 */
	YCbCr601_422_8           = 0x0210003E, /* YCbCr 4:2:2 8-bit BT.601 */
	YCbCr601_422_8_CbYCrY    = 0x02100044, /* YCbCr 4:2:2 8-bit BT.601 */
	YCbCr601_422_10          = 0x0220008D, /* YCbCr 4:2:2 10-bit unpacked BT.601 */
	YCbCr601_422_10_CbYCrY   = 0x0220009D, /* YCbCr 4:2:2 10-bit unpacked BT.601 */
	YCbCr601_422_10p         = 0x0214008E, /* YCbCr 4:2:2 10-bit packed BT.601 */
	YCbCr601_422_10p_CbYCrY  = 0x0214009E, /* YCbCr 4:2:2 10-bit packed BT.601 */
	YCbCr601_422_12          = 0x0220008F, /* YCbCr 4:2:2 12-bit unpacked BT.601 */
	YCbCr601_422_12_CbYCrY   = 0x0220009F, /* YCbCr 4:2:2 12-bit unpacked BT.601 */
	YCbCr601_422_12p         = 0x02180090, /* YCbCr 4:2:2 12-bit packed BT.601 */
	YCbCr601_422_12p_CbYCrY  = 0x021800A0, /* YCbCr 4:2:2 12-bit packed BT.601 */
	YCbCr709_8_CbYCr         = 0x02180040, /* YCbCr 4:4:4 8-bit BT.709 */
	YCbCr709_10_CbYCr        = 0x02300091, /* YCbCr 4:4:4 10-bit unpacked BT.709 */
	YCbCr709_10p_CbYCr       = 0x021E0092, /* YCbCr 4:4:4 10-bit packed BT.709 */
	YCbCr709_12_CbYCr        = 0x02300093, /* YCbCr 4:4:4 12-bit unpacked BT.709 */
	YCbCr709_12p_CbYCr       = 0x02240094, /* YCbCr 4:4:4 12-bit packed BT.709 */
	YCbCr709_411_8_CbYYCrYY  = 0x020C0042, /* YCbCr 4:1:1 8-bit BT.709 */
	YCbCr709_422_8           = 0x02100041, /* YCbCr 4:2:2 8-bit BT.709 */
	YCbCr709_422_8_CbYCrY    = 0x02100045, /* YCbCr 4:2:2 8-bit BT.709 */
	YCbCr709_422_10          = 0x02200095, /* YCbCr 4:2:2 10-bit unpacked BT.709 */
	YCbCr709_422_10_CbYCrY   = 0x022000A1, /* YCbCr 4:2:2 10-bit unpacked BT.709 */
	YCbCr709_422_10p         = 0x02140096, /* YCbCr 4:2:2 10-bit packed BT.709 */
	YCbCr709_422_10p_CbYCrY  = 0x021400A2, /* YCbCr 4:2:2 10-bit packed BT.709 */
	YCbCr709_422_12          = 0x02200097, /* YCbCr 4:2:2 12-bit unpacked BT.709 */
	YCbCr709_422_12_CbYCrY   = 0x022000A3, /* YCbCr 4:2:2 12-bit unpacked BT.709 */
	YCbCr709_422_12p         = 0x02180098, /* YCbCr 4:2:2 12-bit packed BT.709 */
	YCbCr709_422_12p_CbYCrY  = 0x021800A4, /* YCbCr 4:2:2 12-bit packed BT.709 */
	YCbCr2020_8_CbYCr        = 0x021800F4, /* YCbCr 4:4:4 8-bit BT.2020 */
	YCbCr2020_10_CbYCr       = 0x023000F5, /* YCbCr 4:4:4 10-bit unpacked BT.2020 */
	YCbCr2020_10p_CbYCr      = 0x021E00F6, /* YCbCr 4:4:4 10-bit packed BT.2020 */
	YCbCr2020_12_CbYCr       = 0x023000F7, /* YCbCr 4:4:4 12-bit unpacked BT.2020  */
	YCbCr2020_12p_CbYCr      = 0x022400F8, /* YCbCr 4:4:4 12-bit packed BT.2020 */
	YCbCr2020_411_8_CbYYCrYY = 0x020C00F9, /* YCbCr 4:1:1 8-bit BT.2020 */
	YCbCr2020_422_8          = 0x021000FA, /* YCbCr 4:2:2 8-bit BT.2020  */
	YCbCr2020_422_8_CbYCrY   = 0x021000FB, /* YCbCr 4:2:2 8-bit BT.2020 */
	YCbCr2020_422_10         = 0x022000FC, /* YCbCr 4:2:2 10-bit unpacked BT.2020 */
	YCbCr2020_422_10_CbYCrY  = 0x022000FD, /* YCbCr 4:2:2 10-bit unpacked BT.2020 */
	YCbCr2020_422_10p        = 0x021400FE, /* YCbCr 4:2:2 10-bit packed BT.2020 */
	YCbCr2020_422_10p_CbYCrY = 0x021400FF, /* YCbCr 4:2:2 10-bit packed BT.2020 */
	YCbCr2020_422_12         = 0x02200100, /* YCbCr 4:2:2 12-bit unpacked BT.2020 */
	YCbCr2020_422_12_CbYCrY  = 0x02200101, /* YCbCr 4:2:2 12-bit unpacked BT.2020 */
	YCbCr2020_422_12p        = 0x02180102, /* YCbCr 4:2:2 12-bit packed BT.2020 */
	YCbCr2020_422_12p_CbYCrY = 0x02180103, /* YCbCr 4:2:2 12-bit packed BT.2020 */
	YUV8_UYV                 = 0x02180020, /* YUV 4:4:4 8-bit */
	YUV411_8_UYYVYY          = 0x020C001E, /* YUV 4:1:1 8-bit */
	YUV422_8                 = 0x02100032, /* YUV 4:2:2 8-bit */
	YUV422_8_UYVY            = 0x0210001F, /* YUV 4:2:2 8-bit */
	Mono10Packed             = 0x010C0004, /* GigE Vision specific format, Monochrome 10-bit packed */
	Mono12Packed             = 0x010C0006, /* GigE Vision specific format, Monochrome 12-bit packed */
	BayerBG10Packed          = 0x010C0029, /* GigE Vision specific format, Bayer Blue-Green 10-bit packed */
	BayerBG12Packed          = 0x010C002D, /* GigE Vision specific format, Bayer Blue-Green 12-bit packed */
	BayerGB10Packed          = 0x010C0028, /* GigE Vision specific format, Bayer Green-Blue 10-bit packed */
	BayerGB12Packed          = 0x010C002C, /* GigE Vision specific format, Bayer Green-Blue 12-bit packed */
	BayerGR10Packed          = 0x010C0026, /* GigE Vision specific format, Bayer Green-Red 10-bit packed */
	BayerGR12Packed          = 0x010C002A, /* GigE Vision specific format, Bayer Green-Red 12-bit packed */
	BayerRG10Packed          = 0x010C0027, /* GigE Vision specific format, Bayer Red-Green 10-bit packed */
	BayerRG12Packed          = 0x010C002B, /* GigE Vision specific format, Bayer Red-Green 12-bit packed */
	RGB10V1Packed            = 0x0220001C, /* GigE Vision specific format, Red-Green-Blue 10-bit packed - variant 1 */
	RGB12V1Packed            = 0x02240034, /* GigE Vision specific format, Red-Green-Blue 12-bit packed - variant 1 */
	InvalidPixelFormat       = 0
} LE_PFNC_FORMAT;


/*
*struct
*/


//����ڵ���Ϣ�ṹ��
typedef struct tagStructNodeInfo
{
	/*���������������������xml�ļ��������������Ⱥ�˳��Ϊ����*/
	int iNodeIndex;
	/*�������ͣ�����Ŀ¼����CATEGORY����������FEATURE��δ֪���͡�*/
	LE_NODE_TYPE iNodeType;
	/*�������ƣ�LFNC�Ĳ�������*/
	char achNodeName[LEN_128];
	/*Ԥ���ֶ�*/	
	char achReserved[LEN_128];
} LST_NODE_INFO;

//ͼ�����Խṹ�壬���ͼ���һЩ������Ϣ������ͼ�����ݸ�ʽ��ͼ���ȡ�ͼ��߶ȡ�ͼ��λ�ͼ��ͨ������Ԥ���ֶε���Ϣ��
typedef struct tagStructImageProperty
{
	/*ͼ�����ݸ�ʽ���μ�LE_PFNC_FORMATö�� �� PFNC�ĵ�*/
	int iImagePixelFormat;
	/*ͼ��Ŀ��*/
	int iImageWidth;
	/*ͼ��ĸ߶�*/
	int iImageHeight;
	/*ͼ���λ��*/
	int iImageDepth;
	/*ͼ��ͨ���� 1-�ڰ�ͼ�� 3-��ɫͼ��*/
	int iImageChannels;
	/*�ֽڴ���0����Little-endianС�˸�ʽ��1����big-endian��θ�ʽ*/
	int iEndianess;
	/*Ԥ���ֶ�*/
	char achReserved[LEN_128];
} LST_IMAGE_PROPERTY;

//����������Ϣ�ṹ�壬�������ֵ����Сֵ����������ǰֵ��
typedef struct tagStructIntInfo
{
	//���ֵ
	int iMax;
	//��Сֵ
	int iMin;
	//����
	int iInc;
	//��ǰֵ
	int iValue;
} LST_INT_INFO;

//������������Ϣ�ṹ�壬�������ֵ����Сֵ����������ǰֵ��
typedef struct tagStructDoubleInfo
{
	//���ֵ
	double dMax;
	//��Сֵ
	double dMin;
	//����
	double dInc;
	//��ǰֵ
	double dValue;
} LST_DOUBLE_INFO;

//������������Ϣ�ṹ�壬������ǰֵ��
typedef struct tagStructBooleanInfo
{
	//��ǰֵ
	bool bValue;
} LST_BOOLEAN_INFO;

//�ַ���������Ϣ�ṹ�壬������ǰֵ��
typedef struct tagStructStringInfo
{
	//�ַ�����
	int iLength;
	//��ǰֵ
	char achValue[LEN_256];
} LST_STRING_INFO;

//ö����Ԫ�ؽṹ�壬����ö�����ơ�ö��ֵ��
typedef struct tagStructEnumElement
{
	//ö������
	char achEnumName[LEN_256];
	//ö��ֵ
	int iEnumValue;
} LST_ENUM_ELEMENT;

//ö����������Ϣ�ṹ�壬����ö����Ԫ�ؽṹ�����顢ö��������Ĭ��ֵ��
typedef struct tagStructEnumInfo
{
	//ö����Ԫ�ؽṹ������,���֧��ö������Ϊ64
	LST_ENUM_ELEMENT astEnumList[MAX_ENUM_NUM];
	//ö������
	int iEnumNum;
	//��ǰֵ
	int iValue;
} LST_ENUM_INFO;

//������Ϣ�ṹ�壬���������������͡��������˵����������ϸ˵����������ʾ���ơ������û�����
//�����������ԡ���������������Ϣ���Ƿ�Ӧ��Ĭ��ֵ��ʼ����
typedef struct tagStructFeatureInfo
{
	//������������
	LE_FEATURE_TYPE eType;
	//�������˵��
	char achToolTip[LEN_256];
	//������ϸ˵��
	char achDescription[LEN_256];
	//������ʾ����
	char achDisplayName[LEN_256];
	//�����û����𣬰���
	//Beginner  �C ����
	//Expert    - ר��
	//Guru      - ��ʦ
	//Invisible - ���ɼ�
	char achVisibility[LEN_256];
	//�����������ԣ�����
	//NI �C not implemented - δʵ��
	//NA �C not available - ������
	//WO �C write only - ֻд
	//RO �C read only - ֻ��
	//RW �C readable and writable - ��д
	char achAccessMode[LEN_256];
	//���������SDK�Ĳ�������
	char achVenderFeatureName[LEN_256];
	//�Ƿ�Ӧ��Ĭ��ֵ���г�ʼ��
	bool bIsInit;
	//�Ƿ�ɼ�״̬ʱ����
	bool bIsLocked;
	//���������Ա仯˵��
	char achIsAvaliable[LEN_256];
	//���������Ա仯˵��
	char achIsPolling[LEN_256];
	//Ԥ���ֶ�
	char achReserved[LEN_256];
} LST_FEATURE_INFO;







#endif   //LST_COMMON_DEF_H_