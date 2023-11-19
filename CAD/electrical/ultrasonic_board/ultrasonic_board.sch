EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
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
L MCU_Microchip_ATmega:ATmega168PA-AU U1
U 1 1 63681DDF
P 2200 2750
F 0 "U1" H 1750 1300 50  0000 C CNN
F 1 "ATmega168PA-AU" H 2600 1300 50  0000 C CNN
F 2 "Package_QFP:TQFP-32_7x7mm_P0.8mm" H 2200 2750 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48PA_88PA_168PA-Data-Sheet-40002011A.pdf" H 2200 2750 50  0001 C CNN
	1    2200 2750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 63683B08
P 2200 4250
F 0 "#PWR01" H 2200 4000 50  0001 C CNN
F 1 "GND" H 2205 4077 50  0000 C CNN
F 2 "" H 2200 4250 50  0001 C CNN
F 3 "" H 2200 4250 50  0001 C CNN
	1    2200 4250
	1    0    0    -1  
$EndComp
NoConn ~ 1600 1850
NoConn ~ 1600 1750
NoConn ~ 1600 1550
Wire Wire Line
	2200 1250 2250 1250
$Comp
L power:+5V #PWR02
U 1 1 63684818
P 2250 1250
F 0 "#PWR02" H 2250 1100 50  0001 C CNN
F 1 "+5V" H 2265 1423 50  0000 C CNN
F 2 "" H 2250 1250 50  0001 C CNN
F 3 "" H 2250 1250 50  0001 C CNN
	1    2250 1250
	1    0    0    -1  
$EndComp
Connection ~ 2250 1250
Wire Wire Line
	2250 1250 2300 1250
$Comp
L Device:Crystal Y1
U 1 1 6368543C
P 5400 1250
F 0 "Y1" H 5400 1518 50  0000 C CNN
F 1 "16MHz" H 5400 1427 50  0000 C CNN
F 2 "Crystal:Crystal_SMD_HC49-SD" H 5400 1250 50  0001 C CNN
F 3 "~" H 5400 1250 50  0001 C CNN
	1    5400 1250
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 63685C26
P 5700 1450
F 0 "C2" H 5815 1496 50  0000 L CNN
F 1 "22p" H 5815 1405 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5738 1300 50  0001 C CNN
F 3 "~" H 5700 1450 50  0001 C CNN
	1    5700 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 63686741
P 5100 1450
F 0 "C1" H 5215 1496 50  0000 L CNN
F 1 "22p" H 5215 1405 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5138 1300 50  0001 C CNN
F 3 "~" H 5100 1450 50  0001 C CNN
	1    5100 1450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 6368758A
P 5100 1600
F 0 "#PWR05" H 5100 1350 50  0001 C CNN
F 1 "GND" H 5105 1427 50  0000 C CNN
F 2 "" H 5100 1600 50  0001 C CNN
F 3 "" H 5100 1600 50  0001 C CNN
	1    5100 1600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 6368786B
P 5700 1600
F 0 "#PWR08" H 5700 1350 50  0001 C CNN
F 1 "GND" H 5705 1427 50  0000 C CNN
F 2 "" H 5700 1600 50  0001 C CNN
F 3 "" H 5700 1600 50  0001 C CNN
	1    5700 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 1300 5700 1250
Wire Wire Line
	5700 1250 5550 1250
Wire Wire Line
	5250 1250 5100 1250
Wire Wire Line
	5100 1250 5100 1300
Wire Wire Line
	5100 1250 4900 1250
Connection ~ 5100 1250
Wire Wire Line
	5700 1250 5850 1250
Connection ~ 5700 1250
Text Label 4900 1250 0    50   ~ 0
XTAL1
Text Label 5850 1250 2    50   ~ 0
XTAL2
$Comp
L Connector:AVR-ISP-6 J2
U 1 1 6368ABDB
P 4900 2950
F 0 "J2" H 4571 3046 50  0000 R CNN
F 1 "AVR-ISP-6" H 4571 2955 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" V 4650 3000 50  0001 C CNN
F 3 " ~" H 3625 2400 50  0001 C CNN
	1    4900 2950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 6368C107
