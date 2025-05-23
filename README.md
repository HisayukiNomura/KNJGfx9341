---
title: Rapberry PI  Pico(標準SDK/ArduinoSDK) 用　ILI9341 TFT液晶 日本語ライブラリ
tags: ILI9341 RaspberryPiPico ArduinoIDE
author: BUBUBB
slide: false
---

# 概要
このライブラリは、Raspberry PI PICO 上で、ILI9341 TFT液晶をコントロールするためのライブラリです。日本語表示等に対応しています。



次の環境で使用できます。

- Raspberry pi PICO 標準SDK　（ソースコードレベルで組み込んで使用）
- Arduino IDE (ライブラリとしてインポートして使用）

# 非常に手を抜いた目次

- 手っ取り早くソースを見たい
⇒ [ソースコード](#ソースコード)へどうぞ。

- 標準SDK（Visual Studio Code）を使っていて、まずはビルドして実行したい
⇒[ライブラリを使用するためのウォークスルー](#ライブラリを使用するためのウォークスルー)

- Arduino IDEでとにかく使いたい
⇒[Arduino APIから使用する](#arduino-apiから使用する)

- そうじゃないPythonで使いたい
⇒[Pythonから使用する](https://qiita.com/BUBUBB/items/80c37080420601b5f896)

- Arduino API用のライブラリとしてビルドしたい
⇒[Arduino API用にビルドする](#arduino-api用にビルドする)

- そもそもこれは何？
⇒このままよみすすめてください

- どんなメソッドがあるのか知りたい
⇒[付録のライブラリリファレンス](#ライブラリリファレンス)

- どんなフォントが使えるのか知りたい
⇒[付録の使用できるフォントの一覧](#使用できるフォントの一覧)



<hr/>

# このライブラリについて

Adafruitは、Arduinoで簡単に使用できるが、標準SDK用のものがありません。日本語にも対応していないため作成されました。
今回は、なるべくコンパクトになるように作った[Rapberry PI Pico用　ST7735 TFT液晶 SDKライブラリ](https://qiita.com/BUBUBB/items/3caf2aefba4ae3a715e1)とは異なり、Adafruitのソースコードや関数になるべく準拠するように作成しました。おそらく、Adafruit_GFXを使用していたプログラムのほとんどがソースコードを変えることなく動作すると思います。

クラス名や関数名がAdafruit_GFXと同じになっていることから、Adafruitとの競合を避けるため、このプログラムは名前空間を変えてあります。

## 主な変更点
- 漢字の表示
- 文字表示の高速化（漢字のみ）
- TFTへの表示に際しprintfなどの追加
- 透過色のサポート

# 動作例
次の動画は、このライブラリのサンプルプログラム（本体に同梱）です。

- 図形表示のデモ。直線や円などを表示し、最後に実行時間を表示します。Adafruit_GFXから変更がほとんどありません
- ビットマップ表示のデモ。いろいろな表示方法や、それぞれの時間を表示させています。オリジナルに存在しない、透過色指定などの機能を追加しました
- アルファベット表示のデモ。様々なフォントで文字を表示します。Adafruit_GFXから変更がほとんどありません。
- 日本語表示のデモ。日本語のテキストを表示します。このプログラムではフォントの指定方法がST7735と異なり、Adafruit_GFXに近くしてあります。理論上は異なるフォントを複数持てるはずですがメモリの制限から現実的とは言えません。
- キャンバスを使用してフリッカーレスで文字を表示させます。

<iframe width="560" height="315" src="https://www.youtube.com/embed/ic4ei0ZD3Fo?si=GbqZQ4mX_5axNdqh" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>


# ソースコード
ソースコードは

githubの[KNJGfx9341のリリースページ](https://github.com/HisayukiNomura/KNJGfx9341/releases)

で公開しています。

### フォルダ構成と用途
プロジェクトのフォルダ構成と主な用途は次のようになっています。

```
 / 　動作を確認するためのプログラムや、VSCodeのプロジェクト関連ファイル
 ├──　KNJGfx9431.cpp 　　サンプルプログラム
 ├──　pictBackground.data サンプルプログラム用の画像データファイル
 ├──　CMakeLists.txt    cmake（ビルド）用の定義ファイル
 ├──　pico_sdk_import.cmake Raspberry pi PICO SDKを使用するための定義ファイル
 └──── lib-9341　ライブラリ本体のソースコード
        │　
        ├── Adafruit_GFX_Library  デバイス非依存のグラフィック用プログラム
        │    └── Fonts　　　　　　 フォントファイル
        ├── Adafruit_ILI9341      デバイス（ILI9341)依存のグラフィック用プログラム
        ├── core                  動作に必要なプログラム（Arduino IDEの一部分）
        ├── SPI                   Raspberry PI POCOに依存するSPIの処理
        ├── misc                  その他、便利だったりコンパイルに必要な処理
        ├── Kanji                 漢字処理のライブラリ
        │    └── Fonts　　　　　　 漢字フォントファイル（拡張子.inc)とその構造体 
        ├── XPT2046_Touchscreen   タッチスクリーン用ドライバ
        │
        │　以下、MicroPython用のファイル
        ├── ForMPY　　　　　　　　  MicroPythonで使用するためのプログラム
        │    └── Example          Micropython用サンプルプログラム
        │          └─ BitmapData  Micropython用サンプルプログラムのデータファイル
        │
        │　以下、Arduino用のファイル
        ├── Examples              Arduinoライブラリ用のサンプルスケッチ
        │    ├─ All               主要機能すべてを実行するサンプルスケッチ
        │    └─ Text　　　　　　　 テキスト表示のシンプルなサンプルスケッチ
        ├── KNJGfx9341.h          Arduinoスケッチ用のincludeファイル
        └── library.properties    Arduinoライブラリ用のプロパティファイル
       
```

ライブラリの本体は、lib-9341以下に含まれています。このライブラリを使用するプログラムは、lib-9341フォルダ以下を自分のプロジェクトに複製し、使用してください。

Micropython用ファイルや、Arduino用のファイルはSDKから使用する場合には不要です。（存在しても特に弊害はありません）

### フォント情報ファイル

lib-9341\Knaji\Fonts、lib-9341\Adafruit_GFX_Library\Fontsサブフォルダは、テキスト表示の際に使用されるフォントのビットマップ情報です。

#### 英文フォント用ファイル
lib-9341\Adafruit_GFX_Library\Fontsフォルダに存在する、拡張子 .hのファイルは、[adafruitのgfxライブラリ](https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts)で使用されているものがそのまま使用できます。必要な（使用する）ファイルだけをincludeして使用します。

#### 日本語フォント用ファイル
lib-9341\Knaji\Fontsフォルダに存在する、拡張子 .incは、日本語フォント用のファイルです。複数のファイルがあります。必要なフォントをインクルードして使用します。複数のファイルをインクルードし、表示の際にフォントを切り替えて使用することができますが、漢字フォントのデータサイズは英文字と比較にならないサイズです。
使用する際は、教育漢字のみ、など小型のフォントセットで使用してください。

ファイル名は、 **＜もとになったTTFフォント名＞_<横ドット数>_<文字セット>.inc** の形になっています。

現在使用できる（ライブラリに含まれている）フォントは[使用できるフォントの一覧](#使用できるフォントの一覧)を参照してください。

このヘッダファイルは、 [PythonFontConverter](https://github.com/HisayukiNomura/PyFontConverter) でTTFファイルから変換しました。別のフォントを使用したい場合、このプログラムを使ってTrueTypeフォントを、Ｃのヘッダファイルなどの形に変換して使用できます。


# 使用方法

## ハードウェアの準備

### 結線例

今回、みんな大好き秋月電子の[ILI9341搭載2.8インチSPI制御タッチパネル付TFT液晶MSP2807](https://akizukidenshi.com/catalog/g/g116265/)を使って開発しました。この接続でTFT液晶の制御とタッチパネルが使用できるようになります。

そのほか、いろいろな製品がありますが、多くのデバイスは同じピン配列を持っているようです。

ST7732と同様に、このコントローラも、コントローラーからマイコン側に信号を送る、MISOは持っていません。３番ピンのCS信号で、デバイスを使用するかどうかの選択と、５番ピンのD/Cでデータとコマンドの区別を行います。

|Raspberry PI|⇔|TFT液晶|
|:---- |:---:|:----|
|3.3V(36pin)||VCC(1Pin)・LED（8Pin)|
|GND(38Pin)||GND(2Pin)|
|GP22(29Pin)||CS(3Pin)|
|GP21(27Pin)||RESET(4Pin)|
|GP20(26Pin)||D/C(5Pin)|
|GP19(25Pin)-MOSI/SDO/TX||SDI(MOSI)・T_DIN (6Pin&12Pin)|
|GP18(24Pin)-SCK||SCK・T-CLK (7Pin&10Pin)|
|GP17(22Pin)-CSn||T-CS(11Pin)|
|GP16(21Pin)-MISO/SDO/RX||T-DO(13Pin)|

タッチパネルの接続もあるため、結線は少し複雑ですが、次のように接続してください。

<img width=60% alt="RaspTFT.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/d6b408ad-2160-41eb-aec9-297e51195e71.png">

タッチパネルが無いデバイスや、使用しない場合には　TOUCH　にある部分は接続しなくても構いません。

今回のライブラリのサンプルプログラムは、次のデモに進む際にタッチパネルの操作を行いますが、タッチパネルが無い場合でも、一定の時間が来ると自動的に先に進みます。

## ソフトウェアの準備

必要なソフトウェアは、Raspberry PI pico　の標準SDKだけです。

RaspBerry pi の公式ページ、[Get started with Raspberry Pi Pico-series and VS Code](https://www.raspberrypi.com/news/get-started-with-raspberry-pi-pico-series-and-vs-code/)などを参照し、VSCodeにSDKを導入します。日本語でも[これ](https://zenn.dev/usagi1975/articles/2024-08-29-000_pico_development) や、[これ](https://tetsufuku-blog.com/pico-vscode/)などで情報を見つけることができます。

プロジェクトを新規作成し、 [githubページ](https://github.com/HisayukiNomura/KNJGfx9341)から、lib-9341フォルダをダウンロードして、自分のプロジェクトに組み込んで使用してください。

プロジェクトには、spi、dmaが必要になります。ビルド時にエラーになった場合、適宜CMakeList.txtの　target_link_librariesに追加してください。

具体的な手順については、[ライブラリを使用するためのウォークスルー](#ライブラリを使用するためのウォークスルー)の章で順を追って説明します。

# ライブラリを使用するためのウォークスルー
この章では、何もない状態から、Hello, Worldに相当するプログラムを作成します。

## プロジェクトの作成とライブラリのコピー

Raspberry PI SDKから、C++のプロジェクトを作成します。

この時、次の点に注意してください。
- APIバージョンは2.0.0以上が必要です。
- SPI、DMAのサポートが必要です。

<img width=60% alt="1.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/af363c46-fcd3-46cd-a764-fb72d3629432.png">

その他の設定は、使用しているボードや環境にあわせて適宜設定を行ってください。

次に、 [githubページ](https://github.com/HisayukiNomura/KNJGfx9341)から、右上にあるReleaseのリンクをクリック、最新のソースコードをダウンロードします。

 <INS>ダウンロードするのは、Source code(zip)　です。KNJGfx9341_ARDUINO_ONLY.zipではありません。</INS>


<img width=60% alt="1.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/88082616-124e-4142-84ff-7f2bd329722e.png">

ダウンロードしたzipファイル（もしくはtar.gzip）ファイルを展開し、lib-9341フォルダを、作成したプロジェクトのフォルダにコピーします。

下の例は　Test9341\Simpleというフォルダにプロジェクトを作成した　場合のフォルダ構成です。コピーしたlib-9341フォルダの下に、[フォルダ構成と用途](#フォルダ構成と用途)の章に記した複数のフォルダが確認できます。

![3.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/3f7d1a3c-5dff-4610-9354-a51c30d2d832.png)

## CMakeFiles.txtの作成

CMakeList.txtを変更して、作成されたプロジェクトがライブラリをビルド対象に含めるように設定します。設定は、大きく分けて次のようになります。1～３までは、標準SDKで開発をしている人にはほとんど不要な説明かと思いますが、Arduino IDEを主に使っている人にはある意味一番難しい部分かもしれないため、解説してあります。

1. add_executablesに、コンパイル対象の.cpp ファイルを加える
1. target_link_librariesに、lib-9341が内部で使用するライブラリを追加する
1. target_include_directoriesに、lib-9341が使用するフォルダを追加する
1. ライブラリ内部で使用しているボード種類が判別できるよう、マクロ値を設定する
1. ビルドできるかをテストしておく

### 1.add_executablesに、コンパイル対象の.cpp ファイルを加える

自動作成されるCMakeFiles.txtは次のようになっています。

```
  add_executable(simple simple.cpp )
```

この部分に、ライブラリで使用する .cppを追加します。
```
add_executable(simple simple.cpp 
# 使用するプログラムを追加する
    lib-9341/defines.cpp
       　：
       （略）
       　：
    lib-9341/XPT2046_Touchscreen/XPT2046_Touchscreen.cpp
    lib-9341/pins.cpp
)
```

<details><summary>全ボード共通の設定例</summary>
中身の文章
<div>

```
add_executable(simple simple.cpp 
# 使用するプログラムを追加する
    lib-9341/misc/defines.cpp
    lib-9341/Adafruit_GFX_Library/Adafruit_GFX.cpp
    lib-9341/Adafruit_GFX_Library/Adafruit_SPITFT.cpp

    lib-9341/Adafruit_ILI9341/Adafruit_ILI9341.cpp
    lib-9341/spi/spi.cpp
    lib-9341/Kanji/KanjiHelper.cpp
    lib-9341/core/PIOProgram.cpp
    lib-9341/core/cyw43_wrappers.cpp
    lib-9341/core/delay.cpp
    lib-9341/core/_freertos.cpp
    lib-9341/core/CoreMutex.cpp
    lib-9341/core/Print.cpp
    lib-9341/core/RP2040Support.cpp
    lib-9341/core/stdlib_noniso.cpp
    lib-9341/core/WString.cpp
    lib-9341/core/wiring_private.cpp
    lib-9341/core/wiring_digital.cpp
    lib-9341/core/wiring_analog.cpp

    lib-9341/XPT2046_Touchscreen/XPT2046_Touchscreen.cpp

    lib-9341/misc/pins.cpp
)
```

</div>
</details>


### 2.target_link_librariesに、lib-9341が内部で使用するライブラリを追加する

target_link_librariesはリンクするライブラリの指定ですが、今回の場合、SDKが提供する必須のライブラリを指定することになります。

プロジェクトを新規で作成すると、target_link_librariesは２つ作成されます。

前半は、標準で必ず使用するライブラリやユーザーが後から追加したライブラリ、後半は、プロジェクトウィザードで指定したライブラリのようです。

```
# Add the standard library to the build
target_link_libraries(simple
        pico_stdlib)
      :
     (略)
      :
# Add any user requested libraries
target_link_libraries(simple 
        hardware_spi
        hardware_dma
        )        
```

どちらに追加しても構いませんが、一応、前述のルールに従うようにライブラリを追加していきます。
PICO/PICO２で、必要としているライブラリに一部違いがありますが、PICOW用を使用しておけば、どちらでも問題ありません。（若干、使用するフラッシュメモリの量が増えます）

```
# Add the standard library to the build
target_link_libraries(simple
        pico_stdlib
#使用するライブラリを追加する        
        pico_rand
        pico_unique_id
        hardware_exception)
      :
     (略)
      :
# Add any user requested libraries
target_link_libraries(simple 
        hardware_spi
        hardware_dma
#PICOWの場合にのみ必要        
        pico_cyw43_arch_none
        )
```

<details><summary>PICO/PICO2の設定例</summary>

<div>

```
# Add the standard library to the build
target_link_libraries(simple
        pico_stdlib
#使用するライブラリを追加する        
        pico_rand
        pico_unique_id
        hardware_exception)
      :
     (略)
      :
# Add any user requested libraries
target_link_libraries(simple 
        hardware_spi
        hardware_dma
        )
```

</div>
</details>

<details><summary>PICOW/PICO2Wの設定例</summary>

<div>

```
# Add the standard library to the build
target_link_libraries(simple
        pico_stdlib
#使用するライブラリを追加する        
        pico_rand
        pico_unique_id
        hardware_exception)
      :
     (略)
      :
# Add any user requested libraries
target_link_libraries(simple 
        hardware_spi
        hardware_dma
#PICOWの場合にのみ必要        
        pico_cyw43_arch_none
        )
```

</div>
</details>


### 3. target_include_directoriesに、lib-9341が使用するフォルダを追加する

ライブラリ自身が使用するファイルのincludeフォルダを追加します。
ライブラリのディレクトリと、SDKのディレクトリが必要です。


デフォルトの状態では、ソースコードのあるディレクトリだけが含まれています。
```
target_include_directories(simple PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
```
ここに、ライブラリのフォルダと、ハードウェア例外のインクルードフォルダ、ネットワーク関連のフォルダを追加します。

PICO/PICO2の場合はネットワーク関連のincludeフォルダは必要ありませんが、指定してあったとしても（＝PICO_W用であっても）特に弊害はありません。

```
target_include_directories(simple PRIVATE
#インクルードディレクトリも追加する       
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_GFX_Library
　　　　　　　：
      　　　（略）
　　　　　　　：
        ${CMAKE_CURRENT_LIST_DIR}/XPT2046_Touchscreen   
#PICOW以外の場合、これ以降は不要         
        ${PICO_SDK_PATH}/src/rp2_common/hardware_exception/include
　　　　　　　：
      　　　（略）
　　　　　　　：
        ${PICO_SDK_PATH}/src/rp2_common/pico_lwip/include
        ${PICO_SDK_PATH}/lib/cyw43-driver/src/

)
```





<details><summary>PICO/PICO２の設定例</summary>
<div>

```
target_include_directories(simple PRIVATE
#インクルードディレクトリも追加する       
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_GFX_Library
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_GFX_Library/Fonts
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_BusIO
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_ILI9341
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/core
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/spi
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/variants
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Kanji
        ${CMAKE_CURRENT_LIST_DIR}/XPT2046_Touchscreen   
)
```

</div>
</details>



<details><summary>PICOW/PICO２Wの設定例</summary>
<div>

```
target_include_directories(simple PRIVATE
#インクルードディレクトリも追加する       
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_GFX_Library
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_GFX_Library/Fonts
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_BusIO
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_ILI9341
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/core
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/spi
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/variants
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Kanji
        ${CMAKE_CURRENT_LIST_DIR}/XPT2046_Touchscreen   
#PICOW以外の場合、これ以降は不要         
        ${PICO_SDK_PATH}/src/rp2_common/hardware_exception/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_lwip/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_driver/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_driver/include/pico
        ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_arch/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_arch/include/pico
        ${PICO_SDK_PATH}/src/rp2_common/pico_async_context/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_async_context/include/pico
        ${PICO_SDK_PATH}/src/rp2_common/hardware_adc/include/
        ${PICO_SDK_PATH}/src/rp2_common/hardware_pwm/include/
        ${PICO_SDK_PATH}/lib/lwip/src/include
        ${PICO_SDK_PATH}/lib/lwip/src/include/iwip
        ${PICO_SDK_PATH}/lib/cyw43-driver/src/

)
```

</div>
</details>


### 4. ライブラリ内部で使用しているボード種類が判別できるよう、マクロ値を設定する

条件コンパイルのため、使用しているボードによってコンパイラに渡すシンボル値を決定する処理を追加します。


<details><summary>なぜこの処理が必要かに関する小難しい解説はこちら</summary>

---
Adafruit_GFX（おそらくそれに限らず）ではGPIOの出力で「特定のファイルがコンパイル対象に含まれるか、含まれないか」によって処理を分けるために弱参照を使っています。具体的には、pins.cppに含まれる関数は、同名の関数が wiring_digital.cppにも含まれており、これが弱参照で extern されています。
pins.cppがプログラムにリンクされると、こちらの参照の方が強くなるため、wiring_digital.cpp内の関数ではなく、pins.cppの関数の方が呼び出されることになります。一種の「関数オーバーロード」のようなことを実現しているわけです。

これはArduino IDEでは便利なのかもしれませんが、標準SDKでは少々なじみが悪いように感じたため、シンボルを使った条件（普通の#ifdef）に変更してあります。

これにより、ボードを変更（例：pico⇒pico_wなど）するときに、一々　add_executable　のエントリを変更する必要がなく、一般的な方法である　set(PICO_BOARD pico_w CACHE STRING "Board type")　の変更だけで済むようになります。

ここで設定されたシンボルは、ライブラリ内で次のように使用され、PICOとPICOWとで行われる処理を分けるようにしています。

<div>

```
extern "C" void pinMode(pin_size_t pin, PinMode mode) {
	#if PICO_BOARD_VALUE == 1 || PICO_BOARD_VALUE == 2  // PICO/PICO2
	__pinMode(pin, mode);
	#else
	cyw43_pinMode(pin, mode);
	#endif
}
```

</div>

詳細については、pins.cpp内のコメントを参照してください。



---

</details>


デフォルトの状態では、次のようになっています。この、pico_wという値を、使用しているボードにあわせて書き換えます。使用できるボード名は、pico,pico_w,pico2,pico2_wです。

それ以外のボードを使用している場合は、最も近いと思われる値を設定してください。

```
set(PICO_BOARD pico_w CACHE STRING "Board type")
```
ここで設定された変数に従い、コンパイル時にシンボルを設定させます。

```
set(PICO_BOARD pico_w CACHE STRING "Board type")

# プログラム内で処理を分けられるように、マクロ値としてコンパイラに渡す
if(PICO_BOARD STREQUAL "pico")
    set(PICO_BOARD_VALUE 1)
elseif(PICO_BOARD STREQUAL "pico_w")
    set(PICO_BOARD_VALUE 3)
elseif(PICO_BOARD STREQUAL "pico2")
    set(PICO_BOARD_VALUE 2)
elseif(PICO_BOARD STREQUAL "pico2_w")
    set(PICO_BOARD_VALUE 4)
else()
    set(PICO_BOARD_VALUE 0) # 未知のボード
endif()
add_compile_definitions(PICO_BOARD_VALUE=${PICO_BOARD_VALUE})
```

### 5. ビルドできるかをテストしておく

これでプロジェクトの準備は完了ですが、間違いがないか、一度テストしておきます。エディタ画面にフォーカスを合わせて、CTRL+Pを押します。左下の歯車をクリック > [コマンドパレット]でも構いません。コマンドの一覧が表示されたら、 CMake ビルドをクリックします。

<img width=80% alt="1.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/d2cc83ec-224a-45c5-addf-78affc98a3d0.png">

キットの問い合わせがあった場合、[未指定]を選択し、CMakeに任せます。もしくは正しいビルドのためのキットを指定してください。

<img width=80% alt="1.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/66bd47af-c465-41f3-9ad8-aba42f6af4a9.png">


Visual Studio Codeの出力タブに、ビルドの状況が表示されます。終了コード０で終了し、エラーが出ていなければビルドは成功しています。

```
[main] フォルダーのビルド中: e:/work/Test9341/simple/build 
[main] プロジェクトを構成しています: simple 
[proc] コマンドを実行しています: C:\Users\hisay/.pico-sdk/cmake/v3.31.5/bin/cmake -
　　　　　：
[build] [64/64 100% :: 2.463] Linking CXX executable simple.elf
[driver] ビルド完了: 00:00:03.012
[build] ビルドが終了コード 0 で終了しました
```
## ソースコードのコーディング

もっとも簡単なプログラムとして、画面に日本語テキストを表示してみます。

### 自動生成されたコードの確認と削除

SDKのプロジェクトウイザードで自動生成されたメインプログラムには、SPIのポート指定（GP16～GP19)とCS（デバイスセレクト）の処理や、DMA関連の処理がサンプルとして自動的に作られています。これらは邪魔になるため削除していきます。

<details><summary>自動生成されたコードを見る</summary>
自動生成されたコードの参考例です。環境やプロジェクト作成時のオプションにより内容は異なります。
<div>

```
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// Data will be copied from src to dst
const char src[] = "Hello, world! (from DMA)";
char dst[count_of(src)];



int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    // Get a free channel, panic() if there are none
    int chan = dma_claim_unused_channel(true);
    
    // 8 bit transfers. Both read and write address increment after each
    // transfer (each pointing to a location in src or dst respectively).
    // No DREQ is selected, so the DMA transfers as fast as it can.
    
    dma_channel_config c = dma_channel_get_default_config(chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, true);
    
    dma_channel_configure(
        chan,          // Channel to be configured
        &c,            // The configuration we just created
        dst,           // The initial write address
        src,           // The initial read address
        count_of(src), // Number of transfers; in this case each is 1 byte.
        true           // Start immediately.
    );
    
    // We could choose to go and do something else whilst the DMA is doing its
    // thing. In this case the processor has nothing else to do, so we just
    // wait for the DMA to finish.
    dma_channel_wait_for_finish_blocking(chan);
    
    // The DMA has now copied our text from the transmit buffer (src) to the
    // receive buffer (dst), so we can print it out from there.
    puts(dst);

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
```

</div>
</details>

削除したものが次のプログラムです。ほとんど空っぽになってしまいます。

```
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"


int main()
{
    stdio_init_all();


    while (true) {
    }
}

```

### 画面表示プログラムの記述

ここではいくつかの改変をプログラムに加えていきます。

1. 本ライブラリが使用する機能を定義してある、インクルードファイルを追加します。
　
1. 本ライブラリの名前空間を使用するための宣言を行います。
本ライブラリは、多くの関数やクラスで、Adafruit_GFXと同じ名前の関数で異なる実装を行っています。ユーザープログラムが、ある名前の関数を呼び出すとき、本ライブラリの関数か、Adafruit_GFXの関数かを区別するため、本ライブラリには ardPort という名前で名前空間を付けてあります。（arduinoからPortingした、程度の意味で適当につけたものです）ここで追加する２行は、明示的に名前空間を指定せずに呼び出しを行った時、Adafruit_GFXではなく、本ライブラリの関数を呼び出すように定義を行います。
　
1. 使用するポートの定義を行います。
本解説では、[使用するまでの準備>ハードウェア>結線例](#結線例) にあるような接続をしています。これを、ライブラリに対して通知するための処理を行います。#defineでの定義と、ライブラリ関数への登録が必要になります。
登録が終わったら、tft.beginを呼びだして液晶の制御を開始します。
　

1. 漢字関連のフォントを設定し、画面に表示します。フォントファイルを includeし、制御開始後の液晶コントローラオブジェクトに対して、setFontを使用してフォントを指定します。引数は２つあります。実際の文字コードを表すデータと、フォントの文字コードとビットマップデータを結びつける情報です。フォントファイル（拡張子inc）の中を確認すると、指定するデータの名前を確認できます。
 tft.useWindowMode(true); を追加してあります。この指定することで、ILI9451の持っているウインドウ機能を使用して、効率的に文字が表示できることがあります。とくに問題がなければ、おまじないとして追加してください。
 　
1. 画面に文字を表示させる処理を追加してきます。
画面を黒で塗りつぶし、文字の前景色、背景色を決定、表示位置を指定して文字を表示します。１つは固定の文字列、もう一つはprintfでフォーマット指定可能な文字の表示です。

改変後のソースコードは次のようになります。　　＜＜＜＜＜　追加　　（１）　などの形でコメントが追加してあります。これは、前述の手順でのステップに相当します。


```
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"

// ライブラリのヘッダファイル　　　　　　　　＜＜＜＜＜　追加　　（１）　
#include "lib-9341/Adafruit_ILI9341/Adafruit_ILI9341.h"
#include "KanjiHelper.h"

// 漢字フォント情報ファイル　　　　　　　　　＜＜＜＜＜　追加　　（４）　
#include "Kanji/Fonts/JF-Dot-Shinonome12_12x12_ALL.inc"

// 名前空間の使用を宣言　　　　　　　　　　　　＜＜＜＜＜　追加　（２）
using namespace ardPort;
using namespace ardPort::spi;


// 使用するポートの定義　　　　　　　　　　　　＜＜＜＜＜　追加　（３）
#define TFT_SCK 18  // 液晶表示の SCK
#define TFT_MOSI 19 // 液晶表示の MOSI
#define TFT_DC 20   // 液晶画面の DC
#define TFT_RST 21  // 液晶画面の RST
#define TFT_CS 22   // 液晶画面の CS

int main()
{
    stdio_init_all();
    
    // ILI9341ディスプレイのインスタンスを作成　　＜＜＜＜＜　追加　（３）
    Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST); 
    SPI.setTX(TFT_MOSI); // SPI0のTX(MOSI)
    SPI.setSCK(TFT_SCK); // SPI0のSCK  
    tft.begin();         // TFTを初期化

    // 漢字フォントの設定　　　　　　　　　　　　　＜＜＜＜＜　追加　（４）　
    tft.setFont(JFDotShinonome12_12x12_ALL, JFDotShinonome12_12x12_ALL_bitmap); 

    // 文字の表示を高速化させる　　　　　　　　　　＜＜＜＜＜　追加　（４）　
    tft.useWindowMode(true); 

    // 画面を黒で塗りつぶす　　　　　　　　　　　　＜＜＜＜＜　追加　（５）　
    tft.fillScreen(ILI9341_BLACK); 

    while (true) {
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); // 文字色を白に設定
        tft.setCursor(10, 10);
        tft.printf("こんにちは、世界！");                     // 文字を表示
        uint64_t time_us = time_us_64();                    // 起動後の経過時間（ミリ秒）
        tft.setCursor(10, 40);
        tft.printf("経過時間: %llu ms", time_us / 1000); // 経過時間を表示
    }
}

```

### 実行

コーディングが終わったら、ビルドして実行します。ビルドの手順は[ビルドできるかをテストしておく](#5-ビルドできるかをテストしておく)を参照してください。

画面に「こんにちは、世界！」などの文字が出れば正常です。


![6.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/677cf6b6-5a5a-4113-b613-f79468c6b102.png)



### フォントの変更
フォントの変更には、プログラム中２カ所を変更する必要があります。 includeする日本語フォントデータと、setFontの引数です。
フォントデータは、./lib-9341/Kanji/Fonts　の中に拡張子 inc で含まれています。例えば、IPA明朝 24x24ピクセル、教育用漢字データを使用する場合、
```
#include "Kanji\Fonts\ipam_24x24_SCHOOL.inc"
``` 
となります。

ipam_24x24_SCHOOL.incを、テキストエディタで開くと、static const KanjiData で定義された、漢字データ構造体と、static const uint8_t で定義されている字形データが存在します。

```
    :
  （略）
    :
static const KanjiData ipam_24x24_SCHOOL[] = {
	{0x00000000 , 0x0000 , 0x0000 , 12 ,24 , 0x00000000},	// "0x00"
	{0x00000001 , 0x0001 , 0x0001 , 12 ,24 , 0x00000030},	// "0x01"
    :
  （略）
    :
static const uint8_t ipam_24x24_SCHOOL_bitmap[] = {
// UNICODE:0x00000000 -  Offset:0x00000000   -- CHAR:"0x00" 
	0x00,	0x00,		// 0000000000000000 
	0x00,	0x00,		// 0000000000000000 
    :
  （略）
    :
```    


この二つの変数名を、setFontに与えることでフォントを指定します。ipam_24x24_SCHOOL.incの場合であれば次のようになります。

```
tft.setFont(ipam_24x24_SCHOOL, ipam_24x24_SCHOOL_bitmap); 
```
ビルドを行い、実行するとフォントが変更されています。

![7.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/f8a1fe8f-fe6d-4828-9fa7-975159a42ea8.png)


# サンプルプログラムのビルドと実行

より多くの機能を確認するため、デモプログラムが合わせて配布されています。次の手順でデモンストレーションプログラムを実行できます。

※このデモンストレーションプログラムは、XPT2046コントローラを使用し、接続されているTFT液晶用です。タッチパネルのICが無い場合や、異なるコントローラを使用している場合、タッチ機能が働きません。
画面上の＜＜タッチで次へ＞＞などはそのまま無視していると２０秒程度で自動的に次に進みます。


## 新規プロジェクトを作成

[プロジェクトの作成とライブラリのコピー](#プロジェクトの作成とライブラリのコピー)を参照し、新規プロジェクトを作成します。

作成が終わったら、[githubのリリースページ](https://github.com/HisayukiNomura/KNJGfx9341/releases)から最新のソースコードをダウンロードします。

zipファイルを解凍し、すべてのファイルを今回作成した新規プロジェクトのフォルダにコピーします。

上書きの確認があった場合、そのまま上書きしてください。

## メインプログラムの削除

プロジェクトのルートフォルダには、今回作成したプロジェクトのmain 関数を含むファイルがありますが、このファイルは削除してください。
代わりに、ダウンロードしたzipファイルに含まれている、KNJGfx9341.cpp　をメインプログラムとして使用します。

## ビルドと実行

これで準備は終わりです。ビルドして実行してください。


# Arduino API用にビルドする

## ソースコードのダウンロード
もし、これまでの手順で、デモプログラムなどをビルドできている場合、すでに必要なソースコードはダウンロード済みです。このステップは必要ありません。次の、「バッチファイルの入手」に進んでください。

いままで一度も、デモプログラムなどをビルドしたことが無い場合、[ライブラリを使用する](#ライブラリを使用する)を参照してアプリケーションをビルドするか、[Arduino APIから使用する](#arduino-apiAPIから使用する)を参照し、すでに作成済みのArduinoライブラリを使用することをお勧めします。

絶対ビルドせずに、それでも自力でArdiuno ライブラリをビルドしたいという場合は、[ソースコード](#ソースコード)を参照し、[githubのリリースページ](https://github.com/HisayukiNomura/KNJGfx9341/releases)から、ソースコードをダウンロードしてください。


## バッチファイルの入手

Ardiuno用のライブラリにするためのバッチファイル、makeArduinoLib.bat　を、
を、https://github.com/HisayukiNomura/KNJGfx9341　からダウンロードしてください。
直接のURLは[ここ](https://github.com/HisayukiNomura/KNJGfx9341/blob/main/makeArduinoLib.bat)です。　右上にある、Download Raw File をクリックしてダウンロードできます。

![16.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/9eed6381-7584-4fcd-be71-d46125aa2162.png)

ダウンロードしたファイルは、lib-9341　フォルダと同じ場所にコピーしておきます。

```
E:\Work\KNJGfx9341>dir
       :
2025/04/29  15:22    <DIR>          .
2025/04/29  10:02    <DIR>          ..
       :
      
2025/04/29  10:10            49,625 KNJGfx9341.cpp
2025/04/29  10:10    <DIR>          lib-9341
2025/04/29  10:10             2,068 makeArduinoLib.bat
       :
```

## バッチファイルの実行

コマンドプロンプトを開き、makeArduinoLib.bat を実行します。引数などはありません。
確認のメッセージが表示されるので、「続行するには何かキーを押してください」のメッセージが出たら、ENTERキーを押します。

```
E:\Work\KNJGfx9341>makeArduinoLib.bat
-----ddd
This script will create a zip file for the Arduino library.
The created ZIP file can be used in Arduino IDE via Sketch > Include Library > Add .ZIP Library.

It will include all the necessary files and folders.
The zip file will be named KNJGfx9341_ARD.zip and will be created in C:\Users\hisay.

note: This script creates KNJGfx9341_ARD folder as a temporary folder.
It will be deleted before execution and deleted after the zip file is created.

press any key to continue or Ctrl+C to cancel.
続行するには何かキーを押してください . . .
successfully created C:\Users\hoge\KNJGfx9341_ARD.zip
  *If an error occurs, the execution log was saved in outlog.txt.
```
成功すると、ユーザープロファイルのフォルダに、KNJGfx9341_ARD.zip　といおう名前でインポート可能なライブラリが作成されます。

作成された場所については、一番最後のメッセージを参照してください。また、エラーが発生した場合など、実行のログは outlog.txtという名前で保存されています。

必要に応じて参照してください。

# Arduino APIから使用する

## ライブラリの入手
もし、自分でビルドした場合、makeArduinoLib.bat　を実行して生成されたzipファイルを用意します。

すでにビルド済みのライブラリを使用する場合、githubの[KNJGfx9341ページ](https://github.com/HisayukiNomura/KNJGfx9341)に行き、右にあるReleaseをクリック、リリースページから Assets > KNJGfx9341_ARDUINO_ONLY.zip　をダウンロードします。
<img width=60% alt="1.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/95467549-4f53-4dd1-a429-6f69db74c134.png">

## ライブラリのインストール

Arduino IDEを開き、デバイスが Raspberry Pi Pico　（もしくはその系統）になっていることを確認します。

<img width=80% alt="1.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/c20d524f-ac6a-41a8-b182-7f799c2deba1.png">

スケッチ >　ライブラリをインクルード > .zip形式からのライブラリをインストール…　を選択します。

<img width=80% alt="1.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/841485e1-26e8-4332-a8f8-b510abe8acb0.png">

ファイルダイアログが開くので、ダウンロードした zip ファイルを選択します。

<img width=80% alt="1.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/2e96e9e5-bb19-44ef-b66a-806df3462b19.png">

ライブラリの読み込みが成功すると、出力ウインドウに Library Installed　と表示されます。

<img width=80% alt="1.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/e23f25af-7691-4e57-a01f-956da46f8a24.png">

## サンプルスケッチの実行

ファイル > スケッチ例　> KanjiGFX-9341 > Test を選択します。

<img width=80% alt="1.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/c20f51d5-af5b-437e-b92e-fb3c91ff27b1.png">

新しいウインドウが開き、漢字を表示するプログラムが読み込まれます。

<img width=80% alt="1.png" src="https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/9d77faa6-f9da-4d27-b455-608d14714a39.png">

このサンプルプログラムは、画面に４種類の大きさで「こんにちは！」と表示します。


![15.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/2096509/cc857743-afd1-4bd7-8744-b358cfd642fb.png)


より複雑な機能の使用例は、ファイル > スケッチ例　> KanjiGFX-9341 > All に含まれています。（内容は本ページにあるビデオと同様です。




# 付録

## ライブラリリファレンス

※このライブラリリファレンスはChatGPTにソースコードを読み込ませて作成しました。間違ている可能性もありますが、私の責任ではありません。

## オフラインドキュメントのダウンロード
[githubのリリースページ](https://github.com/HisayukiNomura/KNJGfx9341/releases)から、ドキュメントがダウンロードできます。リリースページのAsset、document_unzip_and_open_index.html.zipをダウンロード、解凍して index.htmlを開いてください。


## クラスの階層構造

以下の図は、Print、Adafruit_GFX、Adafruit_SPITFT、Adafruit_ILI9341、GFXcanvas16 などの主要なクラスの継承関係を示しています。

```
Print
└── Adafruit_GFX
    ├── Adafruit_SPITFT
    │   └── Adafruit_ILI9341
    └── GFXcanvas16
```

- Print はテキスト出力のための基底クラスで、print()やprintln()などのメソッドを提供します。
- Adafruit_GFX はグラフィック描画の基底クラスで、Printを継承し、点・線・矩形・円・ビットマップ描画などのグラフィックAPIを提供します。
- Adafruit_SPITFT はSPIやパラレル接続のディスプレイ制御用の中間クラスで、Adafruit_GFXを継承し、さらにハードウェア制御や高速描画機能を追加します。
- Adafruit_ILI9341 は実際のディスプレイドライバクラスで、Adafruit_SPITFTを継承し、ILI9341コントローラに特化した初期化や描画処理を実装しています。
- GFXcanvas16 は16ビットカラーのソフトウェアキャンバスで、Adafruit_GFXを継承し、メモリ上でグラフィック描画を行うためのバッファを持ちます。


通常、ユーザーが直接インスタンス化して利用するのは Adafruit_ILI9341 や GFXcanvas16 などの末端クラスです。
これらのクラスは、親クラスである Adafruit_GFX のすべてのグラフィックAPIや Print のテキスト出力APIをそのまま利用できるため、統一的なインターフェースでディスプレイ描画やテキスト出力が可能です。


## Adafruit_ILI9341 クラス

| 分類             | 関数・メンバ名                                                                 | 説明・用途                                                                                   |
|:-----------------|:------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------|
| コンストラクタ    | Adafruit_ILI9341()                                                            | デフォルトコンストラクタ（MicroPython等向け）                                                |
|   | Adafruit_ILI9341(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst = -1, int8_t miso = -1) | ソフトウェアSPI用コンストラクタ                                                              |
|   | Adafruit_ILI9341(int8_t cs, int8_t dc, int8_t rst = -1)                      | ハードウェアSPI（デフォルトSPI）用コンストラクタ                                              |
|  | Adafruit_ILI9341(SPIClass *spiClass, int8_t dc, int8_t cs, int8_t rst = -1)  | ハードウェアSPI（任意SPI）用コンストラクタ                                                    |
|   | Adafruit_ILI9341(tftBusWidth busWidth, int8_t d0, int8_t wr, int8_t dc, int8_t cs = -1, int8_t rst = -1, int8_t rd = -1) | パラレルインターフェース用コンストラクタ                                                     |
| 初期化・設定      | void begin(uint32_t freq = 0)                                                 | ディスプレイ初期化、SPIクロック周波数指定可                                                  |
|    | void constructObject(int8_t cs, int8_t dc, int8_t mosi, int8_t sck, int8_t rst, int8_t miso) | MicroPython等での後付け初期化（ソフトSPI用）                                                 |
|    | void constructObject(SPIClass *spiClass, int8_t dc, int8_t cs, int8_t rst)    | MicroPython等での後付け初期化（ハードSPI用）                                                 |
| 基本操作          | void setRotation(uint8_t m)                                                   | 画面の回転（0～3）                                                                           |
|       | void invertDisplay(bool invert)                                               | 表示色の反転                                                                                 |
|      | void displaySleep(bool enterSleep)                                            | スリープモード切替                                                                           |
|        | void scrollTo(uint16_t y)                                                     | 縦スクロール位置設定                                                                         |
|         | void setScrollMargins(uint16_t top, uint16_t bottom)                          | スクロールマージン設定                                                                       |
| 描画   | void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)            | 書き込みウィンドウ設定                                                                        |
|   | void drawChar(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmpData, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y) | 高速文字描画（ウィンドウ利用時は最適化）                                                     |
|    | void useWindowMode(bool enable)                                               | ウィンドウモードの有効/無効切り替え                                                          |
| ガンマ・IF設定    | void setGamma(ILI9341_GAMMA a_gamma)                                          | ガンマカーブ設定                                                                             |
|    | void set3DGammaEnable(bool onOff)                                             | 3Dガンマ有効/無効                                                                            |
|    | void setPositiveGamma(uint8_t *val)                                            | ポジティブガンマ補正値設定（15バイト配列）                                                   |
|     | void setNegativeGamma(uint8_t *val)                                            | ネガティブガンマ補正値設定（15バイト配列）                                                   |
|   | void setIFControl(ILI9341_IFCTRL_WEMODE weMode, uint8_t EPF, uint8_t MDT, ILI9341_IFCTRL_ENDIAN endian, ILI9341_IFCTRL_DM dmMode, ILI9341_IFCTRL_RM rmMode, ILI9341_IFCTRL_RIM rimMode) | インターフェース制御レジスタ設定                                                             |
| レジスタアクセス  | uint8_t readcommand8(uint8_t commandByte, uint8_t index = 0)                  | レジスタから8bit値を取得                                                                      |

## Adafruit_SPITFT クラス


| 分類             | 関数・メンバ名                                                                 | 説明・用途                                                                                   |
|:-----------------|:------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------|
| **コンストラクタ** | `Adafruit_SPITFT(...)`                                                        | SPI/パラレル/ソフトSPI各種インターフェース用の複数のコンストラクタ                           |
| **初期化**       | `void begin(uint32_t freq) = 0`                                                | ディスプレイ固有の初期化（純粋仮想）                                                         |
|                  | `void constructObject(...)`                                                    | デフォルトコンストラクタ後の初期化                                                            |
| **アドレス設定** | `void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0`       | 書き込みウィンドウ設定（純粋仮想）                                                            |
| **SPI制御**      | `void initSPI(uint32_t freq, uint8_t spiMode)`                                 | SPI初期化                                                                                     |
|                  | `void setSPISpeed(uint32_t freq)`                                              | SPI速度設定                                                                                   |
| **トランザクション** | `void startWrite()` / `void endWrite()`                                      | SPI/パラレルトランザクション開始・終了                                                        |
| **コマンド送信** | `void sendCommand(...)` / `void sendCommand16(...)`                            | コマンド送信                                                                                  |
|                  | `uint8_t readcommand8(...)`                                                    | 8bitコマンド読出し                                                                            |
|                  | `uint16_t readcommand16(...)`                                                  | 16bitコマンド読出し                                                                           |
| **ピクセル描画** | `void writePixel(int16_t x, int16_t y, uint16_t color)`                        | ピクセル書き込み                                                                              |
|                  | `void writePixels(uint16_t* colors, uint32_t len, ...)`                        | ピクセル配列書き込み                                                                          |
|                  | `void writeColor(uint16_t color, uint32_t len)`                                | 同色連続書き込み                                                                              |
|                  | `void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)` | 矩形塗りつぶし                                                                              |
|                  | `void writeFastHLine(...)` / `void writeFastVLine(...)`                        | 高速水平/垂直線描画                                                                           |
|                  | `void writeFillRectPreclipped(...)`                                            | クリッピング済み矩形塗りつぶし                                                               |
| **DMA**          | `void dmaWait()`                                                               | DMA転送待機                                                                                   |
|                  | `bool dmaBusy() const`                                                         | DMA転送中か判定                                                                               |
|                  | `void swapBytes(uint16_t* src, uint32_t len, uint16_t* dest = NULL)`           | バイトスワップ                                                                                |
| **高レベル描画** | `void drawPixel(...)` / `void fillRect(...)`                                   | ピクセル・矩形描画                                                                            |
|                  | `void drawFastHLine(...)` / `void drawFastVLine(...)`                          | 高速水平/垂直線描画                                                                           |
|                  | `void pushColor(uint16_t color)`                                               | 1ピクセル描画（非推奨）                                                                       |
| **ビットマップ描画** | `void drawRGBBitmap(int16_t x, int16_t y, uint16_t* pcolors, int16_t w, int16_t h)` | 16bitカラー転送                                                                         |
|                  | `void drawRGBBitmap(int16_t x, int16_t y, uint8_t* pcolors, int16_t w, int16_t h)` | 8bitカラー転送                                                                          |
|                  | `void drawBWBitmap(int16_t x, int16_t y, uint8_t* pbits, int16_t w, int16_t h)` | 1bitビットマップ転送                                                                     |
|                  | `void drawRGBBitmap(int16_t x, int16_t y, GFXcanvas16* canvas)`                | GFXcanvas16ラッパー                                                                          |
|                  | `void drawRGBBitmap(int16_t x, int16_t y, GFXcanvas8* canvas)`                 | GFXcanvas8ラッパー                                                                           |
|                  | `void drawRGBBitmap(int16_t x, int16_t y, GFXcanvas1* canvas)`                 | GFXcanvas1ラッパー                                                                           |
|                  | `void drawDMABitmap(const uint16_t* pcolors)`                                  | DMAによるビットマップ転送                                                                     |
| **その他**       | `void invertDisplay(bool i)`                                                   | 反転表示                                                                                      |
|                  | `uint16_t color565(uint8_t r, uint8_t g, uint8_t b)`                           | RGB→16bitカラー変換                                                                           |
| **低レベル制御** | `void spiWrite(uint8_t b)` / `void writeCommand(uint8_t cmd)`                  | SPIバイト/コマンド送信                                                                       |
|                  | `uint8_t spiRead(void)` / `uint16_t read16(void)`                              | SPIバイト/16bit読出し                                                                         |
|                  | `void write16(uint16_t w)` / `void writeCommand16(uint16_t cmd)`               | 16bitデータ/コマンド送信                                                                      |
|                  | `void SPI_WRITE16(uint16_t w)` / `void SPI_WRITE32(uint32_t l)`                | 16/32bitデータ送信                                                                            |
|                  | `void SPI_CS_HIGH()` / `void SPI_CS_LOW()`                                     | CSピン制御                                                                                    |
|                  | `void SPI_DC_HIGH()` / `void SPI_DC_LOW()`                                     | DCピン制御                                                                                    |

---

## Adafruit_GFX クラス

| 分類             | 関数・メンバ名                                                                 | 説明・用途                                                                                   |
|:-----------------|:------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------|
| コンストラクタ    | Adafruit_GFX()                                                                | デフォルトコンストラクタ                                                                      |
|     | Adafruit_GFX(int16_t w, int16_t h)                                            | 幅と高さを指定してグラフィックス基底クラスを生成                                              |
| 初期化           | void constructObject(int16_t w, int16_t h)                                     | オブジェクトの再初期化                                                                        |
| トランザクション | virtual void startWrite(void)                                                | 描画トランザクション開始                                                                      |
|  | virtual void endWrite(void)                                                  | 描画トランザクション終了                                                                      |
| 基本描画     | virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0               | 指定座標にピクセルを描画（純粋仮想）                                                          |
|  | virtual void writePixel(int16_t x, int16_t y, uint16_t color)                | ピクセル書き込み                                                                              |
|  | virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) | 矩形塗りつぶし                                                                      |
|  | virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) | 垂直線描画                                                                                    |
|  | virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) | 水平線描画                                                                                    |
|  | virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) | 線分描画                                                                              |
| 画面制御         | virtual void setRotation(uint8_t r)                                            | 画面の回転設定                                                                               |
|          | virtual void invertDisplay(bool i)                                             | 表示色の反転                                                                                 |
| 図形描画             | virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)    | 高速垂直線描画                                                                               |
|              | virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)    | 高速水平線描画                                                                               |
|              | virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) | 矩形塗りつぶし                                                                          |
|              | virtual void fillScreen(uint16_t color)                                        | 画面全体を塗りつぶし                                                                         |
|              | virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) | 線分描画                                                                              |
|              | virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) | 矩形枠描画                                                                             |
|              | void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)             | 円描画                                                                                       |
|              | void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) | 円の一部描画                                                                       |
|              | void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)             | 円塗りつぶし                                                                                 |
|              | void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) | 円の一部塗りつぶし                                                               |
|              | void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) | 三角形描画                                                                         |
|              | void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) | 三角形塗りつぶし                                                               |
|              | void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color) | 角丸矩形描画                                                                |
|              | void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color) | 角丸矩形塗りつぶし                                                        |
|              | void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) | 1bitビットマップ描画                                                        |
| ビットマップ | void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg) | 1bitビットマップ描画（背景色指定）                                 |
|              | void drawBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h, uint16_t color) | 1bitビットマップ描画（ポインタ）                                                |
|              | void drawBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) | 1bitビットマップ描画（ポインタ・背景色指定）                         |
|              | void drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) | XBM形式ビットマップ描画                                                      |
|              | void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h) | 8bitグレースケールビットマップ描画                                             |
|              | void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h) | 8bitグレースケールビットマップ描画（ポインタ）                                 |
|              | void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], const uint8_t mask[], int16_t w, int16_t h) | マスク付きグレースケールビットマップ描画                        |
|              | void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t* bitmap, uint8_t* mask, int16_t w, int16_t h) | マスク付きグレースケールビットマップ描画（ポインタ）             |
|              | void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) | 16bitカラーRGBビットマップ描画                                                |
|              | void drawRGBBitmap(int16_t x, int16_t y, uint16_t* bitmap, int16_t w, int16_t h) | 16bitカラーRGBビットマップ描画（ポインタ）                                     |
|              | void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], const uint8_t mask[], int16_t w, int16_t h) | マスク付き16bitカラーRGBビットマップ描画                      |
|              | void drawRGBBitmap(int16_t x, int16_t y, uint16_t* bitmap, uint8_t* mask, int16_t w, int16_t h) | マスク付き16bitカラーRGBビットマップ描画（ポインタ）           |
|              | void drawRGBBitmap(int16_t x, int16_t y, uint16_t* pcolors, int16_t w, int16_t h, uint16_t colorTransparent) | 透過色指定付き16bitカラーRGBビットマップ描画                |
|              | void drawRGBBitmap(int16_t x, int16_t y, const uint16_t* pcolors, int16_t w, int16_t h, uint16_t colorTransparent) | 透過色指定付き16bitカラーRGBビットマップ描画（const）      |
|              | void drawRGBBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h) | 8bitカラーRGBビットマップ描画                                                |
|              | void drawRGBBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h) | 8bitカラーRGBビットマップ描画（ポインタ）                                     |
|文字描画             | void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) | 文字描画（等倍）                                                                |
| | void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y) | 文字描画（拡大対応）                                             |
| | virtual void drawChar(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t* bmpData, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y) | 任意サイズ・ビットマップデータによる文字描画（仮想）         |
| | using Print::write                                                              | Printクラスのwrite関数を継承                                                                      |
|テキスト         | void getTextBounds(const char* string, int16_t x, int16_t y, int16_t* x1, int16_t* y1, uint16_t* w, uint16_t* h) | テキスト描画範囲取得                                                        |
|          | void getTextBounds(const __FlashStringHelper* s, int16_t x, int16_t y, int16_t* x1, int16_t* y1, uint16_t* w, uint16_t* h) | テキスト描画範囲取得（Flash文字列）                                 |
|          | void getTextBounds(const String& str, int16_t x, int16_t y, int16_t* x1, int16_t* y1, uint16_t* w, uint16_t* h) | テキスト描画範囲取得（String）                                   |
|          | void setTextSize(uint8_t s)                                                    | テキストサイズ設定（等倍）                                                                       |
|          | void setTextSize(uint8_t sx, uint8_t sy)                                       | テキストサイズ設定（拡大）                                                                        |
|          | void setFont(const GFXfont* f = NULL)                                          | フォント設定                                                                                     |
|          | void setFont(const KanjiData* a_pKanjiData, const uint8_t* a_pBmpData)         | 漢字フォント設定                                                                                 |
|          | void setCursor(int16_t x, int16_t y)                                           | テキストカーソル位置設定                                                                         |
|          | void setTextColor(uint16_t c)                                                  | テキスト色・背景色を同じ色に設定（透過）                                                         |
|          | void setTextColor(uint16_t c, uint16_t bg)                                     | テキスト色・背景色を個別に設定                                                                   |
|          | void setBackgroundColor(uint16_t bg)                                           | テキストの背景色のみ設定                                                                         |
|          | void setTextWrap(bool w)                                                       | テキストの自動折り返し設定                                                                       |
|          | void cp437(bool x = true)                                                      | CP437互換文字セットの有効/無効                                                                   |
|          | int16_t getCursorX(void) const                                                 | テキストカーソルのX座標取得                                                                      |
|          | int16_t getCursorY(void) const                                                 | テキストカーソルのY座標取得                                                                      |
| 情報取得         | int16_t width(void) const                                                      | 現在の回転を考慮した画面幅取得                                                                   |
|          | int16_t height(void) const                                                     | 現在の回転を考慮した画面高さ取得                                                                 |
|          | uint8_t getRotation(void) const                                                | 現在の回転設定取得                                                                              |

