// https://www.cnblogs.com/konglongdanfo/p/9189864.html
//
// 源文件
// SelfUpdateTreeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "CSelfUpdateTreeCtrl.h"
#include <algorithm>

// CSelfUpdateTreeCtrl

IMPLEMENT_DYNAMIC(CSelfUpdateTreeCtrl, CTreeCtrl)

CSelfUpdateTreeCtrl::CSelfUpdateTreeCtrl()
{
	m_strRoot = "";
	m_hAcceptMessage = NULL;
	m_nImgList = LVSIL_NORMAL;
	
}

CSelfUpdateTreeCtrl::CSelfUpdateTreeCtrl(CString strPath) : m_strRoot(strPath)
{
	m_hAcceptMessage = NULL;
	m_nImgList = LVSIL_NORMAL;
	InitRootDirectory();
}

CSelfUpdateTreeCtrl::~CSelfUpdateTreeCtrl()
{
	if (NULL != m_ImgList.GetSafeHandle())
	{
		m_ImgList.DeleteImageList();
		ASSERT(m_ImgList.GetSafeHandle() == NULL);
	}
}

CSelfUpdateTreeCtrl::CSelfUpdateTreeCtrl(CSelfUpdateTreeCtrl & ob)
{
	m_strRoot = ob.m_strRoot;
	m_hAcceptMessage = ob.m_hAcceptMessage;
	if (!m_vecTreeTop.empty())
	{
		m_vecTreeTop.clear();
	}
	if (!m_vecHierarchy.empty())
	{
		m_vecHierarchy.clear();
	}
	if (!m_vecUnwantedString.empty())
	{
		m_vecUnwantedString.clear();
	}
	m_vecTreeTop = ob.m_vecTreeTop;
	m_vecHierarchy = ob.m_vecHierarchy;
	m_vecUnwantedString = ob.m_vecUnwantedString;
	
	m_nImgList = ob.m_nImgList;
	SetTreeImageList(&(ob.m_ImgList));
}

CSelfUpdateTreeCtrl & CSelfUpdateTreeCtrl::operator= (CSelfUpdateTreeCtrl & ob)
{
	//CTreeCtrl::operator= (ob);
	if (this == &ob)
		return *this;
	m_strRoot = ob.m_strRoot;
	m_hAcceptMessage = ob.m_hAcceptMessage;
	if (!m_vecTreeTop.empty())
	{
		m_vecTreeTop.clear();
	}
	if (!m_vecHierarchy.empty())
	{
		m_vecHierarchy.clear();
	}
	if (!m_vecUnwantedString.empty())
	{
		m_vecUnwantedString.clear();
	}
	m_vecTreeTop = ob.m_vecTreeTop;
	m_vecHierarchy = ob.m_vecHierarchy;
	m_vecUnwantedString = ob.m_vecUnwantedString;
	// 图标列表
	m_nImgList = ob.m_nImgList;
	if (m_ImgList.GetSafeHandle())
	{
		m_ImgList.DeleteImageList();
		ASSERT(m_ImgList.GetSafeHandle() == NULL);
	}
	SetTreeImageList(&(ob.m_ImgList));

	return *this;
}

BEGIN_MESSAGE_MAP(CSelfUpdateTreeCtrl, CTreeCtrl)
	//ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CSelfUpdateTreeCtrl::OnTvnSelchanged)
END_MESSAGE_MAP()


// CSelfUpdateTreeCtrl 消息处理程序
void CSelfUpdateTreeCtrl::InitTreeFile()
{
	if (!m_vecTreeTop.empty())
	{
		m_vecTreeTop.clear();
		DeleteAllItems();
	}
	SetImageList(&m_ImgList, m_nImgList);
	InitRootDirectory();
}

void CSelfUpdateTreeCtrl::InitTreeFileCSV()
{
	if (!m_vecTreeTop.empty())
	{
		m_vecTreeTop.clear();
		DeleteAllItems();
	}
	SetImageList(&m_ImgList, m_nImgList);
	//InitRootDirectory();
}

#if 0
BOOL CSelfUpdateTreeCtrl::SetTreeRoot(CString strPath)
{
	m_strRoot = strPath;
	return InitRootDirectory();
}

