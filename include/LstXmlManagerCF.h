/***********************************************************************
*
* Copyright (c) 2000-2001 Luster LightTech(Beijing) Co.Ltd.
* All Rights Reserved.
*
*
* FILE NAME     :  LstXmlManagerCF.h
* PROJECT NAME  :  LSDK
* ABSTRACT      :  XML�����������ӿڣ���Ҫ������ȥ����������������⡣
*				   ��ϸ�μ���XML�����������ӿ���ϸ���˵��_v1.1.0��
*
*
* VERSION       :  1.1.0
* DATE 		    :  2018/04/14
* AUTHOR 		:  ����
* NOTE 		    :  ɾ������Ҫ��tinyxmlͷ�ļ���using namespace std���궨��ȡ�
***********************************************************************/
#pragma once

#ifndef LST_XML_MANAGER_CF_H_
#define LST_XML_MANAGER_CF_H_

#include <vector>
#include "LstCommonDef.h"


/*
*struct
*/

//�������������壬��������������Ϣ��������������Ϣ���ַ���������Ϣ��������������Ϣ��ö����������Ϣ��Ԥ���ֶΡ�
typedef union tagUnionFeatureValue
{
	//����������Ϣ
	LST_INT_INFO stInt;
	//������������Ϣ
	LST_DOUBLE_INFO stDouble;
	//�ַ���������Ϣ
	LST_STRING_INFO stString;
	//������������Ϣ
	LST_BOOLEAN_INFO stBoolean;
	//ö����������Ϣ
	LST_ENUM_INFO stEnum;
	//Ԥ���ֶ�
	char achReserved[LEN_256];
} LU_FEATURE_VALUE;


class TiXmlElement;//tinyxml��
class XmlTreeNode;
template class __declspec(dllexport) std::allocator<XmlTreeNode*>;
template class __declspec(dllexport) std::vector<XmlTreeNode*, std::allocator<XmlTreeNode*>>;


#ifdef LSTXMLMANAGERCF_EXPORTS
#define LXMCF_API __declspec(dllexport)
#else
#define LXMCF_API __declspec(dllimport)
#endif

/******************************************************
ÿһ��XmlTreeNode���������ṹ��״ͼ�е�һ���ڵ㣬������Ա��ͨ��Add�ӿڲ�������ڵ���һ��һ�������µ��ӽڵ㣬
�����γ�һ��������������ṹ��ͬʱ��XmlTreeNode���ڽӿ�����У��ṩ�˱����ӽڵ㡢����ӽڵ㡢����ӽڵ���Ϣ�Ƚӿڣ�
���㿪����Աֱ�ӵ��õĽӿڣ����Կ��ٲ�ѯָ�����ƵĽڵ���Ϣ�����XmlTreeNode�����ṩ�˼���xml�ļ�������xml�ļ��Ľӿڣ�
ʵ�����뱾�����ݹ�ͨ�Ĺ��ܡ�XmlTreeNode��ʹ�������ģʽ��
******************************************************/
class LXMCF_API XmlTreeNode
{
public:

	/******************************************************
	@����˵���� ���캯����
	@�������룺 pchName -- �ڵ�����
	eType   -- �ڵ�����
	@��������� ��
	@����ֵ  �� XmlTreeNode����
	******************************************************/
	XmlTreeNode(const char *pchNodeName, LE_NODE_TYPE eNodeType);

	/******************************************************
	@����˵���� ����������
	@�������룺 ��
	@��������� ��
	@����ֵ  �� ��
	******************************************************/
	~XmlTreeNode(void);

	/******************************************************
	@����˵���� ��ýڵ����ơ�
	@�������룺 ��
	@��������� ��
	@����ֵ  �� ��ǰ�Ľڵ����ơ�
	******************************************************/
	char* NodeName();

	/******************************************************
	@����˵���� ��ýڵ����͡�
	@�������룺 ��
	@��������� ��
	@����ֵ  �� ��ǰ�Ľڵ����͡�
	******************************************************/
	LE_NODE_TYPE NodeType();

	/******************************************************
	@����˵���� ����xml�ļ�����ȡ����xml�ļ��е���Ϣ�����浽һ���������У����øú����Ľڵ㽫���Ϊ���ڵ㡣
	@�������룺 pchXmlFileName -- �����ص�xml�ļ���������·����
	@��������� ��
	@����ֵ  �� �ɹ�����true��ʧ�ܷ���false��
	******************************************************/
	bool LoadXml(const char *pchXmlFilePath);

