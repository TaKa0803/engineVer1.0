## このエンジンについて

DirectX12での自作エンジンです

## 開発環境
・Intel(R)Core(TM)i7 13世代

・NVIDIA GeForceRTX 4060

## 各フォルダについて

・Engine
>エンジン層のコードが入っています

・Application
>アプリ層のコードが入っています

・resources
>モデル、テクスチャ、シェーダー、音、jsonなどが入っています

## ゲーム部分のコードについて

アプリケーションでのすべてのシーンは[Application/SceneManager](Application/SceneManager/)でシーン管理されています。

ゲーム部分の各シーンは[Application/AL/Scene](Application/AL/Scene)にまとめられています
