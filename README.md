[![DebugBuild](https://github.com/TaKa0803/engineVer1.0/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/TaKa0803/engineVer1.0/actions/workflows/DebugBuild.yml)
[![ReleaseBuild](https://github.com/TaKa0803/engineVer1.0/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/TaKa0803/engineVer1.0/actions/workflows/ReleaseBuild.yml)
## このエンジンについて

DirectX12での自作エンジンです

## 各フォルダについて

・Engine
>エンジン層のコードが入っています

・Application
>アプリ層のコードが入っています

・resources
>モデル、テクスチャ、シェーダー、音、jsonなどが入っています

・externals
>外部ライブラリが入っています

## ゲーム部分のコードについて

アプリケーションでのすべてのシーンは[Application/SceneManager](Application/SceneManager/)でシーン管理されています。

ゲーム部分の各シーンは[Application/Game/Scene](Application/Game/Scene)にまとめられています