P 4800 3350
F 0 "#PWR07" H 4800 3100 50  0001 C CNN
F 1 "GND" H 4805 3177 50  0000 C CNN
F 2 "" H 4800 3350 50  0001 C CNN
F 3 "" H 4800 3350 50  0001 C CNN
	1    4800 3350
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR06
U 1 1 6368D69B
P 4800 2450
F 0 "#PWR06" H 4800 2300 50  0001 C CNN
F 1 "+5V" H 4815 2623 50  0000 C CNN
F 2 "" H 4800 2450 50  0001 C CNN
F 3 "" H 4800 2450 50  0001 C CNN
	1    4800 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 2750 5800 2750
Wire Wire Line
	5300 2850 5800 2850
Wire Wire Line
	5300 2950 5800 2950
Wire Wire Line
	5300 3050 6100 3050
Wire Wire Line
	6100 3050 6100 3100
Wire Wire Line
	6100 3050 6100 3000
Connection ~ 6100 3050
$Comp
L Switch:SW_Push SW1
U 1 1 6368F211
P 6100 3300
F 0 "SW1" V 6146 3252 50  0000 R CNN
F 1 "RESET" V 6055 3252 50  0000 R CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 6100 3500 50  0001 C CNN
F 3 "~" H 6100 3500 50  0001 C CNN
	1    6100 3300
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R1
U 1 1 6368FC28
P 6100 2850
F 0 "R1" H 6170 2896 50  0000 L CNN
F 1 "10k" H 6170 2805 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6030 2850 50  0001 C CNN
F 3 "~" H 6100 2850 50  0001 C CNN
	1    6100 2850
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR09
U 1 1 6369126C
P 6100 2700
F 0 "#PWR09" H 6100 2550 50  0001 C CNN
F 1 "+5V" H 6115 2873 50  0000 C CNN
F 2 "" H 6100 2700 50  0001 C CNN
F 3 "" H 6100 2700 50  0001 C CNN
	1    6100 2700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 63692416
P 6100 3500
F 0 "#PWR010" H 6100 3250 50  0001 C CNN
F 1 "GND" H 6105 3327 50  0000 C CNN
F 2 "" H 6100 3500 50  0001 C CNN
F 3 "" H 6100 3500 50  0001 C CNN
	1    6100 3500
	1    0    0    -1  
$EndComp
Text Label 5800 2750 2    50   ~ 0
MISO
Text Label 5800 2850 2    50   ~ 0
MOSI
Text Label 5800 2950 2    50   ~ 0
SCK
Text Label 5800 3050 2    50   ~ 0
RST
Wire Wire Line
	2800 1850 3400 1850
Wire Wire Line
	2800 1950 3400 1950
Wire Wire Line
	2800 2050 3400 2050
Text Label 3400 1850 2    50   ~ 0
MOSI
Text Label 3400 1950 2    50   ~ 0
MISO
Text Label 3400 2050 2    50   ~ 0
SCK
Wire Wire Line
	2800 3050 3400 3050
Text Label 3400 3050 2    50   ~ 0
RST
$Comp
L Connector:Conn_01x04_Female J5
U 1 1 63698490
P 8050 1250
F 0 "J5" H 8078 1226 50  0000 L CNN
F 1 "US0" H 8078 1135 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 8050 1250 50  0001 C CNN
F 3 "~" H 8050 1250 50  0001 C CNN
	1    8050 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 1150 7750 1150
Wire Wire Line
	7750 1150 7750 1050
Wire Wire Line
	7850 1350 7600 1350
Wire Wire Line
	7850 1450 7750 1450
Wire Wire Line
	7750 1450 7750 1550
$Comp
L power:GND #PWR016
U 1 1 6369C10C
P 7750 1550
F 0 "#PWR016" H 7750 1300 50  0001 C CNN
F 1 "GND" H 7755 1377 50  0000 C CNN
F 2 "" H 7750 1550 50  0001 C CNN
F 3 "" H 7750 1550 50  0001 C CNN
	1    7750 1550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR015
U 1 1 6369E1D2
P 7750 1050
F 0 "#PWR015" H 7750 900 50  0001 C CNN
F 1 "+5V" H 7765 1223 50  0000 C CNN
F 2 "" H 7750 1050 50  0001 C CNN
F 3 "" H 7750 1050 50  0001 C CNN
	1    7750 1050
	1    0    0    -1  
$EndComp
Text Label 7600 1350 0    50   ~ 0
US0
Wire Wire Line
	9750 1150 9750 1050
Wire Wire Line
	9850 1350 9600 1350
