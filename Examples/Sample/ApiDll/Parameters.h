/**	@file	Parameters.h
*
*	@brief	Define Class Selector, Parameter and ParameterVector.
*
*	@attention
*	These classes are used to parameter setting and getting.
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
#include "GenIRanger.h"
#include "Consumer.h"
#include <CalibrationWrapper.h>

#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include <map>
  
using namespace GENAPI_NAMESPACE;
using namespace GENICAM_NAMESPACE;
using namespace GenIRanger;

#define VERSION			1
#define DEFAULT_BUF_COT	20
#define DEFAULT_TIMEOUT 2000


namespace SickCam
{

////////////////////////////////////////////////////////////////////////////////

class Selector
{
public:
	Selector		();
	~Selector		();

	typedef std::shared_ptr<Selector> SelecPtr;
	bool				intiSelector(const std::string & _line, const std::string & son_key_value);
	// from string
	bool				hasParent	()	const;
	bool				isValid		()	const { return !(m_key.empty() || m_value.empty()); };
	SelecPtr			getParent	();
	const std::string&	getKey		()	const { return m_key;		};
	const std::string&	getValue	()	const { return m_value;		};

private:
	std::string		m_key;
	std::string		m_value;		///< Selcector points to this value
	std::string		m_parentKey;	///< only one

public:
	static std::map<std::string, SelecPtr>	mapSelectors;
};

typedef std::shared_ptr<Selector> SelecPtr;


////////////////////////////////////////////////////////////////////////////////


/** @brief Class of one parameter, conttains some basic info like name \ value \ options etc. */
class Parameter
{
public:
	Parameter	()	
		:	m_name(""), m_category(""), m_selecPtr(nullptr), m_csvName(""), 
		m_strValue(""), m_options(std::vector<std::string>()), m_readOnly(true)
	{}

	~Parameter	()
	{}

	bool initValueFromDevice	(const std::string & _aLine, const std::string & _category);

	const std::string&			getName		()		const { return m_name; };
	const std::string&			getCategory	()		const { return m_category; };
	const std::string&			getCsvName	()		const { return m_csvName; };
	const std::string&			getStrValue	()		const { return m_strValue;				};
	SelecPtr					getSeletor	()			  { return m_selecPtr;				};
	const std::vector<std::string>&	
								getOptions	()		const { return m_options; };
	bool						isValid		()		const { return !m_strValue.empty();		};
	bool						isReadOnly	()		const { return m_readOnly; };

	bool setValueToDevice		(const GenApi::CNodeMapRef & _nodeMap, const std::string & _v)	;
	bool getValueFromDevice		(const GenApi::CNodeMapRef & _nodeMap, std::string & _v);
	//todo:myParamer
	bool getValueConstraintFromDevice(const GenApi::CNodeMapRef & _nodeMap, std::string & _minV,std::string & _maxV,std::string & _incV);


protected:
	bool modifyValue			(const std::string & _v);
	bool isWithInOptitons		(const std::string & _v) const;
	bool moveSelector			(const GenApi::CNodeMapRef & _nodeMap);


private:
	std::string					m_name;			///< same name as in Ranger3Studio, as nodeName also
	std::string					m_category;		///< #name in csv file
	SelecPtr					m_selecPtr;		///< selectors queue, should be last
	std::string					m_csvName;		///< same name as in csv file, "NodeName_SelectorKey_SeletorValue"
	std::string					m_strValue;		///< value in string format
	std::vector<std::string>	m_options;		///< only used in enum parameter
	bool						m_readOnly;	

};


////////////////////////////////////////////////////////////////////////////////


template<class T> using Ptr = std::shared_ptr<T>;
typedef Ptr<Parameter>									ParaPtr;
typedef std::vector<std::string>						Categories;
//typedef std::map<std::string, std::vector<std::string>>	Parameters;
typedef std::map<std::string, std::vector<ParaPtr>>		Params;
typedef std::map<std::string, ParaPtr>					AllParams;
typedef SiCaliWrapper::RectMethod						RectMethod;

