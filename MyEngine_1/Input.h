#pragma once
#include <windows.h>
#include <wrl.h>
#include "WinApp.h"

#define SIRECTINPUT_VERSION 0x0000	//DirectIput�̃o�[�W�����w��
#include <dinput.h>

//����
class Input
{
public:
	//namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:	//�����o�֐�

	static Input* GetInstance();
	//������
	void Initialize(WinApp* winApp);
	//�X�V
	void Update();
	//�L�[�̉������`�F�b�N
	bool PushKey(BYTE KeyNumber);
	//�L�[�̃g���K�[���`�F�b�N
	bool TriggerKey(BYTE KeyNumber);

private:	//�����o�ϐ�
	//�L�[�{�[�h�f�o�C�X
	ComPtr<IDirectInputDevice8> devkeyboard;
	//DirectInput�̃C���X�^���X
	ComPtr<IDirectInput8> dinput;
	//�S�L�[�̏��
	BYTE key[256] = {};
	//�O��̑S�L�[�̏��
	BYTE Oldkey[256] = {};
	//WindowsAPI
	WinApp* winApp = nullptr;
};