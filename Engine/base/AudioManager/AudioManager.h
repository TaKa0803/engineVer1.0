#pragma once
#include<string>
#include<map>
#include<wrl.h>
#include<xaudio2.h>
#include<vector>


//音声データ
struct SoundData {
	//波形フォーマット
	WAVEFORMATEX wfex;
	//バッファの先頭アドレス
	BYTE* pBuffer;
	//バッファのサイズ
	unsigned int bufferSize;
};

//オーディオすべてを管理するマネージャ
class AudioManager {

public://シングルトンパターン
	static AudioManager* GetInstance();
private://シングルトンパターン
	AudioManager() = default;
	~AudioManager() = default;
	AudioManager(const AudioManager& o) = delete;
	const AudioManager& operator=(const AudioManager& o) = delete;

public: //**静的関数**//

	/// <summary>
	/// タグ名から音声データの配列番号取得
	/// </summary>
	/// <param name="tag">タグ名</param>
	/// <returns>配列番号</returns>
	static int LoadSoundNum(const std::string& tag);
	
	/// <summary>
	/// 音の再生
	/// </summary>
	/// <param name="dataNum">音データの配列番号</param>
	static void PlaySoundData(const int dataNum, float volume=1.0f, bool loop=false);

	/// <summary>
	/// 音の停止
	/// </summary>
	/// <param name="num">音データの配列番号</param>
	static void StopSound(const int num);

public: //**パブリック関数**//

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// Jsonに含まれる音声データをすべて読み込む
	/// </summary>
	void LoadAllSoundData();

	/// <summary>
	/// すべての音の停止
	/// </summary>
	void StopAllSounds();

private: //**プライベート関数**//

	/// <summary>
	/// 音の読み込み
	/// </summary>
	/// <param name="path">音データのあるパス</param>
	/// <returns>音データ</returns>
	SoundData LoadSoundData(const std::string& path);

	/// <summary>
	/// タグ名から音データの配列番号の取得
	/// </summary>
	/// <param name="tag">タグ名</param>
	/// <returns>音データの配列番号</returns>
	int LoadSoundNumFromTag(const std::string tag);

	/// <summary>
	/// 音の再生
	/// </summary>
	/// <param name="num">音データの配列番号</param>
	/// <param name="volume">ボリューム</param>
	/// <param name="loop">ループするか</param>
	void Play(int num, float volume, bool loop);

	/// <summary>
	/// 音の停止
	/// </summary>
	/// <param name="num">音の配列番号</param>
	void Stop(int num);

private: //**プライベート変数**//

	//音データへのパスをまとめたファイルへのパス
	const std::string fileName_ = "resources/jsonfile/resourcePath.json";

	//ファイルのグループ名
	const std::string groupName_ = "audios";

	//サウンドフォルダまでのパス
	const std::string foldaPath ="resources/sounds/";

	//再生に必要なもの
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2=nullptr;
	//マスターボイス
	IXAudio2MasteringVoice* masterVoice=nullptr;

	//音のデータの数
	int soundNum_ = 0;

	//音データの数
	const int maxAudioData_ = 256;

private: //**プライベートデータ格納変数**//

	//要素の番号とタグ名の塊
	std::map<std::string,int>tagDatas_;

	//タグ名とデータの塊
	std::map<int, SoundData>soundDatas_;

	//再生中のデータ保存エリア
	std::map<int, IXAudio2SourceVoice*>playAudioDatas_;

};