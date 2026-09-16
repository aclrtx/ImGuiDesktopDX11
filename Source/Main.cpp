#include "Main.h"

static bool CreateDevice( const HWND& handle )
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = handle;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevel;
    constexpr D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        featureLevelArray,
        2,
        D3D11_SDK_VERSION,
        &sd,
        &g_pSwapChain,
        &g_pD3DDevice,
        &featureLevel,
        &g_pD3DDeviceContext
    );

    if ( result == DXGI_ERROR_UNSUPPORTED )
        result = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            0,
            featureLevelArray,
            2,
            D3D11_SDK_VERSION,
            &sd,
            &g_pSwapChain,
            &g_pD3DDevice,
            &featureLevel,
            &g_pD3DDeviceContext
        );

    if ( result != S_OK )
        return false;

    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer( 0, IID_PPV_ARGS( &pBackBuffer ) );
    g_pD3DDevice->CreateRenderTargetView( pBackBuffer, nullptr, &g_pRenderTargetView );
    pBackBuffer->Release();

    return true;
}

static void CleanupDevice()
{
    if ( g_pRenderTargetView )
    {
        g_pRenderTargetView->Release();
        g_pRenderTargetView = nullptr;
    }
    if ( g_pSwapChain )
    {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if ( g_pD3DDeviceContext )
    {
        g_pD3DDeviceContext->Release();
        g_pD3DDeviceContext = nullptr;
    }
    if ( g_pD3DDevice )
    {
        g_pD3DDevice->Release();
        g_pD3DDevice = nullptr;
    }
}

static void DragWindow()
{
    static POINT lastCursorPos;

    if ( ImGui::IsMouseDragging( ImGuiMouseButton_Left ) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemFocused() )
    {
        POINT cursorPos;
        GetCursorPos( &cursorPos );

        if ( cursorPos.x != lastCursorPos.x || cursorPos.y != lastCursorPos.y )
        {
            if ( lastCursorPos.x != 0 && lastCursorPos.y != 0 )
            {
                const int deltaX = cursorPos.x - lastCursorPos.x;
                const int deltaY = cursorPos.y - lastCursorPos.y;
                RECT windowRect;

                GetWindowRect( g_handle, &windowRect );
                SetWindowPos(
                    g_handle,
                    nullptr,
                    windowRect.left + deltaX,
                    windowRect.top + deltaY,
                    0,
                    0,
                    SWP_NOSIZE | SWP_NOZORDER
                );
            }

            lastCursorPos = cursorPos;
        }
    }
    else
    {
        lastCursorPos.x = 0;
        lastCursorPos.y = 0;
    }
}

static LRESULT WINAPI WndProc( const HWND handle, const UINT message, const WPARAM data, const LPARAM context )
{
    if ( ImGui_ImplWin32_WndProcHandler( handle, message, data, context ) )
        return true;

    switch ( message )
    {
        case WM_SYSCOMMAND:
        {
            if ( ( data & 0xFFF0 ) == SC_KEYMENU )
                return 0;

            break;
        }

        case WM_DESTROY:
        {
            PostQuitMessage( 0 );

            return 0;
        }
    }

    return DefWindowProc( handle, message, data, context );
}

int WINAPI WinMain( HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow )
{
    ImGui_ImplWin32_EnableDpiAwareness();
    const float dpiScale =
        ImGui_ImplWin32_GetDpiScaleForMonitor( MonitorFromPoint( POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY ) );

    g_windowClass = { sizeof( g_windowClass ),
                      CS_CLASSDC,
                      WndProc,
                      0L,
                      0L,
                      instance,
                      nullptr,
                      nullptr,
                      nullptr,
                      nullptr,
                      WINDOW_TITLE,
                      nullptr };
    RegisterClassEx( &g_windowClass );

    const int windowWidth = static_cast<int>( WINDOW_WIDTH * dpiScale );
    const int windowHeight = static_cast<int>( WINDOW_HEIGHT * dpiScale );
    const int screenWidth = GetSystemMetrics( SM_CXSCREEN );
    const int screenHeight = GetSystemMetrics( SM_CYSCREEN );
    const int posX = ( screenWidth - windowWidth ) / 2;
    const int posY = ( screenHeight - windowHeight ) / 2;

    g_handle = CreateWindowEx(
        WS_EX_APPWINDOW | WS_EX_LAYERED,
        WINDOW_TITLE,
        WINDOW_TITLE,
        WS_POPUP | WS_MINIMIZEBOX,
        posX,
        posY,
        windowWidth,
        windowHeight,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if ( !CreateDevice( g_handle ) )
    {
        CleanupDevice();
        DestroyWindow( g_handle );
        g_handle = nullptr;
        UnregisterClass( WINDOW_TITLE, instance );

        return 1;
    }

    SetLayeredWindowAttributes( g_handle, RGB( 0, 0, 0 ), 0, LWA_COLORKEY );
    ShowWindow( g_handle, SW_SHOWDEFAULT );
    UpdateWindow( g_handle );

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = nullptr;

    char fontPath[MAX_PATH];
    sprintf( fontPath, "%s\\Fonts\\ArialBD.ttf", getenv( "WINDIR" ) );
    io.Fonts->AddFontFromFileTTF( fontPath, 14.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic() );

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes( dpiScale );
    style.FontScaleDpi = dpiScale;

    ImGui_ImplWin32_Init( g_handle );
    ImGui_ImplDX11_Init( g_pD3DDevice, g_pD3DDeviceContext );

    bool quit = false;
    while ( !quit )
    {
        MSG message;
        while ( PeekMessage( &message, nullptr, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &message );
            DispatchMessage( &message );

            if ( message.message == WM_QUIT )
                quit = true;
        }

        if ( quit )
            break;

        if ( g_swapChainOccluded && g_pSwapChain->Present( 0, DXGI_PRESENT_TEST ) == DXGI_STATUS_OCCLUDED )
        {
            Sleep( 10 );

            continue;
        }

        g_swapChainOccluded = false;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        DragWindow();
        {
            ImGui::SetNextWindowPos( ImVec2( 0.0f, 0.0f ), ImGuiCond_Always );
            ImGui::SetNextWindowSize( ImGui::GetIO().DisplaySize, ImGuiCond_Always );
            ImGui::Begin(
                WINDOW_TITLE, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
            );

            ImGui::Text( "Text" );

            ImGui::End();
        }
        ImGui::Render();

        constexpr float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_pD3DDeviceContext->OMSetRenderTargets( 1, &g_pRenderTargetView, nullptr );
        g_pD3DDeviceContext->ClearRenderTargetView( g_pRenderTargetView, clearColor );

        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );

        g_swapChainOccluded = ( g_pSwapChain->Present( 1, 0 ) == DXGI_STATUS_OCCLUDED );
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDevice();
    DestroyWindow( g_handle );
    UnregisterClass( WINDOW_TITLE, instance );

    return 0;
}
