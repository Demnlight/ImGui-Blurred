#pragma once
#include "../WindowHelper.hpp"

enum WINDOW_STATE : int {
	DEFAULT = 0,
	MAX = 1,
	MIN = 2
};

class CWindowState
{
public:
	CWindowState( ) {};
	~CWindowState( ) {};

	WINDOW_STATE& WindowState( ) { return this->eWindowState; }

	void MinimizeWindow( ) {
		if ( this->eWindowState != WINDOW_STATE::MIN )
			this->eWindowState = WINDOW_STATE::MIN;
		else {
			this->RestoreWindow( );
			return;
		}
	
		PostMessage( g_Window->hWnd( ), WM_SYSCOMMAND, SC_MINIMIZE, 0 );
	}
	void MaximizeWindow( ) {
		if ( this->eWindowState != WINDOW_STATE::MAX )
			this->eWindowState = WINDOW_STATE::MAX;
		else {
			this->RestoreWindow( );
			return;
		}

		LONG lStyle = GetWindowLong( g_Window->hWnd( ), GWL_STYLE );
		lStyle &= ~WS_BORDER;

		SetWindowLong( g_Window->hWnd( ), GWL_STYLE, lStyle ); 
		PostMessage( g_Window->hWnd( ), WM_SYSCOMMAND, SC_MAXIMIZE, 0 );
	}
	void RestoreWindow( ) {
		if ( this->eWindowState != WINDOW_STATE::DEFAULT )
			this->eWindowState = WINDOW_STATE::DEFAULT;

		PostMessage( g_Window->hWnd( ), WM_SYSCOMMAND, SC_RESTORE, 0 );
		
		LONG lStyle = GetWindowLong( g_Window->hWnd( ), GWL_STYLE );
		lStyle |= WS_BORDER;
		SetWindowLong( g_Window->hWnd( ), GWL_STYLE, lStyle );
	}

private:

	WINDOW_STATE eWindowState = WINDOW_STATE::DEFAULT;


};

inline CWindowState* g_WindowState = new CWindowState( );