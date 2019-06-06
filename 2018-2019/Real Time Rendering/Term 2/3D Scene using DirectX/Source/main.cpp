
//
// main.cpp
//

#include <stdafx.h>
#include <iostream>
#include <exception>
#include <CGDConsole.h>
#include <Scene.h>

using namespace std;


// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {

	CGDConsole		*debugConsole = nullptr;
	Scene	*mainScene = nullptr;

#pragma region 1. Initialise application

	// 1.1 Tell Windows to terminate app if heap becomes corrupted
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// 1.2 Initialise COM
	if (!SUCCEEDED(CoInitialize(NULL)))
		return 0;

	try
	{
		// 1.3 Initialise debug console
		debugConsole = CGDConsole::CreateConsole(L"DirectX11 Debug Console");

		if (!debugConsole)
			throw exception("Cannot create debug console");

		// Since we have to create the console before any memory report can be generated we have +1 malloc error so compensate for this.  debugConsole must be released once the final memory report is given!
		compensate_free_count(1);

		cout << "Hello DirectX 11...\n\n";
		
		// 1.4 Create main application controller object (singleton)
		mainScene = Scene::CreateScene(600, 600, L"DirectX 11", L"DirectX 11", nCmdShow, hInstance, WndProc);

		if (!mainScene)
			throw exception("Cannot create main application controller");
	}
	catch(exception& e)
	{
		cout << e.what() << endl;

		CoUninitialize();

		return 0;
	}

#pragma endregion

#pragma region 2. Main message loop

	while (1) {

		MSG msg;

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			if (WM_QUIT == msg.message)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		} else {

			mainScene->updateAndRenderScene();
		}
	}

#pragma endregion

#pragma region 3. Tear-down and exit

	// 3.1 Report final timing data...
	mainScene->reportTimingData();
	
	// 3.2 Dispose of application resources

	
	// 3.3 Final memory report
	cout << "\nFinal memory allocations:\n";
	gu_memory_report();

	// 3.4 Close debug console


	// 3.5 Shutdown COM
	CoUninitialize();

	return 0;

#pragma endregion

}


// Application event handler
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT				clientRect;
	static POINT		prevMousePos, currentMousePos;

	switch (message) {

	case WM_CREATE:
	{
		// Bind HWND to host DXController - this allows all relevant app data to be accessed when processing events related to the HWND.
		LPCREATESTRUCT createStruct = (LPCREATESTRUCT)lParam;
		Scene *mainScene = (Scene*)createStruct->lpCreateParams;

		SetWindowLongPtrW(hWnd, GWLP_USERDATA, PtrToUlong(mainScene));

		break;
	}

	case WM_SIZE:
	{
		Scene *mainScene = (Scene*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

		if (mainScene)
			mainScene->resizeResources();

		break;
	}

	case WM_SIZING:
	{
		// Make sure window RECT does not fall below a minimum size
		LPRECT R = (LPRECT)lParam;

		if (R->bottom - R->top < 200) {

			// Determine edge being resized and modify RECT accordingly
			if (wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOMRIGHT) {

				// We're resizing from the bottom so set the bottom edge relative to the current top edge
				R->bottom = R->top + 200;
			}
			else if (wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT) {
			
				// We're resizing from the top so set the top edge relative to the current bottom edge
				R->top = R->bottom - 200;
			}
		}

		return TRUE;
	}

	case WM_ENTERSIZEMOVE:
	{
		// Stop the clock during (modal) window resize / move
		Scene *mainScene = (Scene*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

		if (mainScene)
			mainScene->stopClock();

		break;
	}

	case WM_EXITSIZEMOVE:
	{
		// Restart the clock once the window resize / move has completed
		Scene *mainScene = (Scene*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

		if (mainScene)
			mainScene->startClock();

		break;
	}

	case WM_PAINT:
	{
		// Don't do any painting - just validate window RECT
		GetClientRect(hWnd, &clientRect);
		ValidateRect(hWnd, &clientRect);
		break;
	}
	
	case WM_CLOSE:
	{
		// Window will close - decouple HWND from main controller
		Scene *mainScene = (Scene*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

		if (mainScene)
			mainScene->destoryWindow();
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	
#pragma region Mouse Input Handlers

	case WM_LBUTTONDOWN:
	{
		SetCapture(hWnd);
		GetCursorPos(&currentMousePos);
		break;
	}
	
	case WM_MOUSEMOVE:
	{
		// Only interested in mouse move if left button held down
		if (wParam & MK_LBUTTON) {

			prevMousePos = currentMousePos;
			GetCursorPos(&currentMousePos);

			Scene *mainScene = (Scene*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
			
			if (mainScene) {

				POINT disp;

				disp.x = currentMousePos.x - prevMousePos.x;
				disp.y = currentMousePos.y - prevMousePos.y;

				mainScene->handleMouseLDrag(disp);
				mainScene->updateAndRenderScene();
			}
		}
		break;
	}
	
	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		break;
	}

	case WM_MOUSEWHEEL:
	{
		Scene *mainScene = (Scene*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
		
		if (mainScene)
			mainScene->handleMouseWheel((short)HIWORD(wParam));
		
		break;
	}

	case WM_KEYDOWN:
	{
		Scene *mainScene = (Scene*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

		if (mainScene)
			mainScene->handleKeyDown(wParam, lParam);

		break;
	}

	case WM_KEYUP:
	{
		Scene *mainScene = (Scene*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

		if (mainScene)
			mainScene->handleKeyUp(wParam, lParam);

		break;
	}

#pragma endregion

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}
