#include "DirectXCommon.h"
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

void DirectXCommon::Initialize(WinApp* winApp)
{
	//NULL���o	
	assert(winApp);

	//�����o�ϐ��ɋL�^
	this->winApp = winApp;

	HRESULT result;

#ifdef _DEBUG
	//�f�o�b�O���C���[���I����
	ID3D12Debug* debugController;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif


	//DXGI�t�@�N�g�� (�f�o�C�X������͉������Ă悢)
	ComPtr<IDXGIFactory6> dxgiFactory;

	//DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));


	//�A�_�v�^�[�̗񋓗p
	std::vector<ComPtr<IDXGIAdapter1>> adapters;

	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr<IDXGIAdapter1> tmpAdapter;

	for (int i = 0; dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);	//���I�z��ɒǉ�����
	}

	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc);	//�A�_�v�^�[�̏����擾

		//�\�t�g�E�F�A�f�o�C�X�����
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		std::wstring strDesc = adesc.Description;	//�A�_�v�^�[��

		//Intel UHD Graphics (�I���{�[�h�O���t�B�b�N)�����
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i].Get();	//�̗p
			break;
		}
	}

	//�Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (int i = 0; i < _countof(levels); i++)
	{
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));

		if (result == S_OK)
		{
			//�f�o�C�X�𐶐��o�������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}
#pragma endregion

	//---�R�}���h���X�g---//(GPU�ɂ܂Ƃ߂Ė��߂𑗂�)
#pragma region �R�}���h���X�g
//�R�}���h�A���P�[�^�[�𐶐�
	result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator)
	);

	//�R�}���h���X�g�𐶐�
	result = dev->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&cmdList)
	);

	//---�R�}���h�L���[---//(�R�}���h���X�g��GPU�ɏ��Ɏ��s�����Ă����ׂ̎d�g��)
	//�W���ݒ�ŃR�}���h�L���[�𐶐�
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};	//�\���̂̒��g��0�ŃN���A���Ă���

	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

#pragma endregion

	//---�X���b�v�`�F�[��---//(�_�u���o�b�t�@�����O�����邽�߂ɕK�v�Ȏd�g��)
#pragma region �X���b�v�`�F�[��
//�e��ݒ�����ăX���b�v�`�F�[���𐶐�
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//�F���̏���
	swapchainDesc.SampleDesc.Count = 1;	//�}���`�T���v�����Ȃ�
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;	//�o�b�N�o�b�t�@�p
	swapchainDesc.BufferCount = 2;	//�o�b�t�@����2�Ɏw��
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//�t���b�v��͔j��
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//IDXGISwapChain1��ComPtr��p��
	ComPtr<IDXGISwapChain1> swapchain1;

	dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		winApp->GetHwnd(),
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1
	);

	//��������IDXGISwapChain1�̃I�u�W�F�N�g��IDXGISwapChain4�ɕϊ�����
	swapchain1.As(&swapchain);

#pragma endregion

	//---�����_�[�^�[�Q�b�g�r���[---//(�o�b�N�o�b�t�@��`��L�����o�X�Ƃ��Ĉ������߂̃I�u�W�F�N�g)
#pragma region �����_�[�^�[�Q�b�g�r���[
//�e��ݒ�����ăf�X�N���v�^�q�[�v�𐶐�
	//D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	//�����_�[�^�[�Q�b�g�r���[
	heapDesc.NumDescriptors = 2;	//�\����2��


	//�\����2���̃|�C���^��p��
	backBuffers.resize(2);

	for (int i = 0; i < (int)heapDesc.NumDescriptors; i++)
	{
		dev->CreateDescriptorHeap(
			&heapDesc,
			IID_PPV_ARGS(&rtvHeaps)
		);
	}



	for (int i = 0; i < 2; i++)
	{
		//�X���b�v�`�F�[������o�b�t�@���擾
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),	//�q�[�v�擪�̃A�h���X
				i,	//�f�B�X�N���v�^�ԍ�
				dev->GetDescriptorHandleIncrementSize(heapDesc.Type)	//�f�B�X�N���v�^1���̃T�C�Y
			);

		//�����_�[�^�[�Q�b�g�r���[�̐���
		dev->CreateRenderTargetView(
			backBuffers[i].Get(),
			nullptr,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),	//�q�[�v�擪�̃A�h���X
				i,	//�f�B�X�N���v�^�ԍ�
				dev->GetDescriptorHandleIncrementSize(heapDesc.Type)	//�f�B�X�N���v�^1���̃T�C�Y
			)
		);
	}
