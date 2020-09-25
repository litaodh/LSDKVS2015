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
	
	// ���ý�����Ϣ�Ĵ���
	BOOL SetAcceptMsgWnd(const HWND hWnd);
	
	// ��ȡ��ǰĿ¼
	CString GetPath();
	// ������Ϣ����
	afx_msg void OnTvnSelchanged(NMHDR * pNMHDR, LRESULT * pResult);
	//void OnTvnSelchanged2(NMHDR * pNMHDR, LRESULT * pResult);
	// ����ͼ���б�
	BOOL SetTreeImageList(CImageList * ImgList, int nImgList);

	// ��ʼ����Ŀ¼
	BOOL InitRootDirectory();
	// �������ڵ�
	BOOL UpdateTreeNode(const unsigned long nIndex);
	// �жϵ�ǰ�ڵ㼶��
	int JudgeFloor();
	// �������ڵ�
	BOOL InsertTreeNode(const CString strRoot, const unsigned long nIndex);
	// �ݹ����
	BOOL InsertRecursion(const CString strRoot, int nFileNum, const unsigned long nIndex);
	// ����ͼ���б�����
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
	CString m_strRoot;                            // ��Ŀ¼
	HWND m_hAcceptMessage;                        // ������Ϣ�Ĵ���
	std::vector<HTREEITEM> m_vecTreeTop;          // Ŀ¼�ڵ�����ÿһ��Ԫ�ش���һ������Ŀ¼
	std::vector<HTREEITEM> m_vecHierarchy;        // Ŀ¼�ڵ����ڱ�������Ŀ¼�µ���Ŀ¼
	std::vector<CString> m_vecUnwantedString;     // ������������νṹ�ϵ�Ŀ¼�����б�
	CImageList m_ImgList;                      // Ŀ¼��ͼ���б�
	int m_nImgList;
	HTREEITEM m_hItemRoot;

};