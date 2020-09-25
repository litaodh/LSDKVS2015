// Modified from https://www.cnblogs.com/konglongdanfo/p/9189864.html

/**
|	Class CSelfUpdateTreeCtrl Head File.
|
|	- Notes:
|	Self-defineded class to control TreeCtrl in MFC.
|
*/

#pragma once
#include <vector>
#include <map>

#define WM_SELFUPDATETREECTRL_SELCHANGED WM_USER + 779

typedef std::map<std::string, std::vector<std::string>> R3_PARAS;
//typedef std::map<std::string, std::vector<R3SDK::Parameter>>	Params;

class CSelfUpdateTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CSelfUpdateTreeCtrl)

public:
	CSelfUpdateTreeCtrl();
	CSelfUpdateTreeCtrl(CString strPath);
	virtual ~CSelfUpdateTreeCtrl();
	CSelfUpdateTreeCtrl(CSelfUpdateTreeCtrl & ob);
	CSelfUpdateTreeCtrl & operator= (CSelfUpdateTreeCtrl & ob);
protected:
	DECLARE_MESSAGE_MAP()

public:
	void InitTreeFile();
	void InitTreeFileCSV();
	
	//BOOL SetTreeRoot(const CString strPath);
	//BOOL SetTreeRootCSV();

	/** @brief DrawTree() 													   \n
	|																		   \n
	|	Draw all items.														   \n
	*/
	BOOL DrawTree(const std::vector<std::string> _cate, const R3_PARAS _pa);

	
	//void SetUnwantedString(const std::vector<CString> vecStr);
	
	// 设置接收消息的窗口
	BOOL SetAcceptMsgWnd(const HWND hWnd);
	
	// 获取当前目录
	CString GetPath();
	// 发送消息函数
	afx_msg void OnTvnSelchanged(NMHDR * pNMHDR, LRESULT * pResult);
	//void OnTvnSelchanged2(NMHDR * pNMHDR, LRESULT * pResult);
	// 设置图标列表
	BOOL SetTreeImageList(CImageList * ImgList, int nImgList);

	// 初始化根目录
	BOOL InitRootDirectory();
	// 更新树节点
	BOOL UpdateTreeNode(const unsigned long nIndex);
	// 判断当前节点级别
	int JudgeFloor();
	// 插入树节点
	BOOL InsertTreeNode(const CString strRoot, const unsigned long nIndex);
	// 递归插入
	BOOL InsertRecursion(const CString strRoot, int nFileNum, const unsigned long nIndex);
	// 拷贝图标列表数据
	BOOL SetTreeImageList(CImageList * ImgList);

	BOOL InsertParameter(const CString name, const unsigned long ulNum);

	void ExpandTree(HTREEITEM hTreeItem);


protected:
	/** @brief CalTreeID()													  \n
	|																		  \n
	|	Formulation : id += levels[i] * NperL * (LDepth - i);				  \n
	|																		  \n
	|	@Param levels : Levels, like coordinate								  \n
	|	@Param NperL  : Number of items in each level						  \n
	*/
	int CalTreeID(const std::vector<size_t> levels, const size_t NperL);


private:
	CString m_strRoot;                            // 根目录
	HWND m_hAcceptMessage;                        // 接收消息的窗口
	std::vector<HTREEITEM> m_vecTreeTop;          // 目录节点树，每一个元素代表一个顶级目录
	std::vector<HTREEITEM> m_vecHierarchy;        // 目录节点用于遍历顶级目录下的子目录
	std::vector<CString> m_vecUnwantedString;     // 不想出现在树形结构上的目录名称列表
	CImageList m_ImgList;                      // 目录树图标列表
	int m_nImgList;
	HTREEITEM m_hItemRoot;

};