Wire Wire Line
	9750 1450 9750 1550
$Comp
L power:GND #PWR024
U 1 1 636BBE54
P 9750 1550
F 0 "#PWR024" H 9750 1300 50  0001 C CNN
F 1 "GND" H 9755 1377 50  0000 C CNN
F 2 "" H 9750 1550 50  0001 C CNN
F 3 "" H 9750 1550 50  0001 C CNN
	1    9750 1550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR023
U 1 1 636BBE5A
P 9750 1050
F 0 "#PWR023" H 9750 900 50  0001 C CNN
F 1 "+5V" H 9765 1223 50  0000 C CNN
F 2 "" H 9750 1050 50  0001 C CNN
F 3 "" H 9750 1050 50  0001 C CNN
	1    9750 1050
	1    0    0    -1  
$EndComp
Text Label 9600 1350 0    50   ~ 0
US3
$Comp
L Connector:Conn_01x04_Female J13
U 1 1 636BCF24
P 10050 2250
F 0 "J13" H 10078 2226 50  0000 L CNN
F 1 "US8" H 10078 2135 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 10050 2250 50  0001 C CNN
F 3 "~" H 10050 2250 50  0001 C CNN
	1    10050 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 2150 9750 2150
Wire Wire Line
	9750 2150 9750 2050
Wire Wire Line
	9850 2350 9600 2350
Wire Wire Line
	9850 2450 9750 2450
Wire Wire Line
	9750 2450 9750 2550
$Comp
L power:GND #PWR038
U 1 1 636BCF32
P 9750 2550
F 0 "#PWR038" H 9750 2300 50  0001 C CNN
F 1 "GND" H 9755 2377 50  0000 C CNN
F 2 "" H 9750 2550 50  0001 C CNN
F 3 "" H 9750 2550 50  0001 C CNN
	1    9750 2550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR037
U 1 1 636BCF38
P 9750 2050
F 0 "#PWR037" H 9750 1900 50  0001 C CNN
F 1 "+5V" H 9765 2223 50  0000 C CNN
F 2 "" H 9750 2050 50  0001 C CNN
F 3 "" H 9750 2050 50  0001 C CNN
	1    9750 2050
	1    0    0    -1  
$EndComp
Text Label 9600 2350 0    50   ~ 0
US8
$Comp
L Connector:Conn_01x04_Female J6
U 1 1 636C0493
P 8750 1250
F 0 "J6" H 8778 1226 50  0000 L CNN
F 1 "US1" H 8778 1135 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 8750 1250 50  0001 C CNN
F 3 "~" H 8750 1250 50  0001 C CNN
	1    8750 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 1150 8450 1150
Wire Wire Line
	8450 1150 8450 1050
Wire Wire Line
	8550 1350 8300 1350
Wire Wire Line
	8550 1450 8450 1450
Wire Wire Line
	8450 1450 8450 1550
$Comp
L power:GND #PWR018
U 1 1 636C04A1
P 8450 1550
F 0 "#PWR018" H 8450 1300 50  0001 C CNN
F 1 "GND" H 8455 1377 50  0000 C CNN
F 2 "" H 8450 1550 50  0001 C CNN
F 3 "" H 8450 1550 50  0001 C CNN
	1    8450 1550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR017
U 1 1 636C04A7
P 8450 1050
F 0 "#PWR017" H 8450 900 50  0001 C CNN
F 1 "+5V" H 8465 1223 50  0000 C CNN
F 2 "" H 8450 1050 50  0001 C CNN
F 3 "" H 8450 1050 50  0001 C CNN
	1    8450 1050
	1    0    0    -1  
$EndComp
Text Label 8300 1350 0    50   ~ 0
US1
$Comp
L Connector:Conn_01x04_Female J9
U 1 1 636C18FF
P 10700 1250
F 0 "J9" H 10728 1226 50  0000 L CNN
F 1 "US4" H 10728 1135 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 10700 1250 50  0001 C CNN
F 3 "~" H 10700 1250 50  0001 C CNN
	1    10700 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	10500 1150 10400 1150
Wire Wire Line
	10400 1150 10400 1050
Wire Wire Line
	10500 1350 10250 1350
Wire Wire Line
	10500 1450 10400 1450
Wire Wire Line
	10400 1450 10400 1550
