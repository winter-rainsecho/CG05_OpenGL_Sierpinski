# GLFrame —— 三维 Sierpinski 镂垫（MFC + OpenGL）

CG05 作业：利用 MFC 下 OpenGL 编程技术，实现三维 Sierpinski 镂垫。
课件：CG05_OpenGL_2026.pdf（作业在 P98，Deadline 2026-12-31）。

## 目录结构

```
CG05_OpenGL_Sierpinski/
├── GLFrame.sln            VS 解决方案（可直接在 IDE 打开）
├── build.bat              一键编译 + 自动截图自检
├── run.bat                打开交互窗口
├── build.status           最近一次构建结果（脚本写入）
└── GLFrame/
    ├── GLFrame.h/.cpp     应用类：装配单文档模板（课件 P90 SDI）
    ├── MainFrm.h/.cpp     主框架：窗口 500 高宽、中文标题（课件 P90 第 2 步）
    ├── GLFrameDoc.h/.cpp  文档类：SDI 结构必需，无文件数据
    ├── GLFrameView.h/.cpp 视图类：WGL 渲染上下文 + 绘制 + 鼠标交互（核心）
    ├── Sierpinski.h/.cpp  三维镂垫生成（体细分算法）
    ├── Framebuffer.h/.cpp 自检截图工具（后缓冲 → BMP，交作业可删）
    ├── Resource.h / GLFrame.rc  资源：菜单 + 文档模板字符串
    └── GLFrame.vcxproj
```

## 构建与运行

```
build.bat            默认 Release；build.bat Debug 编 Debug 版
run.bat              打开窗口
```

build.bat 自动定位 MSVC 工具集与 Windows SDK（不依赖 vcvarsall.bat），
编译完成后自动运行一次程序生成 `x64\<配置>\selfcheck.bmp`，用于核对渲染结果
（这是环境变量 GLFRAME_CAPTURE 驱动的自检，见 Framebuffer.h）。
在 Visual Studio IDE 中打开 GLFrame.sln 用 MSBuild 构建亦可。

## 操作说明

| 操作 | 效果 |
|---|---|
| 左键拖动 | 绕 X/Y 轴旋转视角 |
| 数字键 0–6 | 设置细分次数（0=单个四面体，默认 4） |
| R 键 / 菜单 View→Reset View | 复位视角 |
| 菜单 View→Depth 0–6 | 同数字键，当前档位打圆点 |

## 算法（体细分，课件 P80–P84）

从正四面体（外接球半径 1，课件 P81 顶点坐标）出发，取六条棱的中点，
把原四面体切成 8 个小四面体，保留贴着四个原顶点的 4 个，丢弃中间由
6 个中点围成的八面体——这就是"镂空"。对保留的 4 个小四面体递归重复。
深度为 n 时共 4^n 个小四面体、4^(n+1)×3 个三角形顶点。

实现要点：
- `divide_tetra()`：递归体细分（对应课件 P83，修正了课件排版笔误：
  `point` 应为 `point3`，`else(triangle(...))` 不是合法 C++）。
- `tetra()`：一个四面体的四个面，红/绿/蓝/黑配色（课件 P82）。
- 与课件 P82 不同：`glBegin/glEnd` 只在视图里调用一次（GL_TRIANGLES），
  `triangle()` 只送顶点。课件 P82 在 triangle 内又开了一对 glBegin/glEnd，
  与 P84 的 display 嵌套使用属于非法操作，按 P84 的意图实现。
- 隐藏面消除按课件 P79 三要素：像素格式带 32 位深度缓冲 +
  `glEnable(GL_DEPTH_TEST)` + 每帧清 `GL_DEPTH_BUFFER_BIT`。
- 防闪烁按课件 P97 三板斧：`PFD_DOUBLEBUFFER` + `SwapBuffers` +
  `OnEraseBkgnd` 返回 TRUE。

## MFC 接入 OpenGL（课件 P89–P97）

| 步骤 | 位置 |
|---|---|
| 像素格式 → `wglCreateContext` → 设为当前 RC | `CGLFrameView::OnCreate` / `CreateContext` |
| `PreCreateWindow` 加 `WS_CLIPSIBLINGS\|WS_CLIPCHILDREN` | `CGLFrameView::PreCreateWindow` |
| `OnDestroy` 释放 RC | `CGLFrameView::OnDestroy` |
| `OnSize` 重设视口与投影 | `CGLFrameView::OnSize` / `ApplyProjection` |
| 窗口 500×400、固定标题 | `CMainFrame::PreCreateWindow` |

说明：课件 P91 提到的 `glaux.h/glaux.lib` 在现代 Windows SDK 中已不存在
（本机核实过），且 MFC 接入正文只用 `wgl*` 系列 API，本工程未使用 glaux。

## 验证记录

- 2026-11-16：build.bat（Release/Debug）实跑通过；自检截图确认渲染结果
  与课件 P76 的镂垫形态一致（四个小四面体 + 镂空 + 四色面）。
- GPU：Intel Iris Xe，OpenGL 4.6；开发期另做了 WGL 探针与 MFC+OpenGL
  探针（见上层目录 glprobe\ 与 CG05_OpenGL\probe\）。
