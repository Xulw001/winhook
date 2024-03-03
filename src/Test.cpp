// platform
#include <Windows.h>
#include <tlhelp32.h>

// common
#include <iostream>
#include <vector>

#define Usage               \
  "Hook Type:\n"            \
  "1) WH_CALLWNDPROC\n"     \
  "2) WH_CALLWNDPROCRET\n"  \
  "3) WH_CBT\n"             \
  "4) WH_DEBUG\n"           \
  "5) WH_FOREGROUNDIDLE\n"  \
  "6) WH_GETMESSAGE\n"      \
  "7) WH_JOURNALPLAYBACK\n" \
  "8) WH_JOURNALRECORD\n"   \
  "9) WH_KEYBOARD\n"        \
  "10) WH_KEYBOARD_LL\n"    \
  "11) WH_MOUSE\n"          \
  "12) WH_MOUSE_LL\n"       \
  "13) WH_MSGFILTER\n"      \
  "14) WH_SHELL\n"          \
  "15) WH_SYSMSGFILTER\n"   \
  "Please Choose A Type:"

#define HOOK "Please Input A Process Id(default is Global):"

#define HookFunc(x) #x

enum HookType {
  HOOK_CALLWNDPROC = 1,  // 线程或全局
  HOOK_CALLWNDPROCRET,   // 线程或全局
  HOOK_CBT,              // 线程或全局
  HOOK_DEBUG,            // 线程或全局
  HOOK_FOREGROUNDIDLE,   // 线程或全局
  HOOK_GETMESSAGE,       // 线程或全局
  HOOK_JOURNALPLAYBACK,  // 仅限全局
  HOOK_JOURNALRECORD,    // 仅限全局
  HOOK_KEYBOARD,         // 线程或全局
  HOOK_KEYBOARD_LL,      // 仅限全局
  HOOK_MOUSE,            // 线程或全局
  HOOK_MOUSE_LL,         // 仅限全局
  HOOK_MSGFILTER,        // 线程或全局
  HOOK_SHELL,            // 线程或全局
  HOOK_SYSMSGFILTER,     //
};

void GetThreadId(int process_id, std::vector<int>* p_thread_ids) {
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (snapshot != INVALID_HANDLE_VALUE) {
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(snapshot, &threadEntry)) {
      do {
        if (threadEntry.th32OwnerProcessID == (DWORD)process_id) {
          p_thread_ids->emplace_back(threadEntry.th32ThreadID);
        }
      } while (Thread32Next(snapshot, &threadEntry));
    }

    CloseHandle(snapshot);
  }
}

void DO_JOURNALRECORD(bool);
void DO_JOURNALPLAYBACK(bool);
void DO_KEYBOARD_LL(bool);
void DO_MOUSE_LL(bool);

std::vector<HHOOK> g_hook_list_;
void UnHook() {
  for (auto handle : g_hook_list_) {
    UnhookWindowsHookEx(handle);
  }
  DO_JOURNALRECORD(false);
  DO_JOURNALPLAYBACK(false);
  DO_KEYBOARD_LL(false);
  DO_MOUSE_LL(false);
}

