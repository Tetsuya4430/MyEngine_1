//�N���X�������w�b�_�t�@�C���̃C���N���[�h
#include "MyGame.h"

//Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	HRESULT result;

	FrameWork* game = new MyGame();

	game->Run();

	delete game;

	return 0;
}