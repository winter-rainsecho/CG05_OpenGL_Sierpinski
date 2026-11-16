/************************************************************************
 * 文件名：GLFrame.cpp
 * 功能说明：应用程序类的实现，装配单文档模板。
 * 作者：[你的名字]
 * 版本：v1.0
 * 日期：2026-11-16
 *
 * 说明：课件 P90 要求"新建一个 MFC 向导程序，命名为 GLFrame，单文档，
 *       去除打印支持"。本机只安装了 Visual Studio Build Tools（没有 IDE），
 *       无法运行 MFC 应用程序向导，因此按向导生成的 SDI 结构手工写出
 *       等价骨架：应用类 + 主框架 + 文档 + 视图 + 单文档模板。
 *       未加入打印/打印预览相关代码，与"去除打印支持"一致。
 ************************************************************************/

#include <afxwin.h>

#include "GLFrame.h"
#include "MainFrm.h"
#include "GLFrameDoc.h"
#include "GLFrameView.h"
#include "Resource.h"   // IDR_MAINFRAME

CGLFrameApp::CGLFrameApp()
{
}

// 全局唯一的应用对象：MFC 在进入 WinMain 之前需要先构造它。
CGLFrameApp theApp;

BEGIN_MESSAGE_MAP(CGLFrameApp, CWinApp)
END_MESSAGE_MAP()

BOOL CGLFrameApp::InitInstance()
{
    // 单文档模板把文档、框架、视图三者绑定在一起。
    CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CGLFrameDoc),
        RUNTIME_CLASS(CMainFrame),
        RUNTIME_CLASS(CGLFrameView));
    if (pDocTemplate == NULL)
    {
        return FALSE;
    }
    AddDocTemplate(pDocTemplate);

    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    if (!ProcessShellCommand(cmdInfo))
    {
        return FALSE;
    }

    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    return TRUE;
}
