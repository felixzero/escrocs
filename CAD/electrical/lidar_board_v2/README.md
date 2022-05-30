# Lidar board (v2)

This board can be used to control and perform absolute localization using a Xiaomi Roborock vacuum cleaner replacement part. It comprises of the "adapter", and edge insertion PCB fitted to the lidar connector and the board by itself.

Its firmware is in the `code/lidar_board` folder.

## Known issues

Several issues:
- Missing pull-up on the I2C line (requires either strong pull-ups on other boards or soldering ~1k resistors between Vcc and SDA/SCL) ;
- Unconnected power jack: solder a thick wire between positive pad of the barrel jack connector and pad 1 of the lidar connector.