---

## GFXcanvas16クラス

| 分類         | 関数・メンバ名                           | 説明・用途                                                                                      |
|--------------|------------------------------------------|-----------------------------------------------------------------------------------------------|
| コンストラクタ/デストラクタ | GFXcanvas16(), GFXcanvas16(uint16_t w, uint16_t h, bool allocate_buffer), ~GFXcanvas16() | 16ビットカラーキャンバスの生成・破棄。内部バッファの確保・解放を行う。                         |
| コピー       | GFXcanvas16(const GFXcanvas16* pSrc, bool allocate_buffer) | 他のGFXcanvas16インスタンスから内容をコピーする。                                              |
| 初期化       | constructObject(uint16_t w, uint16_t h, bool allocate_buffer) | デフォルトコンストラクタ後の初期化・バッファ確保。                                             |
| 破棄         | destructObject()                         | バッファの解放とポインタのリセット。                                                           |
| バッファ操作 | getBuffer()                              | 内部バッファ（uint16_t*）へのポインタを取得。                                                  |
|  | deepCopy(const GFXcanvas16* src)         | 他のGFXcanvas16の内容を自身にコピー。サイズ不一致時はNULL。                                    |
| 描画         | drawPixel(int16_t x, int16_t y, uint16_t color) | 指定座標にピクセルを描画。回転設定に応じて座標変換。                                           |
|          | fillScreen(uint16_t color)               | キャンバス全体を指定色で塗りつぶす。                                                           |
|          | drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) | 垂直線を高速に描画（回転・クリッピング対応）。                                                 |
|          | drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) | 水平線を高速に描画（回転・クリッピング対応）。                                                 |
|          | drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color) | バッファ座標系で垂直線を高速描画（座標変換・クリッピングなし）。                               |
|          | drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color) | バッファ座標系で水平線を高速描画（座標変換・クリッピングなし）。                               |
|          | copyRGBBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t* picBuf, uint16_t bufWidth, uint16_t bufHeight) | 指定位置に16ビットRGB565ビットマップをコピー。                                                 |
| ピクセル取得 | getPixel(int16_t x, int16_t y) const     | 指定座標のピクセル色を取得（回転対応）。                                                       |
|  | getRawPixel(int16_t x, int16_t y) const  | バッファ座標系（回転なし）のピクセル色を取得。                                                 |
| 色指定       | useTransparentColor(uint16_t color)       | 指定色を背景色（透過色）として設定。                                                           |
|        | unUseTransparentColor()                   | 背景色（透過色）設定を無効化。                                                                |
| ヘルパー関数 | byteSwap()                               | バッファ内の全ピクセルのエンディアンを反転。                                                   |
| メンバ変数   | buffer                                   | ピクセルデータ格納用のuint16_t配列。                                                          |
| メンバ変数   | buffer_owned                             | バッファの所有・解放管理フラグ。                                                              |
| メンバ変数   | isBackground, bckColor                   | 透過色（背景色）の有効/無効フラグと色値。                                                     |