$Comp
L power:GND #PWR026
U 1 1 636C190D
P 10400 1550
F 0 "#PWR026" H 10400 1300 50  0001 C CNN
F 1 "GND" H 10405 1377 50  0000 C CNN
F 2 "" H 10400 1550 50  0001 C CNN
F 3 "" H 10400 1550 50  0001 C CNN
	1    10400 1550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR025
U 1 1 636C1913
P 10400 1050
F 0 "#PWR025" H 10400 900 50  0001 C CNN
F 1 "+5V" H 10415 1223 50  0000 C CNN
F 2 "" H 10400 1050 50  0001 C CNN
F 3 "" H 10400 1050 50  0001 C CNN
	1    10400 1050
	1    0    0    -1  
$EndComp
Text Label 10250 1350 0    50   ~ 0
US4
$Comp
L Connector:Conn_01x04_Female J11
U 1 1 636C31BE
P 8700 2250
F 0 "J11" H 8728 2226 50  0000 L CNN
F 1 "US6" H 8728 2135 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 8700 2250 50  0001 C CNN
F 3 "~" H 8700 2250 50  0001 C CNN
	1    8700 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8500 2150 8400 2150
Wire Wire Line
	8400 2150 8400 2050
Wire Wire Line
	8500 2350 8250 2350
Wire Wire Line
	8500 2450 8400 2450
Wire Wire Line
	8400 2450 8400 2550
$Comp
L power:GND #PWR034
U 1 1 636C31CC
P 8400 2550
F 0 "#PWR034" H 8400 2300 50  0001 C CNN
F 1 "GND" H 8405 2377 50  0000 C CNN
F 2 "" H 8400 2550 50  0001 C CNN
F 3 "" H 8400 2550 50  0001 C CNN
	1    8400 2550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR033
U 1 1 636C31D2
P 8400 2050
F 0 "#PWR033" H 8400 1900 50  0001 C CNN
F 1 "+5V" H 8415 2223 50  0000 C CNN
F 2 "" H 8400 2050 50  0001 C CNN
F 3 "" H 8400 2050 50  0001 C CNN
	1    8400 2050
	1    0    0    -1  
$EndComp
Text Label 8250 2350 0    50   ~ 0
US6
$Comp
L Connector:Conn_01x04_Female J7
U 1 1 636C7664
P 9400 1250
F 0 "J7" H 9428 1226 50  0000 L CNN
F 1 "US2" H 9428 1135 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 9400 1250 50  0001 C CNN
F 3 "~" H 9400 1250 50  0001 C CNN
	1    9400 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9200 1150 9100 1150
Wire Wire Line
	9100 1150 9100 1050
Wire Wire Line
	9200 1350 8950 1350
Wire Wire Line
	9200 1450 9100 1450
Wire Wire Line
	9100 1450 9100 1550
$Comp
L power:GND #PWR020
U 1 1 636C7672
P 9100 1550
F 0 "#PWR020" H 9100 1300 50  0001 C CNN
F 1 "GND" H 9105 1377 50  0000 C CNN
F 2 "" H 9100 1550 50  0001 C CNN
F 3 "" H 9100 1550 50  0001 C CNN
	1    9100 1550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR019
U 1 1 636C7678
P 9100 1050
F 0 "#PWR019" H 9100 900 50  0001 C CNN
F 1 "+5V" H 9115 1223 50  0000 C CNN
F 2 "" H 9100 1050 50  0001 C CNN
F 3 "" H 9100 1050 50  0001 C CNN
	1    9100 1050
	1    0    0    -1  
$EndComp
Text Label 8950 1350 0    50   ~ 0
US2
$Comp
L Connector:Conn_01x04_Female J10
U 1 1 636C9941
P 8050 2250
F 0 "J10" H 8078 2226 50  0000 L CNN
F 1 "US5" H 8078 2135 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 8050 2250 50  0001 C CNN
F 3 "~" H 8050 2250 50  0001 C CNN
	1    8050 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 2150 7750 2150
Wire Wire Line
	7750 2150 7750 2050
Wire Wire Line
	7850 2350 7600 2350
Wire Wire Line
	7850 2450 7750 2450
Wire Wire Line
	7750 2450 7750 2550
$Comp
L power:GND #PWR028
U 1 1 636C994F
P 7750 2550
F 0 "#PWR028" H 7750 2300 50  0001 C CNN
F 1 "GND" H 7755 2377 50  0000 C CNN
F 2 "" H 7750 2550 50  0001 C CNN
F 3 "" H 7750 2550 50  0001 C CNN
	1    7750 2550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR027
