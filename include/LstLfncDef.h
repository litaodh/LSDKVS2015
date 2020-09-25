/***********************************************************************
*
* Copyright (c) 2000-2001 Luster LightTech(Beijing) Co.Ltd.
* All Rights Reserved.
*
*
* FILE NAME     :  LusterLfncDef.h
* PROJECT NAME  :  LusterSDK
* ABSTRACT      :  ���������������Լ����LFNC������ͷ�ļ���������Ų������ƵĶ��塣
*
*
*
* VERSION       :  0.1.2.0
* DATE 		    :  2017/12/15
* AUTHOR 		:  ����
* NOTE 		    :  ���壬��Ӧ˵���ĵ������������������Լ��(LFNC)_v0.1.2.0����
***********************************************************************/
#pragma once

#ifndef LUSTER_LFNC_DEF_H_
#define LUSTER_LFNC_DEF_H_


//��ͨ����
/*����������������������������������������������������������������������������������������������������������������������������������������������������������
|       ������                          LFNC����                    ��������           ��λ          ��������		                                       |
����������������������������������������������������������������������������������������������������������������������������������������������������������*/
#define LFNC_WIDTH                      "Width"                     /*  Int            Pixel         ���ͼ����                                         */
#define LFNC_HEIGHT                     "Height"                    /*  Int            Pixel         ���ͼ��߶�                                         */
#define LFNC_OFFSET_X                   "OffsetX"                   /*  Int            Pixel         ���ͼ��ˮƽƫ��                                     */
#define LFNC_OFFSET_Y                   "OffsetY"                   /*  Int            Pixel         ���ͼ��ֱƫ��                                     */
#define LFNC_PIXEL_FORMAT               "PixelFormat"               /*  Enum           -             ���ͼ�����ظ�ʽ���μ�PFNC��                         */
#define LFNC_ACQ_FRAME_RATE             "AcquisitionFrameRate"      /*  Double         Hz            ���ץȡ��һ֡ͼ���Ƶ��                             */
#define LFNC_ACQ_LINE_RATE              "AcquisitionLineRate"       /*  Double         Hz            ���ץȡ��һ��ͼ���Ƶ��                             */
#define LFNC_EXP_TIME                   "ExposureTime"              /*  Double         us            �ع�ʱ�䣨΢�룩                                     */
#define LFNC_GAIN                       "Gain"                      /*  Double         -             ���棬��λȡ�����豸                                 */
#define LFNC_BLACK_LEVEL                "BlackLevel"                /*  Double         -             ƫ�ã���λȡ�����豸                                 */
#define LFNC_GAMMA                      "Gamma"                     /*  Double         -             ٤�����                                             */
#define LFNC_TRIG_MODE                  "TriggerMode"               /*  Enum           -             ����ģʽѡ��0->Off, 1->On��                        */
#define LFNC_TRIG_SRC                   "TriggerSource"             /*  Enum           -             ����Դѡ��0->Line0, 7->Software��                  */
#define LFNC_TRIG_SOFTWARE              "TriggerSoftware"           /*  Int            -             ����һ���ڲ������ź�(���跢��ֵ)                   */
#define LFNC_ACQ_FRAME_RATE_AUTO        "AcquisitionFrameRateAuto"  /*  Enum           -             ֡Ƶ���Զ���0->Off, 2->Continuous��                  */
#define LFNC_EXP_TIME_AUTO              "ExposureAuto"              /*  Enum           -             �ع�ʱ���Զ���0->Off, 1->Once, 2->Continuous��       */
#define LFNC_GAIN_AUTO                  "GainAuto"                  /*  Enum           -             �����Զ���0->Off, 1->Once, 2->Continuous��           */
#define LFNC_BLACK_LEVEL_AUTO           "BlackLevelAuto"            /*  Enum           -             ƫ���Զ���0->Off, 2->Continuous��                    */

//���в���
/*����������������������������������������������������������������������������������������������������������������������������������������������������������������������
|       ������                          LFNC����                         ��������           ��λ          ��������		                                                |
������������������������������������������������������������������������������������������������������������������������������������������������������������������������*/

//�û�������User Parameters�� �C ͼ����� �C δ��csv�ϵĲ��� �C ��������� 
#define LFNC_SICK_SCALEY               "SICK_User_ScaleY"               /*Double              mm          Y�ֱ���*/
#define LFNC_SICK_PARAMETPATH          "SICK_User_ParametPath"          /*std::string/char*               ���������ļ�·��*/
#define LFNC_SICK_CALIBRAPATH          "SICK_User_CalibraPath"          /*std::string/char*               �궨�ļ�·��*/ 
#define LFNC_SICK_CAMMAC               "SICK_Device_CamMac"             /*std::string                     ���mac��ַ         */
#define LFNC_SICK_DEVICENAME           "SICK_Device_Name"               /*std::string                     �豸����*/


//��չ�ӿڲ���
/*����������������������������������������������������������������������������������������������������������������������������������������������
|       ������                          LFNC����                    ��������           ��λ          ��������		                           |
����������������������������������������������������������������������������������������������������������������������������������������������*/
#define LFNC_GET_STORE_IMAGE            "LstGetStoreImage"          /*  Uncertain      -             ��ȡ�洢���ͻ����е�һ֡ͼ������         */
#define LFNC_RELEASE_STORE_IMAGE        "LstReleaseStoreImage"      /*  Uncertain      -             �ͷŴ洢���ͻ����е�һ֡ͼ������         */
#define LFNC_SNAP_WAIT_TIME             "LstSnapWaitTime"           /*  Uncertain      ms            ��ȡͼ�����ݵ����ȴ�ʱ�䣬Ĭ��1000ms   */
#define LFNC_GET_NODE_NUM               "LstGetNodeNum"             /*  Uncertain      -             ��ȡxml�������������������              */
#define LFNC_GET_NODE_INFO              "LstGetNodeInfo"            /*  Uncertain      -             ��ȡxml�����������������Ϣ�ṹ�壨���У�*/
#define LFNC_DESTORY_CAMERA             "LstDestoryCamera"          /*  Uncertain      -             �������е������Դ                       */
#define LFNC_GET_IMAGE_PROPERTY         "LstGetImageProperty"       /*  Uncertain      -             ��ȡͼ�����Խṹ��                       */


#endif   //LUSTER_LFNC_DEF_H_