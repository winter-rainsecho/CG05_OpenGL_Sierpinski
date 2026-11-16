/************************************************************************
 * 文件名：GLFrameView.h
 * 功能说明：视图类的声明 —— OpenGL 渲染上下文的宿主，负责绘制与交互。
 * 作者：[你的名字]
 * 版本：v1.0
 * 日期：2026-11-16
 ************************************************************************/
#pragma once

#include <afxwin.h>
#include <GL/gl.h>

class CGLFrameDoc;

class CGLFrameView : public CView
{
    DECLARE_DYNCREATE(CGLFrameView)

public:
    CGLFrameView();
    virtual ~CGLFrameView();

    CGLFrameDoc* GetDocument() const;

    virtual void OnDraw(CDC* pDC);

protected:
    // ---- OpenGL 渲染上下文（课件 P89：RC 必须与某个 DC 关联） ----
    HGLRC   m_hRC;          // 渲染上下文句柄
    BOOL    m_bGLReady;     // 像素格式与 RC 是否建立成功

    // ---- 场景参数 ----
    int     m_nDepth;       // 细分次数
    GLfloat m_fRotX;        // 绕世界 X 轴的旋转角（度）
    GLfloat m_fRotY;        // 绕世界 Y 轴的旋转角（度）

    // ---- 鼠标拖动状态 ----
    BOOL    m_bDragging;
    CPoint  m_ptLast;

    // ---- 开发期自检：只截图一次 ----
    BOOL    m_bCaptureDone;

    void CreateContext(CDC* pDC);
    void DestroyContext();
    void ApplyProjection(int cx, int cy);
    void RenderScene();

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnCaptureChanged(CWnd* pWnd);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnViewReset();
    afx_msg void OnDepthCommand(UINT nID);
    afx_msg void OnUpdateDepthCommand(CCmdUI* pCmdUI);

    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // 发行版中内联 GetDocument
inline CGLFrameDoc* CGLFrameView::GetDocument() const
{
    return reinterpret_cast<CGLFrameDoc*>(m_pDocument);
}
#endif
