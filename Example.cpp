/**
* @file    Example.cpp
* @author  Tyler Parret Zhu (OwlHowlinMornSky) <mysteryworldgod@outlook.com>
*
* @section LICENSE
*
* MIT License
*
* Copyright (c) 2023 Tyler Parret Zhu
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* @section DESCRIPTION
*
* Example of to avoid Win32 Nonclient(NC) Messages blocking.
*/

#include <stdio.h>
#include <Windows.h>


#define IDLE_DEBUG_OUTPUT
#define DEFAULT_ELAPSE 20
#define TIMER_ELAPSE   50


bool    MyRegistClass(HINSTANCE);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void    CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD);


#ifdef IDLE_DEBUG_OUTPUT

HANDLE hStdOut = 0;

void IdleProcess(bool state) {
	// TODO
	static int cnt = 0;
	WCHAR tmp[64] = { 0 };
	swprintf_s(tmp, 64, L"Idle Process. Message Pump State: %s.", state ? L"In System" : L"Normal");
	WriteConsoleW(hStdOut, tmp, static_cast<DWORD>(wcslen(tmp)), 0, 0);
	for (int i = 0; i < cnt; ++i)
		tmp[i] = L'.';
	tmp[cnt] = '\n';
	tmp[cnt + 1] = '\0';
	cnt = (cnt + 1) % 16;
	WriteConsoleW(hStdOut, tmp, static_cast<DWORD>(wcslen(tmp)), 0, 0);
	return;
}

#else

void IdleFunc(bool state) {
	// TODO
	return;
}

#endif // IDLE_DEBUG_OUTPUT


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR    lpCmdLine,
					  _In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef IDLE_DEBUG_OUTPUT
	AllocConsole();
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
#endif // IDLE_DEBUG_OUTPUT

	if (!MyRegistClass(hInstance)) {
		MessageBoxW(0, L"Regist Class: Failed!", L"Error", MB_ICONERROR);
		return EXIT_FAILURE;
	}

	// TODO

	HWND hWnd0 = CreateWindowW(L"MY_TEST_CLASS_OHMS", L"MY_TEST_WINDOW",
							   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
							   CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
							   0, 0, hInstance, 0);
	if (!hWnd0) {
		MessageBoxW(NULL, L"Create Window: Failed!", L"Error", MB_ICONERROR);
		return EXIT_FAILURE;
	}

	ShowWindow(hWnd0, nCmdShow);
	UpdateWindow(hWnd0);

	// Message Pump
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
			Sleep(DEFAULT_ELAPSE);
		}
	}
	DestroyWindow(hWnd0);

	// TODO

	return EXIT_SUCCESS;
}


bool MyRegistClass(HINSTANCE hInst) {
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hInstance = hInst;
	wcex.lpfnWndProc = WndProc;
	wcex.lpszClassName = L"MY_TEST_CLASS_OHMS";
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	return (RegisterClassExW(&wcex) != 0);
}


void CALLBACK TimerProc(HWND hWnd, UINT message, UINT_PTR nTimerid, DWORD systemTick) {
	IdleProcess(true);
	return;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static WPARAM l_lastHitNC = HTNOWHERE;
	static UINT_PTR l_timerID = 0;

	switch (message) {

	case WM_CREATE: {
		CreateWindowW(L"BUTTON", L"TEST",
					  WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | BS_DEFPUSHBUTTON,
					  10, 10, 50, 50,
					  hWnd, 0, 0, 0);
		// TODO
		break;
	}

	case WM_DESTROY: {
		// TODO
		break;
	}

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_NCMOUSEMOVE:
		if (l_lastHitNC == HTCAPTION) {
			l_lastHitNC = HTNOWHERE;
			PostMessageW(hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, lParam);
			PostMessageW(hWnd, WM_MOUSEMOVE, MK_LBUTTON, 0);
		}
		else return DefWindowProcW(hWnd, message, wParam, lParam);
		break;

	case WM_NCLBUTTONDOWN:
		if (wParam == HTCAPTION || wParam == HTCLOSE ||
			wParam == HTMAXBUTTON || wParam == HTMINBUTTON || wParam == HTHELP) {
			l_lastHitNC = wParam;
			PostMessageW(hWnd, WM_ACTIVATE, WA_CLICKACTIVE, 0);
		}
		else {
			l_lastHitNC = HTNOWHERE;
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
		break;

	case WM_NCLBUTTONUP:
		if (l_lastHitNC == wParam) {
			l_lastHitNC = HTNOWHERE;
			switch (wParam) {
			case HTMAXBUTTON: {
				bool wndMaxed = GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_MAXIMIZE;
				PostMessageW(hWnd, WM_SYSCOMMAND, wndMaxed ? SC_RESTORE : SC_MAXIMIZE, lParam);
				break;
			}
			case HTMINBUTTON:
				PostMessageW(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, lParam);
				break;
			case HTCLOSE:
				PostMessageW(hWnd, WM_SYSCOMMAND, SC_CLOSE, lParam);
				break;
			case HTHELP:
				break;
			default:
				return DefWindowProcW(hWnd, message, wParam, lParam);
			}
		}
		else {
			l_lastHitNC = HTNOWHERE;
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
		break;

	case WM_NCRBUTTONDOWN:
		if (l_lastHitNC == HTNOWHERE && (wParam == HTCAPTION || wParam == HTCLOSE ||
			wParam == HTMAXBUTTON || wParam == HTMINBUTTON || wParam == HTHELP)) {
			l_lastHitNC = wParam | 0x8000;
			PostMessageW(hWnd, WM_ACTIVATE, WA_CLICKACTIVE, 0);
		}
		else
			l_lastHitNC = HTNOWHERE;
		break;

	case WM_NCRBUTTONUP:
		if (l_lastHitNC & 0x8000) {
			l_lastHitNC = HTNOWHERE;
			PostMessageW(hWnd, WM_CONTEXTMENU, 0, lParam);
		}
		else
			l_lastHitNC = HTNOWHERE;
		break;

	case WM_ENTERMENULOOP:
		if (l_timerID) {
			KillTimer(0, l_timerID);
		}
		l_timerID = SetTimer(0, 0, TIMER_ELAPSE, (TIMERPROC)TimerProc);
		break;

	case WM_EXITMENULOOP:
		if (l_timerID) {
			KillTimer(0, l_timerID);
		}
		l_timerID = 0;
		break;

	case WM_ENTERSIZEMOVE:
		if (l_timerID) {
			KillTimer(0, l_timerID);
		}
		l_timerID = SetTimer(0, 0, TIMER_ELAPSE, (TIMERPROC)TimerProc);
		break;

	case WM_EXITSIZEMOVE:
		if (l_timerID) {
			KillTimer(0, l_timerID);
		}
		l_timerID = 0;
		break;

	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return 0;
}
