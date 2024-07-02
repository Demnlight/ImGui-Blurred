#include "Main.h"

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_dx11.h"
#include "ImGui/backends/imgui_impl_win32.h"

#include "WindowHelper.hpp"

#include "ViewModel/MainViewModel.hpp"

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow ) {

	if ( !g_Window->Create( "ImGui Blur", hInstance ) )
		return -1;

	g_Window->AddBlur( );
	g_Window->SetupImGui( );
	g_Window->SetStartWindowPos( );

	while ( true ) {
		if ( g_Window->HandleMessage( ) )
			break;

		if ( !g_Window->Running( ) )
			break;

		g_Window->PreRender( );

		g_MainViewModel->MainForm( );

		g_Window->PostRender( );
	}

	g_Window->Destroy( );
	return 0;
}