#ifndef OLED_H
#define OLED_H

#include "regs.h"   

#include "i2c.h"
#include "font.h"
#include "resolution.h"
#include "framebuffer.h"

bool oled_init(i2c_handle_t *i2c);

bool oled_refresh(i2c_handle_t *i2c);

#endif // OLED_H
