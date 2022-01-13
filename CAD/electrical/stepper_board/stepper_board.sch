EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:GND #PWR076
U 1 1 61A9C66A
P 4950 5350
F 0 "#PWR076" H 4950 5100 50  0001 C CNN
F 1 "GND" H 4955 5177 50  0000 C CNN
F 2 "" H 4950 5350 50  0001 C CNN
F 3 "" H 4950 5350 50  0001 C CNN
	1    4950 5350
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR075
U 1 1 61AA065C
P 5000 2300
F 0 "#PWR075" H 5000 2150 50  0001 C CNN
F 1 "+5V" H 5015 2473 50  0000 C CNN
F 2 "" H 5000 2300 50  0001 C CNN
F 3 "" H 5000 2300 50  0001 C CNN
	1    5000 2300
	1    0    0    -1  
$EndComp
$Comp
L Device:Crystal Y1
U 1 1 61AA7437
P 10050 1100
F 0 "Y1" H 10050 1368 50  0000 C CNN
F 1 "Crystal" H 10050 1277 50  0000 C CNN
F 2 "Crystal:Crystal_SMD_HC49-SD" H 10050 1100 50  0001 C CNN
F 3 "~" H 10050 1100 50  0001 C CNN
	1    10050 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:C C20
U 1 1 61AA899B
P 9750 1350
F 0 "C20" H 9865 1396 50  0000 L CNN
F 1 "22p" H 9865 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 9788 1200 50  0001 C CNN
F 3 "~" H 9750 1350 50  0001 C CNN
	1    9750 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C24
U 1 1 61AA9768
P 10350 1350
F 0 "C24" H 10465 1396 50  0000 L CNN
F 1 "22p" H 10465 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 10388 1200 50  0001 C CNN
F 3 "~" H 10350 1350 50  0001 C CNN
	1    10350 1350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR066
U 1 1 61AAA871
P 9750 1500
F 0 "#PWR066" H 9750 1250 50  0001 C CNN
F 1 "GND" H 9755 1327 50  0000 C CNN
F 2 "" H 9750 1500 50  0001 C CNN
F 3 "" H 9750 1500 50  0001 C CNN
	1    9750 1500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR069
U 1 1 61AAB357
P 10350 1500
F 0 "#PWR069" H 10350 1250 50  0001 C CNN
F 1 "GND" H 10355 1327 50  0000 C CNN
F 2 "" H 10350 1500 50  0001 C CNN
F 3 "" H 10350 1500 50  0001 C CNN
	1    10350 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	10350 1200 10350 1100
Wire Wire Line
	10350 1100 10200 1100
Wire Wire Line
	9900 1100 9750 1100
Wire Wire Line
	9750 1100 9750 1200
Wire Wire Line
	9750 1100 9550 1100
Connection ~ 9750 1100
Wire Wire Line
	10350 1100 10550 1100
Connection ~ 10350 1100
Text Label 9550 1100 0    50   ~ 0
XTAL1
Text Label 10550 1100 2    50   ~ 0
XTAL2
$Comp
L Device:R R28
U 1 1 61AB3234
P 7750 2800
F 0 "R28" H 7820 2846 50  0000 L CNN
F 1 "10k" H 7820 2755 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 7680 2800 50  0001 C CNN
F 3 "~" H 7750 2800 50  0001 C CNN
	1    7750 2800
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 61AB4430
P 7750 3300
F 0 "SW1" V 7796 3252 50  0000 R CNN
F 1 "RESET" V 7705 3252 50  0000 R CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 7750 3500 50  0001 C CNN
F 3 "~" H 7750 3500 50  0001 C CNN
	1    7750 3300
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR080
U 1 1 61AB5A11
P 7750 3500
F 0 "#PWR080" H 7750 3250 50  0001 C CNN
F 1 "GND" H 7755 3327 50  0000 C CNN
F 2 "" H 7750 3500 50  0001 C CNN
F 3 "" H 7750 3500 50  0001 C CNN
	1    7750 3500
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR079
U 1 1 61AB9E93
P 7750 2650
F 0 "#PWR079" H 7750 2500 50  0001 C CNN
F 1 "+5V" H 7765 2823 50  0000 C CNN
F 2 "" H 7750 2650 50  0001 C CNN
F 3 "" H 7750 2650 50  0001 C CNN
	1    7750 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7750 3100 7750 3050
