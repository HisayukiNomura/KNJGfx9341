@mainpage KNJGfx9341 グラフィックライブラリ

# KNJGfx9341 グラフィックライブラリ

---

## 概要

KNJGfx9341は、Raspberry Pi Picoや各種マイコン向けに設計された、TFT液晶ディスプレイ（ILI9341等）用の高機能グラフィックライブラリです。  
Adafruit_GFXライブラリをベースに、日本語（漢字・ひらがな・カタカナ）描画や多彩なカラーユーティリティ、拡張フォント、DMA転送などの機能を追加しています。  
ArduinoライクなAPIで、グラフィック描画やテキスト出力を簡単に実現できます。

---

## 主な特徴

- **ILI9341コントローラ対応**  
  SPI/パラレル接続のTFTディスプレイ（ILI9341等）を高速制御
- **Adafruit_GFX互換API**  
  標準的なグラフィック描画API（点・線・矩形・円・ビットマップ等）をサポート
- **日本語描画対応**  
  漢字・ひらがな・カタカナ・ASCII混在テキストの描画が可能
- **豊富なカラー定数**  
  RGB565形式の明るい色・暗い色・標準色などをSTDCOLORで簡単指定
- **DMA転送・高速描画**  
  SPI DMA転送やバッファリングによる高速描画
- **カスタムフォント・ビットマップ描画**  
  独自フォントや画像データの描画に対応
- **print/printlnによるテキスト描画**  
  ArduinoのSerialと同様のAPIでディスプレイにテキスト出力
- **回転・クリッピング・座標/矩形ユーティリティ**  
  画面回転や描画範囲のクリッピング、座標・矩形管理用の構造体を提供

---

## ディレクトリ構成

- **Adafruit_GFX, Adafruit_SPITFT, Adafruit_ILI9341**  
  グラフィック描画・ディスプレイドライバ本体
- **KanjiHelper, KanjiFontStructure**  
  日本語フォントデータ管理・検索・取得
- **misc/KNJGfx_struct.h**  
  標準カラー定数、座標・矩形構造体
- **core/Print**  
  テキスト出力基底クラス（print/println/printf等）

---

## インストール方法

1. 本リポジトリをダウンロードまたはクローン
2. プロジェクトに`lib-9341`フォルダを追加
3. 必要なヘッダファイルをインクルード

---

## 使い方

### 基本的な初期化と描画

@code{.cpp}
#include <Adafruit_ILI9341.h>
Adafruit_ILI9341 tft(/*ピン設定*/);
tft.begin();
tft.fillScreen(STDCOLOR.BLACK);
tft.setCursor(0, 0);
tft.print("こんにちは世界");
@endcode

### GFXcanvas16によるオフスクリーン描画

@code{.cpp}
#include <Adafruit_GFX.h>
GFXcanvas16 canvas(240, 320);
canvas.fillScreen(STDCOLOR.WHITE);
canvas.drawRect(10, 10, 100, 50, STDCOLOR.RED);
// バッファ内容をディスプレイへ転送
tft.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 320);
@endcode

### 日本語フォントの利用

- `KanjiHelper::SetKanjiFont()`でフォントデータをセット
- print/printlnで日本語テキスト描画が可能

---

## クラス継承関係

```
Print
└── Adafruit_GFX
    ├── Adafruit_SPITFT
    │   └── Adafruit_ILI9341
    └── GFXcanvas16
```

- 末端クラス（Adafruit_ILI9341やGFXcanvas16）をインスタンス化して利用します。
- 親クラスのAPI（グラフィック描画・テキスト出力）がそのまま使えます。

---

## 主要クラスと役割

| クラス名              | 役割・用途                                                                 |
|----------------------|----------------------------------------------------------------------------|
| Print                | テキスト出力の基底クラス。print/println/printf等のAPIを提供                |
| Adafruit_GFX         | グラフィック描画の基底クラス。点・線・矩形・円・ビットマップ等の描画API    |
| Adafruit_SPITFT      | SPI/パラレル制御・高速描画・DMA転送などを追加した中間クラス                |
| Adafruit_ILI9341     | ILI9341コントローラ用ディスプレイドライバ。ハード固有の初期化・描画処理    |
| GFXcanvas16          | 16ビットカラーのソフトウェアキャンバス。オフスクリーン描画や合成に利用      |
| KanjiHelper          | 日本語フォントデータの管理・検索・取得を行う静的ユーティリティクラス        |
| KNJGfx_struct.h      | 標準カラー定数（STDCOLOR）、座標・矩形構造体（XY, XYWH）を提供             |

---

## 詳細情報・ドキュメント

- より詳しい解説や使い方は [Qiitaの解説ページ](https://qiita.com/BUBUBB/items/7c6777c04dedf01d7c3b) をご覧ください。
- ソースコードや最新情報は [GitHubリポジトリ](https://github.com/HisayukiNomura/KNJGfx9341) にて公開しています。

---

## ライセンス

本ライブラリはMITライセンスおよびAdafruitライブラリのライセンスに準拠します。

---

## 作者

- オリジナル: Adafruit Industries
- 拡張・日本語対応: KNJ Project

