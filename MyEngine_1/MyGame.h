#include "FrameWork.h"

#include <vector>

/// <summary>
/// �Q�[���ŗL�̃N���X
/// </summary>
class MyGame : public FrameWork
{
public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// �I��
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

private:
	
};
