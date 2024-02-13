#pragma once

class CMainViewModel
{
public:
	CMainViewModel( ) {};
	~CMainViewModel( ) {};

	void MainForm( );

private:
	int nOldWidth = 0;
	int nOldHeight = 0;

};

inline CMainViewModel* g_MainViewModel = new CMainViewModel( );