#include "d3d12_context.hpp"
#include "d3d12_resources.hpp"
#include "window.hpp"

D3D12DescriptorHeap::D3D12DescriptorHeap(
	ComPtr<ID3D12Device1> device,
	D3D12_DESCRIPTOR_HEAP_TYPE type, 
	usize cap,
	bool shader_visible
) : m_cap(cap) {
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = (UINT)cap;
	desc.Type = type;
	desc.Flags = (D3D12_DESCRIPTOR_HEAP_FLAGS)shader_visible; // D3D12_DESCRIPTOR_HEAP_FLAG_NONE == 0, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE == 1

	throw_if_failed(device->CreateDescriptorHeap(
		&desc,
		IID_PPV_ARGS(&m_heap)
	));

	m_size = device->GetDescriptorHandleIncrementSize(type);
	m_free_slots.reserve(cap);
	// TODO: memcpy this
	for (int i = 0; i < cap; ++i) {
		m_free_slots.push(false);
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::alloc() {
	for (int i = 0; i < m_cap; ++i) {
		auto& slot = m_free_slots[i];
		if (!slot) {
			slot = true;
			D3D12_CPU_DESCRIPTOR_HANDLE handle = m_heap->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += i * m_size;
			return handle;
		}
	}
	PANIC("Descriptor Heap Full");
	return {};
}

void D3D12DescriptorHeap::free(D3D12_CPU_DESCRIPTOR_HANDLE handle) {
	const D3D12_CPU_DESCRIPTOR_HANDLE start = m_heap->GetCPUDescriptorHandleForHeapStart();
	const usize index = (handle.ptr - start.ptr) / m_size;
	VERIFY(m_free_slots[index]);
	m_free_slots[index] = false;
}

D3D12Context::D3D12Context() {
	d3d12_library = Library::open("d3d12.dll").unwrap();
	create_device = (CreateDevice)d3d12_library.find("D3D12CreateDevice");
	serialize_root_signature = (SerializeRootSignature)d3d12_library.find("D3D12SerializeRootSignature");
	get_debug_interface = (GetDebugInterface)d3d12_library.find("D3D12GetDebugInterface");

	// Always enable the debug layer before doing anything DX12 related
	// so all possible errors generated while creating DX12 objects
	// are caught by the debug layer.
#if BUILD_DEBUG
	(get_debug_interface)(IID_PPV_ARGS(&debug_interface));
	debug_interface->EnableDebugLayer();
#endif

	// Find the best adapter. We're not going to use WARP ATM
	ComPtr<IDXGIAdapter4> adapter;
	{
		UINT create_factory_flags = 0;
#if BUILD_DEBUG
		create_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		throw_if_failed(CreateDXGIFactory2(create_factory_flags, IID_PPV_ARGS(&factory)));

		ComPtr<IDXGIAdapter1> adapter1;
		ComPtr<IDXGIAdapter4> adapter4;

		usize max_dedicated_video_memory = 0;
		for (UINT i = 0; factory->EnumAdapters1(i, &adapter1) != DXGI_ERROR_NOT_FOUND; ++i) {
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			adapter1->GetDesc1(&dxgiAdapterDesc1);

			// Check to see if the adapter can create a D3D12 device without actually
			// creating it. The adapter with the largest dedicated video memory
			// is favored.
			if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
				SUCCEEDED((create_device)(adapter1.Get(),
				D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
				dxgiAdapterDesc1.DedicatedVideoMemory > max_dedicated_video_memory) {
				max_dedicated_video_memory = dxgiAdapterDesc1.DedicatedVideoMemory;
				throw_if_failed(adapter1.As(&adapter4));
			}
		}

		adapter = adapter4;
	}

	// Create the device
	{
		ComPtr<ID3D12Device1> device1;
		auto result = (create_device)(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device1));
		throw_if_failed(result);

		// Enable debug messages in debug mode.
#if BUILD_DEBUG
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(device1.As(&pInfoQueue)))
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
	
			// Suppress whole categories of messages
			//D3D12_MESSAGE_CATEGORY Categories[] = {};
	
			// Suppress messages based on their severity level
			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};
	
			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
			};
	
			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			//NewFilter.DenyList.NumCategories = _countof(Categories);
			//NewFilter.DenyList.pCategoryList = Categories;
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;
	
			throw_if_failed(pInfoQueue->PushStorageFilter(&NewFilter));
		}
#endif

		device = device1;
	}

	// Create the direct command queue
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type =     D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags =    D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		throw_if_failed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue)));
	}

	// Create the command allocator
	throw_if_failed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, 
		IID_PPV_ARGS(&command_allocator)
	));

	// Resource Descriptors
	{
		D3D12_DESCRIPTOR_RANGE texture2d_range = {};
		texture2d_range.BaseShaderRegister = 0;
		texture2d_range.NumDescriptors = -1;
		texture2d_range.OffsetInDescriptorsFromTableStart = 0;
		texture2d_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		texture2d_range.RegisterSpace = 1;

		D3D12_ROOT_PARAMETER param1 = {};
		param1.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		param1.Constants.RegisterSpace;
		param1.Constants.Num32BitValues = 16;
		param1.Constants.ShaderRegister;

		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.pParameters = &param1;
		desc.NumParameters = 1;
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		throw_if_failed((serialize_root_signature)(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			&signature,
			&error
		));
		throw_if_failed(device->CreateRootSignature(
			0,
			signature->GetBufferPointer(), 
			signature->GetBufferSize(), 
			IID_PPV_ARGS(&root_signature)
		));

		rtv_heap = D3D12DescriptorHeap(
			device, 
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 
			2048, 
			false
		);
		dsv_heap = D3D12DescriptorHeap(
			device, 
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 
			2048, 
			false
		);
		resource_descriptor_heap = D3D12DescriptorHeap(
			device, 
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 
			4096, 
			true
		);
	}
}