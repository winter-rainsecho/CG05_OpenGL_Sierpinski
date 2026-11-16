/************************************************************************
 * 文件名：GLFrameView.cpp
 * 功能说明：视图类的实现 —— OpenGL 渲染上下文、镂垫绘制与鼠标交互。
 * 作者：[你的名字]
 * 版本：v1.0
 * 日期：2026-11-16
 *
 * 整体对应关系（课件页码）：
 *   P89   RC 不能直接绘图，必须与特定 DC 关联；三步：设像素格式→产 RC→设为当前 RC
 *   P93   PreCreateWindow 加 WS_CLIPSIBLINGS | WS_CLIPCHILDREN
 *   P94   OnCreate：PIXELFORMATDESCRIPTOR + ChoosePixelFormat/SetPixelFormat/wglCreateContext
 *   P95   OnDestroy：wglMakeCurrent(NULL,NULL) + wglDeleteContext
 *   P96   OnSize：设视口与投影
 *   P97   防闪烁：PFD_DOUBLEBUFFER + SwapBuffers + OnEraseBkgnd 返回 TRUE
 *   P79   隐藏面消除：深度缓冲 + glEnable(GL_DEPTH_TEST) + 清深度缓冲
 ************************************************************************/

#include <afxwin.h>
#include <GL/gl.h>

#include "GLFrameView.h"
#include "GLFrameDoc.h"
#include "Resource.h"
#include "Sierpinski.h"
#include "Framebuffer.h"

// 初始视角：稍微转一点，让正四面体一打开就是立体的，而不是正对某个面。
static const GLfloat kInitialRotX = 50.0f;
static const GLfloat kInitialRotY = 30.0f;

// 相机后退距离：正四面体外接球半径 1，退 2.5 留在正交视景体中间。
static const GLfloat kCameraZ = -2.5f;

// 正交投影的半高（世界单位）。1.5 足够装下半径 1 的四面体并留出边距。
static const GLdouble kViewExtent = 1.5;

IMPLEMENT_DYNCREATE(CGLFrameView, CView)

BEGIN_MESSAGE_MAP(CGLFrameView, CView)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
    ON_WM_CAPTURECHANGED()
    ON_COMMAND(ID_VIEW_RESET, &CGLFrameView::OnViewReset)
    ON_COMMAND_RANGE(ID_DEPTH_0, ID_DEPTH_6, &CGLFrameView::OnDepthCommand)
    ON_UPDATE_COMMAND_UI_RANGE(ID_DEPTH_0, ID_DEPTH_6, &CGLFrameView::OnUpdateDepthCommand)
END_MESSAGE_MAP()

CGLFrameView::CGLFrameView()
    : m_hRC(NULL)
    , m_bGLReady(FALSE)
    , m_nDepth(4)
    , m_fRotX(kInitialRotX)
    , m_fRotY(kInitialRotY)
    , m_bDragging(FALSE)
    , m_bCaptureDone(FALSE)
{
    m_ptLast.x = 0;
    m_ptLast.y = 0;
}

CGLFrameView::~CGLFrameView()
{
}

#ifdef _DEBUG
CGLFrameDoc* CGLFrameView::GetDocument() const
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGLFrameDoc)));
    return (CGLFrameDoc*)m_pDocument;
}
#endif

/************************************************************************
 * 函数名：PreCreateWindow
 * 功能：给视图窗口加上 WS_CLIPSIBLINGS | WS_CLIPCHILDREN 样式（课件 P93）。
 *       这两个样式保证 OpenGL 绘制区不会被兄弟/子窗口的绘制覆盖。
 * 参数：cs —— 窗口创建参数，可在此修改
 * 返回：成功返回 TRUE
 ************************************************************************/
BOOL CGLFrameView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    return CView::PreCreateWindow(cs);
}

/************************************************************************
 * 函数名：OnCreate
 * 功能：窗口创建时建立像素格式与渲染上下文（课件 P94）。
 * 参数：lpCreateStruct —— 创建参数
 * 返回：成功返回 0，失败返回 -1（窗口创建随之失败）
 ************************************************************************/
