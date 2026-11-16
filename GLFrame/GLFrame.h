/************************************************************************
 * 文件名：GLFrame.h
 * 功能说明：应用程序类的声明。
 * 作者：[你的名字]
 * 版本：v1.0
 * 日期：2026-11-16
 ************************************************************************/
#pragma once

#include <afxwin.h>

class CGLFrameApp : public CWinApp
{
public:
    CGLFrameApp();

    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CGLFrameApp theApp;
