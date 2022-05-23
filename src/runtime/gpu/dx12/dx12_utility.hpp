#pragma once

#include "core/minimal.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <cstdio>

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;

// From DXSampleHelper.h
// Source: https://github.com/Microsoft/DirectX-Graphics-Samples
inline void throw_if_failed(HRESULT hr) {
	if (FAILED(hr))
	{
		PANIC(true);
		printf("Error\n");
	}
}

#include <d3d12.h>
#include <dxgi1_6.h>

#undef interface