int CGLFrameView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    CClientDC dc(this);
    CreateContext(&dc);

    if (!m_bGLReady)
    {
        AfxMessageBox(_T("OpenGL 初始化失败：无法创建渲染上下文。\n")
                      _T("请确认显卡驱动正常，并且窗口像素格式可用。"));
        return -1;
    }

    return 0;
}

/************************************************************************
 * 函数名：CreateContext
 * 功能：完成课件 P89 列出的三步：设置窗口像素格式、产生 RC、设为当前 RC。
 *       同时初始化那些"设一次就够"的 OpenGL 状态（深度测试、背景色）。
 * 参数：pDC —— 视图的客户区设备上下文
 * 返回：无（结果写入 m_hRC / m_bGLReady）
 ************************************************************************/
void CGLFrameView::CreateContext(CDC* pDC)
{
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),  // pfd 结构的大小
        1,                              // 版本号
        PFD_DRAW_TO_WINDOW |            // 支持在窗口中绘图
        PFD_SUPPORT_OPENGL |            // 支持 OpenGL
        PFD_DOUBLEBUFFER,               // 双缓冲（防闪烁，课件 P97）
        PFD_TYPE_RGBA,                  // RGBA 颜色模式
        24,                             // 24 位颜色深度
        0, 0, 0, 0, 0, 0,               // 忽略颜色位
        0,                              // 无 Alpha 缓冲
        0,                              // 忽略移位位
        0,                              // 无累计缓冲
        0, 0, 0, 0,                     // 忽略累计位
        32,                             // 32 位深度缓冲（隐藏面消除用，课件 P79）
        0,                              // 无模板缓冲
        0,                              // 无辅助缓冲
        PFD_MAIN_PLANE,                 // 主层
        0,                              // 保留
        0, 0, 0                         // 忽略层、可见性、损毁掩模
    };

    HDC hdc = pDC->GetSafeHdc();

    int nFormat = ChoosePixelFormat(hdc, &pfd);
    if (nFormat == 0 || !SetPixelFormat(hdc, nFormat, &pfd))
    {
        return;
    }

    m_hRC = wglCreateContext(hdc);
    if (m_hRC == NULL)
    {
        return;
    }

    if (!wglMakeCurrent(hdc, m_hRC))
    {
        wglDeleteContext(m_hRC);
        m_hRC = NULL;
        return;
    }

    // ---- 一次性状态设置 ----
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   // 白色背景（与课件 P86 一致）
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);                // 开启 z 缓冲区隐藏面消除（课件 P79）

    wglMakeCurrent(NULL, NULL);
    m_bGLReady = TRUE;
}

/************************************************************************
 * 函数名：OnDestroy
 * 功能：释放渲染上下文（课件 P95）。
 * 参数：无
 * 返回：无
 ************************************************************************/
void CGLFrameView::OnDestroy()
{
    DestroyContext();
    CView::OnDestroy();
}

void CGLFrameView::DestroyContext()
{
    if (wglGetCurrentContext() != NULL)
    {
        wglMakeCurrent(NULL, NULL);
    }

    if (m_hRC != NULL)
    {
        wglDeleteContext(m_hRC);
        m_hRC = NULL;
    }

    m_bGLReady = FALSE;
}

/************************************************************************
 * 函数名：OnSize
 * 功能：窗口尺寸变化时重设视口与正交投影（课件 P96 / P85 的宽高比处理）。
 * 参数：nType —— 变化类型；cx, cy —— 新的客户区尺寸
 * 返回：无
 ************************************************************************/
void CGLFrameView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    if (!m_bGLReady || m_hRC == NULL || cx <= 0 || cy <= 0)
    {
        return;
    }

    CClientDC dc(this);
    if (!wglMakeCurrent(dc.GetSafeHdc(), m_hRC))
    {
        return;
    }

    ApplyProjection(cx, cy);

    wglMakeCurrent(NULL, NULL);
}

