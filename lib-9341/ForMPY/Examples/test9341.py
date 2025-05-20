#Pico/Picowではメモリが足りないので事実上フォント指定は使用できない
#import JF_Dot_Shinonome14_14x14_CUSTOM
#ビットマップデータはファイルサイズが大きいとThonnyから転送できない。また、それでも不安定なのでrshellのを使うほうが良い
#全部読み込むとメモリが足りない・・・
import Jan1
import Feb1
import Mar1
import Apr1
#import May1
#import Jun1
#import Jul1
#import Aug1
#import Sep1
#import Oct1
#import Nov1
#import Dec1
import hana1
import hana2
import KNJGfx
import random
import struct
import os
import gc
import sys
import uctypes
import time
import array
#gc.collect()
#before = gc.mem_alloc()
#gc.collect()

#after = gc.mem_alloc()
#print(f"Allocated memory: {after - before} bytes")


def printFlashSize():
    stat = os.statvfs("/")
    size = stat[1] * stat[2]
    free = stat[0] * stat[3]
    used = size - free

    KB = 1024
    MB = 1024 * 1024
    
    sizemb = size / MB
    print(f"Size : {size} bytes , {sizemb:.2f} MBytes")
    return
    print("Used : {:,} bytes, {:,} KB, {} MB".format(used, used / KB, used / MB))
    print("Free : {:,} bytes, {:,} KB, {} MB".format(free, free / KB, free / MB))

    print("Heap free : ", gc.mem_free(), "Bytes")

    
def getRandXYXY():
    return (
        random.randint(0, 320),  # 0～320の乱数
        random.randint(0, 240),  # 0～240の乱数
        random.randint(0, 320),  # 0～320の乱数
        random.randint(0, 240)   # 0～240の乱数
    )
def getRandXY():
    return (
        random.randint(0, 320),  # 0～320の乱数
        random.randint(0, 240),  # 0～240の乱数
        
    )
def getRandXYOFS(x,y):
    return (
        random.randint(0, 320-x),  # 0～320の乱数
        random.randint(0, 240-y),  # 0～240の乱数
        
    )
def getRandXYWH():
    x1, y1, x2, y2 = getRandXYXY()

    # X座標とY座標を正しい順序にする
    x1, x2 = min(x1, x2), max(x1, x2)
    y1, y2 = min(y1, y2), max(y1, y2)

    W = x2 - x1  # 幅
    H = y2 - y1  # 高さ
    return (x1, y1, W, H)
    
    
def Touch10():
    count = 0
    minx = 99999
    miny = 99999
    maxx = 0
    maxy = 0
    
    while True:
        if KNJGfx.isTouch():
            rawpos = KNJGfx.getTouchRawXYZ()
            pos = KNJGfx.getTouchXY()
            KNJGfx.fillCircle(0,(pos[0],pos[1],5),random.randint(0,0xFFFF))
            minx = rawpos[0] if (minx > rawpos[0]) else minx
            miny = rawpos[1] if (miny > rawpos[1]) else miny
            maxx = rawpos[0] if (maxx < rawpos[0]) else maxx
            maxy = rawpos[1] if (maxy < rawpos[1]) else maxy
            count = count + 1
        #print(f"{count}") 
        if count > 10 :
            return [minx,miny,maxx,maxy]
        time.sleep(0.1)    
#
# タッチパネルのキャリブレーションが必要になったら、これを呼びだして値を取得、スクリプトに反映させる
#
def calibrateTouch():
    
    KNJGfx.fillScreen(0, 0x00FF)
    KNJGfx.setCursor(0,30,100)
    KNJGfx.print(0, "Tap Upper Left corner few Times")
    rawPos = Touch10()
    #print(f"minx = {rawPos[0]}, miny = {rawPos[1]}")
    minX = rawPos[0]
    minY = rawPos[1]
    KNJGfx.fillScreen(0, 0x00FF)
    KNJGfx.setCursor(0,30,100)
    KNJGfx.print(0, "Tap Bottom right corner few Times")
    rawPos = Touch10()
    #print(f"maxx = {rawPos[2]}, maxy = {rawPos[3]}")
    maxX = rawPos[2]
    maxY = rawPos[3]

    print (f"min-max : {minX},{minY} - {maxX},{maxY}")
    KNJGfx.setTouchCalibrationValue((minX,minY,maxX,maxY))
    
printFlashSize()
    
KNJGfx.setDebugMode(1)
KNJGfx.initHW((0,20,22,18,19,21,0))
KNJGfx.setRotation(3)
time.sleep(0.1)

KNJGfx.fillScreen(0, 0x00FF)





        

KNJGfx.loadDefaultKanjiFont(0)
KNJGfx.setCursor(0,10,30)

KNJGfx.print(0,"TFTへの直接描画：各種図形など")
time.sleep(5)

for i in range(100):
    KNJGfx.drawLine(0, getRandXYXY(),random.randint(0,0xFFFF))