void DoHook(HookType type, const std::vector<int>& thread_ids) {
  int hook_id = -1;
  HOOKPROC hook_proc = NULL;
  HMODULE hDll = LoadLibrary(TEXT("HookDemo"));
  if (hDll) {
    switch (type) {
      case HOOK_CALLWNDPROC:
        hook_id = WH_CALLWNDPROC;
        hook_proc = (HOOKPROC)GetProcAddress(hDll, HookFunc(HOOK_CALLWNDPROC));
        break;
      case HOOK_CALLWNDPROCRET:
        hook_id = WH_CALLWNDPROCRET;
        hook_proc =
            (HOOKPROC)GetProcAddress(hDll, HookFunc(HOOK_CALLWNDPROCRET));
        break;
      case HOOK_CBT:
        hook_id = WH_CBT;
        hook_proc = (HOOKPROC)GetProcAddress(hDll, HookFunc(HOOK_CBT));
        break;
      case HOOK_DEBUG:
        hook_id = WH_DEBUG;
        hook_proc = (HOOKPROC)GetProcAddress(hDll, HookFunc(HOOK_DEBUG));
        break;
      case HOOK_FOREGROUNDIDLE:
        hook_id = WH_FOREGROUNDIDLE;
        hook_proc =
            (HOOKPROC)GetProcAddress(hDll, HookFunc(HOOK_FOREGROUNDIDLE));
        break;
      case HOOK_GETMESSAGE:
        hook_id = WH_GETMESSAGE;
        hook_proc = (HOOKPROC)GetProcAddress(hDll, HookFunc(HOOK_GETMESSAGE));
        break;
      case HOOK_KEYBOARD:
        hook_id = WH_KEYBOARD;
        hook_proc = (HOOKPROC)GetProcAddress(hDll, HookFunc(HOOK_KEYBOARD));
        break;
      case HOOK_MOUSE:
        hook_id = WH_MOUSE;
        hook_proc = (HOOKPROC)GetProcAddress(hDll, HookFunc(HOOK_MOUSE));
        break;
      case HOOK_MSGFILTER:
        hook_id = WH_MSGFILTER;
        hook_proc = (HOOKPROC)GetProcAddress(hDll, HookFunc(HOOK_MSGFILTER));
        break;
      case HOOK_SHELL:
        hook_id = WH_SHELL;
        hook_proc = (HOOKPROC)GetProcAddress(hDll, HookFunc(HOOK_SHELL));
        break;
      case HOOK_JOURNALRECORD:
        return DO_JOURNALRECORD(true);
      case HOOK_JOURNALPLAYBACK:
        return DO_JOURNALPLAYBACK(true);
      case HOOK_KEYBOARD_LL:
        return DO_KEYBOARD_LL(true);
      case HOOK_MOUSE_LL:
        return DO_MOUSE_LL(true);
      case HOOK_SYSMSGFILTER:
        hook_id = WH_SYSMSGFILTER;
        hook_proc = (HOOKPROC)GetProcAddress(hDll, HookFunc(HOOK_SYSMSGFILTER));
        break;
      default:
        exit(1);
    }

    if (hook_proc == NULL) {
      std::cout << "GetProcAddress Error = " << GetLastError() << std::endl;
      exit(1);
    }

    if (thread_ids.empty()) {
      HHOOK hook = SetWindowsHookEx(hook_id, hook_proc, hDll, 0);
      if (!hook) {
        std::cout << "SetWindowsHookEx Error = " << GetLastError() << std::endl;
        exit(1);
      }
      g_hook_list_.emplace_back(hook);
    } else {
      for (auto thread : thread_ids) {
        HHOOK hook = SetWindowsHookEx(hook_id, hook_proc, hDll, thread);
        if (!hook) {
          continue;
        }
        g_hook_list_.emplace_back(hook);
      }
    }

  } else {
    std::cout << "LoadLibrary Error = " << GetLastError() << std::endl;
    exit(1);
  }
}

int main() {
  char in_buf[MAX_PATH] = {0};
  std::cout << Usage;
  int hook_type = 0;
  std::vector<int> thread_ids;
  scanf_s("%[^\n]", in_buf, (int)sizeof(in_buf) - 1);
  getchar();
  hook_type = atoi(in_buf);
  switch (hook_type) {
    case HOOK_CALLWNDPROC:
    case HOOK_CALLWNDPROCRET:
    case HOOK_CBT:
    case HOOK_DEBUG:
    case HOOK_FOREGROUNDIDLE:
    case HOOK_GETMESSAGE:
    case HOOK_KEYBOARD:
    case HOOK_MOUSE:
    case HOOK_MSGFILTER:
    case HOOK_SHELL: {
      std::cout << HOOK;
      int process_id = 0;
      if (scanf_s("%[^\n]", in_buf, (int)sizeof(in_buf) - 1)) {
        process_id = atoi(in_buf);
        if (process_id != 0) {
          GetThreadId(process_id, &thread_ids);
        }
      }
      getchar();
    }
    case HOOK_JOURNALPLAYBACK:
    case HOOK_JOURNALRECORD:
    case HOOK_KEYBOARD_LL:
    case HOOK_MOUSE_LL:
    case HOOK_SYSMSGFILTER:
      break;
    default:
      std::cout << "unsupport type" << std::endl;
      return 0;
  }

  DoHook((HookType)hook_type, thread_ids);

  getchar();

  UnHook();

  return 0;
}