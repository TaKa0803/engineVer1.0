#include"AudioManager/AudioManager.h"

#include<vector>

#pragma comment(lib, "xaudio2.lib")

//mediaFoundation
#include<mfapi.h>
#include<mfidl.h>
#include<mfreadwrite.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#include<fstream>
#include<json.hpp>
#include<cassert>

AudioManager* AudioManager::GetInstance()
{
	static AudioManager instance;
	return &instance;
}


void AudioManager::Initialize()
{
	// オーディオの初期設定
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));


	hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xAudio2->CreateMasteringVoice(&masterVoice);

	//MediaFoundationの初期化
	hr = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(hr));

}


void AudioManager::Finalize()
{
	//インスタンス開放
	xAudio2.Reset();

	//音声データ解放
	for (std::pair<int,SoundData> data : soundDatas_) {
		delete [] data.second.pBuffer;
		data.second.bufferSize = 0;
		data.second.wfex = {};
	}

	MFShutdown();
	CoUninitialize();
}



int AudioManager::LoadSoundNum(const std::string& tag)
{
	return AudioManager::GetInstance()->LoadSoundNumFromTag(tag);
}

void AudioManager::StopSound(const int num)
{
	AudioManager::GetInstance()->Stop(num);
}




void AudioManager::LoadAllSoundData()
{
	//読み込むjsonファイル
	std::string filepath = fileName_;
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filepath);

	//ファイルオープン失敗処理
	if (ifs.fail()) {
		std::string message = "soundData jsonfile is not find！";
		MessageBoxA(nullptr, message.c_str(), "SoundManager", 0);
		assert(message.c_str());
		return;
	}

	nlohmann::json root;
	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//移したのでファイルを閉じる
	ifs.close();

	//グループ検索
	nlohmann::json::iterator itGroup = root.find(groupName_);

	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {		
			//タグの名前を取得
			const std::string& itemName = itItem.key();
			
			//文字列かチェック
			if (itItem->is_array()) {
				//音のwavデータまでのパス
				std::string path = (std::string)itItem->at(0);

				std::string fullpath = foldaPath + path;

				//モデルデータを作成して設定
				
				//タグに対応する要素番号取得
				tagDatas_[itemName] = soundNum_;
				soundDatas_[soundNum_] = (LoadSoundData(fullpath));

				//カウンター増加
				soundNum_++;
			}
	}
}

void AudioManager::StopAllSounds()
{
	for (auto& playS : playAudioDatas_) {
		playS.second->Stop();
		playS.second->FlushSourceBuffers();
		playS.second->DestroyVoice();

		playS.second=nullptr;

		
	}

	playAudioDatas_.clear();
}


	SoundData AudioManager::LoadSoundData(const std::string& path)
 {
	 IMFSourceReader* pMFSourceReader{ nullptr };
	 std::wstring wstr(path.begin(), path.end());

	 //読み込み処理
	 HRESULT hr= MFCreateSourceReaderFromURL(wstr.c_str(), NULL, &pMFSourceReader);
	 assert(SUCCEEDED(hr));

	 IMFMediaType* pMFMediaType{ nullptr };
	 MFCreateMediaType(&pMFMediaType);
	 pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	 pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	 pMFSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType);

	 pMFMediaType->Release();
	 pMFMediaType = nullptr;
	 pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType);

	 WAVEFORMATEX *waveFormat{nullptr};
	 MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);

	 //読み込みと格納
	 std::vector<BYTE> mediaData;
	 while (true)
	 {
		 IMFSample* pMFSample{ nullptr };
		 DWORD dwStreamFlags{ 0 };
		 pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		 if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
		 {
			 break;
		 }

		 IMFMediaBuffer* pMFMediaBuffer{ nullptr };
		 pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		 BYTE* pBuffer{ nullptr };
		 DWORD cbCurrentLength{ 0 };
		 pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		 mediaData.resize(mediaData.size() + cbCurrentLength);
		 memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);





		 pMFMediaBuffer->Unlock();

		 pMFMediaBuffer->Release();
		 pMFSample->Release();
	 }

#pragma region 読み込んだデータを返す
	 SoundData soundData = {};
	 soundData.wfex = *waveFormat;
	 soundData.pBuffer = new BYTE[mediaData.size()];
	 std::memcpy(soundData.pBuffer, mediaData.data(),sizeof(BYTE)*mediaData.size());
	 soundData.bufferSize = sizeof(BYTE) *static_cast<UINT32>(mediaData.size());
#pragma endregion

	 CoTaskMemFree(waveFormat);
	 pMFMediaType->Release();
	 pMFSourceReader->Release();


	 return soundData;
 }

 int AudioManager::LoadSoundNumFromTag(const std::string tag)
 {
	 return tagDatas_[tag];
 }

 void AudioManager::Play(int num, float volume, bool loop)
 {
	 //要素番号のタグからデータ取得
	 SoundData data = soundDatas_[num];

	 IXAudio2SourceVoice* pSourceVoice = nullptr;
	 HRESULT hr = xAudio2->CreateSourceVoice(&pSourceVoice, &data.wfex);
	 //作成失敗
	 assert(SUCCEEDED(hr));

	 pSourceVoice->SetVolume(volume);
	 

	 XAUDIO2_BUFFER buf{};
	 buf.pAudioData = data.pBuffer;
	 buf.AudioBytes = data.bufferSize;
	 buf.Flags = XAUDIO2_END_OF_STREAM;
	 
	 if (loop) {
		 buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	 }
	 hr = pSourceVoice->SubmitSourceBuffer(&buf);
	 hr = pSourceVoice->Start();

	 assert(SUCCEEDED(hr));

	 playAudioDatas_[num] = pSourceVoice;


	 //

 }

 void AudioManager::Stop(int num)
 {
	 playAudioDatas_[num]->Stop();
	 playAudioDatas_[num]->FlushSourceBuffers();
	 playAudioDatas_[num]->DestroyVoice();

	 playAudioDatas_[num] = nullptr;

	 //キーとデータ削除
	 playAudioDatas_.erase(num);
 }



void AudioManager::PlaySoundData(const int dataNum,float volume, bool loop)
{

	AudioManager* AM = AudioManager::GetInstance();

	AM->Play(dataNum,volume,loop);

}


