#include "Hook.h"
// common
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

std::vector<EVENTMSG> ev_record_;

bool LoadMsg() {
  FILE* fp;
  if (fopen_s(&fp, "data.bin", "rb") || !fp) return false;

  bool ret = false;
  do {
    int e_num = 0;
    if (fread(&e_num, sizeof(e_num), 1, fp) != 1) break;
    if (e_num <= 0) break;
    ev_record_.resize(e_num);
    if (fread(ev_record_.data(), sizeof(EVENTMSG), e_num, fp) != e_num) break;
    unsigned short flag = 0x0;
    if (fread(&flag, sizeof(flag), 1, fp) != 1) break;
    if (flag != 0xEFFF) break;
    ret = true;
  } while (false);

  fclose(fp);
  return ret;
}

bool SaveMsg() {
  FILE* fp;
  if (fopen_s(&fp, "data.bin", "wb") || !fp) return false;

  bool ret = false;
  do {
    int e_num = (int)ev_record_.size();
    if (fwrite(&e_num, sizeof(e_num), 1, fp) != 1) break;
    if (fwrite(ev_record_.data(), sizeof(EVENTMSG), e_num, fp) != e_num) break;

    const unsigned short kflag = 0xEFFF;
    if (fwrite(&kflag, sizeof(kflag), 1, fp) != 1) break;
    ret = true;
  } while (false);
  fclose(fp);
  return ret;
}

void DO_JOURNALRECORD(bool);
void DO_JOURNALPLAYBACK(bool);

bool end_ = false;
int cur_ = 0;
bool flag = false;
LRESULT(CALLBACK HOOK_JOURNALPLAYBACK)
(int code, WPARAM wParam, LPARAM lParam) {
  wchar_t* event = UnKnown;
  if (code == HC_GETNEXT) {
    event = str(HC_GETNEXT);
  } else if (code == HC_SKIP) {
    event = str(HC_SKIP);
  } else if (code == HC_NOREMOVE) {
    event = str(HC_NOREMOVE);
  } else if (code == HC_SYSMODALOFF) {
    event = str(HC_SYSMODALOFF);
  } else if (code == HC_SYSMODALON) {
    event = str(HC_SYSMODALON);
  }

  Ud_Print(L"[Hook] message = %s, wParam = %ld, lParam = %ld\n", event,
           (DWORD)wParam, (DWORD)lParam);

  if (code == HC_SKIP) {
    flag = true;
    if (++cur_ >= ev_record_.size()) {
      std::cout << "DO_JOURNALPLAYBACK Empty!" << std::endl;
      DO_JOURNALPLAYBACK(false);
      PostQuitMessage(0);
      end_ = true;
    }
    return 0;
  } else if (code == HC_GETNEXT) {
    DWORD time = 0;
    PEVENTMSG pEv = (PEVENTMSG)lParam;
    if (cur_ < ev_record_.size() && pEv) {
      *pEv = ev_record_[cur_];
      if (flag) {
        time =
            ev_record_[cur_ + 1 >= ev_record_.size() ? cur_ : cur_ + 1].time -
            ev_record_[cur_].time;
        flag = false;
      }
    }
    if (time < 0) time = 1;
    return time;
  } else if (code == HC_NOREMOVE) {
    return 0;
  }

  return CallNextHookEx(NULL, code, wParam, lParam);
}

bool stop_ = false;
LRESULT(CALLBACK HOOK_JOURNALRECORD)
(int code, WPARAM wParam, LPARAM lParam) {
  wchar_t* event = UnKnown;
  if (code == HC_ACTION) {
    event = str(HC_ACTION);
  } else if (code == HC_SYSMODALOFF) {
    event = str(HC_SYSMODALOFF);
    stop_ = false;
  } else if (code == HC_SYSMODALON) {
    event = str(HC_SYSMODALON);
    stop_ = true;
  }
  PEVENTMSG pEv = (PEVENTMSG)lParam;
  Ud_Print(L"[Hook] message = %s, wParam = %ld, lParam = %ld\n", event,
           (DWORD)wParam, (DWORD)(code == HC_ACTION ? pEv->message : lParam));
  if (!stop_ && !end_) {
    // https://learn.microsoft.com/zh-cn/windows/win32/inputdev/keyboard-input-notifications
    // https://learn.microsoft.com/zh-cn/windows/win32/inputdev/mouse-input-notifications
    if (pEv->message == WM_KEYDOWN) {
      if (LOBYTE(pEv->paramL) == VK_CANCEL) {
        std::cout << "DO_JOURNALRECORD VK_CANCEL!" << std::endl;
        DO_JOURNALRECORD(false);
        PostQuitMessage(0);
        return 0;
      }
    }
    ev_record_.push_back(*pEv);
  }
  return CallNextHookEx(NULL, code, wParam, lParam);
}

HHOOK hook_record_ = NULL;
HHOOK hook_playback_ = NULL;

void DO_MONITOR_JOURNAL() {
  MSG msg;
  BOOL bRet;
  while (!end_) {
    // The call is made by sending a message to the thread that installed the
    // hook. Therefore, the thread that installed the hook must have a message
    // loop.
    if ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
      if (bRet == -1) {
        std::cout << "GetMessage Error = " << GetLastError() << std::endl;
        break;
      } else {
        TranslateMessage(&msg);
        if (msg.message == WM_CANCELJOURNAL) {
          std::cout << "DO_JOURNALRECORD WM_CANCELJOURNAL!" << std::endl;
          hook_playback_ = NULL;
          hook_record_ = NULL;
          end_ = true;
        }
        DispatchMessage(&msg);
      }
    }
  }
}

void DO_JOURNALRECORD(bool init) {
  if (!init) {
    end_ = true;
    if (hook_record_) {
      UnhookWindowsHookEx(hook_record_);
      if (!SaveMsg()) {
        std::cout << "SaveMsg Error = " << GetLastError() << std::endl;
        exit(1);
      }
      hook_record_ = NULL;
    }
  } else {
    end_ = false;
    hook_record_ = SetWindowsHookEx(WH_JOURNALRECORD, HOOK_JOURNALRECORD,
                                    GetModuleHandle(NULL), 0);
    if (!hook_record_) {
      std::cout << "SetWindowsHookEx Error = " << GetLastError() << std::endl;
      exit(1);
    }

    DO_MONITOR_JOURNAL();
  }
}

void DO_JOURNALPLAYBACK(bool init) {
  if (!init) {
    end_ = true;
    if (hook_playback_) {
      UnhookWindowsHookEx(hook_playback_);
      hook_playback_ = NULL;
    }
  } else {
    end_ = false;
    if (!LoadMsg()) {
      std::cout << "LoadMsg Error = " << GetLastError() << std::endl;
      exit(1);
    }

    hook_playback_ = SetWindowsHookEx(WH_JOURNALPLAYBACK, HOOK_JOURNALPLAYBACK,
                                      GetModuleHandle(NULL), 0);
    if (!hook_playback_) {
      std::cout << "SetWindowsHookEx Error = " << GetLastError() << std::endl;
      exit(1);
    }

    DO_MONITOR_JOURNAL();
  }
}