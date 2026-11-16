/************************************************************************
 * 文件名：Sierpinski.h
 * 功能说明：三维 Sierpinski 镂垫的生成（体细分方案）。
 * 作者：[你的名字]
 * 版本：v1.0
 * 日期：2026-11-16
 *
 * 算法来源：课程课件 CG05_OpenGL_2026.pdf
 *           P80 "面细分 vs 体细分"、P81 正四面体顶点、P82 tetra()、
 *           P83 divide_tetra()、P84 显示回调函数。
 *
 * 与课件代码的两处差异（课件为排版稿，直接照抄无法编译）：
 *   1) 课件 P82 的 triangle() 内部自己调了 glBegin/glEnd，而 P84 的 display()
 *      又在外面套了一层 glBegin/glEnd —— 嵌套调用 glBegin 是非法操作。
 *      这里按课件 P84 的意图处理：glBegin/glEnd 只由调用者（视图）发起，
 *      triangle() 只负责送顶点。
 *   2) 课件 P74 的局部变量写成 point v1,v2,v3，应为 point3；P67/P74 的
 *      else(triangle(...)) 也不是合法 C++ 语句。这里按正确语义书写。
 ************************************************************************/
#pragma once

// GL/gl.h 依赖 windows.h 提供的 APIENTRY / WINGDIAPI 等修饰宏，
// 必须先包含 windows.h 再包含 gl.h。
#include <windows.h>
#include <GL/gl.h>

class CSierpinski
{
public:
    /// 递归生成三维 Sierpinski 镂垫。
    /// 调用前必须已经 glBegin(GL_TRIANGLES)，调用后由调用者 glEnd()。
    /// depth 为细分次数（0 表示只画一个四面体，建议 0~6）。
    static void Draw(int depth);

    /// 预估深度 depth 对应的顶点数：4^(depth+1) * 3。
    static long long VertexCount(int depth);

private:
    /// 输出一个四面体的四个面（每个面一种颜色）。
    static void Tetra(const GLfloat* a, const GLfloat* b,
                      const GLfloat* c, const GLfloat* d);

    /// 体细分：用 6 个棱中点把四面体切成 4 个小四面体，丢弃中间那个。
    static void DivideTetra(const GLfloat* a, const GLfloat* b,
                            const GLfloat* c, const GLfloat* d, int depth);

    /// 输出一个三角面。
    static void Face(const GLfloat* a, const GLfloat* b, const GLfloat* c);
};