U 1 1 636C9955
P 7750 2050
F 0 "#PWR027" H 7750 1900 50  0001 C CNN
F 1 "+5V" H 7765 2223 50  0000 C CNN
F 2 "" H 7750 2050 50  0001 C CNN
F 3 "" H 7750 2050 50  0001 C CNN
	1    7750 2050
	1    0    0    -1  
$EndComp
Text Label 7600 2350 0    50   ~ 0
US5
$Comp
L Connector:Conn_01x04_Female J12
U 1 1 636CC54F
P 9400 2250
F 0 "J12" H 9428 2226 50  0000 L CNN
F 1 "US7" H 9428 2135 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 9400 2250 50  0001 C CNN
F 3 "~" H 9400 2250 50  0001 C CNN
	1    9400 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9200 2150 9100 2150
Wire Wire Line
	9100 2150 9100 2050
Wire Wire Line
	9200 2350 8950 2350
Wire Wire Line
	9200 2450 9100 2450
Wire Wire Line
	9100 2450 9100 2550
$Comp
L power:GND #PWR036
U 1 1 636CC55D
P 9100 2550
F 0 "#PWR036" H 9100 2300 50  0001 C CNN
F 1 "GND" H 9105 2377 50  0000 C CNN
F 2 "" H 9100 2550 50  0001 C CNN
F 3 "" H 9100 2550 50  0001 C CNN
	1    9100 2550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR035
U 1 1 636CC563
P 9100 2050
F 0 "#PWR035" H 9100 1900 50  0001 C CNN
F 1 "+5V" H 9115 2223 50  0000 C CNN
F 2 "" H 9100 2050 50  0001 C CNN
F 3 "" H 9100 2050 50  0001 C CNN
	1    9100 2050
	1    0    0    -1  
$EndComp
Text Label 8950 2350 0    50   ~ 0
US7
$Comp
L Connector:Conn_01x04_Female J14
U 1 1 636CF5D2
P 10700 2250
F 0 "J14" H 10728 2226 50  0000 L CNN
F 1 "US9" H 10728 2135 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 10700 2250 50  0001 C CNN
F 3 "~" H 10700 2250 50  0001 C CNN
	1    10700 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	10500 2150 10400 2150
Wire Wire Line
	10400 2150 10400 2050
Wire Wire Line
	10500 2350 10250 2350
Wire Wire Line
	10500 2450 10400 2450
Wire Wire Line
	10400 2450 10400 2550
$Comp
L power:GND #PWR041
U 1 1 636CF5E0
P 10400 2550
F 0 "#PWR041" H 10400 2300 50  0001 C CNN
F 1 "GND" H 10405 2377 50  0000 C CNN
F 2 "" H 10400 2550 50  0001 C CNN
F 3 "" H 10400 2550 50  0001 C CNN
	1    10400 2550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR040
U 1 1 636CF5E6
P 10400 2050
F 0 "#PWR040" H 10400 1900 50  0001 C CNN
F 1 "+5V" H 10415 2223 50  0000 C CNN
F 2 "" H 10400 2050 50  0001 C CNN
F 3 "" H 10400 2050 50  0001 C CNN
	1    10400 2050
	1    0    0    -1  
$EndComp
Text Label 10250 2350 0    50   ~ 0
US9
Wire Wire Line
	2800 3450 3400 3450
Text Label 3400 3450 2    50   ~ 0
US4
Wire Wire Line
	2800 3550 3400 3550
Text Label 3400 3550 2    50   ~ 0
US5
Wire Wire Line
	2800 3650 3400 3650
Text Label 3400 3650 2    50   ~ 0
US6
Wire Wire Line
	2800 3750 3400 3750
Text Label 3400 3750 2    50   ~ 0
US7
Wire Wire Line
	2800 3850 3400 3850
Text Label 3400 3850 2    50   ~ 0
US8
Wire Wire Line
	2800 3950 3400 3950
Text Label 3400 3950 2    50   ~ 0
US9
Wire Wire Line
	9850 1450 9750 1450
Wire Wire Line
	9850 1150 9750 1150
