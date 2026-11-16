/************************************************************************
 * 文件名：Framebuffer.cpp
 * 功能说明：OpenGL 后缓冲 -> 24 位 BMP 的实现。
 * 作者：[你的名字]
 * 版本：v1.0
 * 日期：2026-11-16
 *
 * BMP 格式要点：文件头 14 字节 + 信息头 40 字节，像素按 BGR 存放，
 * 行序自下而上，每行字节数补齐到 4 的倍数。OpenGL 的 glReadPixels
 * 也以左下角为原点、行序自下而上，因此像素行不需要翻转，只需把
 * RGB 换成 BGR。
 ************************************************************************/

#include "Framebuffer.h"

#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool SaveFramebufferBMP(const char* path)
{
    if (path == NULL || path[0] == '\0')
    {
        return false;
    }

    GLint viewport[4] = {0, 0, 0, 0};
    glGetIntegerv(GL_VIEWPORT, viewport);
    const int width = viewport[2];
    const int height = viewport[3];
    if (width <= 0 || height <= 0)
    {
        return false;
    }

    const int rowBytes = (width * 3 + 3) & ~3;
    const int imageSize = rowBytes * height;

    unsigned char* pixels = (unsigned char*)malloc((size_t)width * height * 3);
    if (pixels == NULL)
    {
        return false;
    }

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    unsigned char header[54];
    memset(header, 0, sizeof(header));
    header[0] = 'B';
    header[1] = 'M';

    const int fileSize = 54 + imageSize;
    memcpy(header + 2, &fileSize, 4);
    const int dataOffset = 54;
    memcpy(header + 10, &dataOffset, 4);
    const int infoSize = 40;
    memcpy(header + 14, &infoSize, 4);
    memcpy(header + 18, &width, 4);
    memcpy(header + 22, &height, 4);
    const short planes = 1;
    const short bpp = 24;
    memcpy(header + 26, &planes, 2);
    memcpy(header + 28, &bpp, 2);
    memcpy(header + 34, &imageSize, 4);

    FILE* fp = NULL;
    if (fopen_s(&fp, path, "wb") != 0 || fp == NULL)
    {
        free(pixels);
        return false;
    }

    fwrite(header, 1, sizeof(header), fp);

    unsigned char* line = (unsigned char*)calloc(1, (size_t)rowBytes);
    if (line == NULL)
    {
        free(pixels);
        fclose(fp);
        return false;
    }

    for (int y = 0; y < height; y++)
    {
        const unsigned char* src = pixels + (size_t)y * width * 3;
        for (int x = 0; x < width; x++)
        {
            line[x * 3 + 0] = src[x * 3 + 2];   // B
            line[x * 3 + 1] = src[x * 3 + 1];   // G
            line[x * 3 + 2] = src[x * 3 + 0];   // R
        }
        fwrite(line, 1, (size_t)rowBytes, fp);
    }

    free(line);
    free(pixels);
    fclose(fp);
    return true;
}

const char* CaptureRequestPath()
{
    const char* value = getenv("GLFRAME_CAPTURE");
    if (value == NULL || value[0] == '\0')
    {
        return NULL;
    }
    return value;
}
