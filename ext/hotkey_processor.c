#include "ruby.h"
#include "windows.h"
#include <stdio.h>

int process_keypress_function;
VALUE HotkeyProcessor;
HHOOK keyboard_hook;

LRESULT CALLBACK callback_function(int Code, WPARAM wParam, LPARAM lParam)
{
  PKBDLLHOOKSTRUCT kbd = (PKBDLLHOOKSTRUCT)lParam;
  
  if (Code < 0 || Qtrue == rb_funcall(HotkeyProcessor, process_keypress_function, 2,
                                      INT2NUM(kbd->vkCode),                                                /* code */
                                      (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) ? Qtrue : Qfalse)) /* down */
      return CallNextHookEx(keyboard_hook, Code, wParam, lParam);
  else
    return 1;
}

static BOOL get_message(MSG *msg) {
  return GetMessage(msg, 0, 0, 0);
}

static VALUE start(VALUE self)
{
    HMODULE Module = GetModuleHandle(NULL);
    MSG msg;

    keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) callback_function, Module, 0);
    
    get_message(&msg); /* footnote 2 */
      /* rb_thread_blocking_region(get_message, &msg, 0, 0) */ /* footnote 3, tentative non-blocking version of the line above */

    /* Never get here. Wait forever for get_message() to return
       to keep the program alive and responding to callbacks. */
    return self;
}

static VALUE process_keypress(VALUE self, VALUE code)
{
  return Qnil;
}

void Init_hotkey_processor() {
  HotkeyProcessor = rb_define_module("HotkeyProcessor");
  rb_define_module_function(HotkeyProcessor, "process_keypress", process_keypress, 1);
  rb_define_module_function(HotkeyProcessor, "start", start, 0);
  process_keypress_function = rb_intern("process_keypress");
}
