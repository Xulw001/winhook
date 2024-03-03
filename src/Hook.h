#pragma once
// platform
#include <Windows.h>
#include <stdio.h>

#define UnKnown L"Unknown"
#define str(x) L#x
#define Ud_Print(format, ...)                                            \
  WCHAR fmt[MAX_PATH];                                                   \
  swprintf_s(fmt, sizeof(fmt) / sizeof(WCHAR) - 1, format, __VA_ARGS__); \
  OutputDebugStringW(fmt);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_CALLWNDPROC)(int code, WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_CALLWNDPROCRET)(int code, WPARAM wParam,
                                          LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_CBT)(int code, WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_DEBUG)(int code, WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_FOREGROUNDIDLE)(int code, WPARAM wParam,
                                          LPARAM lParam);
extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_GETMESSAGE)(int code, WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_KEYBOARD)(int code, WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_KEYBOARD_LL)(int code, WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_MOUSE)(int code, WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_MOUSE_LL)(int code, WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_MSGFILTER)(int code, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_SHELL)(int code, WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_SYSMSGFILTER)(int code, WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_JOURNALPLAYBACK)(int code, WPARAM wParam,
                                           LPARAM lParam);

extern "C" __declspec(dllexport)
    LRESULT(CALLBACK HOOK_JOURNALRECORD)(int code, WPARAM wParam,
                                         LPARAM lParam);