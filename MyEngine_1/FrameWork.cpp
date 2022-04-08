#include "FrameWork.h"

void FrameWork::Run()
{
	//�Q�[���̏�����
	Initialize();

	while (true)	//�Q�[�����[�v
	{
		//���t���[���X�V
		Update();

		//�I�����N�G�X�g�������甲����
		if (IsEndRequest())
		{
			break;
		}
		//�`��
		Draw();
	}
	//�Q�[���̏I��
	Finalize();
}

void FrameWork::Initialize()
{
	//WindowsAPI�̏�����
	winApp = new WinApp();
	winApp->Initialize();

	//DirectX�̏�����
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//�X�v���C�g���ʕ����̏�����
	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Initialize(dxCommon->GetDev(), dxCommon->GetCmdList(), winApp->window_width, winApp->window_height);

	//�f�o�b�O�e�L�X�g
	debugText = DebugText::GetInstance();

	//�f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	const int debugTextTexNumber = 0;
	//�f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	spriteCommon->SpriteCommonLoadTexture(debugTextTexNumber, L"Resources/Font/ASCII_Font.png");
	//�f�o�b�O�e�L�X�g������
	debugText->Initialize(spriteCommon, debugTextTexNumber);

	//���͂̏�����
	input = Input::GetInstance();
	input->Initialize(winApp);

	//�I�[�f�B�I�̏�����
	audio = Audio::GetInstance();
	audio->Initialize();

	//3D�I�u�W�F�N�g�̐ÓI������
	Object3d::StaticInitialize(dxCommon->GetDev(), dxCommon->GetCmdList(), WinApp::window_width, WinApp::window_height);

}

void FrameWork::Finalize()
{
	//�V�[���t�@�N�g�����
	delete sceneFactory_;

	//�f�o�b�O�e�L�X�g���
	//debugText->Finalize();

	//�I�[�f�B�I���
	audio->Finalize();


	//DirectX���
	delete dxCommon;

	//Windows��API�̏I������
	winApp->Finalie();

	//WindowsAPI�̉��
	delete winApp;
}

void FrameWork::Update()
{
	//Windows�̃��b�Z�[�W����
	if (winApp->ProcessMessage())
	{
		//�Q�[�����[�v�𔲂���
		endRequest_ = true;
		return;
	}

	//���͂̍X�V
	input->Update();

	//�V�[���̍X�V
	SceneManager::GetInstance()->Update();
}

void FrameWork::Draw()
{
	//�`��O����
	dxCommon->PreDraw();

	SceneManager::GetInstance()->Draw();

	//�f�o�b�O�e�L�X�g�`��
	debugText->DrawAll();


	//4,�`��R�}���h�����܂�

	dxCommon->PostDraw();
}
