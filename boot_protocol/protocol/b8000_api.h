#ifndef protocol_b8000_api
#define protocol_b8000_api

#ifdef __cplusplus
extern "C" {
#endif

#ifndef protocol_util
#include "../util.h"
#endif

#ifndef protocol_types
#include "../types.h"
#endif

#define rows        25
#define cols        80

#ifndef protocol_cursor
#include "keyboard_driver/cursor.h"
#endif

/* This is here so `keyboard.h` can use `put_char` and possible `print`. */
extern void put_char(uint8 c);
extern void print(const uint8 *str, uint8 endc);

#ifndef protocol_keyboard
#include "keyboard_driver/keyboard.h"
#endif

#ifndef protocol_printing
#include "keyboard_driver/printing.h"
#endif

#ifdef __cplusplus
}
#endif

#endif