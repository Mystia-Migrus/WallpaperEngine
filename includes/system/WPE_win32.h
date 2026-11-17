#pragma once
#ifndef WPE_WIN32_H
#define WPE_WIN32_H
#include <system/WPE_base.h>
#include <windows.h>
#include <D3DFrame/GameTimer.h>


#define WPE_WIN32_DEF_CLIENT_WIDTH 800
#define WPE_WIN32_DEF_CLIENT_HEIGHT 600
#define WPE_WIN32_DEF_WNDCAPTION TEXT("Win32")
#define WPE_WIN32_DEF_WNDCAPTION_SIZE 64

WPE_NAMESPACE_BEGIN
namespace Win32 {
    //TODO: 窗口通信

    class Win32 {
    public:

    public:
        Win32(HINSTANCE hInstance);

        Win32(const Win32&) = delete;
        Win32& operator=(const Win32&) = delete;

        virtual ~Win32();
    public:
        HINSTANCE AppInstance() const;
        HWND MainWin() const;
        float AspectRatio() const;

        virtual bool Initialize();
        virtual LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);
        
    public:
        virtual void OnResize(){}
        virtual void Update(const GameTimer&) = 0;
        virtual void Render(const GameTimer&) = 0;

        virtual void OnMouseDown(WPARAM, int, int){}
        virtual void OnMouseUp(WPARAM, int, int){}
        virtual void OnMouseMove(WPARAM, int, int){}
    
    protected:
        bool InitializeWindow();

    protected:
        HINSTANCE hInstance;
        HWND hwnd;
        bool bAppPaused = 0;        
        bool bMinimized = 0;        
        bool bMaximized = 0;        
        bool bResizing = 0;         
        bool bFullscreenState = 0;
        GameTimer timer;

        unsigned int cxClient = WPE_WIN32_DEF_CLIENT_WIDTH;
        unsigned int cyClient = WPE_WIN32_DEF_CLIENT_HEIGHT;

        TCHAR wndCaption[WPE_WIN32_DEF_WNDCAPTION_SIZE] = WPE_WIN32_DEF_WNDCAPTION;
    };
}
WPE_NAMESPACE_END
#endif