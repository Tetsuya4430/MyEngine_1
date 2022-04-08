#pragma once
#include "Sprite.h"

#include <windows.h>
#include <string>

//�f�o�b�O������N���X�̒�`
class DebugText
{
public:
	//�����ɒ萔�錾���L�q
	static const int maxCharCount = 256;	//1�t���[���ł̍ő�\��������
	static const int fontWidth = 25;		//�t�H���g�摜��1�������̉���
	static const int fontHeight = 25;		//�t�H���g�摜��1�������̏c��
	static const int fontLineCount = 10;	//�t�H���g�摜��1�����̕�����

//�����Ƀ����o�֐��̐錾���L�q
	static DebugText* GetInstance();

	void Initialize(SpriteCommon* spriteCommon, UINT texnumber);
	void Print(const std::string& text, float x, float y, float scale = 1.0f);
	void DrawAll();

	void Finalize();
private:
	//�����Ƀ����o�ϐ��̐錾���L�q

		//�X�v���C�g���ʕ���(�؂�Ă���)
	SpriteCommon* spriteCommon_ = nullptr;
	//�X�v���C�g�̔z��
	Sprite* sprites_[maxCharCount];
	//�X�v���C�g�z��̓Y�����ԍ�
	int spriteIndex_ = 0;
};