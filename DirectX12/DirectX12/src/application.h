#pragma once

// アプリケーションクラス
class CApplication
{
public:
	CApplication();
	~CApplication();

public:
	// 実行
	virtual b8 Run();

private:
	/// 初期化
	virtual b8 Initialize();

	/// ループ
	virtual void Loop();

	/// 終了処理
	virtual void Finalize();

private:
	b8 InitializeApp();
	void FinalizeApp();

private:
	void Update();
	void Draw();
};