time.sleep(1)
for i in range(100):
    KNJGfx.drawCircle(0, getRandXY()+(random.randint(5,20),),random.randint(0,0xFFFF))
time.sleep(1)     

for i in range(100):
    KNJGfx.fillCircle(0,  getRandXY()+(random.randint(5,20),),random.randint(0,0xFFFF))
time.sleep(1)     

for i in range(100):
    KNJGfx.drawRect(0, getRandXYWH(),random.randint(0,0xFFFF))
time.sleep(1)     
for i in range(100):
    KNJGfx.fillRect(0, getRandXYWH(),random.randint(0,0xFFFF))
time.sleep(1)     

KNJGfx.fillScreen(0,0x0000)

for i in range(1000):
    KNJGfx.drawPixel(0, (random.randint(0,320),random.randint(0,240)),random.randint(0,0xFFFF))
time.sleep(5)     

KNJGfx.loadDefaultKanjiFont(0)
KNJGfx.setCursor(0,10,30)
KNJGfx.print(0,"TFTへの直接描画：文字")
time.sleep(5)


KNJGfx.setCursor(0, (0,0))
KNJGfx.fillScreen(0, 0x000F)

KNJGfx.loadDefaultAsciiFont(0)
KNJGfx.loadDefaultKanjiFont(0)
KNJGfx.setKanjiMode(0, True);
KNJGfx.print(0, "こんにちは")
KNJGfx.setKanjiMode(0, False);
KNJGfx.setCursor(0, 0,40)
KNJGfx.print(0, "Hello!")

KNJGfx.setKanjiMode(0, True);
KNJGfx.setTextWrap(0, True);
KNJGfx.setCursor(0, 0,80)
KNJGfx.setTextColor(0, 0xFFF0,0xFFF0)
#pico/picoWでは、メモリが足りないので事実上フォントは使用できない。
#漢字を使うときは、micropythonに含まれる、デフォルトフォントは使用できる。
#result = KNJGfx.setKanjiFont(JJF-Dot-Shinonome12_12x12_SCHOOL.code,JF-Dot-Shinonome12_12x12_SCHOOL.bitmap)

KNJGfx.print(0, "谷川の岸に小さな学校がありました。教室はたった一つでしたが生徒は三年生がないだけで、あとは一年から六年までみんなありました。運動場もテニスコートのくらいでしたが、すぐうしろは栗くりの木のあるきれいな草の山でしたし、運動場のすみにはごぼごぼつめたい水を噴ふく岩穴もあったのです。")
time.sleep(5) 
KNJGfx.fillScreen(0, 0x000F)

KNJGfx.loadDefaultKanjiFont(0)
KNJGfx.setCursor(0,10,30)
KNJGfx.print(0,"TFTへの直接描画：ビットマップ")
time.sleep(5)

hanafuda=[Jan1.Jan1,Feb1.Feb1,Mar1.Mar1,Apr1.Apr1]
KNJGfx.fillScreen(0,0x000F)
for i in range(100):
    randx = random.randint(0, 320 - 64)  # X座標: 0～256の範囲
    randy = random.randint(0, 240 - 105)  # Y座標: 0～135の範囲
    random_card = random.choice(hanafuda)
    KNJGfx.drawBitmap(0, (randx,randy,64,105),random_card)
time.sleep(5)


sakura=[hana1.data,hana2.data]
KNJGfx.fillScreen(0,0x000F)
for i in range(100):
    randx = random.randint(0, 320 - 76)  # X座標: 0～256の範囲
    randy = random.randint(0, 240 - 79)  # Y座標: 0～135の範囲
    random_card = random.choice(sakura)    
    KNJGfx.drawBitmap(0, (randx,randy,76,79),random_card)

time.sleep(5)
KNJGfx.fillScreen(0,0x000F)
for i in range(100):
    randx = random.randint(0, 320 - 76)  # X座標: 0～256の範囲
    randy = random.randint(0, 240 - 79)  # Y座標: 0～135の範囲
    random_card = random.choice(sakura)    
    KNJGfx.drawBitmapWithKeyColor(0, (randx,randy,76,79),random_card,0xFFFF)

KNJGfx.setKanjiMode(0, True);
KNJGfx.setTextWrap(0, True);
KNJGfx.setCursor(0, 0,80)
KNJGfx.setTextColor(0, 0x0000,0000)
KNJGfx.print(0, "透過色：桜の樹の下には屍体が埋まっている")

KNJGfx.setKanjiMode(0, True);
KNJGfx.setTextWrap(0, True);
KNJGfx.setCursor(0, 0,160)
KNJGfx.setTextColor(0, 0x0000,0x0F3d)
KNJGfx.print(0, "背景色：桜の樹の下には屍体が埋まっている")
time.sleep(5)
#########################
#  キャンバスの処理
#########################

