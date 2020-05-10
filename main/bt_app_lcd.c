/* 
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tftspi.h"
#include "tft.h"
#include "touch.h"
#include "esp_spiffs.h"
#include "esp_log.h"

#include "bt_app_lcd.h"

#ifdef CONFIG_EXAMPLE_USE_WIFI

#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "esp_sntp.h"
#include "nvs_flash.h"

#endif

//==================================================================================
// configuration for testing the touch sensor

// #define DEBUG_TOUCH
// #define TEST_TOUCH

#ifdef DEBUG_TOUCH
  #define SKIP_TFT_TESTS
  #undef  TEST_TOUCH
  #undef  CONFIG_EXAMPLE_USE_WIFI
#endif

#define DEFAULT_TFT_ORIENTATION PORTRAIT

// ==========================================================
// Define which spi bus to use TFT_VSPI_HOST or TFT_HSPI_HOST
#define SPI_BUS TFT_HSPI_HOST
// ==========================================================

//=============
void bt_app_lcd_draw_ui()
{
	TFT_setRotation(PORTRAIT);
	TFT_resetclipwin();

	/* Album Art Window */
	TFT_saveClipWin();
	TFT_resetclipwin();
	TFT_setclipwin(10, 10, tft_width - 10, (tft_height*1/2) - 10);
	TFT_fillWindow(TFT_BLACK);

	TFT_jpg_image(CENTER, 0, 1, "/spiffs/images/album.jpg", NULL, 0);

	TFT_restoreClipWin();

	/* Track Info Window */
	TFT_saveClipWin();
	TFT_resetclipwin();
	TFT_setclipwin(10, (tft_height*1/2) - 10, tft_width - 10, (tft_height*3/4) - 10);
	TFT_fillWindow(TFT_BLACK);

	tft_fg = TFT_WHITE;
	tft_bg = TFT_BLACK;
	TFT_setFont(DEFAULT_FONT, NULL);
	TFT_print("Go Robot", 0, 5);
	TFT_print("Red Hot Chili Peppers - ", 0, TFT_getfontheight() + 10);

	TFT_drawLine(0, TFT_getfontheight() + 30, tft_dispWin.x2, TFT_getfontheight() + 30, (color_t){128, 128, 128});

	//TFT_setFont(FONT_7SEG, NULL);
	TFT_print("0:00", 0, TFT_getfontheight() + 40);
	TFT_print("-4:37", tft_dispWin.x2 - 47 /*TFT_getStringWidth("-4:37")*/, TFT_getfontheight() + 40); 

	TFT_restoreClipWin();

	/* Control Window */
	TFT_saveClipWin();
	TFT_resetclipwin();
	TFT_setclipwin(10, (tft_height*3/4) - 10, tft_width - 10, tft_height - 10);
	TFT_fillWindow((color_t){ 64, 64, 64 });	

	/* Play Button */
	TFT_drawCircle(((tft_dispWin.x2 - tft_dispWin.x1)/2), ((tft_dispWin.y2 - tft_dispWin.y1)/2), 25, TFT_WHITE);
	TFT_fillTriangle(((tft_dispWin.x2 - tft_dispWin.x1)/2) - 5, ((tft_dispWin.y2 - tft_dispWin.y1)/2) - 8, 
	((tft_dispWin.x2 - tft_dispWin.x1)/2) - 5, ((tft_dispWin.y2 - tft_dispWin.y1)/2) + 8, 
	((tft_dispWin.x2 - tft_dispWin.x1)/2) + 8.856, ((tft_dispWin.y2 - tft_dispWin.y1)/2), TFT_WHITE);
	

	/* Pause Button 
	TFT_drawCircle(((tft_dispWin.x2 - tft_dispWin.x1)/2), ((tft_dispWin.y2 - tft_dispWin.y1)/2), 25, TFT_WHITE);
	TFT_fillRect(((tft_dispWin.x2 - tft_dispWin.x1)/2) - 8, ((tft_dispWin.y2 - tft_dispWin.y1)/2) - 8, 4, 16, TFT_WHITE);
	TFT_fillRect(((tft_dispWin.x2 - tft_dispWin.x1)/2) + 4, ((tft_dispWin.y2 - tft_dispWin.y1)/2) - 8, 4, 16, TFT_WHITE);
	*/

	/* Next */
	TFT_fillTriangle(((tft_dispWin.x2 - tft_dispWin.x1)*3/4) - 5, ((tft_dispWin.y2 - tft_dispWin.y1)/2) - 8, 
	((tft_dispWin.x2 - tft_dispWin.x1)*3/4) - 5, ((tft_dispWin.y2 - tft_dispWin.y1)/2) + 8, 
	((tft_dispWin.x2 - tft_dispWin.x1)*3/4) + 8.856, ((tft_dispWin.y2 - tft_dispWin.y1)/2), TFT_WHITE);
	TFT_fillRect(((tft_dispWin.x2 - tft_dispWin.x1)*3/4) + 8.856, ((tft_dispWin.y2 - tft_dispWin.y1)/2) - 8, 4, 16, TFT_WHITE);

	/* Prev */
	TFT_fillTriangle(((tft_dispWin.x2 - tft_dispWin.x1)/4) + 8.856, ((tft_dispWin.y2 - tft_dispWin.y1)/2) - 8, 
	((tft_dispWin.x2 - tft_dispWin.x1)/4) + 8.856, ((tft_dispWin.y2 - tft_dispWin.y1)/2) + 8, 
	((tft_dispWin.x2 - tft_dispWin.x1)/4) - 5, ((tft_dispWin.y2 - tft_dispWin.y1)/2), TFT_WHITE);
	TFT_fillRect(((tft_dispWin.x2 - tft_dispWin.x1)/4) - 9, ((tft_dispWin.y2 - tft_dispWin.y1)/2) - 8, 4, 16, TFT_WHITE);

	TFT_restoreClipWin();
}

