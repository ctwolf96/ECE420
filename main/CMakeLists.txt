file(GLOB SOURCES *.c)
idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}
                       REQUIRES nvs_flash
								tft	
								touch
								spiffs
								bt)
								
spiffs_create_partition_image(storage ../components/spiffs_image FLASH_IN_PROJECT)