Wire Wire Line
	7750 3050 8150 3050
Connection ~ 7750 3050
Wire Wire Line
	7750 3050 7750 2950
Text Label 8150 3050 2    50   ~ 0
RST
$Comp
L Connector_Generic:Conn_02x03_Odd_Even J12
U 1 1 61ACAF80
P 10000 3000
F 0 "J12" H 10050 3317 50  0000 C CNN
F 1 "ISP" H 10050 3226 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 10000 3000 50  0001 C CNN
F 3 "~" H 10000 3000 50  0001 C CNN
	1    10000 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 3100 9450 3100
Wire Wire Line
	9800 3000 9450 3000
Wire Wire Line
	9800 2900 9450 2900
Wire Wire Line
	10300 3000 10650 3000
Text Label 9450 2900 0    50   ~ 0
SPI_MISO
Text Label 9450 3000 0    50   ~ 0
SPI_SCK
Text Label 9450 3100 0    50   ~ 0
RST
Text Label 10650 3000 2    50   ~ 0
SPI_MOSI
$Comp
L power:+5V #PWR081
U 1 1 61AD438D
P 10300 2900
F 0 "#PWR081" H 10300 2750 50  0001 C CNN
F 1 "+5V" H 10315 3073 50  0000 C CNN
F 2 "" H 10300 2900 50  0001 C CNN
F 3 "" H 10300 2900 50  0001 C CNN
	1    10300 2900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR082
U 1 1 61AD8134
P 10300 3100
F 0 "#PWR082" H 10300 2850 50  0001 C CNN
F 1 "GND" H 10305 2927 50  0000 C CNN
F 2 "" H 10300 3100 50  0001 C CNN
F 3 "" H 10300 3100 50  0001 C CNN
	1    10300 3100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Male J11
U 1 1 61AF1057
P 1000 4450
F 0 "J11" H 1150 4650 50  0000 R CNN
F 1 "I2C" H 1150 4150 50  0000 R CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-04A_1x04_P2.54mm_Vertical" H 1000 4450 50  0001 C CNN
F 3 "~" H 1000 4450 50  0001 C CNN
	1    1000 4450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR078
U 1 1 61AF604D
P 1200 4650
F 0 "#PWR078" H 1200 4400 50  0001 C CNN
F 1 "GND" H 1205 4477 50  0000 C CNN
F 2 "" H 1200 4650 50  0001 C CNN
F 3 "" H 1200 4650 50  0001 C CNN
	1    1200 4650
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR077
U 1 1 61AF9B72
P 1200 4350
F 0 "#PWR077" H 1200 4200 50  0001 C CNN
F 1 "+5V" H 1215 4523 50  0000 C CNN
F 2 "" H 1200 4350 50  0001 C CNN
F 3 "" H 1200 4350 50  0001 C CNN
	1    1200 4350
	1    0    0    -1  
$EndComp
Text Label 1600 4550 2    50   ~ 0
I2C_SCL
Text Label 1600 4450 2    50   ~ 0
I2C_SDA
Wire Wire Line
	1200 4550 1600 4550
Wire Wire Line
	1600 4450 1200 4450
Text Label 2300 4000 0    50   ~ 0
HOME0
$Comp
L Connector:Conn_01x02_Male J7
U 1 1 61BFDDE2
P 2850 4100
F 0 "J7" H 2822 3982 50  0000 R CNN
F 1 "Home 0" H 2822 4073 50  0000 R CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 2850 4100 50  0001 C CNN
F 3 "~" H 2850 4100 50  0001 C CNN
	1    2850 4100
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR063
U 1 1 61BFF357
P 2650 4100
F 0 "#PWR063" H 2650 3850 50  0001 C CNN
F 1 "GND" H 2655 3927 50  0000 C CNN
F 2 "" H 2650 4100 50  0001 C CNN
F 3 "" H 2650 4100 50  0001 C CNN
	1    2650 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 4000 2650 4000