void bt_app_lcd_update_ui(uint8_t attr_id, uint8_t *attr_text)
{
	/* Track Info Window */
	TFT_saveClipWin();
	TFT_resetclipwin();
	TFT_setclipwin(10, (tft_height*1/2) - 10, tft_width - 10, (tft_height*3/4) - 10);
	TFT_fillWindow(TFT_BLACK);

	tft_fg = TFT_WHITE;
	tft_bg = TFT_BLACK;
	TFT_setFont(DEFAULT_FONT, NULL);
	TFT_print("Go Robot", 0, 5); //01
	TFT_print("Red Hot Chili Peppers - ", 0, TFT_getfontheight() + 10); //02 - 04

	TFT_drawLine(0, TFT_getfontheight() + 30, tft_dispWin.x2, TFT_getfontheight() + 30, (color_t){128, 128, 128});

	//TFT_setFont(FONT_7SEG, NULL);
	TFT_print("0:00", 0, TFT_getfontheight() + 40);
	TFT_print("-4:37", tft_dispWin.x2 - 47 /*TFT_getStringWidth("-4:37")*/, TFT_getfontheight() + 40); 

	TFT_restoreClipWin();


	ESP_LOGI(BT_APP_LCD_TAG, "************************ESP32 METADATA RSP EVENT************************");
    ESP_LOGI(BT_APP_LCD_TAG, "Attribute ID: 0x%x", attr_id);
	ESP_LOGI(BT_APP_LCD_TAG, "Attribute ID: %s", attr_text);
}

