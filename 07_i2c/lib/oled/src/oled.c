#include "oled.h"

#include <stdio.h>

#include "framebuffer.h"

#define SSD1306_ADDR 0x3C

// Control bytes
#define COMMAND_MODE 0x00
#define SINGLE_COMMAND_MODE 0x80
#define DATA_MODE 0x40

// Fundamental Command
#define DISPLAY_LINE_START 0x40
#define CONTRAST_CONTROL 0x81
#define DISABLE_ENTIRE_DISPLAY 0xA4
#define DISPLAY_ON_REG02 0xA5
#define NORMAL_DISPLAY 0xA6
#define DISPLAY_INVERSE  0xA7
#define DISPLAY_RESET 0xAE
#define DISPLAY_ON 0xAF

// Addressing Setting
#define MEM_ADDR_MODE 0x20
#define HORIZONTAL_ADDR_MODE 0x00
#define VERTICAL_ADDR_MODE 0x01
#define PAGE_ADDR_MODE 0x02
#define LOWER_COL_START_ADDR 0x00
#define HIGHER_COL_START_ADDR 0x10
#define PAGE_START_ADDR 0xB0
#define SET_COLUMN_ADDR 0x21
#define SET_PAGE_ADDR 0x22

// Scrolling
#define DEACTIVATE_SCROLL 0x2E
#define ACTIVATE_SCROLL 0x2F
#define VERTICAL_AND_RIGHT_HOR_SCROLL 0x29
#define DUMMY_BYTE 0x00
#define SIX_FRAMES_PER_SEC 0x00
#define VERTICAL_OFFSET_ONE 0x01

// Hardware Config
#define DISPLAY_START_LINE 0x40
#define SEGMENT_REMAP_NORMAL 0xA0
#define SEGMENT_REMAP_INVERSE 0xA1
#define MULTIPLEX_RATIO 0xA8
#define COM_OUTPUT_SCAN_DIR_NORMAL 0xC0
#define COM_OUTPUT_SCAN_DIR_REMAP 0xC8
#define DISPLAY_OFFSET 0xD3
#define COM_PINS_HARDWARE_CONFIG 0xDA

// Timing & Driving Scheme
#define DISPLAY_CLK_RATIO 0xD5
#define PRE_CHARGE_PER 0xD9
#define VCOMH_DESELECT_LEVEL 0xDB
#define NOOPERATION 0xE3

// Charge Pump
#define CHARGE_PUMP_SET 0x8D

bool oled_init(i2c_handle_t *i2c){
        if(!i2c_start(i2c)) return false;
        
        i2c_write_byte(i2c, (SSD1306_ADDR << 1) | I2C_MASTER_WRITE);

        /* Command stream mode */
        i2c_write_byte(i2c, COMMAND_MODE);

        /* Turn the display off during configuration */
        i2c_write_byte(i2c, DISPLAY_RESET);

        /* Set the horizontal addressing mode */
        i2c_write_byte(i2c, MEM_ADDR_MODE);
        i2c_write_byte(i2c, HORIZONTAL_ADDR_MODE);

        /* Zero column and page pointers */
        i2c_write_byte(i2c, SET_COLUMN_ADDR);
        i2c_write_byte(i2c, 0);
        i2c_write_byte(i2c, DISPLAY_WIDTH - 1);

        i2c_write_byte(i2c, SET_PAGE_ADDR);
        i2c_write_byte(i2c, 0);
        i2c_write_byte(i2c, DISPLAY_PAGES - 1);

        // Mux Ratio
        i2c_write_byte(i2c, MULTIPLEX_RATIO);
        i2c_write_byte(i2c, 0x3F);

        // Set display offset
        i2c_write_byte(i2c, DISPLAY_OFFSET);
        i2c_write_byte(i2c, 0x00);

        // Set display line start
        i2c_write_byte(i2c, DISPLAY_LINE_START);
        i2c_write_byte(i2c, 0x00);

        // Set Segment re-map
        i2c_write_byte(i2c, SEGMENT_REMAP_INVERSE);

        // Set COM output scan dir
        i2c_write_byte(i2c, COM_OUTPUT_SCAN_DIR_REMAP);

        // Set COM pins hardware config
        i2c_write_byte(i2c, COM_PINS_HARDWARE_CONFIG);
        i2c_write_byte(i2c, 0x12);

        // Set contrast Control
        i2c_write_byte(i2c, CONTRAST_CONTROL);
        i2c_write_byte(i2c, 0xCF);

        // Disable entire display
        i2c_write_byte(i2c, DISABLE_ENTIRE_DISPLAY);

        // Set normal display
        i2c_write_byte(i2c, NORMAL_DISPLAY);

        // Set OSC frequency
        i2c_write_byte(i2c, DISPLAY_CLK_RATIO);
        i2c_write_byte(i2c, 0x80);

        // Enable charge pump regulator
        i2c_write_byte(i2c, CHARGE_PUMP_SET);
        i2c_write_byte(i2c, 0x14);

        // Display on
        i2c_write_byte(i2c, DISPLAY_ON);

        i2c_stop(i2c);

        framebuffer_clear();
        oled_refresh(i2c);

        return true;
}

bool oled_refresh(i2c_handle_t *i2c) {
    for(uint8_t page = 0; page < DISPLAY_PAGES; page++) {
    	if(!i2c_start(i2c)) {
            return false;
        }

    	i2c_write_byte(i2c, (SSD1306_ADDR << 1) | I2C_MASTER_WRITE);

        i2c_write_byte(i2c, SINGLE_COMMAND_MODE);
    	i2c_write_byte(i2c, (PAGE_START_ADDR | page));

    	i2c_write_byte(i2c, DATA_MODE);
        for(uint8_t col = 0; col < DISPLAY_WIDTH; col++) {
    	    i2c_write_byte(i2c, display_content[page][col]);
        }
        
    	i2c_stop(i2c);
    }

    return true;
}
