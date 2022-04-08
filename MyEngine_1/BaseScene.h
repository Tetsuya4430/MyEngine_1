#pragma once

/// <summary>
/// �O���錾
/// </summary>
class SceneManager;

/// <summary>
/// �V�[�����
/// </summary>
class BaseScene
{
public:
	virtual ~BaseScene() = default;

	//������
	virtual void Initialize() = 0;

	//�I������
	virtual void Finalize() = 0;

	//�X�V
	virtual void Update() = 0;

	//�`��
	virtual void Draw() = 0;

};

