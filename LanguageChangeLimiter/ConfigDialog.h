#pragma once

#include <windows.h>
#include <vector>
#include "LanguageChangeLimiter.h"

// Function prototypes
LRESULT CALLBACK ConfigWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateConfigDialog(HINSTANCE hInstance);
