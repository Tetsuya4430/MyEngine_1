#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "WinApp.h"

using namespace Microsoft::WRL;

//DirectX���
class DirectXCommon
{
public:	//�����o�֐�
	//������
	void Initialize(WinApp* winApp);
	//�`��O����
	void PreDraw();
	//�`��㏈��
	void PostDraw();
	//�f�o�C�X�擾getter
	ID3D12Device* GetDev() { return dev.Get(); }
	//�R�}���h���X�g�擾getter
	ID3D12GraphicsCommandList* GetCmdList() { return cmdList.Get(); }


private:	//�����o�ϐ�
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<IDXGISwapChain4> swapchain;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12Fence> fence;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};


	UINT64 fenceVal = 0;

	//WindowsAPI
	WinApp* winApp = nullptr;
	

private:
	//�f�o�C�X
	ComPtr<ID3D12Device> dev;
	//DXGI�t�@�N�g��
	ComPtr<IDXGIFactory6> dxgiFactory;
};