/************************************************************************
 * 函数名：ApplyProjection
 * 功能：设置视口与正交投影矩阵。按窗口宽高比扩展较大的那一维，
 *       保证图形在窗口拉伸时不变形（课件 P85 myReshape 的做法）。
 * 参数：cx, cy —— 客户区像素尺寸
 * 返回：无
 ************************************************************************/
void CGLFrameView::ApplyProjection(int cx, int cy)
{
    glViewport(0, 0, cx, cy);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const GLdouble aspect = (GLdouble)cx / (GLdouble)cy;
    if (aspect >= 1.0)
    {
        glOrtho(-kViewExtent * aspect, kViewExtent * aspect,
                -kViewExtent, kViewExtent, -100.0, 100.0);
    }
    else
    {
        glOrtho(-kViewExtent, kViewExtent,
                -kViewExtent / aspect, kViewExtent / aspect, -100.0, 100.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/************************************************************************
 * 函数名：OnEraseBkgnd
 * 功能：屏蔽 MFC 的背景擦除（课件 P97 第 C 步）。背景改由 glClear 负责，
 *       若不屏蔽，MFC 会用白刷子重绘背景，与 OpenGL 抢同一块像素，产生闪烁。
 * 参数：pDC —— 设备上下文（此处不使用）
 * 返回：固定返回 TRUE，表示"背景已处理"
 ************************************************************************/
BOOL CGLFrameView::OnEraseBkgnd(CDC* pDC)
{
    UNREFERENCED_PARAMETER(pDC);
    return TRUE;
}

/************************************************************************
 * 函数名：RenderScene
 * 功能：绘制一帧。清颜色与深度缓冲 → 摆好视角 → 递归生成镂垫。
 * 参数：无（读成员变量 m_nDepth / m_fRotX / m_fRotY）
 * 返回：无
 ************************************************************************/
void CGLFrameView::RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // 深度缓冲必须一起清（课件 P79）

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 先把物体推到相机前方，再旋转；否则平移量会被旋转矩阵带偏。
    glTranslatef(0.0f, 0.0f, kCameraZ);
    glRotatef(m_fRotX, 1.0f, 0.0f, 0.0f);
    glRotatef(m_fRotY, 0.0f, 1.0f, 0.0f);

    // 整个图形共用一次 glBegin/glEnd（课件 P70 "注解：效率"）。
    glBegin(GL_TRIANGLES);
    CSierpinski::Draw(m_nDepth);
    glEnd();
}

/************************************************************************
 * 函数名：OnDraw
 * 功能：MFC 的绘制入口。把 RC 绑定到本次绘制的 DC，渲染，交换缓冲。
 * 参数：pDC —— MFC 传入的设备上下文（WM_PAINT 的 CPaintDC）
 * 返回：无
 ************************************************************************/
void CGLFrameView::OnDraw(CDC* pDC)
{
    if (!m_bGLReady || m_hRC == NULL)
    {
        return;
    }

    HDC hdc = pDC->GetSafeHdc();
    if (!wglMakeCurrent(hdc, m_hRC))
    {
        return;
    }

    RenderScene();

    // ---- 开发期自检（见 Framebuffer.h；交作业可整段删） ----
    if (!m_bCaptureDone)
    {
        const char* capturePath = CaptureRequestPath();
        if (capturePath != NULL)
        {
            m_bCaptureDone = TRUE;
            SaveFramebufferBMP(capturePath);
            PostMessage(WM_CLOSE);
        }
    }

    SwapBuffers(hdc);                       // 双缓冲交换（课件 P97）
    wglMakeCurrent(NULL, NULL);
}

// ---------------------------------------------------------------------
// 交互：左键拖动旋转
// ---------------------------------------------------------------------

/************************************************************************
 * 函数名：OnLButtonDown
 * 功能：开始拖动。捕获鼠标并取得焦点，保证移出窗口也能继续旋转、键盘可用。
 * 参数：nFlags —— 修饰键；point —— 光标位置（客户区坐标）
 * 返回：无
 ************************************************************************/
void CGLFrameView::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetFocus();
    SetCapture();
    m_bDragging = TRUE;
    m_ptLast = point;
    CView::OnLButtonDown(nFlags, point);
}

/************************************************************************
 * 函数名：OnMouseMove
 * 功能：拖动时按鼠标位移换算旋转角。水平位移绕 Y 轴，垂直位移绕 X 轴。
 * 参数：nFlags —— 按键状态；point —— 当前光标位置
 * 返回：无
 ************************************************************************/
void CGLFrameView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bDragging)
    {
        const CPoint delta = point - m_ptLast;
        m_fRotY += (GLfloat)delta.x * 0.5f;
        m_fRotX += (GLfloat)delta.y * 0.5f;

        // 把角度收敛到 [0,360)，避免长时间拖动后数值过大损失精度。
        if (m_fRotX >= 360.0f) m_fRotX -= 360.0f;
        if (m_fRotX < 0.0f)    m_fRotX += 360.0f;
        if (m_fRotY >= 360.0f) m_fRotY -= 360.0f;
        if (m_fRotY < 0.0f)    m_fRotY += 360.0f;

        m_ptLast = point;
        Invalidate(FALSE);      // 只重绘，不擦背景
    }

    CView::OnMouseMove(nFlags, point);
}

