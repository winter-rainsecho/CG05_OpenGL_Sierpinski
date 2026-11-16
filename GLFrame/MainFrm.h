/************************************************************************
 * 文件名：MainFrm.h
 * 功能说明：主框架窗口类的声明。
 * 作者：[你的名字]
 * 版本：v1.0
 * 日期：2026-11-16
 ************************************************************************/
#pragma once

#include <afxwin.h>

class CMainFrame : public CFrameWnd
{
    DECLARE_DYNCREATE(CMainFrame)

public:
    CMainFrame();
    virtual ~CMainFrame();

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    DECLARE_MESSAGE_MAP()
};