$Comp
L Connector:Conn_01x04_Female J8
U 1 1 636BBE46
P 10050 1250
F 0 "J8" H 10078 1226 50  0000 L CNN
F 1 "US3" H 10078 1135 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 10050 1250 50  0001 C CNN
F 3 "~" H 10050 1250 50  0001 C CNN
	1    10050 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 2850 3400 2850
Wire Wire Line
	2800 2950 3400 2950
Text Label 3400 2850 2    50   ~ 0
I2C_SDA
Text Label 3400 2950 2    50   ~ 0
I2C_SCL
$Comp
L Connector:Conn_01x04_Male J3
U 1 1 636EA11D
P 3750 6250
F 0 "J3" H 3858 6531 50  0000 C CNN
F 1 "I2C" H 3650 6200 50  0000 C CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-04A_1x04_P2.54mm_Vertical" H 3750 6250 50  0001 C CNN
F 3 "~" H 3750 6250 50  0001 C CNN
	1    3750 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 6250 4550 6250
Wire Wire Line
	3950 6350 4550 6350
Text Label 4550 6250 2    50   ~ 0
I2C_SDA
Text Label 4550 6350 2    50   ~ 0
I2C_SCL
$Comp
L power:GND #PWR011
U 1 1 636F7E6E
P 3950 6450
F 0 "#PWR011" H 3950 6200 50  0001 C CNN
F 1 "GND" H 3955 6277 50  0000 C CNN
F 2 "" H 3950 6450 50  0001 C CNN
F 3 "" H 3950 6450 50  0001 C CNN
	1    3950 6450
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR012
U 1 1 636FF0B8
P 4000 6100
F 0 "#PWR012" H 4000 5950 50  0001 C CNN
F 1 "+5V" H 4015 6273 50  0000 C CNN
F 2 "" H 4000 6100 50  0001 C CNN
F 3 "" H 4000 6100 50  0001 C CNN
	1    4000 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 6150 4000 6150
Wire Wire Line
	4000 6150 4000 6100
$Comp
L Device:CP C3
U 1 1 6370AA33
P 9050 4600
F 0 "C3" H 9168 4646 50  0000 L CNN
F 1 "10u" H 9168 4555 50  0000 L CNN
F 2 "Capacitor_SMD:CP_Elec_4x5.4" H 9088 4450 50  0001 C CNN
F 3 "~" H 9050 4600 50  0001 C CNN
	1    9050 4600
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 6370B6C1
P 9400 4600
F 0 "C4" H 9515 4646 50  0000 L CNN
F 1 "100n" H 9515 4555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 9438 4450 50  0001 C CNN
F 3 "~" H 9400 4600 50  0001 C CNN
	1    9400 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 4750 9200 4750
Wire Wire Line
	9400 4450 9200 4450
$Comp
L power:GND #PWR014
U 1 1 63715030
P 9200 4800
F 0 "#PWR014" H 9200 4550 50  0001 C CNN
F 1 "GND" H 9205 4627 50  0000 C CNN
F 2 "" H 9200 4800 50  0001 C CNN
F 3 "" H 9200 4800 50  0001 C CNN
	1    9200 4800
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR013
U 1 1 6371A33A
P 9200 4400
F 0 "#PWR013" H 9200 4250 50  0001 C CNN
F 1 "+5V" H 9215 4573 50  0000 C CNN
F 2 "" H 9200 4400 50  0001 C CNN
F 3 "" H 9200 4400 50  0001 C CNN
	1    9200 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	9200 4400 9200 4450
Connection ~ 9200 4450
Wire Wire Line
	9200 4450 9050 4450
Wire Wire Line
	9200 4800 9200 4750
Connection ~ 9200 4750
Wire Wire Line
	9200 4750 9400 4750
Wire Wire Line
	2800 1550 3400 1550
Text Label 3400 1550 2    50   ~ 0
LED_DIG
$Comp
L Connector:Conn_01x03_Male J1
U 1 1 638F4373
P 7600 4650
F 0 "J1" H 7708 4931 50  0000 C CNN
F 1 "LED Strip" H 7708 4840 50  0000 C CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-03A_1x03_P2.54mm_Vertical" H 7600 4650 50  0001 C CNN
F 3 "~" H 7600 4650 50  0001 C CNN
	1    7600 4650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 638F89F4