BOOL CSelfUpdateTreeCtrl::SetTreeRootCSV()
{
	if (!m_vecTreeTop.empty())
	{
		m_vecTreeTop.clear();
	}

	m_hItemRoot = InsertItem(CString("Parameters"), 0, 0, NULL);

	InsertParameter(CString("DeviceControl"), 0);
	InsertParameter(CString("ImageFormatContol"), 1);
	InsertParameter(CString("Scan3dControl"), 2);
	InsertParameter(CString("AcquisitionControl"), 3);
	InsertParameter(CString("DigitalIOControl"), 4);
	InsertParameter(CString("TimerControl"), 5);
	InsertParameter(CString("EncoderControl"), 6);
	InsertParameter(CString("EventControl"), 7);
	InsertParameter(CString("FileAccessControl"), 8);
	InsertParameter(CString("ChunkDataControl"), 9);
	InsertParameter(CString("TestControl"), 10);
	InsertParameter(CString("TransportLayerControl"), 11);
	InsertParameter(CString("FirmwareUpdate"), 12);

	ExpandTree(m_hItemRoot);

	return TRUE;
}

#endif // 0

BOOL CSelfUpdateTreeCtrl::DrawTree(const std::vector<std::string> _cate, const R3_PARAS _pa)
{
	if (_pa.empty()) return FALSE;

	if (!m_vecTreeTop.empty())	m_vecTreeTop.clear();

	m_hItemRoot = InsertItem(CString("Parameters"), 0, 0, NULL);

	int nPerL(50);
	for (size_t i=0; i<_cate.size(); ++i)
	{
		std::vector<size_t> levels = { i, 0, 0, 0 };
		InsertParameter(CString(_cate[i].c_str()), CalTreeID(levels, nPerL));

		if (_pa.count(_cate[i]) == 1)
		{
			std::vector<std::string> paras(_pa.at(_cate[i]));
			if (paras.size() <= levels.size())
			{
				for (size_t j = 0; j < paras.size(); ++j)
				{
					levels[j] += 1;
					InsertParameter(CString(paras[j].c_str()), CalTreeID(levels, nPerL));
				}
			}
		}
	}

	ExpandTree(m_hItemRoot);

	return TRUE;
}

//void CSelfUpdateTreeCtrl::SetUnwantedString(std::vector<CString> vecStr)
//{
//	m_vecUnwantedString = vecStr;
//}