#初期化処理。キャンバスを４枚用意して、それらを背景あり、無しで表示する
textID1 = KNJGfx.createCanvas((95,30))
KNJGfx.fillScreen(textID1,0x0000)
KNJGfx.fillRoundRect(textID1,(0,0,95,30,8),0x2200)
KNJGfx.drawRoundRect(textID1,(0,0,95,30,8),0x1100)
KNJGfx.loadDefaultKanjiFont(textID1)
KNJGfx.setKanjiMode(textID1,True);
KNJGfx.setTextColor(textID1,0xFFFF,0xFFFF)
KNJGfx.setCursor(textID1,5,8)
KNJGfx.print(textID1,"こんにちは")

textID2 = KNJGfx.createCanvas((80,30))
KNJGfx.fillScreen(textID2,0x0000)
KNJGfx.fillRoundRect(textID2,(0,0,80,30,8),0x0220)
KNJGfx.drawRoundRect(textID2,(0,0,80,30,8),0x01100)
KNJGfx.loadDefaultAsciiFont(textID2)
KNJGfx.setKanjiMode(textID2,False);
KNJGfx.setTextColor(textID2,0xFFFF,0xFFFF)
KNJGfx.setCursor(textID2,5,20)
KNJGfx.print(textID2,"Hello!")


hanaID1 = KNJGfx.createCanvas((76,79))
KNJGfx.drawBitmap(hanaID1,(0,0,76,79),hana1.data)
hanaID2 = KNJGfx.createCanvas((76,79))
KNJGfx.drawBitmap(hanaID2,(0,0,76,79),hana2.data)
time.sleep(5)


# 透過色ありでの表示
KNJGfx.fillScreen(0,0x1212)
KNJGfx.loadDefaultKanjiFont(0)
KNJGfx.setCursor(0,10,30)
KNJGfx.print(0,"キャンバスの処理:背景透過あり")
time.sleep(5)
#背景色を指定
KNJGfx.setCanvasKeyColor(textID1,0x0000)
KNJGfx.setCanvasKeyColor(textID2,0x0000)
KNJGfx.setCanvasKeyColor(hanaID1,0xFFFF)
KNJGfx.setCanvasKeyColor(hanaID2,0xFFFF)
for i in range(100):
    KNJGfx.drawBitmapFromCanvas(0,getRandXYOFS(95,30),textID2)
    KNJGfx.drawBitmapFromCanvas(0,getRandXYOFS(80,30),textID1)
for i in range(100):
    KNJGfx.drawBitmapFromCanvas(0,getRandXYOFS(76,79),hanaID1)
    KNJGfx.drawBitmapFromCanvas(0,getRandXYOFS(76,79),hanaID2)

#透過色無しでの処理
KNJGfx.fillScreen(0,0x1212)
KNJGfx.loadDefaultKanjiFont(0)
KNJGfx.setCursor(0,10,30)
KNJGfx.print(0,"キャンバスの処理:背景透過なし")
time.sleep(5)
KNJGfx.resetCanvasKeyColor(textID1)
KNJGfx.resetCanvasKeyColor(textID2)
KNJGfx.resetCanvasKeyColor(hanaID1)
KNJGfx.resetCanvasKeyColor(hanaID2)
for i in range(100):
    KNJGfx.drawBitmapFromCanvas(0,getRandXYOFS(95,30),textID2)
    KNJGfx.drawBitmapFromCanvas(0,getRandXYOFS(80,30),textID1)
for i in range(100):
    KNJGfx.drawBitmapFromCanvas(0,getRandXYOFS(76,79),hanaID1)
    KNJGfx.drawBitmapFromCanvas(0,getRandXYOFS(76,79),hanaID2)


KNJGfx.deleteCanvas(textID1)
KNJGfx.deleteCanvas(textID2)
KNJGfx.deleteCanvas(hanaID1)
KNJGfx.deleteCanvas(hanaID2)

KNJGfx.fillScreen(0,0x1212)
KNJGfx.loadDefaultKanjiFont(0)
KNJGfx.setCursor(0,10,30)
KNJGfx.print(0,"タッチパネル")

KNJGfx.initTouchHW(17,255)
KNJGfx.setTouchRotation(3)
KNJGfx.beginTouch()

# タッチパネルのキャリブレーション値を設定する。
# 購入した液晶に現状合わせの数字が入っている。タッチするポイントと
# 座標が大きくズレる用なら、calibrateTouchを呼びだして、キャリブレーションを
# 実行する。
# min-max : {minX},{minY} - {maxX},{maxY}") で、４つの値が表示されるので
# その値を指定すること。
# calibrateTouch()
KNJGfx.setTouchCalibrationValue((432,374,3773,3638))
while True:
    if KNJGfx.isTouch():
        pos = KNJGfx.getTouchXY()
        
        KNJGfx.fillCircle(0,(pos[0],pos[1],5),0xFFFF)
    time.sleep(0.1)

