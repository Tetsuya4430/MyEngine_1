#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>	//ComPtr���g���̂ɕK�v

class XAudio2VoiceCallback : public IXAudio2VoiceCallback
{
public:
	// �{�C�X�����p�X�̊J�n��
	//STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired) {};
	void OnVoiceProcessingPassStart(UINT32 BytesRequired) {};
	// �{�C�X�����p�X�̏I����
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
	// �o�b�t�@�X�g���[���̍Đ����I��������
	STDMETHOD_(void, OnStreamEnd) (THIS) {};
	// �o�b�t�@�̎g�p�J�n��
	STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
	// �o�b�t�@�̖����ɒB������
	STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext) {
		// �o�b�t�@���������
		delete[] pBufferContext;
	};
	// �Đ������[�v�ʒu�ɒB������
	STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
	// �{�C�X�̎��s�G���[��
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
};

class Audio
{
private:
	//Microsoft::WRL���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:	//�T�u�N���X
	//�`�����N�w�b�_
	struct ChunkHeader
	{
		char id[4];		//�`�����N���Ƃ�ID
		int size;	//�`�����N�T�C�Y
	};

	//RIFT�w�b�_�`�����N
	struct RiffHeader
	{
		ChunkHeader chunk;	//"RIFF"
		char type[4];		//"WAVE"
	};

	//MFT�`�����N
	struct FormatChunk
	{
		ChunkHeader chunk;	//"fmt"
		WAVEFORMAT fmt;	//�g�`�t�H�[�}�b�g
	};

public:	//�����o�֐�

	static Audio* GetInstance();

	//������
	bool Initialize();

	//�T�E���h�t�@�C���̓ǂݍ��݂ƍĐ�(.wav�̂�)
	void PlayMusic(const char* filename);

	//���
	void Finalize();

private:	//�����o�ϐ�
	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	XAudio2VoiceCallback voiceCallback;
};