Text Label 2300 4400 0    50   ~ 0
HOME1
$Comp
L Connector:Conn_01x02_Male J8
U 1 1 61C05C7E
P 2850 4500
F 0 "J8" H 2822 4382 50  0000 R CNN
F 1 "Home 1" H 2822 4473 50  0000 R CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 2850 4500 50  0001 C CNN
F 3 "~" H 2850 4500 50  0001 C CNN
	1    2850 4500
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR064
U 1 1 61C05C84
P 2650 4500
F 0 "#PWR064" H 2650 4250 50  0001 C CNN
F 1 "GND" H 2655 4327 50  0000 C CNN
F 2 "" H 2650 4500 50  0001 C CNN
F 3 "" H 2650 4500 50  0001 C CNN
	1    2650 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 4400 2650 4400
Text Label 2300 4800 0    50   ~ 0
HOME2
$Comp
L Connector:Conn_01x02_Male J9
U 1 1 61C08CAE
P 2850 4900
F 0 "J9" H 2822 4782 50  0000 R CNN
F 1 "Home 2" H 2822 4873 50  0000 R CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 2850 4900 50  0001 C CNN
F 3 "~" H 2850 4900 50  0001 C CNN
	1    2850 4900
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR065
U 1 1 61C08CB4
P 2650 4900
F 0 "#PWR065" H 2650 4650 50  0001 C CNN
F 1 "GND" H 2655 4727 50  0000 C CNN
F 2 "" H 2650 4900 50  0001 C CNN
F 3 "" H 2650 4900 50  0001 C CNN
	1    2650 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 4800 2650 4800
Text Label 750  2850 0    50   ~ 0
PUMP0
Text Label 750  3050 0    50   ~ 0
PUMP1
Wire Wire Line
	1350 2850 1250 2850
Wire Wire Line
	1350 3050 1250 3050
Text Label 750  3250 0    50   ~ 0
PUMP2
Wire Wire Line
	1350 3250 1250 3250
Text Label 7000 1050 0    50   ~ 0
STATUS
$Comp
L Device:R R25
U 1 1 61C2154E
P 7350 1200
F 0 "R25" H 7420 1246 50  0000 L CNN
F 1 "470" H 7420 1155 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 7280 1200 50  0001 C CNN
F 3 "~" H 7350 1200 50  0001 C CNN
	1    7350 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 61C24825
P 7350 1500
F 0 "D1" V 7389 1382 50  0000 R CNN
F 1 "ST" V 7298 1382 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 7350 1500 50  0001 C CNN
F 3 "~" H 7350 1500 50  0001 C CNN
	1    7350 1500
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R26
U 1 1 61C263B5
P 7700 1200
F 0 "R26" H 7770 1246 50  0000 L CNN
F 1 "470" H 7770 1155 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 7630 1200 50  0001 C CNN
F 3 "~" H 7700 1200 50  0001 C CNN
	1    7700 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 61C263BB
P 7700 1500
F 0 "D2" V 7739 1382 50  0000 R CNN
F 1 "5V" V 7648 1382 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 7700 1500 50  0001 C CNN
F 3 "~" H 7700 1500 50  0001 C CNN
	1    7700 1500
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R27
U 1 1 61C29CED
P 8050 1200
F 0 "R27" H 8120 1246 50  0000 L CNN
F 1 "470" H 8120 1155 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 7980 1200 50  0001 C CNN
F 3 "~" H 8050 1200 50  0001 C CNN
	1    8050 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 61C29CF3
P 8050 1500
F 0 "D3" V 8089 1382 50  0000 R CNN
F 1 "12V" V 7998 1382 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 8050 1500 50  0001 C CNN
F 3 "~" H 8050 1500 50  0001 C CNN
	1    8050 1500
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR070
U 1 1 61C2DEA1
P 7350 1650
F 0 "#PWR070" H 7350 1400 50  0001 C CNN
F 1 "GND" H 7355 1477 50  0000 C CNN
F 2 "" H 7350 1650 50  0001 C CNN
F 3 "" H 7350 1650 50  0001 C CNN
	1    7350 1650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR072
