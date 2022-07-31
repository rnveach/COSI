// COSIDlg.h : header file
//

#if !defined(AFX_COSIDLG_H__DE84368D_DC66_46A4_9135_D022E872AD6E__INCLUDED_)
#define AFX_COSIDLG_H__DE84368D_DC66_46A4_9135_D022E872AD6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCOSIDlg dialog

class CCOSIDlg : public CDialog
{
// Construction
public:
	CCOSIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCOSIDlg)
	enum { IDD = IDD_COSI_DIALOG };
	CButton	m_compoption1;
	CStatic	m_status;
	CProgressCtrl	m_progress;
	CButton	m_decompressb;
	CButton	m_compressb;
	CString	m_filename;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCOSIDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	bool compress, end;
	char sector[2352], modedata[2352], sectordata[2352], *returnsector, stage;
	unsigned long modesize, datasize;
	unsigned int timer;
	unsigned long pos, posb, sectorpos, newsize, inputlength;

	unsigned long a, b;

	CString buff, buff2;

	CString files[20];
	unsigned long numfiles, currentfile;

	FILE *input, *output1, *output2;

	HANDLE Lib;

	CreateDLLVariable(_Name);
	CreateDLLVariable(_Version);
	CreateDLLVariable(_DecompressSector);
	CreateDLLVariable(_CompressSector);
	CreateDLLVariable(CloseFiles);
	CreateDLLVariable(SetFiles);
	CreateDLLVariable(GetModeFileName);
	CreateDLLVariable(GetDataFileName);
	CreateDLLVariable(ReadFileSector);
	CreateDLLVariable(SeekFileSector);
	CreateDLLVariable(SeekFilePosition);
	CreateDLLVariable(GetFileCurrentPosition);
	CreateDLLVariable(GetFileCurrentSector);
	CreateDLLVariable(InitCOSI);
	CreateDLLVariable(DeInitCOSI);
	CreateDLLVariable(Set_FileRead);

	unsigned long DLLversion;

	void RunFiles();

	void _Compress();
	void _DeCompress();

	// Generated message map functions
	//{{AFX_MSG(CCOSIDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCompress();
	afx_msg void OnDecompress();
	afx_msg void OnBrowse();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COSIDLG_H__DE84368D_DC66_46A4_9135_D022E872AD6E__INCLUDED_)