#pragma endregion

	//---�[�x�e�X�g---//(���ɕ`��L�����o�X�ɏ�����Ă�����̂���O�̎������`���悤�ɂ��邱�ƂőO��֌W�������)
	//���`�揉���������ɕ`���̂��D�܂������u�����_�[�^�[�Q�b�g�v�u�X���b�v�`�F�[���v�Ɋ֌W����̂ŋ߂��ɏ����Ă���
#pragma region �[�x�e�X�g�֌W

//�[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;	//�[�x�r���[��1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//�f�v�X�X�e���V���r���[
	//ComPtr<ID3D12DescriptorHeap> dsvHeap;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//ComPtr<ID3D12Resource> depthBuffer;

	//�[�x�o�b�t�@���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WinApp::window_width,
		WinApp::window_height,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	//�[�x�o�b�t�@�̐���
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//�[�x�l�������݂Ɏg�p
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer)
	);

	//�[�x�r���[�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

#pragma endregion


	//---�t�F���X---//(CPU��GPU�œ�������邽�߂̎d�g��)
#pragma region �t�F���X
//�t�F���X�̐���
	//ComPtr<ID3D12Fence> fence;
	/*UINT64 fenceVal = 0;*/

	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

}

void DirectXCommon::PreDraw()
{
	//�o�b�N�o�b�t�@�̔ԍ����擾
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();


	//1,���\�[�X�o���A�ŏ������݉\�ɕύX
		//�\����Ԃ���`���ԂɕύX
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

#pragma endregion

	//2,�`���w��(�o�b�N�o�b�t�@��`���Ƃ��Ďw��)
#pragma region �`���w��R�}���h
//�����_�[�^�[�Q�b�g�r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);

	//�[�x�X�e���V���r���[�p�̃f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
#pragma endregion

	//3,��ʃN���A
	float clearColor[] = { 0.0f, 0.25f, 0.25f, 0.0f };	//��ʂ̏����J���[
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


//	//---�r���[�|�[�g�ݒ�---//
//	//�r���[�|�[�g�̐ݒ�R�}���h
//	D3D12_VIEWPORT viewport{};
//
//	viewport.Width = WinApp::window_width;
//	viewport.Height = WinApp::window_height;
//	viewport.TopLeftX = 0;
//	viewport.TopLeftY = 0;
//	viewport.MinDepth = 0.0f;
//	viewport.MaxDepth = 1.0f;
//
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::window_width, WinApp::window_height));
//
//	//---�V�U�[��`�ݒ�---//
//	D3D12_RECT scissorrect{};
//
//	scissorrect.left = 0;
//	scissorrect.right = scissorrect.left + WinApp::window_width;
//	scissorrect.top = 0;
//	scissorrect.bottom = scissorrect.top + WinApp::window_height;
//
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, WinApp::window_width, WinApp::window_height));

}

void DirectXCommon::PostDraw()
{
	

	//�o�b�N�o�b�t�@�̔ԍ����擾
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	//5,���\�[�X�o���A��߂�(�o�b�N�o�b�t�@���������݉\��Ԃ̂܂܂��Ɖ�ʕ\���Ɏg���Ȃ��̂�{D3D12_RESOURCE_STATE_PRESENT}��ʕ\����ԂɕύX����)
#pragma region ���\�[�X�o���A�������݋֎~
//�\����Ԃ���`���ԂɕύX
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

#pragma endregion

	//---�R�}���h�̃t���b�V��---//(�����܂Œ��߂��R�}���h�����s���`�悷��)
#pragma region �R�}���h�̎��s
//���߂̃N���[�Y
	cmdList->Close();

	//�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };	//�R�}���h���X�g�̔z��
	cmdQueue->ExecuteCommandLists(1, cmdLists);

	//�o�b�t�@���t���b�v(���\�̓���ւ�)
	swapchain->Present(1, 0);

	//�R�}���h�L���[�̎��s������҂�
	cmdQueue->Signal(fence.Get(), ++fenceVal);

	if (fence->GetCompletedValue() != fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
#pragma endregion
	//��ʓ���ւ�(���̎d�l�ɔ����A�R�}���h�A���P�[�^�ƃR�}���h���X�g�����Z�b�g����)
#pragma region ��ʓ���ւ�
	cmdAllocator->Reset();	//�L���[���N���A
	cmdList->Reset(cmdAllocator.Get(), nullptr);	//�ĂуR�}���h���X�g�𒙂߂鏀��
#pragma endregion
}
