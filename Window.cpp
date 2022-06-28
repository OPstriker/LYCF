
//Code From: https://github.com/ocornut/imgui/issues/2022#issue-351276670

// Used and Modified by OPstriker



#include <Windows.h>

//#define WIN32_LEAN_AND_MEAN

#include "LYCF.h"
#include <Uxtheme.h>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>

#include <stdbool.h>

// For GLEW
//#define GLEW_STATIC // Gotta set this to STATIC
//#include <GL/glew.h>
//#include <GL/GL.h>

// For GLAD
#include <glad/glad.h>

#include <wglext.h>
#include <tchar.h>

#include "style.h"

#include <wingdi.h>
#include <vssym32.h>
#include <uxtheme.h>

#include <dwmapi.h>
//#include "resource.h"



//DEFINES/MACROS


//GLOBAL VARIABLES
HGLRC   g_GLRenderContext;
HDC     g_HDCDeviceContext;
HWND    g_hwnd;
PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT;
PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT;

HWND LYCF_hwnd;

int     g_display_w = 850; // 750
int     g_display_h = 550; // 450

bool LYCF::Display::isWindowActive;

void CreateGlContext();
bool SetSwapInterval(int interval); //0 - No Interval, 1 - Sync whit VSYNC, n - n times Sync with VSYNC
bool WGLExtensionSupported(const char* extension_name);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Aplication Entry
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2); // DPI Scalling...

    LPCSTR CLASS_NAME = "LYCF";

    HBRUSH hBrBackground = CreateSolidBrush(RGB(25, 33, 36)); // Custom Window Background Color...

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    WNDCLASSEX wc = { 0 };
    {
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.hbrBackground = hBrBackground;
        wc.lpszClassName = CLASS_NAME;
        wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
        wc.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
    }
    if (!RegisterClassEx(&wc))
        return 1;


    // Display The Window if it Already is Running, without Creating another Instance.
    // https://stackoverflow.com/a/33531179/17882108
    HANDLE m_singleInstanceMutex = CreateMutex(NULL, TRUE, wc.lpszClassName);
    if (m_singleInstanceMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        LYCF_hwnd = FindWindow(wc.lpszClassName, CLASS_NAME);
        HWND existingApp = FindWindow(wc.lpszClassName, CLASS_NAME); //
        if (existingApp) SetForegroundWindow(existingApp);
        if (existingApp) ShowWindow(existingApp, SW_RESTORE);
        return FALSE;
    }

    // check whether windoe is on top...


    g_hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        wc.lpszClassName,
        "LYCF",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, // 
        520, 250, // 500, 250
        g_display_w, g_display_h,
        0, 0,
        hInstance,
        0
    );


    SetWindowTextA(g_hwnd, "LYCF");

    // Show the window
    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);

    //Prepare OpenGlContext
    CreateGlContext();
    SetSwapInterval(1);
    gladLoadGL();

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.IniFilename = NULL;
    //io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\bahnschrift.ttf", 20.0f);

    style::load_font();

    //Init Win32
    ImGui_ImplWin32_Init(g_hwnd);

    //Init OpenGL Imgui Implementation
    // GL 3.3 + GLSL 330
    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

    //Set Window bg color
    //ImVec4 clear_color = ImVec4(1.000F, 1.000F, 1.000F, 1.0F);

    ImVec4 clear_color = ImVec4(0.10f, 0.13f, 0.14f, 1.0f);

    // Setup style
    ImGui::StyleColorsClassic();

    // Main loop
    MSG msg = { };
    ZeroMemory(&msg, sizeof(msg));

    if (msg.message == WM_QUIT)
    {
        return 0;
    }

    while (msg.message != WM_QUIT)
    {

        while (PeekMessage(&msg, NULL, 0u, 0u, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (GetForegroundWindow() != g_hwnd) // Pause Process While minizmized;
            Sleep(1);

        // Start the Dear ImGui frame


            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            //show Main Window



            style::MainWindow();
            {
              
              ImGui::Begin("LYCF", 0, ImGuiWindowFlags_NoResize
                  | ImGuiWindowFlags_NoTitleBar
                  | ImGuiWindowFlags_NoMove
                  | ImGuiWindowFlags_NoBackground
              );



              ImGui::SetWindowSize(ImVec2(850.0f, 550.0f), 0); // 850, 550
              ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

              //ImGui::ShowDemoWindow(0);
              style::render();


              ImGui::End();
            }



            ImGui::EndFrame();

            // Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);
            glViewport(0, 0, g_display_w, g_display_h);                 //Display Size got from Resize Command
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);
            SwapBuffers(g_HDCDeviceContext);
            
        
    }

    // Cleanup

    ImGui_ImplOpenGL3_Shutdown();
    void gladUnloadWGL(void);
    ImGui_ImplWin32_Shutdown();
    wglDeleteContext(g_GLRenderContext);
    ImGui::DestroyContext();

    UnregisterClass("LYCF", hInstance);
    //DestroyWindow(g_hwnd);
    UnregisterClass(_T("LYCF"), wc.hInstance);
    
    //glad_polle
    //DestroyWindow(g_hwnd);
    PostMessage(g_hwnd, WM_DESTROY, 0, 0);
    return 0;
}

