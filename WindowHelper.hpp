#pragma once
#include <memory>
#include <string>
#include <Windows.h>
#include <d3d11.h>

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_dx11.h"
#include "ImGui/backends/imgui_impl_win32.h"

#pragma comment(lib, "D3D11.lib")

class CWindowHelper
{
public:
	CWindowHelper( ) {

	};
	~CWindowHelper( ) {};

	static LRESULT __stdcall WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Create( std::string sWindowName, HINSTANCE hInstance ) {

		RECT desktop;
		const HWND hDesktop = GetDesktopWindow( );
		GetWindowRect( hDesktop, &desktop );

		wc.lpfnWndProc = WindowProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = sWindowName.c_str( );
		wc.lpszMenuName = sWindowName.c_str( );
		RegisterClass( &wc );

		HWND hwnd = CreateWindowEx( WS_EX_COMPOSITED, sWindowName.c_str( ), sWindowName.c_str( ), WS_OVERLAPPEDWINDOW, desktop.right / 2 - 400, desktop.bottom / 2 - 400, 799, 799, NULL, NULL, hInstance, NULL );

		this->Data.hWnd = hwnd;

		// Initialize Direct3D
		if ( !stDirectXFunc::CreateDeviceD3D( this->hWnd( ) ) ) {
			stDirectXFunc::CleanupDeviceD3D( );
			::UnregisterClass( wc.lpszClassName, wc.hInstance );
			return 1;
		}

		ShowWindow( this->hWnd( ), 1 );
		UpdateWindow( this->hWnd( ) );

		return hwnd != NULL;
	}

	void Destroy( ) {
		ImGui_ImplDX11_Shutdown( );
		ImGui_ImplWin32_Shutdown( );
		ImGui::DestroyContext( );

		this->DirectXFunc.CleanupDeviceD3D( );
		::DestroyWindow( this->hWnd( ) );
		::UnregisterClass( this->GetWC( ).lpszClassName, this->GetWC( ).hInstance );
	}

	void AddBlur( ) {
		struct ACCENTPOLICY
		{
			int na;
			int nf;
			int nc;
			int nA;
		};
		struct WINCOMPATTRDATA
		{
			int na;
			PVOID pd;
			ULONG ul;
		};

		const HINSTANCE hm = LoadLibrary( "user32.dll" );
		if ( hm ) {
			typedef BOOL( WINAPI* pSetWindowCompositionAttribute )( HWND, WINCOMPATTRDATA* );

			const pSetWindowCompositionAttribute SetWindowCompositionAttribute = ( pSetWindowCompositionAttribute ) GetProcAddress( hm, "SetWindowCompositionAttribute" );
			if ( SetWindowCompositionAttribute ) {
				ACCENTPOLICY policy = { 4,0,155,0 };
				WINCOMPATTRDATA data = { 19, &policy,sizeof( ACCENTPOLICY ) };
				SetWindowCompositionAttribute( this->hWnd( ), &data );
			}
			FreeLibrary( hm );
		}
	}

	void SetupImGui( ) {
		IMGUI_CHECKVERSION( );
		ImGui::CreateContext( );
		ImGuiIO& io = ImGui::GetIO( ); ( void ) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark( );

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init( this->hWnd( ) );
		ImGui_ImplDX11_Init( this->Device( ), this->DeviceContext( ) );
	}

	void PreRender( ) {
		if ( this->DirectXVar.ResizeWidth != 0 && this->DirectXVar.ResizeHeight != 0 ) {
			stDirectXFunc::CleanupRenderTarget( );
			this->DirectXVar.SwapChain->ResizeBuffers( 0, this->DirectXVar.ResizeWidth, this->DirectXVar.ResizeHeight, DXGI_FORMAT_UNKNOWN, 0 );
			this->DirectXVar.ResizeWidth = this->DirectXVar.ResizeHeight = 0;
			stDirectXFunc::CreateRenderTarget( );
		}

		ImGui_ImplDX11_NewFrame( );
		ImGui_ImplWin32_NewFrame( );
		ImGui::NewFrame( );
	}

	void PostRender( ) {
		ImGui::Render( );
		const float clear_color_with_alpha[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
		this->DirectXVar.DeviceContext->OMSetRenderTargets( 1, &this->DirectXVar.RenderTargetView, nullptr );
		this->DirectXVar.DeviceContext->ClearRenderTargetView( this->DirectXVar.RenderTargetView, clear_color_with_alpha );
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

		this->DirectXVar.SwapChain->Present( 1, 0 );
	}

	void FixIssues( ) {
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow( );
		GetWindowRect( hDesktop, &desktop );
		SetWindowPos( this->hWnd( ), NULL, desktop.right / 2 - 400, desktop.bottom / 2 - 400, 800, 800, SWP_SHOWWINDOW );

	}

	bool HandleMessage( ) {
		bool done = false;
		MSG msg;
		while ( ::PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) ) {
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
			if ( msg.message == WM_QUIT )
				done = true;
		}

		return done;
	}

	HWND& hWnd( ) {
		return this->Data.hWnd;
	}

	ID3D11Device* Device( ) { return this->DirectXVar.Device; };
	ID3D11DeviceContext* DeviceContext( ) { return this->DirectXVar.DeviceContext; };
	IDXGISwapChain* SwapChain( ) { return this->DirectXVar.SwapChain; };
	UINT& ResizeWidth( ) { return this->DirectXVar.ResizeWidth; }
	UINT& ResizeHeight( ) { return this->DirectXVar.ResizeHeight; };
	ID3D11RenderTargetView* RenderTargetView( ) { return this->DirectXVar.RenderTargetView; };

	struct stDirectXFunc
	{
		static bool CreateDeviceD3D( HWND hWnd );
		static void CleanupDeviceD3D( );
		static void CreateRenderTarget( );
		static void CleanupRenderTarget( );
	} DirectXFunc;

	WNDCLASS& GetWC( ) {
		return this->wc;
	};

private:
	WNDCLASS wc = { };

	struct stDirectXVar
	{
		ID3D11Device* Device = nullptr;
		ID3D11DeviceContext* DeviceContext = nullptr;
		IDXGISwapChain* SwapChain = nullptr;
		UINT ResizeWidth = 0, ResizeHeight = 0;
		ID3D11RenderTargetView* RenderTargetView = nullptr;

	} DirectXVar;

	struct stData
	{
		HWND hWnd = NULL;
	} Data;
};

inline std::shared_ptr<CWindowHelper> g_Window = std::make_shared<CWindowHelper>( );