	/******************************************************
	@����˵���� �������������е���Ϣ���浽һ��xml�ļ��С�ע�⣺�ú��������ڵ�ɵ���(�ڵ�����Ϊroot�����򷵻�false)��
	@�������룺 pchXmlFileName -- �����ص�xml�ļ���������·����
	@��������� ��
	@����ֵ  �� �ɹ�����true��ʧ�ܷ���false��
	******************************************************/
	bool SaveXml(const char *pchXmlFilePath);

	/******************************************************
	@����˵���� ��ȡ��ǰ�ڵ��еĲ�����Ϣ���õ�һ��LST_FEATURE_INFO�ṹ�塣
	@�������룺 ��
	@��������� ��
	@����ֵ  �� LST_FEATURE_INFO�ṹ�塣���ýڵ���Ŀ¼�ͽڵ㣬�򷵻ص�LST_FEATURE_INFO�е�eTypeֵΪLE_NODE_TYPE.TYPE_UNKNOW��
	******************************************************/
	LST_FEATURE_INFO GetFeatureInfo();

	/******************************************************
	@����˵���� ��ȡָ�����Ƶ��ӽڵ��еĲ�����Ϣ���õ�һ��LST_FEATURE_INFO�ṹ�壬���Բ��Ҷ༶��Ŀ¼�µ��ӽڵ㡣
	@�������룺 pchNodeName -- �ڵ�����
	@��������� ��
	@����ֵ  �� LST_FEATURE_INFO�ṹ�塣���ýڵ���Ŀ¼�ͽڵ㣬�򷵻ص�LST_FEATURE_INFO�е�eTypeֵΪLE_NODE_TYPE.TYPE_UNKNOW��
	******************************************************/
	LST_FEATURE_INFO GetChildFeatureInfo(const char *pchNodeName);

	/******************************************************
	@����˵���� ���ýڵ�Ĳ�����Ϣ�����������Ϣ���浽�ڵ��С�
	@�������룺 stFeatureInfo -- ����Ĳ�����Ϣ
	@��������� ��
	@����ֵ  �� �ɹ�����true��ʧ�ܷ���false�����ڵ�ΪĿ¼�ͻ�δ֪�ͣ��򷵻�false��
	******************************************************/
	bool SetFeatureInfo(LST_FEATURE_INFO stFeatureInfo);

	/******************************************************
	@����˵���� ���ýڵ�Ĳ�����Ϣ�����������Ϣ���浽�ڵ��С�
	@�������룺 pchNodeName -- �ڵ�����
	stFeatureInfo -- ����Ĳ�����Ϣ
	@��������� ��
	@����ֵ  �� �ɹ�����true��ʧ�ܷ���false�����ڵ�ΪĿ¼�ͻ�δ֪�ͣ��򷵻�false��
	******************************************************/
	bool SetChildFeatureInfo(const char *pchNodeName, LST_FEATURE_INFO stFeatureInfo);

	/******************************************************
	@����˵���� ��ȡ��ǰ�ڵ��еĲ���ֵ�����壬�õ�һ��LU_FEATURE_VALUE�����塣
	@�������룺 ��
	@��������� ��
	@����ֵ  �� LU_FEATURE_VALUE�����塣���ýڵ���Ŀ¼�ͽڵ㣬�򷵻ص�NULL��
	******************************************************/
	LU_FEATURE_VALUE GetFeatureValue();

	/******************************************************
	@����˵���� ��ȡָ�����Ƶ��ӽڵ��еĲ���ֵ�����壬�õ�һ��LU_FEATURE_VALUE�����壬���Բ��Ҷ༶��Ŀ¼�µ��ӽڵ㡣
	@�������룺 pchNodeName -- �ڵ�����
	@��������� ��
	@����ֵ  �� LU_FEATURE_VALUE�����塣���ýڵ���Ŀ¼�ͽڵ㣬�򷵻ص�NULL��
	******************************************************/
	LU_FEATURE_VALUE GetChildFeatureValue(const char *pchNodeName);

	/******************************************************
	@����˵���� ���ýڵ�Ĳ���ֵ�����壬���������Ϣ���浽�ڵ��С�
	@�������룺 uFeatureValue -- ����Ĳ���ֵ������
	@��������� ��
	@����ֵ  �� �ɹ�����true��ʧ�ܷ���false�����ڵ�ΪĿ¼�ͻ�δ֪�ͣ��򷵻�false��
	******************************************************/
	bool SetFeatureValue(LU_FEATURE_VALUE uFeatureValue);