## printクラス

| 分類         | 関数・メンバ名                                 | 説明・用途                                                                                      |
|--------------|-----------------------------------------------|-----------------------------------------------------------------------------------------------|
| コンストラクタ/デストラクタ | Print() / ~Print()                             | Printクラスの生成・破棄。通常は派生クラスから呼び出される。                                    |
| 出力         | write(uint8_t c)                              | 1バイト（文字）を出力。純粋仮想関数で、派生クラスで実装が必要。                               |
| 出力         | write(const uint8_t *buffer, size_t size)     | バッファの内容を連続して出力。デフォルト実装はwrite(uint8_t)を繰り返し呼び出す。               |
| 出力         | print(...)                                    | 文字列・数値・浮動小数点・Printableオブジェクトなどを出力。                                    |
| 出力         | println(...)                                  | printの出力後に改行コードを追加して出力。                                                      |
| 出力         | printf(const char* format, ...)               | printf形式でフォーマットした文字列を出力。                                                     |
| 出力         | KanjiMode(bool enable)                        | 漢字モードの有効/無効を切り替える（日本語描画対応拡張）。                                     |
| 出力         | flush()                                       | 出力バッファをフラッシュ（必要な場合のみ、デフォルトは何もしない）。                           |
| 内部         | setWriteError(int err = 1)                    | 書き込みエラー状態を設定。                                                                     |
| 内部         | getWriteError()                               | 書き込みエラー状態を取得。                                                                     |
| 内部         | clearWriteError()                             | 書き込みエラー状態をクリア。                                                                   |
| メンバ変数   | write_error                                   | 書き込みエラー状態を保持する内部変数。                                                         |

