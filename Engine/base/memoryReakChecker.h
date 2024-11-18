#pragma once

#include<dxgidebug.h>
#include<dxgi1_6.h>
#include<d3d12.h>
#include<wrl.h>
#include <iostream>


struct D3DResourceLeakChecker {
    static void CheckLeaks() {
        Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
            debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
            debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
            debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
 
        }
    }
};


// メモリリークチェックの登録
void RegisterLeakCheck() {
	std::atexit(D3DResourceLeakChecker::CheckLeaks);
}