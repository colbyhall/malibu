#pragma once

#include "../context.hpp"
#include "dx12_utility.hpp"

constexpr i32 NUM_BUFFERS = 3;

class Dx12Context : public gpu::ContextInterface {
public:
	explicit Dx12Context();

private:
	ComPtr<ID3D12Device1> device;
	ComPtr<ID3D12CommandQueue> queue;
};