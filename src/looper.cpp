#include "pch.h"
#include "looper.h"

Looper gLooper;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    return MessageBox(NULL, "hello world", "MessageBox", 0);
}

void Looper::Update() {
}
