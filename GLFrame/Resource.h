/************************************************************************
 * File    : Resource.h
 * Purpose : Resource identifiers for the GLFrame project.
 * Author  : [your name]
 * Version : v1.0
 * Date    : 2026-11-16
 *
 * NOTE: This header is included by BOTH C++ sources and the resource
 *       compiler (rc.exe). Keep every comment in this file pure ASCII:
 *       rc.exe reads sources with the ANSI code page, so UTF-8 Chinese
 *       comments can desynchronise its preprocessor and make the
 *       following #define lines disappear.
 *
 * ID_APP_EXIT uses the standard MFC value 0xE141 so that GLFrame.rc only
 * needs <windows.h> and does not have to pull in the MFC afxres.h.
 ************************************************************************/
#pragma once

/* Main frame resource: menu + document template string. */
#define IDR_MAINFRAME           128

/* View commands. */
#define ID_VIEW_RESET           32771

/* Subdivision depth: 7 consecutive IDs handled by ON_COMMAND_RANGE. */
#define ID_DEPTH_0              32780
#define ID_DEPTH_1              32781
#define ID_DEPTH_2              32782
#define ID_DEPTH_3              32783
#define ID_DEPTH_4              32784
#define ID_DEPTH_5              32785
#define ID_DEPTH_6              32786

/* Exit (standard MFC command id). */
#define ID_APP_EXIT             0xE141