P 7800 4750
F 0 "#PWR03" H 7800 4500 50  0001 C CNN
F 1 "GND" H 7805 4577 50  0000 C CNN
F 2 "" H 7800 4750 50  0001 C CNN
F 3 "" H 7800 4750 50  0001 C CNN
	1    7800 4750
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR04
U 1 1 639011DD
P 7950 4500
F 0 "#PWR04" H 7950 4350 50  0001 C CNN
F 1 "+5V" H 7965 4673 50  0000 C CNN
F 2 "" H 7950 4500 50  0001 C CNN
F 3 "" H 7950 4500 50  0001 C CNN
	1    7950 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 4550 7950 4500
Wire Wire Line
	7800 4550 7950 4550
Wire Wire Line
	7800 4650 8400 4650
Text Label 8400 4650 2    50   ~ 0
LED_DIG
$Comp
L power:PWR_FLAG #FLG01
U 1 1 63926F6C
P 8800 4400
F 0 "#FLG01" H 8800 4475 50  0001 C CNN
F 1 "PWR_FLAG" H 8800 4573 50  0000 C CNN
F 2 "" H 8800 4400 50  0001 C CNN
F 3 "~" H 8800 4400 50  0001 C CNN
	1    8800 4400
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG02
U 1 1 63927B5B
P 8800 4800
F 0 "#FLG02" H 8800 4875 50  0001 C CNN
F 1 "PWR_FLAG" H 8800 4973 50  0000 C CNN
F 2 "" H 8800 4800 50  0001 C CNN
F 3 "~" H 8800 4800 50  0001 C CNN
	1    8800 4800
	-1   0    0    1   
$EndComp
Wire Wire Line
	9050 4750 8800 4750
Wire Wire Line
	8800 4750 8800 4800
Connection ~ 9050 4750
Wire Wire Line
	9050 4450 8800 4450
Wire Wire Line
	8800 4450 8800 4400
Connection ~ 9050 4450
Wire Wire Line
	2800 2150 3400 2150
Wire Wire Line
	2800 2250 3400 2250
Text Label 3400 2150 2    50   ~ 0
XTAL1
Text Label 3400 2250 2    50   ~ 0
XTAL2
Wire Wire Line
	7850 1250 7850 1350
Connection ~ 7850 1350
Wire Wire Line
	8550 1250 8550 1350
Connection ~ 8550 1350
Wire Wire Line
	9200 1250 9200 1350
Connection ~ 9200 1350
Wire Wire Line
	9850 1250 9850 1350
Connection ~ 9850 1350
Wire Wire Line
	10500 1250 10500 1350
Connection ~ 10500 1350
Wire Wire Line
	7850 2250 7850 2350
Connection ~ 7850 2350
Wire Wire Line
	8500 2250 8500 2350
Connection ~ 8500 2350
Wire Wire Line
	9200 2250 9200 2350
Connection ~ 9200 2350
Wire Wire Line
	9850 2250 9850 2350
Connection ~ 9850 2350
Wire Wire Line
	10500 2250 10500 2350
Connection ~ 10500 2350
Text Label 3400 2750 2    50   ~ 0
US3
Wire Wire Line
	2800 2750 3400 2750
Text Label 3400 2650 2    50   ~ 0
US2
Wire Wire Line
	2800 2650 3400 2650
Text Label 3400 2550 2    50   ~ 0
US1
Wire Wire Line
	2800 2550 3400 2550
Text Label 3400 2450 2    50   ~ 0
US0
Wire Wire Line
	2800 2450 3400 2450
Wire Wire Line
	2800 1650 3400 1650
Text Label 3400 1650 2    50   ~ 0
LED_STAT
Wire Wire Line
	5450 5900 4850 5900
Text Label 4850 5900 0    50   ~ 0
LED_STAT
$Comp
L Device:R R3
U 1 1 63FC37DE
P 5450 6050
F 0 "R3" H 5520 6096 50  0000 L CNN
F 1 "500" H 5520 6005 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5380 6050 50  0001 C CNN
F 3 "~" H 5450 6050 50  0001 C CNN
	1    5450 6050
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 63FC419C
P 5450 6350
F 0 "D1" V 5489 6232 50  0000 R CNN
F 1 "Status" V 5398 6232 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 5450 6350 50  0001 C CNN
F 3 "~" H 5450 6350 50  0001 C CNN
	1    5450 6350
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR031
U 1 1 63FC4DF5
P 5450 6500
F 0 "#PWR031" H 5450 6250 50  0001 C CNN
F 1 "GND" H 5455 6327 50  0000 C CNN
F 2 "" H 5450 6500 50  0001 C CNN
F 3 "" H 5450 6500 50  0001 C CNN
	1    5450 6500
	1    0    0    -1  