**備考**  
- Printクラスは抽象基底クラスであり、Adafruit_GFXやSerial、SDカードクラスなど多くの派生クラスで共通の出力インターフェースを提供します。  
- print/println/printfなどのAPIは、ディスプレイやシリアルなど出力先を意識せず同じ書式で利用できます。
- Printableインターフェースを実装したクラスは、print/printlnで直接出力可能です。


## 使用できるフォントの一覧

### IPAフォント
| ファイル名 | 説明 |
|------------|------|
| `ipag_24x24_ALL.inc` | IPAゴシック 24x24ピクセル、すべての漢字データ |
| `ipag_24x24_LEVEL1.inc` | IPAゴシック 24x24ピクセル、第1水準漢字データ |
| `ipag_24x24_SCHOOL.inc` | IPAゴシック 24x24ピクセル、教育用漢字データ |
| `ipam_24x24_ALL.inc` | IPA明朝 24x24ピクセル、すべての漢字データ |
| `ipam_24x24_LEVEL1.inc` | IPA明朝 24x24ピクセル、第1水準漢字データ |
| `ipam_24x24_SCHOOL.inc` | IPA明朝 24x24ピクセル、教育用漢字データ |

### 東雲フォント
| ファイル名 | 説明 |
|------------|------|
| `JF-Dot-Shinonome12_12x12_ALL.inc` | JF-Dot-Shinonome 12x12ピクセル、すべての漢字データ |
| `JF-Dot-Shinonome12_12x12_LEVEL1.inc` | JF-Dot-Shinonome 12x12ピクセル、第1水準漢字データ |
| `JF-Dot-Shinonome12_12x12_SCHOOL.inc` | JF-Dot-Shinonome 12x12ピクセル、教育用漢字データ |
| `JF-Dot-Shinonome14_14x14_ALL.inc` | JF-Dot-Shinonome 14x14ピクセル、すべての漢字データ |
| `JF-Dot-Shinonome14_14x14_LEVEL1.inc` | JF-Dot-Shinonome 14x14ピクセル、第1水準漢字データ |
| `JF-Dot-Shinonome14_14x14_SCHOOL.inc` | JF-Dot-Shinonome 14x14ピクセル、教育用漢字データ |
| `JF-Dot-Shinonome16_16x16_ALL.inc` | JF-Dot-Shinonome 16x16ピクセル、すべての漢字データ |
| `JF-Dot-Shinonome16_16x16_LEVEL1.inc` | JF-Dot-Shinonome 16x16ピクセル、第1水準漢字データ |
| `JF-Dot-Shinonome16_16x16_SCHOOL.inc` | JF-Dot-Shinonome 16x16ピクセル、教育用漢字データ |

## 美咲フォント
| ファイル名 | 説明 |
|------------|------|
| `misaki_gothic_2nd_08x08_ALL.inc` | Misaki Gothic 8x8ピクセル、すべての漢字データ |
| `misaki_gothic_2nd_08x08_LEVEL1.inc` | Misaki Gothic 8x8ピクセル、第1水準漢字データ |
| `misaki_gothic_2nd_08x08_SCHOOL.inc` | Misaki Gothic 8x8ピクセル、教育用漢字データ |

