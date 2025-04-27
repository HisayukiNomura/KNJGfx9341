
// Arduinoのmainなどで定義されたり行われる処理を代替するためのコード
#ifndef ARDUINO
// このライブラリが、FreeRTOS下で実行されているかを示すフラグ。
/// @brief このライブラリが、FreeRTOS下で実行されているかを示すフラグ。
/// @details Arduino の Raspberry PI Coreでは、main.cpp (packages\rp2040\hardware\rp2040\4.5.1\cores\rp2040\main.cpp)で、実体として定義されている。
///          主にMutex管理において、FreeRTOSのMutexを使用するか、RP2040のMutexを使用するかを決定するために、_freertos.cppで使用される。
///          FreeRTOSのMutexを使用する場合は、__freertos_mutex_create()が呼ばれるが、FreeRTOSのMutexを使用しない場合は、RP2040のMutexを使用する。
///          現実的に、使われているのがwiring_private.cpp位なので、削除してしまおうかとも思ったが、やっていることも明確なので残すことにする。
volatile bool __isFreeRTOS = false;
/// @brief FreeRTOSが初期化されたかどうかを示すフラグ。
/// @details Arduino の Raspberry PI Coreでは、main.cpp (packages\rp2040\hardware\rp2040\4.5.1\cores\rp2040\main.cpp)で、実体として定義されている。
///          設定は、\hardware\rp2040\4.5.1\libraries\FreeRTOS\src\variantHooks.cpp内で行われる。
///          非Arduino 環境でFreeRTOSを使用する場合は、この辺を見ていく必要があるだろう。ただTFT液晶に関して言えば、Mutexについては割り込み禁止/許可位しか使用していないので、あまり関係ないかもしれない。
volatile bool __freeRTOSinitted;
#endif