#include "Input.h"
#include <wrl.h>

using namespace Microsoft::WRL;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


Input* Input::GetInstance()
{
	static Input instance;

	return &instance;
}

void Input::Initialize(WinApp* winApp)
{
	HRESULT result;

	this->winApp = winApp;

	//DirectInput�̃C���X�^���X����
	result = DirectInput8Create(
		winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr
	);

	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);


	//���̓f�[�^�`���̃Z�b�g
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);	//�W���`��

	//�r���I���䃌�x���̃��Z�b�g
	result = devkeyboard->SetCooperativeLevel(
		winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
}

void Input::Update()
{
	HRESULT result;

	//�O��̃L�[���͂�ۑ�
	memcpy(Oldkey, key, sizeof(key));

	//�L�[�{�[�h���̎擾�J�n
	result = devkeyboard->Acquire();

	//�S�ẴL�[�̓��͏�Ԃ��擾����
	result = devkeyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE KeyNumber)
{
	//�w��L�[�������Ă����true��Ԃ�
	if (key[KeyNumber])
	{
		return true;
	}
	//�����łȂ����false��Ԃ�
	return false;
}

bool Input::TriggerKey(BYTE KeyNumber)
{
	//�w��L�[�������Ă����true��Ԃ�
	if (key[KeyNumber])
	{
		if (Oldkey[KeyNumber] == false)
		{
			return true;
		}
	}
	return false;
}