void SetForGroundWindow_Main()
{
    SetForegroundWindow(g_hwnd);
}

// For NotifyIcon in StatusBar
#define WM_LYCF (WM_USER + 1)
bool NotifyCon = true;
#define ID_DISPLAY_APP_LYCF 103
#define ID_LYCF_CLOSE 104


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    NOTIFYICONDATA nid;

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 100;
    nid.uVersion = NOTIFYICON_VERSION;
    nid.uCallbackMessage = WM_LYCF;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    strcpy_s(nid.szTip, "LYCF");
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

    if(NotifyCon)
    Shell_NotifyIcon(NIM_ADD, &nid);


    //Custom Frame Expiriment... (Adandoned for Now...)


    bool fCallDWP = true;
    BOOL fDwnEnabled = false;

    LRESULT lRet = 0;
    HRESULT hr = S_OK;

    hr = DwmIsCompositionEnabled(&fDwnEnabled);

    // Custom Frame Expiririement...
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_CREATE: {
        RECT rcClient;

        GetWindowRect(hWnd, &rcClient);

        SetWindowPos(hWnd,
            NULL,
            rcClient.left, rcClient.top,
            rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
            SWP_FRAMECHANGED);

        fCallDWP = true;

        lRet = 0;
        return 0;

    }
    case WM_CLOSE:
        ShowWindow(hWnd, SW_HIDE);
        return 0;
        break;
    case WM_DESTROY:
        NotifyCon = false;
        Shell_NotifyIcon(NIM_DELETE, &nid);
        //PostQuitMessage(0);
        if (PostMessage(hWnd, WM_QUIT, 0, 0))
            DestroyWindow(hWnd);
        return 0;
        break;
    case WM_NCCALCSIZE: {
        if (!wParam) return DefWindowProc(hWnd, msg, wParam, lParam);
        UINT dpi = GetDpiForWindow(hWnd);


        int frame_x = GetSystemMetricsForDpi(SM_CXFRAME, dpi);
        int frame_y = GetSystemMetricsForDpi(SM_CYFRAME, dpi);
        int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

        NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)lParam;
        RECT* requested_client_rect = params->rgrc;

        requested_client_rect->right -= frame_x + padding;
        requested_client_rect->left += frame_x + padding;
        requested_client_rect->bottom -= frame_y + padding;

        return 0;
        break;
    }
    case WM_NCHITTEST: 
    {
        // Let the default procedure handle resizing areas
        LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);
        switch (hit) {
        case HTNOWHERE:
        case HTRIGHT:
        case HTLEFT:
        case HTTOPLEFT:
        case HTTOP:
        case HTTOPRIGHT:
        case HTBOTTOMRIGHT:
        case HTBOTTOM:
        case HTCLIENT: {
            if (LYCF::EN_LYCF::captionmove)
            {
                hit = HTCAPTION;
                return hit;
            }
            else
            {
                hit = HTCLIENT;
                return hit;
            }
            //;
            //return hit;
        }
        case HTBOTTOMLEFT: {
            return hit;
        }
      }
        //if (hit == HTCLIENT) hit = HTCAPTION;
    
        // Looks like adjustment happening in NCCALCSIZE is messing with the detection
        // of the top hit area so manually fixing that.
        UINT dpi = GetDpiForWindow(hWnd);
        int frame_y = GetSystemMetricsForDpi(SM_CYFRAME, dpi);
        int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
        POINT cursor_point = { 0 };
        cursor_point.x = LOWORD(lParam);
        cursor_point.y = HIWORD(lParam);
        ScreenToClient(hWnd, &cursor_point);
        if (cursor_point.y > 0 && cursor_point.y < frame_y + padding) {
            return HTTOP;
        }
    
        // Since we are drawing our own caption, thi needs to be a custom test
    
        return HTCLIENT;
    }
    case WM_LYCF: {
        switch (lParam)
        {
        case WM_LBUTTONUP: {
            if (SetForegroundWindow(hWnd))
                ShowWindow(hWnd, SW_RESTORE);
            break;
        }
        case WM_RBUTTONUP: {

            POINT ps_tl;
            GetCursorPos(&ps_tl);

            HMENU ToolTipMenu = CreatePopupMenu();

            BOOL wndState = IsWindowVisible(hWnd);

            AppendMenu(ToolTipMenu, MF_STRING, ID_DISPLAY_APP_LYCF, wndState ? "Hide LYCF" : "Show LYCF");
            AppendMenu(ToolTipMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(ToolTipMenu, MF_STRING, ID_LYCF_CLOSE, "Exit");

            SetForegroundWindow(hWnd);

            int id = TrackPopupMenu(ToolTipMenu, TPM_RIGHTBUTTON | TPM_LEFTBUTTON | TPM_RETURNCMD, ps_tl.x, ps_tl.y, 0, hWnd, NULL);

            PostMessage(hWnd, WM_NULL, 0, 0);


            if (id == ID_DISPLAY_APP_LYCF)
            {
                if (wndState)
                {
                    ShowWindow(hWnd, SW_HIDE);
                }
                else
                {
                    ShowWindow(hWnd, SW_RESTORE);
                    SetActiveWindow(hWnd);
                    SetForegroundWindow(hWnd);
                }
            }
            else if (id == ID_LYCF_CLOSE) {
                NotifyCon = false;
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostMessage(hWnd, WM_QUIT, 0, 0);
                PostMessage(hWnd, WM_DESTROY, 0, 0);
            }
            else
            {
                NULL; // Lets just Ingore this...
            }
            break;
        }
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        };
        break;
    }
    case WM_SIZING: {
        SwapBuffers(g_HDCDeviceContext);
        break;
    }
    case WM_SYSKEYDOWN: {
        if (wParam == VK_F4)
        {
            if (NotifyCon)
            {
                ShowWindow(g_hwnd, SW_HIDE);
            }
            else {
                PostMessage(hWnd, WM_DESTROY, 0, 0);
            }
        }
        break;
    }
    case WM_MOVE:
        SwapBuffers(g_HDCDeviceContext);
        break;
    }
        return DefWindowProc(hWnd, msg, wParam, lParam);

        PostMessage(hWnd, WM_DESTROY, 0, 0);

        return 0;
}


