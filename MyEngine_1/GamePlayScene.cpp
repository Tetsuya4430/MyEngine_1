#include "GamePlayScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

void GamePlayScene::Initialize()
{
	////�X�v���C�g���ʃe�N�X�`���ǂݍ���
	SpriteCommon::GetInstance()->SpriteCommonLoadTexture(1, L"Resources/Image/GamePlay.png");

	//	�X�v���C�g�̐���
	sprite = Sprite::Create(1, { 0, 0 }, false, false);

	//OBJ���烂�f���f�[�^��ǂݍ���
	model_1 = Model::LoadFromObj("triangle_mat");
	model_2 = Model::LoadFromObj("triangle_mat");

	//�I�u�W�F�N�g�}�l�[�W���[����
	objectManager_1 = std::make_unique<ObjectManager>();
	objectManager_2 = std::make_unique<ObjectManager>();
	objectManager_3 = std::make_unique<ObjectManager>();

	//�I�u�W�F�N�g�}�l�[�W���[����->3D�I�u�W�F�N�g����->�I�u�W�F�N�g�̎Q�ƃJ�E���g�𑝉�
	std::shared_ptr<Object3d> object_1 = Object3d::Create(model_1);
	std::weak_ptr<Object3d> ObjectWp_1 = objectManager_1->AddObject(object_1);
	object3d_1 = ObjectWp_1.lock();

	std::shared_ptr<Object3d> object_2 = Object3d::Create(model_2);
	std::weak_ptr<Object3d> ObjectWp_2 = objectManager_2->AddObject(object_2);
	object3d_2 = ObjectWp_2.lock();

	std::shared_ptr<Object3d> object_3 = Object3d::Create(model_2);
	std::weak_ptr<Object3d> ObjectWp_3 = objectManager_3->AddObject(object_3);
	object3d_3 = ObjectWp_3.lock();

	//3D�I�u�W�F�N�g�̈ʒu�Ɗg�嗦���w��
	
		object3d_1->SetScale({ 20.0f, 20.0f, 20.0f });

		object3d_2->SetPosition({ -15, 0, -15 });
		object3d_2->SetScale({ 21.0f, 21.0f, 21.0f });
	
		object3d_3->SetPosition({ +15, 0, +15 });
		object3d_3->SetScale({ 21.0f, 21.0f, 21.0f });
	
	
	//�����ǂݍ��݂ƍĐ�
	Audio::GetInstance()->PlayMusic("Resources/Music/Alarm01.wav");

}

void GamePlayScene::Finalize()
{
	//�X�v���C�g���
	delete sprite;

	//���f���̉��
	delete model_1;
	delete model_2;
}

void GamePlayScene::Update()
{
	//---�f�o�b�O�e�L�X�g�֌W---//
	//X���W�AY���W���w�肵�ĕ\��
	DebugText::GetInstance()->Print("Debug Text", 0, 0);

	//X���W�AY���W�A�k�ڂ��w�肵�ĕ\��
	DebugText::GetInstance()->Print("Debug Text = 0", 0, 50, 2.0f);


	if (Input::GetInstance()->TriggerKey(DIK_RETURN))
	{
		//�V�[���؂�ւ�
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//���W����
	if (Input::GetInstance()->PushKey(DIK_UP) || Input::GetInstance()->PushKey(DIK_DOWN) || Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT))
	{
			DirectX::XMFLOAT3 pos = object3d_1->GetPosition();

			pos.x += 1.0f;

			object3d_1->SetPosition(pos);
	}

	//3D�I�u�W�F�N�g�̍X�V
	objectManager_1->Update();
	objectManager_2->Update();
	objectManager_3->Update();
	
	//�X�v���C�g�̍X�V
	sprite->Update();


	//Esc�L�[�ŃE�B���h�E�����
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE))	//ESC�L�[�ŃE�B���h�E�����
	{
		//endRequest_ = true;
		return;
	}
}

void GamePlayScene::Draw()
{
	////�X�v���C�g�̋��ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();

	////�X�v���C�g�`��
	sprite->Draw();

	//3D�I�u�W�F�N�g�̕`��O����
	Object3d::PreDraw();

	//3D�I�u�W�F�N�g�̕`��
	objectManager_1->Draw();
	objectManager_2->Draw();
	objectManager_3->Draw();

	//3D�I�u�W�F�N�g�̕`��㏈��
	Object3d::PostDraw();


	////�X�v���C�g�̋��ʃR�}���h
	SpriteCommon::GetInstance()->PreDraw();

	//////�X�v���C�g�`��
	//sprite->Draw();
}