/************************************************************************
 * 函数名：OnLButtonUp
 * 功能：结束拖动，释放鼠标捕获。
 * 参数：nFlags —— 修饰键；point —— 光标位置
 * 返回：无
 ************************************************************************/
void CGLFrameView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bDragging)
    {
        m_bDragging = FALSE;
        ReleaseCapture();
    }
    CView::OnLButtonUp(nFlags, point);
}

/************************************************************************
 * 函数名：OnCaptureChanged
 * 功能：鼠标捕获被系统强行收走时（例如 Alt+Tab），同步复位拖动状态，
 *       否则下次移动鼠标会莫名其妙地继续旋转。
 * 参数：pWnd —— 新的捕获窗口
 * 返回：无
 ************************************************************************/
void CGLFrameView::OnCaptureChanged(CWnd* pWnd)
{
    m_bDragging = FALSE;
    CView::OnCaptureChanged(pWnd);
}

// ---------------------------------------------------------------------
// 交互：键盘与菜单
// ---------------------------------------------------------------------

/************************************************************************
 * 函数名：OnKeyDown
 * 功能：数字键 0~6 设置细分次数；R 键复位视角。
 * 参数：nChar —— 键码；nRepCnt, nFlags —— 重复次数与扫描码
 * 返回：无
 ************************************************************************/
void CGLFrameView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar >= '0' && nChar <= '6')
    {
        m_nDepth = (int)(nChar - '0');
        Invalidate(FALSE);
    }
    else if (nChar == 'R' || nChar == 'r')
    {
        OnViewReset();
    }

    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

/************************************************************************
 * 函数名：OnViewReset
 * 功能：把视角恢复到初始值（不改变细分次数）。
 * 参数：无
 * 返回：无
 ************************************************************************/
void CGLFrameView::OnViewReset()
{
    m_fRotX = kInitialRotX;
    m_fRotY = kInitialRotY;
    Invalidate(FALSE);
}

/************************************************************************
 * 函数名：OnDepthCommand
 * 功能：菜单 View→Depth 0..6 的统一处理（ID 连续，用 RANGE 映射）。
 * 参数：nID —— 被点击的命令 ID
 * 返回：无
 ************************************************************************/
void CGLFrameView::OnDepthCommand(UINT nID)
{
    m_nDepth = (int)(nID - ID_DEPTH_0);
    Invalidate(FALSE);
}

/************************************************************************
 * 函数名：OnUpdateDepthCommand
 * 功能：在当前细分次数对应的菜单项上打圆点（单选效果）。
 * 参数：pCmdUI —— 命令 UI 状态
 * 返回：无
 ************************************************************************/
void CGLFrameView::OnUpdateDepthCommand(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio((int)pCmdUI->m_nID - (int)ID_DEPTH_0 == m_nDepth);
}
