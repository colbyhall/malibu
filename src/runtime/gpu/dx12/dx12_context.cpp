#include "dx12_context.hpp"
#include "core/window.hpp"

Dx12Context::Dx12Context() {
	// Always enable the debug layer before doing anything DX12 related
	// so all possible errors generated while creating DX12 objects
	// are caught by the debug layer.
#if BUILD_DEBUG
	//    ComPtr<ID3D12Debug> debug_interface;
	//    throw -(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
	//    debugInterface->EnableDebugLayer();
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
				SUCCEEDED(D3D12CreateDevice(adapter1.Get(),
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
		auto result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device1));
		throw_if_failed(result);

		// Enable debug messages in debug mode.
#if BUILD_DEBUG
		//    ComPtr<ID3D12InfoQueue> pInfoQueue;
		//    if (SUCCEEDED(device1.As(&pInfoQueue)))
		//    {
		//        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		//        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		//        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		//
		//        // Suppress whole categories of messages
		//        //D3D12_MESSAGE_CATEGORY Categories[] = {};
		//
		//        // Suppress messages based on their severity level
		//        D3D12_MESSAGE_SEVERITY Severities[] =
		//        {
		//            D3D12_MESSAGE_SEVERITY_INFO
		//        };
		//
		//        // Suppress individual messages by their ID
		//        D3D12_MESSAGE_ID DenyIds[] = {
		//            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
		//            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
		//            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
		//        };
		//
		//        D3D12_INFO_QUEUE_FILTER NewFilter = {};
		//        //NewFilter.DenyList.NumCategories = _countof(Categories);
		//        //NewFilter.DenyList.pCategoryList = Categories;
		//        NewFilter.DenyList.NumSeverities = _countof(Severities);
		//        NewFilter.DenyList.pSeverityList = Severities;
		//        NewFilter.DenyList.NumIDs = _countof(DenyIds);
		//        NewFilter.DenyList.pIDList = DenyIds;
		//
		//        ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
		//    }
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
}

bool Dx12Context::register_window(const core::window::Window& window) const {
	Dx12Context& self = const_cast < Dx12Context& > (*this); // TODO: Thread safety

	if (self.swapchain.is_set()) {
		return false;
	}

	const auto size = window.client_size();

	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.BufferCount = Dx12Swapchain::frame_count;
	desc.Width = size.width;
	desc.Height = size.height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> swapchain1;
	throw_if_failed(self.factory->CreateSwapChainForHwnd(
		self.queue.Get(),
		(HWND)window.handle(),
		&desc,
		nullptr,
		nullptr,
		&swapchain1
	));

	throw_if_failed(self.factory->MakeWindowAssociation(
		(HWND)window.handle(),
		DXGI_MWA_NO_ALT_ENTER
	));

	ComPtr<IDXGISwapChain3> swapchain; 
	throw_if_failed(swapchain1.As(&swapchain));
	const auto current = (u8)swapchain->GetCurrentBackBufferIndex();

	ComPtr<ID3D12Fence> fence;
	throw_if_failed(self.device->CreateFence(
		0, 
		D3D12_FENCE_FLAG_NONE, 
		IID_PPV_ARGS(&fence)
	));
	auto fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	VERIFY(fence_event);
	const int fence_value = 1;

	self.swapchain = Dx12Swapchain {
		swapchain,
		// backbuffers,
		current,

		fence,
		fence_event,
		fence_value
	};

	self.wait_for_previous();

	return true;
}

void Dx12Context::wait_for_previous() const {
	Dx12Context& self = const_cast < Dx12Context& > (*this); // TODO: Thread safety
	Dx12Swapchain& swapchain = self.swapchain.as_ref().unwrap();

	const auto fence_value = swapchain.fence_value;
	throw_if_failed(self.queue->Signal(swapchain.fence.Get(), fence_value));
	swapchain.fence_value += 1;

	// Wait until the previous frame is finished.
	if (swapchain.fence->GetCompletedValue() < fence_value)
	{
		throw_if_failed(swapchain.fence->SetEventOnCompletion(fence_value, swapchain.fence_event));
		WaitForSingleObject(swapchain.fence_event, INFINITE);
	}

	swapchain.current = swapchain.handle->GetCurrentBackBufferIndex();
}