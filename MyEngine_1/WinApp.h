#pragma once
#include <windows.h>

#define WINDOW_H_
#define WINDOW_CLASS_NAME "DirectXGame"	//!< �E�B���h�E�N���X�� Window class name


//WindowAPI
class WinApp
{

public:
	static const wchar_t windowClassName[];

public:	//�ÓI�����o�֐�
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//�E�B���h�E�n���h��
	HWND hwnd = nullptr;

	//�E�B���h�E�N���X�̐ݒ�
	WNDCLASSEX w{};

public:	//�����o�֐�
	//WindowsAPI�֘A�̏�����
	void Initialize();
	//getter
	HWND GetHwnd() { return hwnd; }
	HINSTANCE GetHInstance() { return w.hInstance; }
	//���b�Z�[�W�̏���
	bool ProcessMessage();
	//�I��
	void Finalie();
	

public:	//�萔
	//�E�B���h�E����
	static const int window_width = 1280;
	//�E�B���h�E�c��
	static const int window_height = 720;
};