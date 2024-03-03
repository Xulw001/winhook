#include "Hook.h"
// common
#include <iostream>
#include <thread>

static bool end_ = false;
LRESULT(CALLBACK HOOK_KEYBOARD_LL)(int code, WPARAM wParam, LPARAM lParam) {
  if (code == HC_ACTION) {
    Ud_Print(L"[Hook] message = %s, event = %ld, key event = (%d, %d)\n",
             str(HC_ACTION), (DWORD)wParam, ((PKBDLLHOOKSTRUCT)lParam)->vkCode,
             ((PKBDLLHOOKSTRUCT)lParam)->flags);
    if (wParam == WM_KEYDOWN) {
      if (((PKBDLLHOOKSTRUCT)lParam)->vkCode == VK_CANCEL) {
        end_ = true;
        return 1;
      }
    }
  }

  return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT(CALLBACK HOOK_MOUSE_LL)(int code, WPARAM wParam, LPARAM lParam) {
  if (code == HC_ACTION) {
    Ud_Print(L"[Hook] message = %s, event = %ld, point = (%d, %d)\n",
             str(HC_ACTION), (DWORD)wParam, ((PMSLLHOOKSTRUCT)lParam)->pt.x,
             ((PMSLLHOOKSTRUCT)lParam)->pt.y);
  }
  return CallNextHookEx(NULL, code, wParam, lParam);
}

void DO_KEYBOARD_LL(bool);
void DO_MOUSE_LL(bool);

void DO_MONITOR_LL() {
  MSG msg;
  while (!end_) {
    // The call is made by sending a message to the thread that installed the
    // hook. Therefore, the thread that installed the hook must have a message
    // loop.
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      Sleep(3000);
    }
  }
}

HHOOK hook_keyboard_ll_ = NULL;
HHOOK hook_mouse_ll_ = NULL;
std::thread work_;
void DO_KEYBOARD_LL(bool init) {
  if (!init) {
    end_ = true;
    if (hook_keyboard_ll_) {
      UnhookWindowsHookEx(hook_keyboard_ll_);
      hook_keyboard_ll_ = NULL;
    }
    if (work_.joinable()) work_.join();
  } else {
    end_ = false;
    work_ = std::thread([]() {
      hook_keyboard_ll_ = SetWindowsHookEx(WH_KEYBOARD_LL, HOOK_KEYBOARD_LL,
                                           GetModuleHandle(NULL), 0);
      if (!hook_keyboard_ll_) {
        std::cout << "SetWindowsHookEx Error = " << GetLastError() << std::endl;
        exit(1);
      }

      DO_MONITOR_LL();
    });
  }
}

void DO_MOUSE_LL(bool init) {
  if (!init) {
    if (hook_mouse_ll_) {
      UnhookWindowsHookEx(hook_mouse_ll_);
      hook_mouse_ll_ = NULL;
    }

    if (work_.joinable()) work_.join();
  } else {
    work_ = std::thread([]() {
      hook_mouse_ll_ = SetWindowsHookEx(WH_MOUSE_LL, HOOK_MOUSE_LL,
                                        GetModuleHandle(NULL), 0);
      if (!hook_mouse_ll_) {
        std::cout << "SetWindowsHookEx Error = " << GetLastError() << std::endl;
        exit(1);
      }

      DO_MONITOR_LL();
    });
  }
}