U 1 1 61C31460
P 7700 1650
F 0 "#PWR072" H 7700 1400 50  0001 C CNN
F 1 "GND" H 7705 1477 50  0000 C CNN
F 2 "" H 7700 1650 50  0001 C CNN
F 3 "" H 7700 1650 50  0001 C CNN
	1    7700 1650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR074
U 1 1 61C34A28
P 8050 1650
F 0 "#PWR074" H 8050 1400 50  0001 C CNN
F 1 "GND" H 8055 1477 50  0000 C CNN
F 2 "" H 8050 1650 50  0001 C CNN
F 3 "" H 8050 1650 50  0001 C CNN
	1    8050 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 1050 7000 1050
$Comp
L power:+5V #PWR071
U 1 1 61C4AE0E
P 7700 1050
F 0 "#PWR071" H 7700 900 50  0001 C CNN
F 1 "+5V" H 7715 1223 50  0000 C CNN
F 2 "" H 7700 1050 50  0001 C CNN
F 3 "" H 7700 1050 50  0001 C CNN
	1    7700 1050
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR073
U 1 1 61C52E14
P 8050 1050
F 0 "#PWR073" H 8050 900 50  0001 C CNN
F 1 "+12V" H 8065 1223 50  0000 C CNN
F 2 "" H 8050 1050 50  0001 C CNN
F 3 "" H 8050 1050 50  0001 C CNN
	1    8050 1050
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C14
U 1 1 61C5B4FB
P 1150 5900
F 0 "C14" H 1268 5946 50  0000 L CNN
F 1 "10u" H 1268 5855 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 1188 5750 50  0001 C CNN
F 3 "~" H 1150 5900 50  0001 C CNN
	1    1150 5900
	1    0    0    -1  
$EndComp
$Comp
L Device:C C16
U 1 1 61C5E6C8
P 1550 5900
F 0 "C16" H 1665 5946 50  0000 L CNN
F 1 "100n" H 1665 5855 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 1588 5750 50  0001 C CNN
F 3 "~" H 1550 5900 50  0001 C CNN
	1    1550 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 6050 1150 6050
Wire Wire Line
	1550 5750 1150 5750
$Comp
L power:+5V #PWR039
U 1 1 61C6A404
P 1150 5750
F 0 "#PWR039" H 1150 5600 50  0001 C CNN
F 1 "+5V" H 1165 5923 50  0000 C CNN
F 2 "" H 1150 5750 50  0001 C CNN
F 3 "" H 1150 5750 50  0001 C CNN
	1    1150 5750
	1    0    0    -1  
$EndComp
Connection ~ 1150 5750
$Comp
L power:GND #PWR040
U 1 1 61C6ABE8
P 1150 6050
F 0 "#PWR040" H 1150 5800 50  0001 C CNN
F 1 "GND" H 1155 5877 50  0000 C CNN
F 2 "" H 1150 6050 50  0001 C CNN
F 3 "" H 1150 6050 50  0001 C CNN
	1    1150 6050
	1    0    0    -1  
$EndComp
Connection ~ 1150 6050
$Comp
L power:PWR_FLAG #FLG01
U 1 1 61C6B1F9
P 1550 5750
F 0 "#FLG01" H 1550 5825 50  0001 C CNN
F 1 "PWR_FLAG" H 1550 5923 50  0000 C CNN
F 2 "" H 1550 5750 50  0001 C CNN
F 3 "~" H 1550 5750 50  0001 C CNN
	1    1550 5750
	1    0    0    -1  
$EndComp
Connection ~ 1550 5750
$Comp
L power:PWR_FLAG #FLG02
U 1 1 61C6B7E7
P 1550 6050
F 0 "#FLG02" H 1550 6125 50  0001 C CNN
F 1 "PWR_FLAG" H 1550 6223 50  0000 C CNN
F 2 "" H 1550 6050 50  0001 C CNN
F 3 "~" H 1550 6050 50  0001 C CNN
	1    1550 6050
	-1   0    0    1   
