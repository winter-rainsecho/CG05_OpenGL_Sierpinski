/************************************************************************
 * 文件名：GLFrameDoc.cpp
 * 功能说明：文档类的实现。
 * 作者：[你的名字]
 * 版本：v1.0
 * 日期：2026-11-16
 *
 * 说明：本程序的"场景数据"只有视图里的几个渲染参数（细分次数、视角），
 *       没有需要保存的文件内容，所以文档类保持最小实现。它存在的意义
 *       是支撑课件 P90 要求的"单文档（SDI）"结构：MFC 的单文档模板
 *       必须同时具备 文档类 + 框架类 + 视图类。
 ************************************************************************/

#include <afxwin.h>

#include "GLFrameDoc.h"

IMPLEMENT_DYNCREATE(CGLFrameDoc, CDocument)

BEGIN_MESSAGE_MAP(CGLFrameDoc, CDocument)
END_MESSAGE_MAP()

CGLFrameDoc::CGLFrameDoc()
{
}

CGLFrameDoc::~CGLFrameDoc()
{
}

BOOL CGLFrameDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
    {
        return FALSE;
    }
    return TRUE;
}
