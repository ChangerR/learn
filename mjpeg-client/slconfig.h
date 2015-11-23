#ifndef __SLCONFIG__H
#define __SLCONFIG__H
/***************************************************************************
*
*  Project libmjpeg
*
* Copyright (C) 2014-2015, Changer, <dingjinchengyx@163.com>.
*
* This software is licensed as described in the file COPYING, which
* you should have received as part of this distribution.
*
* You may opt to use, copy, modify, merge, publish, distribute and/or sell
* copies of the Software, and permit persons to whom the Software is
* furnished to do so, under the terms of the COPYING file.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express or implied.
*
***************************************************************************/

#ifndef byte
typedef unsigned char byte;
#endif

#ifndef u32
typedef unsigned int  u32;
#endif

#ifndef s32
typedef int s32;
#endif

#ifndef u8
typedef unsigned char u8;
#endif

#ifndef s8
typedef char s8;
#endif

#ifdef _WIN32
#define SLSERVER_WIN32
#elif defined(__linux__)
#define SLSERVER_LINUX
#else
#error "We cannot support this platform!!!"
#endif

#include <stdio.h>
#define LOGOUT printf

#ifdef SLSERVER_LINUX
#define strcpy_s strcpy
#define sprintf_s sprintf
#define strncpy_s strncpy
#define strcat_s strcat
#define __cdecl
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKADDR_IN struct sockaddr_in
#define SOCKADDR struct sockaddr
#define SOCKET_ERROR -1
#define closesocket ::close
#endif

#ifdef SLSERVER_WIN32
#include <windows.h>
#endif
#endif