$EndComp
Connection ~ 1550 6050
$Comp
L Connector:Conn_01x02_Male J3
U 1 1 61C7A1F4
P 2050 5750
F 0 "J3" H 2050 5550 50  0000 C CNN
F 1 "12V" H 2150 5900 50  0000 C CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 2050 5750 50  0001 C CNN
F 3 "~" H 2050 5750 50  0001 C CNN
	1    2050 5750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR049
U 1 1 61C7BAE3
P 2250 5850
F 0 "#PWR049" H 2250 5600 50  0001 C CNN
F 1 "GND" H 2255 5677 50  0000 C CNN
F 2 "" H 2250 5850 50  0001 C CNN
F 3 "" H 2250 5850 50  0001 C CNN
	1    2250 5850
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C18
U 1 1 61C80AD7
P 2550 5950
F 0 "C18" H 2668 5996 50  0000 L CNN
F 1 "10u" H 2668 5905 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 2588 5800 50  0001 C CNN
F 3 "~" H 2550 5950 50  0001 C CNN
	1    2550 5950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C19
U 1 1 61C80ADD
P 2950 5950
F 0 "C19" H 3065 5996 50  0000 L CNN
F 1 "100n" H 3065 5905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 2988 5800 50  0001 C CNN
F 3 "~" H 2950 5950 50  0001 C CNN
	1    2950 5950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR053
U 1 1 61C85579
P 2550 6100
F 0 "#PWR053" H 2550 5850 50  0001 C CNN
F 1 "GND" H 2555 5927 50  0000 C CNN
F 2 "" H 2550 6100 50  0001 C CNN
F 3 "" H 2550 6100 50  0001 C CNN
	1    2550 6100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR054
U 1 1 61C88CC8
P 2950 6100
F 0 "#PWR054" H 2950 5850 50  0001 C CNN
F 1 "GND" H 2955 5927 50  0000 C CNN
F 2 "" H 2950 6100 50  0001 C CNN
F 3 "" H 2950 6100 50  0001 C CNN
	1    2950 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 5800 2950 5750
Wire Wire Line
	2550 5800 2550 5750
Connection ~ 2550 5750
Wire Wire Line
	2550 5750 2250 5750
$Comp
L power:PWR_FLAG #FLG03
U 1 1 61C948B6
P 2550 5750
F 0 "#FLG03" H 2550 5825 50  0001 C CNN
F 1 "PWR_FLAG" H 2550 5923 50  0000 C CNN
F 2 "" H 2550 5750 50  0001 C CNN
F 3 "~" H 2550 5750 50  0001 C CNN
	1    2550 5750
	1    0    0    -1  
$EndComp
$Comp
L Transistor_Array:ULN2003 U3
U 1 1 61CAB4CA
P 1750 3050
F 0 "U3" H 1750 3717 50  0000 C CNN
F 1 "ULN2003" H 1750 3626 50  0000 C CNN
F 2 "Package_SO:SO-16_3.9x9.9mm_P1.27mm" H 1800 2500 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/uln2003a.pdf" H 1850 2850 50  0001 C CNN
	1    1750 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1350 2950 1250 2950
Wire Wire Line
	1250 2950 1250 2850
Connection ~ 1250 2850
Wire Wire Line
	1250 2850 750  2850
Wire Wire Line
	1350 3150 1250 3150
Wire Wire Line
	1250 3150 1250 3050
Connection ~ 1250 3050
Wire Wire Line
	1250 3050 750  3050
Wire Wire Line
	1350 3350 1250 3350
Wire Wire Line
	1250 3350 1250 3250
Connection ~ 1250 3250
Wire Wire Line
	1250 3250 750  3250
Wire Wire Line
	1350 3450 1250 3450
Wire Wire Line
	1250 3450 1250 3350