BOOL CSelfUpdateTreeCtrl::SetAcceptMsgWnd(HWND hWnd)
{
	if (hWnd)
	{
		m_hAcceptMessage = hWnd;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

CString CSelfUpdateTreeCtrl::GetPath()
{
	HTREEITEM CurrentNode = GetSelectedItem();
	HTREEITEM ParentNode = GetParentItem(CurrentNode);
	CString strPath = GetItemText(ParentNode);
	CString strSelf = GetItemText(CurrentNode);
	if ("" == strPath)
	{
		strPath += strSelf + _T("\\");
	}
	else
	{
		strPath += _T("\\") + strSelf + _T("\\");
	}
	while ((ParentNode = GetParentItem(ParentNode)) != NULL)
	{
		CString strTemp = GetItemText(ParentNode);
		strPath = strTemp + _T("\\") + strPath;
	}
	strPath = m_strRoot + _T("\\") + strPath;
	return strPath;
}

void CSelfUpdateTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	int nFloor = JudgeFloor();
	
	if (0 == nFloor && !ItemHasChildren(GetSelectedItem()))
	{
		unsigned long ulNum = static_cast<unsigned long>(GetItemData(GetSelectedItem()));
	}
	else
	{
		if (m_hAcceptMessage)
		{
			::PostMessage(m_hAcceptMessage, WM_SELFUPDATETREECTRL_SELCHANGED, 0, 0);
		}
	}
	*pResult = 0;
}



BOOL CSelfUpdateTreeCtrl::SetTreeImageList(CImageList * ImgList, int nImgList)
{
	if ((NULL == ImgList->GetSafeHandle()) || (ImgList == &m_ImgList))
	{
		return FALSE;
	}
	if (m_ImgList.GetSafeHandle())
	{
		m_ImgList.DeleteImageList();
		ASSERT(m_ImgList.GetSafeHandle() == NULL);
	}
	m_ImgList.Create(ImgList);
	m_nImgList = nImgList;
	SetImageList(&m_ImgList, m_nImgList);
	return TRUE;
}

BOOL CSelfUpdateTreeCtrl::InitRootDirectory()
{
	if ("" == m_strRoot)
	{
		return FALSE;
	}
	if (!m_vecTreeTop.empty())
	{
		m_vecTreeTop.clear();
	}
	CFileFind file;
	CString strDirectory = m_strRoot;
	if (strDirectory.Right(1) != "\\")
	{
		strDirectory += _T("\\");
	}
	strDirectory += _T("*.*");
	BOOL bRet = file.FindFile(strDirectory);
	unsigned long ulNum = 0; // 给每个结点设置索引号
	while (bRet)
	{
		bRet = file.FindNextFile();  // 是否有下一个目录
		if (file.IsDirectory() && !file.IsDots())
		{
			CString strPath = file.GetFilePath();
			CString strTitle = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\') - 1);
			HTREEITEM hItem = InsertItem(strTitle, 0, 0, NULL);
			m_vecTreeTop.push_back(hItem);
			SetItemData(hItem, ulNum);
			ulNum++;
		}
	}
	file.Close();
	return TRUE;
}

BOOL CSelfUpdateTreeCtrl::UpdateTreeNode(unsigned long nIndex)
{
	// 调用插入节点函数
	return InsertTreeNode(GetPath(), nIndex);
}

int CSelfUpdateTreeCtrl::JudgeFloor()
{
	int nDepth = 0;
	HTREEITEM hCurrentNode = GetSelectedItem();
	while ((hCurrentNode = GetParentItem(hCurrentNode)) != NULL)
	{
		nDepth++;
	}
	return nDepth;
}

BOOL CSelfUpdateTreeCtrl::InsertTreeNode(CString strRoot, unsigned long nIndex)
{
	if ("" == strRoot || nIndex >= m_vecTreeTop.size())
	{
		return FALSE;
	}
	BOOL bRet = InsertRecursion(strRoot, 0, nIndex);
	m_vecHierarchy.clear();
	return bRet;
}

BOOL CSelfUpdateTreeCtrl::InsertRecursion(CString strRoot, int nFileNum, unsigned long nIndex)
{
	nFileNum++;//这个一定要放在switch之前，不能放在下面
	CFileFind file;
	CString strDirectory = strRoot;
	if (strDirectory.Right(1) != "\\")
	{
		strDirectory += _T("\\");
	}
	strDirectory += _T("*.*");
	BOOL bRet = file.FindFile(strDirectory);
	while (bRet)
	{
		bRet = file.FindNextFile();  // 是否有下一个目录
		if (file.IsDirectory() && !file.IsDots())
		{
			CString strPath = file.GetFilePath();
			CString strTitle = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\') - 1);
			if (std::find(m_vecUnwantedString.begin(), m_vecUnwantedString.end(), strTitle) != m_vecUnwantedString.end())
			{
				continue;
			}
			switch (nFileNum)
			{
			case 1:
				if (m_vecHierarchy.empty())
				{
					m_vecHierarchy.push_back(InsertItem(strTitle, nFileNum, nFileNum, m_vecTreeTop[nIndex]));
				}
				else
				{
					m_vecHierarchy[nFileNum - 1] = InsertItem(strTitle, nFileNum, nFileNum, m_vecTreeTop[nIndex]);
				}
				break;
			default:
				if (m_vecHierarchy.size() <= nFileNum - 1)
				{
					m_vecHierarchy.push_back(InsertItem(strTitle, nFileNum, nFileNum, m_vecHierarchy[nFileNum - 2]));
				}
				else
				{
					m_vecHierarchy[nFileNum - 1] = InsertItem(strTitle, nFileNum, nFileNum, m_vecHierarchy[nFileNum - 2]);
				}
				break;
			}
			InsertRecursion(strPath, nFileNum, nIndex);//递归遍历子目录
		}
		else if (!file.IsDirectory() && !file.IsDots())//如果不是一个目录，并且也不是当前目录
		{
			;//暂时不处理其他类型的图片
		}
	}//是否找到文件
	file.Close();
	return TRUE;
}

BOOL CSelfUpdateTreeCtrl::SetTreeImageList(CImageList * ImgList)
{
	if ((NULL == ImgList->GetSafeHandle()) || (ImgList == &m_ImgList))
	{
		return FALSE;
	}
	if (m_ImgList.GetSafeHandle())
	{
		m_ImgList.DeleteImageList();
		ASSERT(m_ImgList.GetSafeHandle() == NULL);
	}
	m_ImgList.Create(ImgList);
	return TRUE;
}

BOOL CSelfUpdateTreeCtrl::InsertParameter(const CString name, const unsigned long ulNum)
{
	HTREEITEM hItem = InsertItem(name, m_hItemRoot, TVI_LAST);
	m_vecTreeTop.push_back(hItem);
	SetItemData(hItem, ulNum); 
	
	HTREEITEM ht;
	HTREEITEM ht2;
	unsigned long num(13);
	unsigned long num2(27);
	if (ulNum == 0)
	{
		ht = InsertItem(CString("DeviceTemperatureSelector = Sensor"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num);
	}
	if (ulNum == 1)
	{
		num = 14;
		num2 = 27;
		ht = InsertItem(CString("RegionSelector = Region0"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);
		ht2 = InsertItem(CString("ComponentSelector = Intensity"), ht, TVI_LAST);
		SetItemData(ht2, num2++);
		ht2 = InsertItem(CString("ComponentSelector = Reflectance"), ht, TVI_LAST);
		SetItemData(ht2, num2++);
		ht2 = InsertItem(CString("ComponentSelector = Range"), ht, TVI_LAST);
		SetItemData(ht2, num2++);

		
		ht = InsertItem(CString("RegionSelector = Region1"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);
		/*ht2 = InsertItem(CString("ComponentSelector = Reflectance"), ht, TVI_LAST);
		SetItemData(ht2, num2++);
		ht2 = InsertItem(CString("ComponentSelector = Range"), ht, TVI_LAST);
		SetItemData(ht2, num2++);*/

		ht = InsertItem(CString("RegionSelector = Scan3dExtraction1"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);
		ht2 = InsertItem(CString("ComponentSelector = Reflectance"), ht, TVI_LAST);
		SetItemData(ht2, num2++);
		ht2 = InsertItem(CString("ComponentSelector = Range"), ht, TVI_LAST);
		SetItemData(ht2, num2++);

	}
	if (ulNum == 2)
	{
		num = 17;
		ht = InsertItem(CString("Scan3dExtractionSelector = Scan3dExtraction1"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);
	}
	if (ulNum == 3)
	{
		num = 18;
		ht = InsertItem(CString("TriggerSelector = LineStart"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);

		ht = InsertItem(CString("TriggerSelector = FrameStart"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);

		ht = InsertItem(CString("TriggerSelector = ExposureStart"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);
	}
	if (ulNum == 4)
	{
		num = 21;
		ht = InsertItem(CString("LineSelector = LaserStrobe1Output"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);

		ht = InsertItem(CString("LineSelector = LaserStrobe2Output"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);

	}
	if (ulNum == 5)
	{
		num = 23;
		ht = InsertItem(CString("TimerSelector = LaserStrobe1Timer"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);

		ht = InsertItem(CString("TimerSelector = LaserStrobe2Timer"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);
	}
	if (ulNum == 6)
	{
		
	}
	if (ulNum == 7)
	{
		num = 25;
		ht = InsertItem(CString("EventSelector = LogMessage"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);
	}
	if (ulNum == 8)
	{
		// Customers do not need to change parameter for FileAcessControl.
	}
	if (ulNum == 9)
	{}
	if (ulNum == 10)
	{}
	if (ulNum == 11)
	{
		num = 26;
		ht = InsertItem(CString("GigEVision"), m_vecTreeTop[ulNum], TVI_LAST);
		SetItemData(ht, num++);
	}
	if (ulNum == 12)
	{}

	return 0;
}

void CSelfUpdateTreeCtrl::ExpandTree(HTREEITEM hTreeItem)
{
	if (!ItemHasChildren(hTreeItem))
	{
		return;
	}
	HTREEITEM hNextItem = GetChildItem(hTreeItem);
	while (hNextItem != NULL)
	{
		ExpandTree(hNextItem);
		hNextItem = GetNextItem(hNextItem, TVGN_NEXT);
	}
	Expand(hTreeItem, TVE_EXPAND);
}

int CSelfUpdateTreeCtrl::CalTreeID(const  std::vector<size_t> levels, const  size_t NperL)
{
	int id(0);
	size_t LDepth = levels.size();
	for (size_t i=0; i<levels.size(); ++i)
	{
		id += levels[i] * NperL * (LDepth - i);
	}

	return id;
}
