#include "ruby.h"
#include "windows.h"
#include <stdio.h>

int process_keypress_function;
VALUE HotkeyProcessor;
HHOOK keyboard_hook;
struct KEYEVENT { int vkcode;
                  BOOL down; };

static BOOL pass_key_event_to_ruby(struct KEYEVENT *key_event) {
  return (Qtrue == rb_funcall(HotkeyProcessor, process_keypress_function, 2,
                       INT2NUM(key_event->vkcode),
                       (key_event->down ? Qtrue : Qfalse)));
}

LRESULT CALLBACK callback_function(int Code, WPARAM wParam, LPARAM lParam)
{
  PKBDLLHOOKSTRUCT kbd = (PKBDLLHOOKSTRUCT)lParam;
  struct KEYEVENT key_event;

  key_event.vkcode = kbd->vkCode;
  key_event.down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

  if (Code < 0 || rb_thread_call_with_gvl(pass_key_event_to_ruby, &key_event))
      return CallNextHookEx(keyboard_hook, Code, wParam, lParam);
  else
    return 1;
}

static BOOL get_message(MSG *msg) {
  return GetMessage(msg, 0, 0, 0);
}

static void pump_messages(int fuck) {
  MSG msg;
  while (GetMessage(&msg, 0, 0, 0) == 1) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

static VALUE start(VALUE self)
{
    HMODULE Module = GetModuleHandle(NULL);

    keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) callback_function, Module, 0);
    
    rb_thread_blocking_region(pump_messages, 0, 0, 0);
    /* clean up */
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