/** @brief Class to hold and process all parameters.
*
* 
* ## 1 There defines 3 kinds of parameters. 
*
* Extra Parameters | NULL
* ---------------- | -------------
* RectMethod	   | m_RectMethod;	
* double		   | m_scaleY;		
* float			   | m_missingData;	
* std::string	   | m_CaliPath; 	   	
* std::string	   | m_ParaPath;	   	
* uint64_t		   | m_timeOut;		
*
*
* User Parameters | not saved in device
* --------------- | -------------------------
*   RectMethod	  | m_RectMethod;		
*   double		  | m_scaleY;			
*   float		  | m_missingData;		
*   std::string	  | m_CaliPath; 	   		
*   std::string	  | m_ParaPath;	   		
*   uint64_t	  | m_timeOut;			
*
*
* Device Parameters | Saved in device, can be found in csv file
* ------------------|-----------------------
*  RectMethod		| m_RectMethod;		
*  double			| m_scaleY;			
*  float			| m_missingData;		
*  std::string		| m_CaliPath; 	   		
*  std::string		| m_ParaPath;	   		
*  uint64_t			| m_timeOut;
*
*
* ## 2. Mac can not be set.
* ## 3. IP and Subnet both have 2 kinds of setting.

* Camera Status	| Special name		| Notes									
* --------------| ------------------| -------------------------------------------
* disconnect	| ForceIP			| Ip and subnet settings is temporary. They will be recoverd when power off.
* connect		| [No special name]	| Ip and subnet settings is permanent. They will be kept when power off.
* 
*
*/																						   
class ParameterVector
{
public:
	ParameterVector		();
	~ParameterVector	();
	
	/** @brief Traverse all avaliable parameters of device. 
	*																								
	*	1. Scan parameters of the device.
	*	2. Save all avaliable parameters as <"parameter.tmp"> from device.	
	*	3. Prase the file, and initialize Class ParameterVector.
	*	4. Delete <"parameter.tmp">.
	*
	*	@param [in] nodeMap Node map of device parameters.
	*	@param [in] path Temperary saving path.
	*
	*/
	bool ScanDeviceParameters	(INodeMap * const nodeMap, const std::string path = "parameter.tmp");

	bool		isValid				()	const { return !m_AllParams.empty();	};
	const Categories& getCategories	()	const {	return	m_Categories;			};
	//Parameters	getParameters()		const {	return	m_Parameters;			};
	const Params	& getParams		()	const {	return	m_Params;				};
	const AllParams	& getAllParams	()	const {	return	m_AllParams;			};

	/**	@brief Get Device Parameters
	*
	*	@param [in] nodeMap			Node map of device parameters.
	*	@param [in] ParamterName	The name listed in csv file.
	*	@param [in] value			New value to be set.
	*
	*	@note Avaliable input string names are listed in csv file.
	*
	*/
	bool getParameter(	const GenApi::CNodeMapRef	& nodeMap, 
						const std::string			& ParamterName, 
						std::string					& value);

	//todo:my
	bool getConstraintParameter(const GenApi::CNodeMapRef	& nodeMap,
		const std::string			& ParamterName,
		std::string                 & minV,
		std::string                 & maxV,
		std::string                 & incV);
	

	const RectMethod  & getRectMethos	()	const {	return m_RectMethod;	};
	const double	  & getScaleY		()	const {	return m_scaleY;		};
	const float		  & getMissingData	()	const {	return m_missingData;	};
	const std::string & getCalibraPath	()	const {	return m_CaliPath;		};
	const std::string & getParametPath	()	const {	return m_ParaPath;		};
	const uint64_t	  & getTimeOut		()	const {	return m_timeOut;		};

	//const std::string & getMAC			(const GenApi::CNodeMapRef & nodeMap, 
	//									 const int64_t & id,
	//									 const bool isPersistent);

	const bool		    getDoRectify	()	const { return m_doRectify;		};
	const size_t	  & getBufferCount	()	const { return m_buffersCount;	};
	const std::string & getTmpPath		()	const {	return m_TmpPath;		};

	/** @brief	Get IP	
	*
	*	@param [in] nodeMap			Node map of device parameters.
	*	@param [in] interfaceHandle	Interface handle of connecting device.
	*	@param [in] tl				Interface api pointer.
	*	@param [in] id				Id of the device in interface.
	*	@param [in] isPersistent	If isPersistent == true, process forceIp; else, process permanent IP setting.
	*/
	std::string getIP		(const GenApi::CNodeMapRef	& nodeMap, 
							 const GenTL::IF_HANDLE		& interfaceHandle,
							 const GenTLApi				* tl, 
							 const int64_t				& id, 
							 const bool					isPersistent);
	
