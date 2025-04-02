#pragma once
#include "../core/Common.h"
extern "C" void __pinMode(pin_size_t ulPin, PinMode ulMode);
extern "C" void __digitalWrite(pin_size_t ulPin, PinStatus ulVal);
extern "C" PinStatus __digitalRead(pin_size_t ulPin);