	/******************************************************
	@����˵���� ���ýڵ�Ĳ���ֵ�����壬���������Ϣ���浽�ڵ��С�
	@�������룺 pchNodeName -- �ڵ�����
	uFeatureValue -- ����Ĳ���ֵ������
	@��������� ��
	@����ֵ  �� �ɹ�����true��ʧ�ܷ���false�����ڵ�ΪĿ¼�ͻ�δ֪�ͣ��򷵻�false��
	******************************************************/
	bool SetChildFeatureValue(const char *pchNodeName, LU_FEATURE_VALUE uFeatureValue);

	/******************************************************
	@����˵���� ��ȡ�ýڵ���ӽڵ��б��˺��������ȡ�ڵ���һ�����ӽڵ㣬�������ȡ�ӽڵ���ӽڵ㡣
	@�������룺 ��
	@��������� ��
	@����ֵ  �� ��ָ��Ĳ������顣vector<XmlTreeNode*> �ڵ�ָ�������������������һ���ӽڵ��ָ�롣
	******************************************************/
	std::vector<XmlTreeNode*> GetChildrenNodeList();

	/******************************************************
	@����˵���� ��ȡָ�����Ƶ��ӽڵ㣬�˺������Ի�ȡ�༶��Ŀ¼�µ��ӽڵ㡣
	@�������룺 pchNodeName -- �ڵ�����
	@��������� ��
	@����ֵ  �� ��ָ�롣XmlTreeNode* �����ҵ��ӽڵ��ָ�룬��δ�ҵ��ýڵ㣬��������ΪLE_NODE_TYPE.TYPE_UNKNOW��
	******************************************************/
	XmlTreeNode* GetChildNode(const char *pchNodeName);

	/******************************************************
	@����˵���� Ϊ�ýڵ������ӽڵ㡣
	@�������룺 poTreeNode -- �ӽڵ�ָ��
	@��������� ��
	@����ֵ  �� �ɹ�����true��ʧ�ܷ���false�����Ѿ�����ͬ���Ƶ��ӽڵ����ӽڵ�����ΪLE_NODE_TYPE.TYPE_UNKNOW���򷵻�false��
	******************************************************/
	bool Add(XmlTreeNode* pTreeNode);

	/******************************************************
	@����˵���� �Ƴ�ָ�����ӽڵ㣬����ͨ��Ҫ�Ƴ����ӽڵ��ָ����������Ƴ���
	@�������룺 poTreeNode -- �ӽڵ�ָ��
	@��������� ��
	@����ֵ  �� �ɹ�����true��ʧ�ܷ���false��
	******************************************************/
	bool Remove(XmlTreeNode* pTreeNode);

	/******************************************************
	@����˵���� �Ƴ�ָ�����ӽڵ㣬����ͨ��Ҫ�Ƴ����ӽڵ��ָ����������Ƴ���
	@�������룺 pchNodeName -- Ҫ�Ƴ����ӽڵ�����
	@��������� ��
	@����ֵ  �� �ɹ�����true��ʧ�ܷ���false��
	******************************************************/
	bool Remove(const char *pchNodeName);


private:

	//��ǰ�ڵ�����
	char* m_pchNodeName;

	//��ǰ�ڵ�����
	LE_NODE_TYPE m_eNodeType;

	//�����ӽڵ������
	int m_iChildNodeCount;

	//�����ӽڵ�������ָ�룬�����vector���档
	std::vector<XmlTreeNode*> m_vecChildNode;

	//������Ϣ
	LST_FEATURE_INFO m_stFeatureInfo;

	//����ֵ
	LU_FEATURE_VALUE m_uFeatureValue;

	//��ȡ���е��ӽڵ�Ԫ�أ����ڱ���
	TiXmlElement* SaveAllElement();

	//�������е��ӽڵ�Ԫ�أ����ڶ�ȡ
	bool LoadAllElement(TiXmlElement *pElement);

	//�򸸽ڵ�����ӽڵ㣬�����ӽڵ�Ԫ�����ơ��ӽڵ�Ԫ������
	bool AddChildElement(TiXmlElement *pElement, const char *pchChildName, const char *pchChildText);

	//��ö�����͵ĸ��ڵ�����ӽڵ㣬����EnumEntryԪ���EnumEntryԪ������
	bool AddChildEnumElement(TiXmlElement *pElement, const char *pchEnumName, int iEnumValue);

	//�Եݹ�ķ�ʽ���ͷ������ӽڵ㲢��յ�ǰvector
	bool FreeChildNode();

};


#endif   //LST_XML_MANAGER_CF_H_