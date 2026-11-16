/************************************************************************
 * 文件名：Sierpinski.cpp
 * 功能说明：三维 Sierpinski 镂垫的生成（体细分方案）的实现。
 * 作者：[你的名字]
 * 版本：v1.0
 * 日期：2026-11-16
 *
 * 算法：从正四面体出发，取六条棱的中点，把原四面体切成 8 个小四面体，
 *       保留与四个顶点相连的 4 个，丢弃中间那个（正是"镂空"的部分），
 *       对保留的 4 个小四面体递归重复。极限图形即三维 Sierpinski 镂垫，
 *       其 Hausdorff 维数为 log4/log2 = 2（二维课件 P64 中为 log3/log2≈1.585）。
 ************************************************************************/

#include "Sierpinski.h"

namespace
{
    // 正四面体的四个顶点，外接球半径 1（课件 P81）。
    // 底面三点在 z = -1/3 平面上，顶点在 z = 1。
    const GLfloat kVertex[4][3] =
    {
        {  0.0f,       0.0f,      1.0f      },
        {  0.0f,       0.942809f, -0.33333f },
        { -0.816497f, -0.471405f, -0.333333f},
        {  0.816497f, -0.471405f, -0.333333f}
    };

    // 四个面的颜色：红、绿、蓝、黑（课件 P81 的 colors[]）。
    const GLfloat kFaceColor[4][3] =
    {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 0.0f}
    };
}

void CSierpinski::Face(const GLfloat* a, const GLfloat* b, const GLfloat* c)
{
    glVertex3fv(a);
    glVertex3fv(b);
    glVertex3fv(c);
}

void CSierpinski::Tetra(const GLfloat* a, const GLfloat* b,
                        const GLfloat* c, const GLfloat* d)
{
    // 顶点顺序与配色照课件 P82：四个面两两共用三个顶点，颜色各不相同。
    glColor3fv(kFaceColor[0]);
    Face(a, b, c);

    glColor3fv(kFaceColor[1]);
    Face(a, c, d);

    glColor3fv(kFaceColor[2]);
    Face(a, d, b);

    glColor3fv(kFaceColor[3]);
    Face(b, d, c);
}

void CSierpinski::DivideTetra(const GLfloat* a, const GLfloat* b,
                              const GLfloat* c, const GLfloat* d, int depth)
{
    GLfloat mid[6][3];
    int j;

    if (depth <= 0)
    {
        // 递归到底：这个四面体是最终镂垫的一部分，整块画出来。
        Tetra(a, b, c, d);
        return;
    }

    // 六条棱的中点（课件 P83）：ab, ac, ad, bc, cd, bd
    for (j = 0; j < 3; j++) mid[0][j] = (a[j] + b[j]) * 0.5f;
    for (j = 0; j < 3; j++) mid[1][j] = (a[j] + c[j]) * 0.5f;
    for (j = 0; j < 3; j++) mid[2][j] = (a[j] + d[j]) * 0.5f;
    for (j = 0; j < 3; j++) mid[3][j] = (b[j] + c[j]) * 0.5f;
    for (j = 0; j < 3; j++) mid[4][j] = (c[j] + d[j]) * 0.5f;
    for (j = 0; j < 3; j++) mid[5][j] = (b[j] + d[j]) * 0.5f;

    // 保留的四个小四面体：各自贴近原四面体的一个顶点，
    // 中间那块由六个中点围成的八面体被丢弃，形成镂空。
    DivideTetra(a,    mid[0], mid[1], mid[2], depth - 1);
    DivideTetra(mid[0], b,    mid[3], mid[5], depth - 1);
    DivideTetra(mid[1], mid[3], c,    mid[4], depth - 1);
    DivideTetra(mid[2], mid[4], d,    mid[5], depth - 1);
}

void CSierpinski::Draw(int depth)
{
    if (depth < 0)
    {
        depth = 0;
    }

    DivideTetra(kVertex[0], kVertex[1], kVertex[2], kVertex[3], depth);
}

long long CSierpinski::VertexCount(int depth)
{
    if (depth < 0)
    {
        depth = 0;
    }

    long long tetrahedra = 1;          // 4^depth 个小四面体
    for (int i = 0; i < depth; i++)
    {
        tetrahedra *= 4;
    }

    return tetrahedra * 4 * 3;         // 每个小四面体 4 个面、每面 3 个顶点
}
