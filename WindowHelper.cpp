#include "WindowHelper.hpp"
#include "ImGui/backends/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT CWindowHelper::WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	if ( ImGui_ImplWin32_WndProcHandler( hwnd, uMsg, wParam, lParam ) )
		return true;

	switch ( uMsg ) {
	case WM_SIZE:
		if ( wParam == SIZE_MINIMIZED )
			return 0;

		g_Window->DirectXVar.ResizeWidth = ( UINT ) LOWORD( lParam );
		g_Window->DirectXVar.ResizeHeight = ( UINT ) HIWORD( lParam );

		if ( g_Window->IsFrameFixed( ) ) {
			g_Window->WindowWidth( ) = ( UINT ) LOWORD( lParam );
			g_Window->WindowHeight( ) = ( UINT ) HIWORD( lParam );

		}

		return 0;
	case WM_SYSCOMMAND:
		if ( ( wParam & 0xfff0 ) == SC_KEYMENU ) // Disable ALT application menu
			return 0;
		break;
	case WM_LBUTTONDOWN:
	{
		g_Window->Position( ) = MAKEPOINTS( lParam ); // set click points
	}return 0;

	case WM_MOUSEMOVE:
	{
		if ( wParam == MK_LBUTTON && ImGui::GetIO( ).Fonts ) {
			const auto points = MAKEPOINTS( lParam );
			auto rect = ::RECT{ };

			GetWindowRect( g_Window->hWnd( ), &rect );

			rect.left += points.x - g_Window->Position( ).x;
			rect.top += points.y - g_Window->Position( ).y;

			if ( g_Window->Position( ).x >= 0 && g_Window->Position( ).x <= g_Window->WindowWidth( ) && g_Window->Position( ).y >= 0 && g_Window->Position( ).y <= ImGui::GetIO().Fonts->Fonts[0]->FontSize * 2 )
				SetWindowPos( g_Window->hWnd( ), HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER );
		}
	}return 0;
	case WM_DESTROY:
		::PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

bool CWindowHelper::stDirectXFunc::CreateDeviceD3D( HWND hWnd ) {
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 144;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[ 2 ] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_Window->DirectXVar.SwapChain, &g_Window->DirectXVar.Device, &featureLevel, &g_Window->DirectXVar.DeviceContext );
	if ( res == DXGI_ERROR_UNSUPPORTED ) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_Window->DirectXVar.SwapChain, &g_Window->DirectXVar.Device, &featureLevel, &g_Window->DirectXVar.DeviceContext );
	if ( res != S_OK )
		return false;

	CreateRenderTarget( );
	return true;
}

void CWindowHelper::stDirectXFunc::CleanupDeviceD3D( ) {
	CleanupRenderTarget( );
	if ( g_Window->SwapChain( ) ) { g_Window->SwapChain( )->Release( ); g_Window->DirectXVar.SwapChain = nullptr; }
	if ( g_Window->DeviceContext( ) ) { g_Window->DeviceContext( )->Release( ); g_Window->DirectXVar.DeviceContext = nullptr; }
	if ( g_Window->Device( ) ) { g_Window->Device( )->Release( ); g_Window->DirectXVar.Device = nullptr; }
}

void CWindowHelper::stDirectXFunc::CreateRenderTarget( ) {
	ID3D11Texture2D* pBackBuffer;
	g_Window->SwapChain( )->GetBuffer( 0, IID_PPV_ARGS( &pBackBuffer ) );
	g_Window->Device( )->CreateRenderTargetView( pBackBuffer, nullptr, &g_Window->DirectXVar.RenderTargetView );
	pBackBuffer->Release( );
}

void CWindowHelper::stDirectXFunc::CleanupRenderTarget( ) {
	if ( g_Window->RenderTargetView( ) ) {
		g_Window->RenderTargetView( )->Release( );
		g_Window->DirectXVar.RenderTargetView = nullptr;
	}
}