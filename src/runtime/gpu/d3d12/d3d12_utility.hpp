#pragma once

#include "core.hpp"

#define WIN32_MEAN_AND_LEAN
#include <windows.h>

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using Microsoft::WRL::ComPtr;

// From DXSampleHelper.h
// Source: https://github.com/Microsoft/DirectX-Graphics-Samples
inline void throw_if_failed(HRESULT hr) {
	if (FAILED(hr))
	{
		PANIC("D3D12 Crash");
	}
}

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <d3d12sdklayers.h>

#undef interface