/***********************************************************************
*
* Copyright (c) 2000-2001 Luster LightTech(Beijing) Co.Ltd.
* All Rights Reserved.
*
*
* FILE NAME     :  LstXmlManagerCF.h
* PROJECT NAME  :  LSDK
* ABSTRACT      :  XML相机参数管理接口，主要解决如何去配置相机参数的问题。
*				   详细参见《XML相机参数管理接口详细设计说明_v1.1.0》
*
*
* VERSION       :  1.1.0
* DATE 		    :  2018/04/14
* AUTHOR 		:  刘麒
* NOTE 		    :  删除不必要的tinyxml头文件、using namespace std、宏定义等。
***********************************************************************/
#pragma once

#ifndef LST_XML_MANAGER_CF_H_
#define LST_XML_MANAGER_CF_H_

#include <vector>
#include "LstCommonDef.h"


/*
*struct
*/

//参数数据联合体，包括整形数据信息、浮点型数据信息、字符串属性信息、布尔型数据信息、枚举型数据信息、预留字段。
typedef union tagUnionFeatureValue
{
	//整形数据信息
	LST_INT_INFO stInt;
	//浮点型数据信息
	LST_DOUBLE_INFO stDouble;
	//字符串数据信息
	LST_STRING_INFO stString;
	//布尔型数据信息
	LST_BOOLEAN_INFO stBoolean;
	//枚举型数据信息
	LST_ENUM_INFO stEnum;
	//预留字段
	char achReserved[LEN_256];
} LU_FEATURE_VALUE;


class TiXmlElement;//tinyxml类
class XmlTreeNode;
template class __declspec(dllexport) std::allocator<XmlTreeNode*>;
template class __declspec(dllexport) std::vector<XmlTreeNode*, std::allocator<XmlTreeNode*>>;


#ifdef LSTXMLMANAGERCF_EXPORTS
#define LXMCF_API __declspec(dllexport)
#else
#define LXMCF_API __declspec(dllimport)
#endif

/******************************************************
每一个XmlTreeNode类代表参数结构树状图中的一个节点，开发人员可通过Add接口不断向根节点中一层一层地添加新的子节点，
最终形成一棵相机参数的树结构。同时，XmlTreeNode类在接口设计中，提供了遍历子节点、获得子节点、获得子节点信息等接口，
方便开发人员直接调用的接口，可以快速查询指定名称的节点信息。最后，XmlTreeNode类亦提供了加载xml文件、保存xml文件的接口，
实现了与本地数据沟通的功能。XmlTreeNode类使用了组合模式。
******************************************************/
class LXMCF_API XmlTreeNode
{
public:

	/******************************************************
	@函数说明： 构造函数。
	@参数输入： pchName -- 节点名称
	eType   -- 节点类型
	@参数输出： 无
	@返回值  ： XmlTreeNode对象
	******************************************************/
	XmlTreeNode(const char *pchNodeName, LE_NODE_TYPE eNodeType);

	/******************************************************
	@函数说明： 析构函数。
	@参数输入： 无
	@参数输出： 无
	@返回值  ： 无
	******************************************************/
	~XmlTreeNode(void);

	/******************************************************
	@函数说明： 获得节点名称。
	@参数输入： 无
	@参数输出： 无
	@返回值  ： 当前的节点名称。
	******************************************************/
	char* NodeName();

	/******************************************************
	@函数说明： 获得节点类型。
	@参数输入： 无
	@参数输出： 无
	@返回值  ： 当前的节点类型。
	******************************************************/
	LE_NODE_TYPE NodeType();

	/******************************************************
	@函数说明： 加载xml文件，读取整个xml文件中的信息并保存到一个参数树中，调用该函数的节点将会成为根节点。
	@参数输入： pchXmlFileName -- 待加载的xml文件名，绝对路径。
	@参数输出： 无
	@返回值  ： 成功返回true，失败返回false。
	******************************************************/
	bool LoadXml(const char *pchXmlFilePath);

	/******************************************************
	@函数说明： 将整个参数树中的信息保存到一个xml文件中。注意：该函数仅根节点可调用(节点名称为root，否则返回false)。
	@参数输入： pchXmlFileName -- 待加载的xml文件名，绝对路径。
	@参数输出： 无
	@返回值  ： 成功返回true，失败返回false。
	******************************************************/
	bool SaveXml(const char *pchXmlFilePath);

	/******************************************************
	@函数说明： 获取当前节点中的参数信息，得到一个LST_FEATURE_INFO结构体。
	@参数输入： 无
	@参数输出： 无
	@返回值  ： LST_FEATURE_INFO结构体。若该节点是目录型节点，则返回的LST_FEATURE_INFO中的eType值为LE_NODE_TYPE.TYPE_UNKNOW。
	******************************************************/
	LST_FEATURE_INFO GetFeatureInfo();

	/******************************************************
	@函数说明： 获取指定名称的子节点中的参数信息，得到一个LST_FEATURE_INFO结构体，可以查找多级子目录下的子节点。
	@参数输入： pchNodeName -- 节点名称
	@参数输出： 无
	@返回值  ： LST_FEATURE_INFO结构体。若该节点是目录型节点，则返回的LST_FEATURE_INFO中的eType值为LE_NODE_TYPE.TYPE_UNKNOW。
	******************************************************/
	LST_FEATURE_INFO GetChildFeatureInfo(const char *pchNodeName);