void bt_app_lcd_init()
{
    //test_sd_card();
    // ========  PREPARE DISPLAY INITIALIZATION  =========

    esp_err_t ret;

    // === SET GLOBAL VARIABLES ==========================

	// ===================================================
	// ==== Set maximum spi clock for display read    ====
	//      operations, function 'find_rd_speed()'    ====
	//      can be used after display initialization  ====
	tft_max_rdclock = 8000000;
	// ===================================================

    // ====================================================================
    // === Pins MUST be initialized before SPI interface initialization ===
    // ====================================================================
    TFT_PinsInit();
	TS_PinsInit();

    // ====  CONFIGURE SPI DEVICES(s)  ====================================================================================

    spi_lobo_device_handle_t spi;
	
    spi_lobo_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,				// set SPI MISO pin
        .mosi_io_num=PIN_NUM_MOSI,				// set SPI MOSI pin
        .sclk_io_num=PIN_NUM_CLK,				// set SPI CLK pin
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
		.max_transfer_sz = 6*1024,
    };
    spi_lobo_device_interface_config_t devcfg={
        .clock_speed_hz=8000000,                // Initial clock out at 8 MHz
        .mode=0,                                // SPI mode 0
        .spics_io_num=-1,                       // we will use external CS pin
		.spics_ext_io_num=PIN_NUM_CS,           // external CS pin
		.flags=LB_SPI_DEVICE_HALFDUPLEX,        // ALWAYS SET  to HALF DUPLEX MODE!! for display spi
    };

    spi_lobo_device_handle_t tsspi;

    spi_lobo_device_interface_config_t tsdevcfg={
        .clock_speed_hz=1000000,                //Clock out at 1 MHz
        .mode=STMPE610_SPI_MODE,                //SPI mode 0
        .spics_io_num=PIN_NUM_TOUCH_CS,			//Touch CS pin
		.spics_ext_io_num=-1,    				//Not using the external CS
        .flags = 0,
    };

    // ====================================================================================================================


    vTaskDelay(500 / portTICK_RATE_MS);
	printf("\r\n==============================\r\n");
    printf("TFT display DEMO, LoBo 11/2017\r\n");
	printf("==============================\r\n");
    printf("Pins used: miso=%d, mosi=%d, sck=%d, cs=%d\r\n", PIN_NUM_MISO, PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_CS);
    printf("Touch CS: %d\r\n", PIN_NUM_TOUCH_CS);
	printf("==============================\r\n\r\n");

	// ==================================================================
	// ==== Initialize the SPI bus and attach the LCD to the SPI bus ====

	ret=spi_lobo_bus_add_device(SPI_BUS, &buscfg, &devcfg, &spi);
    assert(ret==ESP_OK);
	printf("SPI: display device added to spi bus (%d)\r\n", SPI_BUS);
	tft_disp_spi = spi;

	// ==== Test select/deselect ====
	ret = spi_lobo_device_select(spi, 1);
    assert(ret==ESP_OK);
	ret = spi_lobo_device_deselect(spi);
    assert(ret==ESP_OK);

	printf("SPI: attached display device, speed=%u\r\n", spi_lobo_get_speed(spi));
	printf("SPI: bus uses native pins: %s\r\n", spi_lobo_uses_native_pins(spi) ? "true" : "false");

	// =====================================================
    // ==== Attach the touch screen to the same SPI bus ====

	ret=spi_lobo_bus_add_device(SPI_BUS, &buscfg, &tsdevcfg, &tsspi);
    assert(ret==ESP_OK);
	printf("SPI: touch screen device added to spi bus (%d)\r\n", SPI_BUS);
	tft_ts_spi = tsspi;

	// ==== Test select/deselect ====
	ret = spi_lobo_device_select(tsspi, 1);
    assert(ret==ESP_OK);
	ret = spi_lobo_device_deselect(tsspi);
    assert(ret==ESP_OK);

	printf("SPI: attached TS device, speed=%u\r\n", spi_lobo_get_speed(tsspi));

	// ================================
	// ==== Initialize the Display ====

	printf("SPI: display init...\r\n");
	TFT_display_init();
    printf("OK\r\n");
	stmpe610_Init();
	vTaskDelay(10 / portTICK_RATE_MS);
    uint32_t tver = stmpe610_getID();
    printf("STMPE touch initialized, ver: %04x - %02x\r\n", tver >> 8, tver & 0xFF);
	
	// ---- Detect maximum read speed ----
	tft_max_rdclock = find_rd_speed();
	printf("SPI: Max rd speed = %u\r\n", tft_max_rdclock);

    // ==== Set SPI clock used for display operations ====
	spi_lobo_set_speed(spi, DEFAULT_SPI_CLOCK);
	printf("SPI: Changed speed to %u\r\n", spi_lobo_get_speed(spi));

    printf("\r\n---------------------\r\n");
	printf("Graphics demo started\r\n");
	printf("---------------------\r\n");

	tft_font_rotate = 0;
	tft_text_wrap = 0;
	tft_font_transparent = 0;
	tft_font_forceFixed = 0;
	tft_gray_scale = 0;
	TFT_setGammaCurve(DEFAULT_GAMMA_CURVE);
	TFT_setRotation( DEFAULT_TFT_ORIENTATION );
	TFT_setFont(DEFAULT_FONT, NULL);
	TFT_resetclipwin();

 #ifdef CONFIG_EXAMPLE_USE_WIFI

    ESP_ERROR_CHECK( nvs_flash_init() );

    // ===== Set time zone ======
	setenv("TZ", "CET-1CEST", 0);
	tzset();
	// ==========================

	//disp_header("GET NTP TIME");

    time(&time_now);
	tm_info = localtime(&time_now);

	// Is time set? If not, tm_year will be (1970 - 1900).
    if (tm_info->tm_year < (2016 - 1900)) {
        ESP_LOGI(tag, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        tft_fg = TFT_CYAN;
    	TFT_print("Time is not set yet", CENTER, CENTER);
    	TFT_print("Connecting to WiFi", CENTER, LASTY+TFT_getfontheight()+2);
    	TFT_print("Getting time over NTP", CENTER, LASTY+TFT_getfontheight()+2);
    	tft_fg = TFT_YELLOW;
    	TFT_print("Wait", CENTER, LASTY+TFT_getfontheight()+2);
        if (obtain_time()) {
        	tft_fg = TFT_GREEN;
        	TFT_print("System time is set.", CENTER, LASTY);
        }
        else {
        	tft_fg = TFT_RED;
        	TFT_print("ERROR.", CENTER, LASTY);
        }
        time(&time_now);
    	update_header(NULL, "");
    	Wait(-2000);
    }
 #endif  // CONFIG_EXAMPLE_USE_WIFI

	//disp_header("File system INIT");
    tft_fg = TFT_CYAN;
	TFT_print("Initializing SPIFFS...", CENTER, CENTER);

    // ==== Initialize the file system ====
    const esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = "storage", // NULL
      .max_files = 5,
      .format_if_mount_failed = false
    };
    
    
    printf("\r\n\n");
	esp_vfs_spiffs_register(&conf);
    if (!esp_spiffs_mounted(conf.partition_label)) {
    	tft_fg = TFT_RED;
    	TFT_print("SPIFFS not mounted !", CENTER, LASTY+TFT_getfontheight()+2);
    }
    else {
    	tft_fg = TFT_GREEN;
    	TFT_print("SPIFFS Mounted.", CENTER, LASTY+TFT_getfontheight()+2);
    }

	//TS_test_touch();
}
