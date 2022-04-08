#pragma once

#include "Object3d.h"
#include <memory>

//�I�u�W�F�N�g�}�l�[�W���[
class ObjectManager
{
public:

	void Update();

	void Draw();

	/// <summary>
	/// �I�u�W�F�N�g�ǉ�
	/// </summary>
	/// <param name="object"></param>
	std::weak_ptr<Object3d> AddObject(std::shared_ptr<Object3d> object);

private:
	//�I�u�W�F�N�g���X�g
	std::vector<std::shared_ptr<Object3d>> objects_;
};

