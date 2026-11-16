/************************************************************************
 * 文件名：GLFrameDoc.h
 * 功能说明：文档类的声明。
 * 作者：[你的名字]
 * 版本：v1.0
 * 日期：2026-11-16
 ************************************************************************/
#pragma once

#include <afxwin.h>

class CGLFrameDoc : public CDocument
{
    DECLARE_DYNCREATE(CGLFrameDoc)

public:
    CGLFrameDoc();
    virtual ~CGLFrameDoc();

    virtual BOOL OnNewDocument();

    DECLARE_MESSAGE_MAP()
};