$EndComp
$Comp
L Interface_UART:MAX485E U2
U 1 1 64065BED
P 1950 6150
F 0 "U2" H 1700 6600 50  0000 C CNN
F 1 "MAX485E" H 2200 6600 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 1950 5450 50  0001 C CNN
F 3 "https://datasheets.maximintegrated.com/en/ds/MAX1487E-MAX491E.pdf" H 1950 6200 50  0001 C CNN
	1    1950 6150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR022
U 1 1 64067019
P 1950 6750
F 0 "#PWR022" H 1950 6500 50  0001 C CNN
F 1 "GND" H 1955 6577 50  0000 C CNN
F 2 "" H 1950 6750 50  0001 C CNN
F 3 "" H 1950 6750 50  0001 C CNN
	1    1950 6750
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR021
U 1 1 64067D15
P 1950 5650
F 0 "#PWR021" H 1950 5500 50  0001 C CNN
F 1 "+5V" H 1965 5823 50  0000 C CNN
F 2 "" H 1950 5650 50  0001 C CNN
F 3 "" H 1950 5650 50  0001 C CNN
	1    1950 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 6050 1050 6050
Wire Wire Line
	1550 6350 1050 6350
Text Label 1050 6050 0    50   ~ 0
UART_RXD
Text Label 1050 6350 0    50   ~ 0
UART_TXD
Wire Wire Line
	1550 6250 1550 6200
Wire Wire Line
	1550 6200 1050 6200
Connection ~ 1550 6200
Wire Wire Line
	1550 6200 1550 6150
Text Label 1050 6200 0    50   ~ 0
RS485_TXE
$Comp
L Device:R R2
U 1 1 640A12BC
P 2450 6200
F 0 "R2" H 2520 6246 50  0000 L CNN
F 1 "120R" H 2520 6155 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 2380 6200 50  0001 C CNN
F 3 "~" H 2450 6200 50  0001 C CNN
	1    2450 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 6350 2450 6350
Wire Wire Line
	2450 6350 3000 6350
Connection ~ 2450 6350
Wire Wire Line
	2350 6050 2450 6050
$Comp
L Connector:Conn_01x04_Male J4
U 1 1 640B3EE1
P 3200 6350
F 0 "J4" H 3172 6232 50  0000 R CNN
F 1 "RS485" H 3172 6323 50  0000 R CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-04A_1x04_P2.54mm_Vertical" H 3200 6350 50  0001 C CNN
F 3 "~" H 3200 6350 50  0001 C CNN
	1    3200 6350
	-1   0    0    1   
$EndComp
Wire Wire Line
	3000 6250 2750 6250
Wire Wire Line
	2750 6250 2750 6050
Wire Wire Line
	2750 6050 2450 6050
Connection ~ 2450 6050
Wire Wire Line
	3000 6150 2950 6150
Wire Wire Line
	2950 6150 2950 6500
Wire Wire Line
	3000 6450 2850 6450
Wire Wire Line
	2850 6450 2850 6050
$Comp
L power:+5V #PWR029
U 1 1 640F3BCA
P 2850 6050
F 0 "#PWR029" H 2850 5900 50  0001 C CNN
F 1 "+5V" H 2865 6223 50  0000 C CNN
F 2 "" H 2850 6050 50  0001 C CNN
F 3 "" H 2850 6050 50  0001 C CNN
	1    2850 6050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR030
U 1 1 640F4168
P 2950 6500
F 0 "#PWR030" H 2950 6250 50  0001 C CNN
F 1 "GND" H 2955 6327 50  0000 C CNN
F 2 "" H 2950 6500 50  0001 C CNN
F 3 "" H 2950 6500 50  0001 C CNN
	1    2950 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 3250 3400 3250
Wire Wire Line
	2800 3350 3400 3350
Text Label 3400 3250 2    50   ~ 0
UART_RXD
Text Label 3400 3350 2    50   ~ 0
UART_TXD
Text Label 3400 1750 2    50   ~ 0
RS485_TXE
Wire Wire Line
	2800 1750 3400 1750
$EndSCHEMATC