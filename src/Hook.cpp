#include "Hook.h"

#include <windowsx.h>

LRESULT(CALLBACK HOOK_CALLWNDPROC)(int code, WPARAM wParam, LPARAM lParam) {
  Ud_Print(L"[Hook] process = %ld, res = %ld\n", (DWORD)wParam,
           (DWORD)((PCWPRETSTRUCT)lParam)->lResult);
  return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT(CALLBACK HOOK_CALLWNDPROCRET)(int code, WPARAM wParam, LPARAM lParam) {
  Ud_Print(L"[Hook] process = %ld, res = %ld\n", (DWORD)wParam,
           (DWORD)((PCWPRETSTRUCT)lParam)->lResult);
  return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT(CALLBACK HOOK_CBT)(int code, WPARAM wParam, LPARAM lParam) {
  wchar_t* event = UnKnown;
  switch (code) {
    case HCBT_CLICKSKIPPED: {
      event = str(HCBT_CLICKSKIPPED);
      Ud_Print(L"[Hook] message = %s, wParam = %ld, point = (%d, %d)\n", event,
               (DWORD)wParam, ((PMOUSEHOOKSTRUCT)lParam)->pt.x,
               ((PMOUSEHOOKSTRUCT)lParam)->pt.y);
      return 0;
    }
    case HCBT_KEYSKIPPED:
      event = str(HCBT_KEYSKIPPED);
      break;
    case HCBT_QS:
      event = str(HCBT_QS);
      break;
      // allow the action with 0, otherwise forbidden
    case HCBT_ACTIVATE: {
      event = str(HCBT_ACTIVATE);
      Ud_Print(L"[Hook] message = %s, Wnd = %ld, mouse = %d\n", event,
               (DWORD)wParam, ((LPCBTACTIVATESTRUCT)lParam)->fMouse);
      return 0;
    }
    case HCBT_CREATEWND: {
      event = str(HCBT_CREATEWND);
      Ud_Print(L"[Hook] message = %s, Name = %s, (x,y,l,h) = (%d,%d),(%d,%d)\n",
               event, ((LPCBT_CREATEWND)lParam)->lpcs->lpszName,
               ((LPCBT_CREATEWND)lParam)->lpcs->x,
               ((LPCBT_CREATEWND)lParam)->lpcs->y,
               ((LPCBT_CREATEWND)lParam)->lpcs->cx,
               ((LPCBT_CREATEWND)lParam)->lpcs->cy);
      return 0;
    }
    case HCBT_MOVESIZE: {
      event = str(HCBT_MOVESIZE);
      Ud_Print(L"[Hook] message = %s, Wnd = %ld, Point = (%d,%d), (%d,%d)\n",
               event, (DWORD)wParam, ((PRECTL)lParam)->left,
               ((PRECTL)lParam)->top, ((PRECTL)lParam)->right,
               ((PRECTL)lParam)->bottom);
      return 0;
    }
    case HCBT_SYSCOMMAND: {
      event = str(HCBT_SYSCOMMAND);
      Ud_Print(L"[Hook] message = %s, event = %ld, point = (%d, %d)\n", event,
               (DWORD)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      return 0;
    }
    case HCBT_DESTROYWND:
      event = str(HCBT_DESTROYWND);
      break;
    case HCBT_MINMAX:
      event = str(HCBT_MINMAX);
      break;
    case HCBT_SETFOCUS:
      event = str(HCBT_SETFOCUS);
      break;
    default:
      return CallNextHookEx(NULL, code, wParam, lParam);
  }
  Ud_Print(L"[Hook] message = %s, wParam = %ld, lParam = %ld\n", event,
           (DWORD)wParam, (DWORD)lParam);
  return 0;
}

LRESULT(CALLBACK HOOK_DEBUG)(int code, WPARAM wParam, LPARAM lParam) {
  if (code == HC_ACTION) {
    Ud_Print(L"[Hook] message = %s, hook type = %ld\n", str(HC_ACTION),
             (DWORD)wParam);
  }
  return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT(CALLBACK HOOK_FOREGROUNDIDLE)(int code, WPARAM wParam, LPARAM lParam) {
  if (code == HC_ACTION) {
    Ud_Print(L"[Hook] message = %s\n", str(HC_ACTION));
  }
  return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT(CALLBACK HOOK_GETMESSAGE)(int code, WPARAM wParam, LPARAM lParam) {
  if (code == HC_ACTION) {
    Ud_Print(L"[Hook] message = %s, event = %ld, msg = %d\n", str(HC_ACTION),
             (DWORD)wParam, ((PMSG)lParam)->message);
  }
  return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT(CALLBACK HOOK_KEYBOARD)(int code, WPARAM wParam, LPARAM lParam) {
  if (code == HC_ACTION) {
    Ud_Print(L"[Hook] message = %s, key event = (%ld, %ld)\n", str(HC_ACTION),
             (DWORD)wParam, (DWORD)lParam);
  } else if (code == HC_NOREMOVE) {
    Ud_Print(L"[Hook] message = %s, key event = (%ld, %ld)\n", str(HC_NOREMOVE),
             (DWORD)wParam, (DWORD)lParam);
  }
  return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT(CALLBACK HOOK_MOUSE)(int code, WPARAM wParam, LPARAM lParam) {
  if (code == HC_ACTION) {
    Ud_Print(L"[Hook] message = %s, event = %ld, point = (%d, %d)\n",
             str(HC_ACTION), (DWORD)wParam, ((PMOUSEHOOKSTRUCT)lParam)->pt.x,
             ((PMOUSEHOOKSTRUCT)lParam)->pt.y);
  } else if (code == HC_NOREMOVE) {
    Ud_Print(L"[Hook] message = %s, event = %ld, point = (%d, %d)\n",
             str(HC_NOREMOVE), (DWORD)wParam, ((PMOUSEHOOKSTRUCT)lParam)->pt.x,
             ((PMOUSEHOOKSTRUCT)lParam)->pt.y);
  }
  return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT(CALLBACK HOOK_MSGFILTER)(int code, WPARAM wParam, LPARAM lParam) {
  wchar_t* event = UnKnown;
  switch (code) {
    case MSGF_DDEMGR:  // DDEML event
      event = str(MSGF_DDEMGR);
      break;
    case MSGF_DIALOGBOX:  // dialog event
      event = str(MSGF_DIALOGBOX);
      break;
    case MSGF_MENU:  // menu event
      event = str(MSGF_MENU);
    case MSGF_SCROLLBAR:  // scrollbar event
      event = str(MSGF_SCROLLBAR);
      break;
    default:
      return CallNextHookEx(NULL, code, wParam, lParam);
  }
  Ud_Print(L"[Hook] message = %s, event = %d\n", event,
           ((PMSG)lParam)->message);
  return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT(CALLBACK HOOK_SHELL)(int code, WPARAM wParam, LPARAM lParam) {
  wchar_t* event = UnKnown;
  switch (code) {
    case HSHELL_ACCESSIBILITYSTATE:  // The accessibility state has changed.
      event = str(HSHELL_ACCESSIBILITYSTATE);
      break;
    case HSHELL_ACTIVATESHELLWINDOW:  // The shell should activate its main
                                      // window
      event = str(HSHELL_ACTIVATESHELLWINDOW);
      break;
    case HSHELL_TASKMAN:  // The user has selected the task list
      event = str(HSHELL_TASKMAN);
      break;
    case HSHELL_LANGUAGE:  // Keyboard language was changed
      event = str(HSHELL_LANGUAGE);
      break;
    case HSHELL_REDRAW:  // The title of a window in the task bar has been
                         // redrawn.
      event = str(HSHELL_REDRAW);
      break;
    case HSHELL_WINDOWACTIVATED:  // 	The activation has changed to a
                                  // different top-level, unowned window.
      event = str(HSHELL_WINDOWACTIVATED);
      break;
    case HSHELL_WINDOWCREATED:  // 	A top-level, unowned window has been
                                // created.
      event = str(HSHELL_WINDOWCREATED);
      break;
    case HSHELL_WINDOWDESTROYED:  // A top-level, unowned window is about to be
                                  // destroyed.
      event = str(HSHELL_WINDOWDESTROYED);
      break;
    case HSHELL_WINDOWREPLACED:  // A top-level window is being replaced.
      event = str(HSHELL_WINDOWREPLACED);
      break;
    case HSHELL_GETMINRECT:  // A window is being minimized or maximized.
    {
      PRECT pRect = (PRECT)lParam;
      Ud_Print(
          L"[Hook] message = %s, Wnd = %ld, Point = (%d,%d), "
          L"(%d,%d)\n",
          str(HSHELL_GETMINRECT), (DWORD)wParam, pRect->left, pRect->top,
          pRect->right, pRect->bottom);
      return 0;
    }
    case HSHELL_APPCOMMAND: {
      Ud_Print(L"[Hook] message = %s, Cmd = %d, Device = %d, KeyState = %d\n",
               str(HSHELL_APPCOMMAND), GET_APPCOMMAND_LPARAM(lParam),
               GET_DEVICE_LPARAM(lParam), GET_KEYSTATE_LPARAM(lParam));
      return 0;
    };
    default:
      return CallNextHookEx(NULL, code, wParam, lParam);
  }

  Ud_Print(L"[Hook] message = %s, wParam = %ld, lParam = %ld\n", event,
           (DWORD)wParam, (DWORD)lParam);
  return 0;
}

LRESULT(CALLBACK HOOK_SYSMSGFILTER)(int code, WPARAM wParam, LPARAM lParam) {
  wchar_t* event = UnKnown;
  switch (code) {
    case MSGF_DIALOGBOX:  // dialog event
      event = str(MSGF_DIALOGBOX);
      break;
    case MSGF_MENU:  // menu event
      event = str(MSGF_MENU);
      break;
    case MSGF_SCROLLBAR:  // scrollbar event
      event = str(MSGF_SCROLLBAR);
      break;
    default:
      return CallNextHookEx(NULL, code, wParam, lParam);
  }
  Ud_Print(L"[Hook] message = %s, event = %d\n", event,
           ((PMSG)lParam)->message);
  return CallNextHookEx(NULL, code, wParam, lParam);
}