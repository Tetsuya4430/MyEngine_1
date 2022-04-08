#include "TitleScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

void TitleScene::Initialize()
{
	////�X�v���C�g���ʃe�N�X�`���ǂݍ���
	SpriteCommon::GetInstance()->SpriteCommonLoadTexture(1, L"Resources/Image/Title.png");

	//	�X�v���C�g�̐���
	sprite = Sprite::Create(1, { 0, 0 }, false, false);
}

void TitleScene::Finalize()
{
	//�X�v���C�g���
	delete sprite;

	
}

void TitleScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	{
		//�V�[���؂�ւ�
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
	
	//�X�v���C�g�̍X�V
	sprite->Update();


	//Esc�L�[�ŃE�B���h�E�����
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE))	//ESC�L�[�ŃE�B���h�E�����
	{
		//endRequest_ = true;
		return;
	}
}

void TitleScene::Draw()
{
	////�X�v���C�g�̋��ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();

	////�X�v���C�g�`��
	sprite->Draw();

}
