#include "SpriteCommon.h"

#include <cassert>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <string>
#include <d3dx12.h>
#include <DirectXTex.h>

using namespace Microsoft::WRL;
using namespace DirectX;

SpriteCommon* SpriteCommon::GetInstance()
{
	static SpriteCommon instance;

	return &instance;
}

void SpriteCommon::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int window_width, int window_height)
{
	//null�|�C���^�`�F�b�N
	assert(device);

	HRESULT result = S_FALSE;

	device_ = device;
	commandList_ = commandList;

	//�X�v���C�g�p�p�C�v���C������
	CreateGraphicsPipeline();

	//���s���e�̎ˉe�s�񐶐�
	matProjection_ = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)window_width, (float)window_height, 0.0f, 0.0f, 1.0f
	);

	//�f�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = spriteSRVCount;
	result = device_->CreateDescriptorHeap(&descHeapDesc,
		IID_PPV_ARGS(&descHeap_));

}

void SpriteCommon::PreDraw()
{
	//�p�C�v���C���X�e�[�g�̐ݒ�
	commandList_->SetPipelineState(pipelineSet_.pipelinestate.Get());

	//���[�g�V�O�l�`���̐ݒ�
	commandList_->SetGraphicsRootSignature(pipelineSet_.rootsignature.Get());

	//�v���~�e�B�u�`���ݒ�
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�e�N�X�`���p�f�X�N���v�^�q�[�v�̐ݒ�
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap_.Get() };
	commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

}

void SpriteCommon::SpriteCommonLoadTexture(UINT texnumber, const wchar_t* filename)
{
	//�ُ�Ȕԍ��̎w������o
	assert(texnumber <= spriteSRVCount - 1);

	HRESULT result;

	//WIC�e�N�X�`���̃��[�h
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		filename,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	const Image* img = scratchImg.GetImage(0, 0, 0);	//���f�[�^���o

	//���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	//�e�N�X�`���o�b�t�@�̐���
	result = device_->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	//�e�N�X�`���p�w��
		nullptr,
		IID_PPV_ARGS(&texBuff_[texnumber])
	);

	//�e�N�X�`���o�b�t�@�ɓ]��
	result = texBuff_[texnumber]->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,		//1���C���T�C�Y
		(UINT)img->slicePitch	//�S�T�C�Y
	);

	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//�ݒ�\����
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;

	//�q�[�v��texnumber�ɃV�F�[�_�[���\�[�X�r���[�쐬
	device_->CreateShaderResourceView(
		texBuff_[texnumber].Get(),	//�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc,		//�e�N�X�`���ݒ���
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap_->GetCPUDescriptorHandleForHeapStart(), texnumber,
			device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

}

void SpriteCommon::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, UINT texnumber)
{
	commandList_->SetGraphicsRootDescriptorTable(rootParameterIndex,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap_->GetGPUDescriptorHandleForHeapStart(),
			texnumber,
			device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
}

ID3D12Resource* SpriteCommon::GetTexBuff(int texNumber)
{
	assert(0 <= texNumber && texNumber < spriteSRVCount);


	return texBuff_[texNumber].Get();
}

void SpriteCommon::CreateGraphicsPipeline()
{
	HRESULT result;


	ComPtr<ID3DBlob> vsBlob;	//���_�V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;	//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob;	//�G���[�I�u�W�F�N�g

	//---���_�V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��---//
	result = D3DCompileFromFile(
		L"Resources/shaders/SpriteVS.hlsl",	//�V�F�[�_�[�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//�C���N���[�h�\�ɂ���
		"main", "vs_5_0",	//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob
	);

	if (FAILED(result))
	{
		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
		errorBlob->GetBufferSize(),
		errstr.begin());
		errstr += "/n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
	}

	//---�s�N�Z���V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��---//
	result = D3DCompileFromFile(
		L"Resources/shaders/SpritePS.hlsl",		//�V�F�[�_�[�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,		//�C���N���[�h�\�ɂ���
		"main", "ps_5_0",						//�G���g���[�|�C���g���A�V�F�[�_�[���f���ݒ�
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�O�ݒ�
		0,
		&psBlob, &errorBlob
	);

	if (FAILED(result))
	{
		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "/n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
	}

	//---���_���C�A�E�g�z��̐錾�Ɗe�퍀�ڂ̐ݒ�---//
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{//xyz���W
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

		{//uv���W
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};



	//���_�V�F�[�_�[�A�s�N�Z���V�F�[�_�[���p�C�v���C���ɐݒ�
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());


	//�T���v���}�X�N�̐ݒ�
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//�W���ݒ�

	//���X�^���C�U�X�e�[�g�̕W���I�Ȑݒ�(�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;		//�w�ʃJ�����O�����Ȃ�

	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�(�[�x�e�X�g���s���A�������݋��A�[�x����������΍��i)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	//��U�W���l�����Z�b�g
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;		//��ɏ㏑�����[��
	gpipeline.DepthStencilState.DepthEnable = false;	//�[�x�e�X�g�����Ȃ�
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g

	//���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//�}�`�̌`����O�p�`�ɐݒ�
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//���̑��̐ݒ�
	gpipeline.NumRenderTargets = 1;	//�`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	//0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1;	//1�s�N�Z���ɂ�1��T���v�����O

	//�e�N�X�`���[�T���v���[�̐ݒ�
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);


	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	//descRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	//b0���W�X�^
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	//t0���W�X�^

	CD3DX12_ROOT_PARAMETER rootparams[2];
	//�萔�p
	rootparams[0].InitAsConstantBufferView(0);	//�萔�o�b�t�@�r���[�Ƃ��ď�����(b0���W�X�^)

	//�e�N�X�`���p
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV);


	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//�����_�[�^�[�Q�b�g�u�����h�̐ݒ�(�A���t�@�l���ׂ����ݒ肷�邽�߂̏���)
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//�W���ݒ�
	blenddesc.BlendEnable = false;					//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//�f�X�g�̒l��0%�g��

	//���Z����
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;	//���Z
	blenddesc.SrcBlend = D3D12_BLEND_ONE;	//�\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_ONE;	//�f�X�Ƃ̒l��0%�g��




	ComPtr<ID3DBlob> rootSigBlob;
	//�o�[�W������������ł̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob,
		&errorBlob);

	//���[�g�V�O�l�`���̐���
	result = device_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&pipelineSet_.rootsignature));

	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = pipelineSet_.rootsignature.Get();

	result = device_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet_.pipelinestate));


}
