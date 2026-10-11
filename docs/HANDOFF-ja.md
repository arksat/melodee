# MBP14への引き継ぎ

作業ブランチ: `stage-preset-browser`。mainは変更していない。

## 取得とWeb Player

初回はターミナルで実行する（既存の同名フォルダがあれば重ねてcloneしない）。

```sh
git clone --branch stage-preset-browser https://github.com/arksat/melodee.git melodee
cd melodee
python3 -m http.server 8000 --bind 127.0.0.1 --directory web/player
```

http://localhost:8000/ を開き「Switch on」。終了はCtrl+C。
Python 3が必要。完成版は `web/player/`、ソースは `web/emu/`。
完成版の起動にDockerやビルドは不要。公開サイトへのデプロイはしていない。
ローカル起動後の試奏はネット接続不要。HTMLの直接ダブルクリックでは起動しない。
ユーザー音色・パターンはRAM内のみで、再読み込みすると消える。
現在のWeb Player内の演奏データを別PCへ転送する機能はない。

「実機へ書き込み」から同梱のカスタムファームウェアを転送できる。
ChromeまたはEdgeで開き、FM-1をUSB接続し、MIDI/SysExを許可する。
Web Playerの音色やパターンは転送しない。必要な実機データは先にバックアップする。
書き込み中はUSBを抜かず、ブラウザを閉じない。実機への転送確認は未実施。

## macOSのビルド環境

Docker Desktopをインストール・起動し、`docker info` のServer表示を確認する。
Homebrew未導入なら https://brew.sh/ の手順で導入する。
リポジトリ直下で次を実行する。

```sh
brew install python fribidi node
python3 -m venv work/build-venv
work/build-venv/bin/python -m pip install Pillow fonttools mido python-rtmidi
sh tools/get_toolchain.sh "$PWD/work/jieli"
git clone --depth 1 --branch AC79NN_SDK_V1.2.1_2023-12-13 https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git work/fw-AC79_AIoT_SDK
PYTHON="$PWD/work/build-venv/bin/python" JIELI_TOOLCHAIN="$PWD/work/jieli/toolchain" AC79_SDK="$PWD/work/fw-AC79_AIoT_SDK" ./build.sh --release 1.1.0
sh web/emu/build.sh
node web/emu/smoke_test.mjs
python3 -m http.server 8000 --bind 127.0.0.1 --directory build/emu
```

依存ファイルと初回Dockerイメージ取得にはネット接続が必要。
`work/`（SDK・ツールチェーン・venv）と `build/` はGit管理外。
詳細は `BUILDING.md`。`web/player/` は引き継ぎ時点のスナップショットで、
再ビルド先 `build/emu/` とは自動同期しない。

## 変更と検証範囲

- StageでPRESETS回転によりブラウザを開く。最初の回転では音色を変えない。
- ブラウザ内のALGOはカテゴリ／LIST選択、PRESETSは音色選択。ブラウザ外のALGOは変更なし。
- カテゴリ欄を拡幅し、カテゴリと非選択音色の文字を16pxへ拡大。
- Stage音色名を13px・最大2行へ変更。
- 初期BPM 100、再生クオンタイズ1/16。保存済み設定は優先される。
- Web PlayerはContrast配色で起動。
- SEQ下部にノート削除／クリアメニューのヘルプ。
- PATTERN > LENGTHでHOME + KNOB 1により16/32/48/64を選択。
- 追加フォントB/Tを圧縮・1位相・ペアカーニングなしにして容量削減。
- Web Playerと実機転送ページ追加。Felucca由来の著作権・ライセンスを保持。

関連するStage／ブラウザ操作テスト、パターン長操作テスト、Wasmスモークテスト、
ハードウェア向けビルドは成功。完全回帰は今回省略。実機動作と転送は未検証。

同梱ファームウェアはカスタム版だが表記は1.1.0、identityはFM-1_91010。
`web/player/installer/firmware.fwsc` SHA-256:
`b7bae04b440d1d27b27cd1004829c93a824aefd681d04bef6800501d26d0604d`

Codexでリポジトリフォルダを開き、まずこの文書を読むよう依頼する。
cloneだけでは会話履歴は移らない。
