# Probable-Solution-to-Win32-DefWindowProc-Blocking
Probable Solution to Win32 DefWindowProc Blocking

When we code for win32 app, if we use idle process in message pump like this
```cpp
MSG msg = { 0 };
while (true) {
    if (PeekMessageW(&msg, 0, 0, 0, PM_NOREMOVE)) {
        if (GetMessageW(&msg, 0, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else break;
    }
    else {
        IdleProcess(false);
    }
}
```
our code will get blocked while dragging or sizing the window(s).

It's too complex to clearly tell why this happens.
Shortly, it's related to DefWindowProc and some special messages.

### You can check `Example.cpp` to learn the whole thing.

[Chinese Blog (CSDN)](https://blog.csdn.net/MysteryWorldGod/article/details/129335988)
