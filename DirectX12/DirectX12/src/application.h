#pragma once

// �A�v���P�[�V�����N���X
class CApplication
{
public:
	CApplication();
	~CApplication();

public:
	// ���s
	virtual b8 Run();

private:
	/// ������
	virtual b8 Initialize();

	/// ���[�v
	virtual void Loop();

	/// �I������
	virtual void Finalize();

private:
	b8 InitializeApp();
	void FinalizeApp();

private:
	void Update();
	void Draw();
};