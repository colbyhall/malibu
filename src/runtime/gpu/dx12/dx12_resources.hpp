#pragma once

#include "../resources.hpp"
#include "dx12_utility.hpp"

class Dx12Texture : public gpu::TextureInterface {
public:

private:
	ComPtr<ID3D12Resource> m_resource;
};