Connection ~ 1250 3350
$Comp
L power:GND #PWR046
U 1 1 61CDF7B7
P 1750 3650
F 0 "#PWR046" H 1750 3400 50  0001 C CNN
F 1 "GND" H 1755 3477 50  0000 C CNN
F 2 "" H 1750 3650 50  0001 C CNN
F 3 "" H 1750 3650 50  0001 C CNN
	1    1750 3650
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR052
U 1 1 61CEAA4F
P 2200 2550
F 0 "#PWR052" H 2200 2400 50  0001 C CNN
F 1 "+12V" H 2215 2723 50  0000 C CNN
F 2 "" H 2200 2550 50  0001 C CNN
F 3 "" H 2200 2550 50  0001 C CNN
	1    2200 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 2650 2200 2650
Wire Wire Line
	2200 2650 2200 2550
Wire Wire Line
	2150 2850 2250 2850
Wire Wire Line
	2150 2950 2250 2950
Wire Wire Line
	2250 2950 2250 2850
Connection ~ 2250 2850
Wire Wire Line
	2250 2850 2550 2850
Wire Wire Line
	2150 3050 2250 3050
Wire Wire Line
	2150 3150 2250 3150
Wire Wire Line
	2250 3150 2250 3050
Connection ~ 2250 3050
Wire Wire Line
	2250 3050 2550 3050
Wire Wire Line
	2150 3250 2250 3250
Wire Wire Line
	2150 3350 2250 3350
Wire Wire Line
	2250 3350 2250 3250
Wire Wire Line
	2150 3450 2250 3450
Wire Wire Line
	2250 3450 2250 3350
Connection ~ 2250 3350
$Comp
L Connector:Conn_01x02_Male J4
U 1 1 61D3D72C
P 2900 2650
F 0 "J4" H 2872 2532 50  0000 R CNN
F 1 "Pump 0" H 2872 2623 50  0000 R CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 2900 2650 50  0001 C CNN
F 3 "~" H 2900 2650 50  0001 C CNN
	1    2900 2650
	-1   0    0    1   
$EndComp
$Comp
L power:+12V #PWR056
U 1 1 61D3FC56
P 2700 2550
F 0 "#PWR056" H 2700 2400 50  0001 C CNN
F 1 "+12V" H 2715 2723 50  0000 C CNN
F 2 "" H 2700 2550 50  0001 C CNN
F 3 "" H 2700 2550 50  0001 C CNN
	1    2700 2550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J5
U 1 1 61D4C4D2
P 2900 3000
F 0 "J5" H 2872 2882 50  0000 R CNN
F 1 "Pump 1" H 2872 2973 50  0000 R CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 2900 3000 50  0001 C CNN
F 3 "~" H 2900 3000 50  0001 C CNN
	1    2900 3000
	-1   0    0    1   
$EndComp
$Comp
L power:+12V #PWR057
U 1 1 61D4C4D8
P 2700 2900
F 0 "#PWR057" H 2700 2750 50  0001 C CNN
F 1 "+12V" H 2715 3073 50  0000 C CNN
F 2 "" H 2700 2900 50  0001 C CNN
F 3 "" H 2700 2900 50  0001 C CNN
	1    2700 2900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J6
U 1 1 61D51F16
P 2900 3350
F 0 "J6" H 2872 3232 50  0000 R CNN
F 1 "Pump 2" H 2872 3323 50  0000 R CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 2900 3350 50  0001 C CNN
F 3 "~" H 2900 3350 50  0001 C CNN
	1    2900 3350
	-1   0    0    1   
$EndComp
$Comp
L power:+12V #PWR058
U 1 1 61D51F1C
P 2700 3250
F 0 "#PWR058" H 2700 3100 50  0001 C CNN
F 1 "+12V" H 2715 3423 50  0000 C CNN
F 2 "" H 2700 3250 50  0001 C CNN
F 3 "" H 2700 3250 50  0001 C CNN
	1    2700 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 2850 2550 2650
Wire Wire Line
	2550 2650 2700 2650
Wire Wire Line
	2550 3050 2550 3000
Wire Wire Line
	2550 3000 2700 3000
Wire Wire Line
	2250 3350 2700 3350
Wire Wire Line
	1250 1100 700  1100