	/** @brief	Get Device subnet
	*
	*	@param [in] nodeMap			Node map of device parameters.
	*	@param [in] interfaceHandle	Interface handle of connecting device.
	*	@param [in] tl				Interface api pointer.
	*	@param [in] id				Id of the device in interface.
	*	@param [in] isPersistent	If isPersistent == true, process forceIp; else, process permanent IP setting.
	*/
	std::string getSubnet	(const GenApi::CNodeMapRef	& nodeMap, 
							 const GenTL::IF_HANDLE		& interfaceHandle, 
							 const GenTLApi				* tl,
							 const int64_t				& id,
							 const bool					isPersistent);


	/** @brief setParameter
	*
	*	Set Device Parameters
	*
	*	@note Avaliable input string names are listed in csv file.
	*
	*/
	bool setParameter		(const GenApi::CNodeMapRef & nodeMap, const std::string & paramterName, const std::string & value);
	
	bool setRectMethos		(const RectMethod	& value)	{	m_RectMethod	= value;	return value == getRectMethos	();};
	bool setScaleY			(const double		& value)	{	m_scaleY		= value;	return value == getScaleY		();};
	bool setMissingData		(const float		& value)	{	m_missingData	= value;	return value == getMissingData	();};
	bool setCalibraPath		(const std::string	& value)	{	m_CaliPath		= value;	return value == getCalibraPath	();};
	bool setParametPath		(const std::string	& value)	{	m_ParaPath		= value;	return value == getParametPath	();};
	bool setTimeOut			(const uint64_t		& value)	{	m_timeOut		= value;	return value == getTimeOut		();};
	bool setDoRectify		(const bool			  value)	{	m_doRectify		= value;	return value == getDoRectify	();};		
	bool setBufferCount		(const size_t		& value)	{   m_buffersCount	= value;	return value == getBufferCount	();};
	bool setTmpPath			(const std::string	& value)	{	m_TmpPath		= value;	return value == getTmpPath		();};

	/**	@param [in] isPersistent : if isPersistent == true, process forceIp; else, process permanent IP setting.*/
	bool setIP				(const GenApi::CNodeMapRef	& nodeMap, 
							 const GenTL::IF_HANDLE		& interfaceHandle,
							 const GenTLApi				* tl, 
							 const int64_t				& id, 
							 const std::string			& IP, 
							 const bool isPersistent = false);

	/**	@param [in] isPersistent : if isPersistent == true, process forceIp; else, process permanent IP setting.*/
	bool setSubnet			(const GenApi::CNodeMapRef	& nodeMap, 
							 const GenTL::IF_HANDLE		& interfaceHandle,
							 const GenTLApi				* tl, 
							 const int64_t				& id, 
							 const std::string			& subNet, 
							 const bool isPersistent = false);

protected:
	/** Both IP and Subnet are necessary. */
	bool _forceIP			(const GenApi::CNodeMapRef	& nodeMap,
							 const GenTL::IF_HANDLE		& interfaceHandle,
							 const GenTLApi				* tl,
							 const int64_t				& id, 
							 const std::string			& IP, 
							 const std::string			& subNet);

	bool _persistentIP		(const GenApi::CNodeMapRef	& nodeMap, 
							 const GenTL::IF_HANDLE		& interfaceHandle,
							 const GenTLApi				* tl,
							 const int64_t				& id,
							 const std::string			& IP, 
							 const std::string			& subNet);

private:
	Categories		m_Categories;
	Params			m_Params;			///< key is category, for CMD. Need update after seting, base
	AllParams		m_AllParams;		///< key is csv_name, for device parameters settings. Need update after seting
	
	RectMethod		m_RectMethod;		///< user input
	double			m_scaleY;			///< user input
	float			m_missingData;		///< user input
	std::string		m_CaliPath; 		///< user input
	std::string		m_ParaPath;			///< user input
	uint64_t		m_timeOut;			///< user input
	bool			m_doRectify;		///< user input temp parameter
	size_t			m_buffersCount;		///< user input
	std::string		m_TmpPath;			///< user inpur Unchanged, no showing in parameter list.

};

}

