#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"

/// <summary>
/// �^�C�g���V�[��
/// </summary>
class TitleScene : public BaseScene
{
public:
	//������
	void Initialize() override;

	//�I������
	void Finalize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;


private:
	Sprite* sprite = nullptr;
};

