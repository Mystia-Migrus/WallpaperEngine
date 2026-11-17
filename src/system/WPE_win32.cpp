#include <system/WPE_win32.h>

using namespace WPE_NAMESPACE::Win32;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Win32* app = (Win32*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch(msg) {
    case WM_CREATE:
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (PTR_ADDRESS)((LPCREATESTRUCT)lParam)->lpCreateParams);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        if(app)
            return app->MsgProc(hwnd, msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

Win32::Win32(HINSTANCE hInstance): hInstance(hInstance) {

}

Win32::~Win32() {

}

bool Win32::Initialize() {
    if(!InitializeWindow())
        return false;
    return true;
}

bool Win32::InitializeWindow() {
    WNDCLASSEX wndclass;
    RECT rect = {0, 0, (long)cxClient, (long)cyClient};

    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndclass.hIconSm = nullptr;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW);
    wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndclass.lpszMenuName = nullptr;
    wndclass.lpszClassName = TEXT("WPE_Win32");

    if(!RegisterClassEx(&wndclass))
        return false;

    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, 0);
    hwnd = CreateWindowEx(0, TEXT("WPE_Win32"), wndCaption, WS_OVERLAPPEDWINDOW, 
        CW_USEDEFAULT, CW_USEDEFAULT, 
        rect.right - rect.left, 
        rect.bottom - rect.top, 
        nullptr, nullptr, hInstance, this);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return true;
}

LRESULT Win32::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
    case WM_ACTIVATE:
        if(LOWORD(wParam) == WA_INACTIVE) {
            bAppPaused = 1;
            timer.Stop();
        } else {
            bAppPaused = 0;
            timer.Start();
        }
        return 0;
    case WM_ENTERSIZEMOVE:
        bAppPaused = 1;
        bResizing = 1;
        timer.Stop();
        return 0;
    case WM_SIZE:
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        return 0;
    case WM_EXITSIZEMOVE:
        bAppPaused = 0;
        bResizing = 0;
        timer.Start();
        OnResize();
        return 0;
    case WM_MENUCHAR:
        return MAKELRESULT(0, MNC_CLOSE);
    case WM_GETMINMAXINFO:
        (reinterpret_cast<MINMAXINFO*>(lParam))->ptMinTrackSize.x = 200;
        (reinterpret_cast<MINMAXINFO*>(lParam))->ptMinTrackSize.y = 200;
        return 0;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        OnMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));
        return 0;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        OnMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));
        return 0;
    case WM_MOUSEMOVE:
        OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));
        return 0;
    }   

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

HWND Win32::MainWin() const {
    return hwnd;
}

HINSTANCE Win32::AppInstance() const{
    return hInstance;
}

float Win32::AspectRatio() const {
    return static_cast<float>(cxClient) / static_cast<float>(cyClient);
}