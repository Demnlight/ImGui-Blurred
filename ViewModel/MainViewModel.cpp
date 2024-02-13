#include "MainViewModel.hpp"

#include "../ImGui/imgui.h"
#include "../WindowHelper.hpp"
#include <dwmapi.h>

void CMainViewModel::MainForm( ) {
	ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
	ImGui::SetNextWindowSize( ImVec2( g_Window->WindowWidth( ), g_Window->WindowHeight( ) ) );
	ImGui::Begin( "MainWindow", &g_Window->Running( ), ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar );

	g_Window->WindowWidth( ) = ImGui::GetWindowWidth( );
	g_Window->WindowHeight( ) = ImGui::GetWindowHeight( );

	ImGui::End( );

	if ( this->nOldWidth != g_Window->WindowWidth( ) ||
		 this->nOldHeight != g_Window->WindowHeight( ) ) {

		SetWindowPos( g_Window->hWnd( ), HWND_TOPMOST, 0, 0, g_Window->WindowWidth( ), g_Window->WindowHeight( ), SWP_SHOWWINDOW | SWP_NOMOVE );

		this->nOldWidth = g_Window->WindowWidth( );
		this->nOldHeight = g_Window->WindowHeight( );
	}
}