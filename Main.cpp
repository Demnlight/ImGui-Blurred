#include "Main.h"

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_dx11.h"
#include "ImGui/backends/imgui_impl_win32.h"

#include "WindowHelper.hpp"

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow ) {

	if ( !g_Window->Create( "ImGui Blur", hInstance ) )
		return -1;

	g_Window->AddBlur( );
	g_Window->SetupImGui( );
	g_Window->FixIssues( );

	while ( true ) {
		if ( g_Window->HandleMessage( ) )
			break;

		g_Window->PreRender( );

		ImGui::ShowDemoWindow( );

		g_Window->PostRender( );
	}

	g_Window->Destroy( );
	return 0;
}