void LYCF::EN_LYCF::WindowMove()
{
    
}

void LYCF::EN_LYCF::WindowClose()
{
    if (NotifyCon)
    {
        ShowWindow(g_hwnd, SW_HIDE);
    }
    else {
        DestroyWindow(g_hwnd);
    }
}

void LYCF::EN_LYCF::WindowMinimize()
{
    ShowWindow(g_hwnd, SW_MINIMIZE);
}



void CreateGlContext() {

    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
        32,                   // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Number of bits for the depthbuffer
        8,                    // Number of bits for the stencilbuffer
        0,                    // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    g_HDCDeviceContext = GetDC(g_hwnd);

    int pixelFormal = ChoosePixelFormat(g_HDCDeviceContext, &pfd);
    SetPixelFormat(g_HDCDeviceContext, pixelFormal, &pfd);
    g_GLRenderContext = wglCreateContext(g_HDCDeviceContext);
    wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);
}

bool SetSwapInterval(int interval) {
    if (WGLExtensionSupported("WGL_EXT_swap_control"))
    {
        // Extension is supported, init pointers.
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

        // this is another function from WGL_EXT_swap_control extension
        wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

        wglSwapIntervalEXT(interval);
        return true;
    }

    return false;

}

//Got from https://stackoverflow.com/questions/589064/how-to-enable-vertical-sync-in-opengl/589232
bool WGLExtensionSupported(const char* extension_name) {
    // this is pointer to function which returns pointer to string with list of all wgl extensions
    PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

    // determine pointer to wglGetExtensionsStringEXT function
    _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

    if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
    {
        // string was not found
        return false;
    }

    // extension is supported
    return true;
}