#pragma once
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Object3d.h"
#include "Sprite.h"
#include "DebugText.h"
#include "Audio.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"


/// <summary>
/// �t���[�����[�N
/// </summary>
class FrameWork
{
public:

	/// <summary>
	/// ���s
	/// </summary>
	void Run();

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// �I��
	/// </summary>
	virtual  void Finalize();

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw();

	virtual bool IsEndRequest() { return endRequest_; }

protected:
	bool endRequest_ = false;

	WinApp* winApp = nullptr;
	Input* input = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Audio* audio = nullptr;
	DebugText* debugText = nullptr;
	SpriteCommon* spriteCommon = nullptr;
	AbstractSceneFactory* sceneFactory_ = nullptr;
};

