/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __BT_APP_LCD_H__
#define __BT_APP_LCD_H__

#define BT_APP_LCD_TAG      "BT_APP_LCD"

void bt_app_lcd_draw_ui();
void bt_app_lcd_update_ui(uint8_t attr_id, uint8_t *attr_text);
void bt_app_lcd_init();

#endif
