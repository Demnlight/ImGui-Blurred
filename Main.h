#pragma once

#include <window.h>
#include <memory>
#include <d3d11.h>

class CMain {

public:
	CMain( ) {}
	~CMain( ) {}
};

inline std::shared_ptr<CMain> g_Main = std::make_shared<CMain>( );