Text Label 700  1100 0    50   ~ 0
STEP0
Wire Wire Line
	1250 1000 700  1000
Text Label 700  1000 0    50   ~ 0
DIR0
Text Label 700  1300 0    50   ~ 0
RST
$Sheet
S 1250 900  900  800 
U 6213AC6D
F0 "StepperControl" 50
F1 "stepper_control.sch" 50
F2 "DIR" I L 1250 1000 50 
F3 "STEP" I L 1250 1100 50 
F4 "RST" I L 1250 1300 50 
F5 "USM0" I L 1250 1500 50 
F6 "USM1" I L 1250 1600 50 
$EndSheet
Wire Wire Line
	700  1300 1250 1300
Wire Wire Line
	1250 1500 1050 1500
Wire Wire Line
	1050 1500 1050 1600
Wire Wire Line
	1250 1600 1050 1600
Connection ~ 1050 1600
Wire Wire Line
	1050 1600 1050 1850
$Comp
L power:GND #PWR01
U 1 1 62260E21
P 1050 1850
F 0 "#PWR01" H 1050 1600 50  0001 C CNN
F 1 "GND" H 1055 1677 50  0000 C CNN
F 2 "" H 1050 1850 50  0001 C CNN
F 3 "" H 1050 1850 50  0001 C CNN
	1    1050 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 1100 2400 1100
Text Label 2400 1100 0    50   ~ 0
STEP1
Wire Wire Line
	2950 1000 2400 1000
Text Label 2400 1000 0    50   ~ 0
DIR1
Text Label 2400 1300 0    50   ~ 0
RST
$Sheet
S 2950 900  900  800 
U 622BE002
F0 "sheet622BDFF6" 50
F1 "stepper_control.sch" 50
F2 "DIR" I L 2950 1000 50 
F3 "STEP" I L 2950 1100 50 
F4 "RST" I L 2950 1300 50 
F5 "USM0" I L 2950 1500 50 
F6 "USM1" I L 2950 1600 50 
$EndSheet
Wire Wire Line
	2400 1300 2950 1300
Wire Wire Line
	2950 1500 2750 1500
Wire Wire Line
	2750 1500 2750 1600
Wire Wire Line
	2950 1600 2750 1600
Connection ~ 2750 1600
Wire Wire Line
	2750 1600 2750 1850
$Comp
L power:GND #PWR02
U 1 1 622BE00E
P 2750 1850
F 0 "#PWR02" H 2750 1600 50  0001 C CNN
F 1 "GND" H 2755 1677 50  0000 C CNN
F 2 "" H 2750 1850 50  0001 C CNN
F 3 "" H 2750 1850 50  0001 C CNN
	1    2750 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 1100 4150 1100
Text Label 4150 1100 0    50   ~ 0
STEP2
Wire Wire Line
	4700 1000 4150 1000
Text Label 4150 1000 0    50   ~ 0
DIR2
Text Label 4150 1300 0    50   ~ 0
RST
$Sheet
S 4700 900  900  800 
U 622F44AF
F0 "sheet622F44A3" 50
F1 "stepper_control.sch" 50
F2 "DIR" I L 4700 1000 50 
F3 "STEP" I L 4700 1100 50 
F4 "RST" I L 4700 1300 50 
F5 "USM0" I L 4700 1500 50 
F6 "USM1" I L 4700 1600 50 
$EndSheet
Wire Wire Line
	4150 1300 4700 1300
Wire Wire Line
	4700 1500 4500 1500
Wire Wire Line
	4500 1500 4500 1600
Wire Wire Line
	4700 1600 4500 1600
Connection ~ 4500 1600
Wire Wire Line
	4500 1600 4500 1850
$Comp
L power:GND #PWR03
U 1 1 622F44BB
P 4500 1850
F 0 "#PWR03" H 4500 1600 50  0001 C CNN
F 1 "GND" H 4505 1677 50  0000 C CNN
F 2 "" H 4500 1850 50  0001 C CNN
F 3 "" H 4500 1850 50  0001 C CNN
	1    4500 1850
	1    0    0    -1  
