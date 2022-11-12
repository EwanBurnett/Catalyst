#pragma once

//Framework Include File
//Contains headers commonly used throughout the codebase, with
//certain features switched on or off.
//Ewan Burnett - 2022

//Target Windows 7 or later
#define _WIN32_WINNT 0x0601

#include <sdkddkver.h>

//These #Defines disable unused windows features.
//They are optional, but omitting them will increase build times.
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE

#define NOMINMAX

#include <windows.h>
#include <cassert>
#include <cstdint>