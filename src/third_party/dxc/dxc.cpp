#include "dxc.hpp"

#include "memory.hpp"

#define WIN32_MEAN_AND_LEAN
#include <windows.h>

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <dxcapi.h>

namespace dxc {
	// From DXSampleHelper.h
	// Source: https://github.com/Microsoft/DirectX-Graphics-Samples
	void throw_if_failed(HRESULT hr) {
		if (FAILED(hr))
		{
			PANIC("DXC Error");
		}
	}

	Result<Array<u8>, String> compile(const Input& input) {
		ComPtr<IDxcUtils> utils;
		throw_if_failed(DxcCreateInstance(
			CLSID_DxcUtils, 
			IID_PPV_ARGS(utils.GetAddressOf())
		));

		DxcBuffer source = {};
		source.Ptr = input.source.ptr();
		source.Size = input.source.len();

		ComPtr<IDxcCompiler3> compiler;
		throw_if_failed(DxcCreateInstance(
			CLSID_DxcCompiler, 
			IID_PPV_ARGS(compiler.GetAddressOf())
		));

		Array<LPCWSTR> args;

		// entry point
		args.push(L"-E");
		WString wide_main;
		wide_main.reserve(input.main.len() + 16);
		wide_main.push(input.main);
		args.push(wide_main.ptr());

		// target profile
		args.push(L"-T");
		switch (input.type) {
		case ShaderType::Vertex:
			args.push(L"vs_6_2");
			break;
		case ShaderType::Pixel:
			args.push(L"ps_6_2");
			break;
		}

		args.push(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
		// args.push(DXC_ARG_DEBUG); //-Zi
		// args.push(DXC_ARG_PACK_MATRIX_ROW_MAJOR);

		ComPtr<IDxcResult> result;
		throw_if_failed(compiler->Compile(
			&source,
			args.ptr(),
			(u32)args.len(),
			nullptr,
			IID_PPV_ARGS(result.GetAddressOf())
		));

		if (result->HasOutput(DXC_OUT_ERRORS))
		{
			ComPtr<IDxcBlobUtf8> output;
			throw_if_failed(result->GetOutput(
				DXC_OUT_ERRORS,
				IID_PPV_ARGS(output.GetAddressOf()),
				nullptr
			));

			if (output->GetBufferSize() > 0) {
				String error;
				error.reserve(output->GetBufferSize());
				error.set_len(output->GetBufferSize());
				core::mem::copy(error.ptr(), output->GetBufferPointer(), error.len());
				OutputDebugStringA(error.ptr());
				// return error;
			}
		}

		ComPtr<IDxcBlob> output;
		throw_if_failed(result->GetOutput(
			DXC_OUT_OBJECT, 
			IID_PPV_ARGS(output.GetAddressOf()),
			nullptr
		));

		Array<u8> binary;
		binary.reserve(output->GetBufferSize());
		binary.set_len(output->GetBufferSize());
		core::mem::copy(binary.ptr(), output->GetBufferPointer(), binary.len());

		return binary;
	}
}