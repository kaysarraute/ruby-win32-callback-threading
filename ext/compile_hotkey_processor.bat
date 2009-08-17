nmake
mt.exe -manifest hotkey_processor.so.manifest -outputresource:hotkey_processor.so;2
xcopy hotkey_processor.so ..\hotkey_processor.so /D /Y