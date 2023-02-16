flash_wp_tool.exe --to-wp-bin --input flash_wp_info.csv --output wp_flash.bin
packres.exe -n test_dir -o dir_sys_info wp_flash.bin -normal
del wp_flash.bin

pause