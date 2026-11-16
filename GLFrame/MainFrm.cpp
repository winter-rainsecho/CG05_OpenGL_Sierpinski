/************************************************************************
 * 文件名：MainFrm.cpp
 * 功能说明：主框架窗口类的实现。
 * 作者：[你的名字]
 * 版本：v1.0
 * 日期：2026-11-16
 *
 * 对应课件 P90 第 2 步"得到简单的窗口风格"：
 *   cs.cx = 500; cs.cy = 400;
 *   cs.lpszName = _T("OpenGL 框架程序");
 *   cs.style &= ~FWS_ADDTOTITLE;   // 不把文档名加到标题栏
 ************************************************************************/

#include <afxwin.h>

#include "MainFrm.h"

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
END_MESSAGE_MAP()

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
    {
        return FALSE;
    }

    cs.cx = 560;    // 窗口宽度（含边框），客户区约 540 像素
    cs.cy = 480;    // 窗口高度（含标题栏与边框）
    cs.lpszName = _T("三维 Sierpinski 镂垫 —— MFC + OpenGL");

    // 去掉 FWS_ADDTOTITLE，否则 MFC 会用文档名覆盖上面的标题。
    cs.style &= ~FWS_ADDTOTITLE;

    return TRUE;
}