	/******************************************************
	@函数说明： 设置节点的参数信息，将输入的信息保存到节点中。
	@参数输入： stFeatureInfo -- 输入的参数信息
	@参数输出： 无
	@返回值  ： 成功返回true，失败返回false。若节点为目录型或未知型，则返回false。
	******************************************************/
	bool SetFeatureInfo(LST_FEATURE_INFO stFeatureInfo);

	/******************************************************
	@函数说明： 设置节点的参数信息，将输入的信息保存到节点中。
	@参数输入： pchNodeName -- 节点名称
	stFeatureInfo -- 输入的参数信息
	@参数输出： 无
	@返回值  ： 成功返回true，失败返回false。若节点为目录型或未知型，则返回false。
	******************************************************/
	bool SetChildFeatureInfo(const char *pchNodeName, LST_FEATURE_INFO stFeatureInfo);

	/******************************************************
	@函数说明： 获取当前节点中的参数值联合体，得到一个LU_FEATURE_VALUE联合体。
	@参数输入： 无
	@参数输出： 无
	@返回值  ： LU_FEATURE_VALUE联合体。若该节点是目录型节点，则返回的NULL。
	******************************************************/
	LU_FEATURE_VALUE GetFeatureValue();

	/******************************************************
	@函数说明： 获取指定名称的子节点中的参数值联合体，得到一个LU_FEATURE_VALUE联合体，可以查找多级子目录下的子节点。
	@参数输入： pchNodeName -- 节点名称
	@参数输出： 无
	@返回值  ： LU_FEATURE_VALUE联合体。若该节点是目录型节点，则返回的NULL。
	******************************************************/
	LU_FEATURE_VALUE GetChildFeatureValue(const char *pchNodeName);

	/******************************************************
	@函数说明： 设置节点的参数值联合体，将输入的信息保存到节点中。
	@参数输入： uFeatureValue -- 输入的参数值联合体
	@参数输出： 无
	@返回值  ： 成功返回true，失败返回false。若节点为目录型或未知型，则返回false。
	******************************************************/
	bool SetFeatureValue(LU_FEATURE_VALUE uFeatureValue);

	/******************************************************
	@函数说明： 设置节点的参数值联合体，将输入的信息保存到节点中。
	@参数输入： pchNodeName -- 节点名称
	uFeatureValue -- 输入的参数值联合体
	@参数输出： 无
	@返回值  ： 成功返回true，失败返回false。若节点为目录型或未知型，则返回false。
	******************************************************/
	bool SetChildFeatureValue(const char *pchNodeName, LU_FEATURE_VALUE uFeatureValue);

	/******************************************************
	@函数说明： 获取该节点的子节点列表，此函数仅会获取节点下一级的子节点，并不会获取子节点的子节点。
	@参数输入： 无
	@参数输出： 无
	@返回值  ： 类指针的不定数组。vector<XmlTreeNode*> 节点指针的容器，包含所有下一级子节点的指针。
	******************************************************/
	std::vector<XmlTreeNode*> GetChildrenNodeList();

	/******************************************************
	@函数说明： 获取指定名称的子节点，此函数可以获取多级子目录下的子节点。
	@参数输入： pchNodeName -- 节点名称
	@参数输出： 无
	@返回值  ： 类指针。XmlTreeNode* 所查找的子节点的指针，若未找到该节点，则其类型为LE_NODE_TYPE.TYPE_UNKNOW。
	******************************************************/
	XmlTreeNode* GetChildNode(const char *pchNodeName);

	/******************************************************
	@函数说明： 为该节点增加子节点。
	@参数输入： poTreeNode -- 子节点指针
	@参数输出： 无
	@返回值  ： 成功返回true，失败返回false。若已经有相同名称的子节点或该子节点类型为LE_NODE_TYPE.TYPE_UNKNOW，则返回false。
	******************************************************/
	bool Add(XmlTreeNode* pTreeNode);

	/******************************************************
	@函数说明： 移除指定的子节点，可以通过要移除的子节点的指针或名称来移除。
	@参数输入： poTreeNode -- 子节点指针
	@参数输出： 无
	@返回值  ： 成功返回true，失败返回false。
	******************************************************/
	bool Remove(XmlTreeNode* pTreeNode);

	/******************************************************
	@函数说明： 移除指定的子节点，可以通过要移除的子节点的指针或名称来移除。
	@参数输入： pchNodeName -- 要移除的子节点名称
	@参数输出： 无
	@返回值  ： 成功返回true，失败返回false。
	******************************************************/
	bool Remove(const char *pchNodeName);


private:

	//当前节点名称
	char* m_pchNodeName;

	//当前节点类型
	LE_NODE_TYPE m_eNodeType;

	//所有子节点的数量
	int m_iChildNodeCount;

	//所有子节点的类对象指针，存放于vector里面。
	std::vector<XmlTreeNode*> m_vecChildNode;

	//参数信息
	LST_FEATURE_INFO m_stFeatureInfo;

	//参数值
	LU_FEATURE_VALUE m_uFeatureValue;

	//获取所有的子节点元素，用于保存
	TiXmlElement* SaveAllElement();

	//加载所有的子节点元素，用于读取
	bool LoadAllElement(TiXmlElement *pElement);

	//向父节点添加子节点，给入子节点元素名称、子节点元素内容
	bool AddChildElement(TiXmlElement *pElement, const char *pchChildName, const char *pchChildText);

	//向枚举类型的父节点添加子节点，给入EnumEntry元素项、EnumEntry元素内容
	bool AddChildEnumElement(TiXmlElement *pElement, const char *pchEnumName, int iEnumValue);

	//以递归的方式，释放所有子节点并清空当前vector
	bool FreeChildNode();

};


#endif   //LST_XML_MANAGER_CF_H_