$EndComp
$Comp
L MCU_Microchip_ATmega:ATmega168PA-AU U1
U 1 1 62325B16
P 4950 3850
F 0 "U1" H 4500 2400 50  0000 C CNN
F 1 "ATmega168PA-AU" H 5350 2400 50  0000 C CNN
F 2 "Package_QFP:TQFP-32_7x7mm_P0.8mm" H 4950 3850 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48PA_88PA_168PA-Data-Sheet-40002011A.pdf" H 4950 3850 50  0001 C CNN
	1    4950 3850
	1    0    0    -1  
$EndComp
NoConn ~ 4350 2650
NoConn ~ 4350 2850
NoConn ~ 4350 2950
Wire Wire Line
	5050 2350 5000 2350
Wire Wire Line
	5000 2350 5000 2300
Connection ~ 5000 2350
Wire Wire Line
	5000 2350 4950 2350
Wire Wire Line
	5550 4550 6150 4550
Text Label 6150 4550 2    50   ~ 0
STATUS
Wire Wire Line
	5550 5050 6150 5050
Wire Wire Line
	5550 4950 6150 4950
Wire Wire Line
	5550 4750 6150 4750
Wire Wire Line
	5550 3750 6150 3750
Wire Wire Line
	5550 3650 6150 3650
Wire Wire Line
	5550 3550 6150 3550
Wire Wire Line
	5550 3850 6150 3850
Wire Wire Line
	5550 2750 6150 2750
Wire Wire Line
	5550 2650 6150 2650
Text Label 6150 5050 2    50   ~ 0
HOME2
Text Label 6150 4950 2    50   ~ 0
HOME1
Text Label 6150 4750 2    50   ~ 0
HOME0
Text Label 6150 3850 2    50   ~ 0
PUMP2
Text Label 6150 2750 2    50   ~ 0
PUMP1
Text Label 6150 2650 2    50   ~ 0
PUMP0
Text Label 6150 3750 2    50   ~ 0
DIR2
Text Label 6150 3650 2    50   ~ 0
DIR1
Wire Wire Line
	5550 4650 6150 4650
Text Label 6150 4650 2    50   ~ 0
STEP2
Wire Wire Line
	5550 2850 6150 2850
Text Label 6150 2850 2    50   ~ 0
STEP1
Wire Wire Line
	5550 4850 6150 4850
Wire Wire Line
	5550 3950 6150 3950
Wire Wire Line
	5550 4050 6150 4050
Text Label 6150 3950 2    50   ~ 0
I2C_SDA
Text Label 6150 4050 2    50   ~ 0
I2C_SCL
Wire Wire Line
	5550 2950 6150 2950
Wire Wire Line
	5550 3050 6150 3050
Wire Wire Line
	5550 3150 6150 3150
Text Label 6150 3150 2    50   ~ 0
SPI_SCK
Text Label 6150 3050 2    50   ~ 0
SPI_MISO
Text Label 6150 2950 2    50   ~ 0
SPI_MOSI
Text Label 6150 3550 2    50   ~ 0
DIR0
Text Label 6150 4850 2    50   ~ 0
STEP0
Text Label 6150 4150 2    50   ~ 0
RST
Wire Wire Line
	5550 4150 6150 4150
Text Label 6150 3350 2    50   ~ 0
XTAL2
Text Label 6150 3250 2    50   ~ 0
XTAL1
Wire Wire Line
	5550 3350 6150 3350
Wire Wire Line
	5550 3250 6150 3250
NoConn ~ 5550 4350
NoConn ~ 5550 4450
$Comp
L power:+12V #PWR042
U 1 1 62452212
P 2950 5750
F 0 "#PWR042" H 2950 5600 50  0001 C CNN
F 1 "+12V" H 2965 5923 50  0000 C CNN
F 2 "" H 2950 5750 50  0001 C CNN
F 3 "" H 2950 5750 50  0001 C CNN
	1    2950 5750
	1    0    0    -1  
$EndComp
Connection ~ 2950 5750
Wire Wire Line
	2550 5750 2950 5750
$EndSCHEMATC
