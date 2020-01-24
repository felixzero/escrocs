EESchema Schematic File Version 4
LIBS:carte-radio-cache
EELAYER 26 0
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
L RF_AM_FM:RFM69HCW U1
U 1 1 5DD6F993
P 3550 1950
F 0 "U1" H 3350 2400 50  0000 C CNN
F 1 "RFM69HCW" H 3800 2400 50  0000 C CNN
F 2 "carte-radio:RFM69HCW-XXXS2" H 250 3600 50  0001 C CNN
F 3 "http://www.hoperf.com/upload/rf/RFM69HCW-V1.1.pdf" H 250 3600 50  0001 C CNN
	1    3550 1950
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_Coaxial J1
U 1 1 5DD6FAE2
P 4700 1650
F 0 "J1" H 4799 1626 50  0000 L CNN
F 1 "Conn_Coaxial" H 4799 1535 50  0000 L CNN
F 2 "Connector_Coaxial:SMA_Samtec_SMA-J-P-H-ST-EM1_EdgeMount" H 4700 1650 50  0001 C CNN
F 3 " ~" H 4700 1650 50  0001 C CNN
	1    4700 1650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5DD6FC29
P 4700 1850
F 0 "#PWR02" H 4700 1600 50  0001 C CNN
F 1 "GND" H 4705 1677 50  0000 C CNN
F 2 "" H 4700 1850 50  0001 C CNN
F 3 "" H 4700 1850 50  0001 C CNN
	1    4700 1850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5DD6FC6F
P 3550 2650
F 0 "#PWR01" H 3550 2400 50  0001 C CNN
F 1 "GND" H 3555 2477 50  0000 C CNN
F 2 "" H 3550 2650 50  0001 C CNN
F 3 "" H 3550 2650 50  0001 C CNN
	1    3550 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 2550 3550 2550
Wire Wire Line
	3450 2550 3550 2550
Connection ~ 3550 2550
Wire Wire Line
	3550 2550 3550 2650
$Comp
L power:GND #PWR03
U 1 1 5DD6FCC7
P 6950 5000
F 0 "#PWR03" H 6950 4750 50  0001 C CNN
F 1 "GND" H 6955 4827 50  0000 C CNN
F 2 "" H 6950 5000 50  0001 C CNN
F 3 "" H 6950 5000 50  0001 C CNN
	1    6950 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 4950 6850 4950
Connection ~ 6950 4950
Wire Wire Line
	6950 4950 6950 5000
Text GLabel 7050 2850 1    50   Input ~ 0
3V3
Text GLabel 2250 850  0    50   Input ~ 0
3V3
Wire Wire Line
	4050 1650 4500 1650
$Comp
L Transistor_FET:2N7000 Q1
U 1 1 5DD6FF0D
P 2050 1350
F 0 "Q1" V 2300 1350 50  0000 C CNN
F 1 "2N7000" V 2391 1350 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline_Wide" H 2250 1275 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7000.pdf" H 2050 1350 50  0001 L CNN
	1    2050 1350
	0    -1   1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 5DD701BF
P 1650 1250
F 0 "R1" H 1720 1296 50  0000 L CNN
F 1 "10k" H 1720 1205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 1580 1250 50  0001 C CNN
F 3 "~" H 1650 1250 50  0001 C CNN
	1    1650 1250
	1    0    0    -1  
$EndComp
Text GLabel 7150 2850 1    50   Input ~ 0
5V
Text GLabel 1600 850  0    50   Input ~ 0
5V
Wire Wire Line
	1650 1100 1650 850 
Wire Wire Line
	1650 850  1600 850 
$Comp
L Device:R R6
U 1 1 5DD700E2
P 2300 1200
F 0 "R6" H 2370 1246 50  0000 L CNN
F 1 "10k" H 2370 1155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2230 1200 50  0001 C CNN
F 3 "~" H 2300 1200 50  0001 C CNN
	1    2300 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 1150 2050 1000
Wire Wire Line
	2050 1000 2300 1000
Wire Wire Line
	2250 1450 2300 1450
Wire Wire Line
	1850 1450 1650 1450
Wire Wire Line
	1650 1450 1650 1400
Wire Wire Line
	1650 1450 1300 1450
Connection ~ 1650 1450
Wire Wire Line
	2300 1350 2300 1450
Wire Wire Line
	2300 1050 2300 1000
Connection ~ 2300 1000
Wire Wire Line
	2300 1000 2300 850 
Wire Wire Line
	2300 850  2250 850 
Text GLabel 2250 1850 0    50   Input ~ 0
3V3
$Comp
L Transistor_FET:2N7000 Q2
U 1 1 5DD7361D
P 2050 2350
F 0 "Q2" V 2300 2350 50  0000 C CNN
F 1 "2N7000" V 2391 2350 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline_Wide" H 2250 2275 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7000.pdf" H 2050 2350 50  0001 L CNN
	1    2050 2350
	0    -1   1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 5DD73624
P 1650 2250
F 0 "R2" H 1720 2296 50  0000 L CNN
F 1 "10k" H 1720 2205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 1580 2250 50  0001 C CNN
F 3 "~" H 1650 2250 50  0001 C CNN
	1    1650 2250
	1    0    0    -1  
$EndComp
Text GLabel 1600 1850 0    50   Input ~ 0
5V
Wire Wire Line
	1650 2100 1650 1850
Wire Wire Line
	1650 1850 1600 1850
$Comp
L Device:R R7
U 1 1 5DD7362E
P 2300 2200
F 0 "R7" H 2370 2246 50  0000 L CNN
F 1 "10k" H 2370 2155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2230 2200 50  0001 C CNN
F 3 "~" H 2300 2200 50  0001 C CNN
	1    2300 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 2150 2050 2000
Wire Wire Line
	2050 2000 2300 2000
Wire Wire Line
	2250 2450 2300 2450
Wire Wire Line
	1850 2450 1650 2450
Wire Wire Line
	1650 2450 1650 2400
Wire Wire Line
	2300 2350 2300 2450
Connection ~ 2300 2450
Wire Wire Line
	2300 2050 2300 2000
Connection ~ 2300 2000
Wire Wire Line
	2300 2450 2750 2450
Wire Wire Line
	2300 2000 2300 1850
Wire Wire Line
	2300 1850 2250 1850
Text GLabel 3500 1100 0    50   Input ~ 0
3V3
Wire Wire Line
	3550 1100 3500 1100
Wire Wire Line
	3550 1100 3550 1450
Text GLabel 2250 2850 0    50   Input ~ 0
3V3
$Comp
L Transistor_FET:2N7000 Q3
U 1 1 5DD75F61
P 2050 3350
F 0 "Q3" V 2300 3350 50  0000 C CNN
F 1 "2N7000" V 2391 3350 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline_Wide" H 2250 3275 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7000.pdf" H 2050 3350 50  0001 L CNN
	1    2050 3350
	0    -1   1    0   
$EndComp
$Comp
L Device:R R3
U 1 1 5DD75F68
P 1650 3250
F 0 "R3" H 1720 3296 50  0000 L CNN
F 1 "10k" H 1720 3205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 1580 3250 50  0001 C CNN
F 3 "~" H 1650 3250 50  0001 C CNN
	1    1650 3250
	1    0    0    -1  
$EndComp
Text GLabel 1600 2850 0    50   Input ~ 0
5V
Wire Wire Line
	1650 3100 1650 2850
Wire Wire Line
	1650 2850 1600 2850
$Comp
L Device:R R8
U 1 1 5DD75F72
P 2300 3200
F 0 "R8" H 2370 3246 50  0000 L CNN
F 1 "10k" H 2370 3155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2230 3200 50  0001 C CNN
F 3 "~" H 2300 3200 50  0001 C CNN
	1    2300 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 3150 2050 3000
Wire Wire Line
	2050 3000 2300 3000
Wire Wire Line
	2250 3450 2300 3450
Wire Wire Line
	1850 3450 1650 3450
Wire Wire Line
	1650 3450 1650 3400
Wire Wire Line
	1650 3450 1300 3450
Connection ~ 1650 3450
Wire Wire Line
	2300 3350 2300 3450
Connection ~ 2300 3450
Wire Wire Line
	2300 3050 2300 3000
Connection ~ 2300 3000
Wire Wire Line
	2300 3000 2300 2850
Wire Wire Line
	2300 2850 2250 2850
Wire Wire Line
	2750 1750 2750 2450
Wire Wire Line
	2800 1850 2800 3450
Wire Wire Line
	2300 3450 2800 3450
Text GLabel 2250 3850 0    50   Input ~ 0
3V3
$Comp
L Transistor_FET:2N7000 Q4
U 1 1 5DD7D99B
P 2050 4350
F 0 "Q4" V 2300 4350 50  0000 C CNN
F 1 "2N7000" V 2391 4350 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline_Wide" H 2250 4275 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7000.pdf" H 2050 4350 50  0001 L CNN
	1    2050 4350
	0    -1   1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 5DD7D9A2
P 1650 4250
F 0 "R4" H 1720 4296 50  0000 L CNN
F 1 "10k" H 1720 4205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 1580 4250 50  0001 C CNN
F 3 "~" H 1650 4250 50  0001 C CNN
	1    1650 4250
	1    0    0    -1  
$EndComp
Text GLabel 1600 3850 0    50   Input ~ 0
5V
Wire Wire Line
	1650 4100 1650 3850
Wire Wire Line
	1650 3850 1600 3850
$Comp
L Device:R R9
U 1 1 5DD7D9AC
P 2300 4200
F 0 "R9" H 2370 4246 50  0000 L CNN
F 1 "10k" H 2370 4155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2230 4200 50  0001 C CNN
F 3 "~" H 2300 4200 50  0001 C CNN
	1    2300 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 4150 2050 4000
Wire Wire Line
	2050 4000 2300 4000
Wire Wire Line
	2250 4450 2300 4450
Wire Wire Line
	1850 4450 1650 4450
Wire Wire Line
	1650 4450 1650 4400
Connection ~ 1650 4450
Wire Wire Line
	2300 4350 2300 4450
Connection ~ 2300 4450
Wire Wire Line
	2300 4050 2300 4000
Connection ~ 2300 4000
Wire Wire Line
	2300 4000 2300 3850
Wire Wire Line
	2300 3850 2250 3850
Text GLabel 2850 4800 0    50   Input ~ 0
3V3
$Comp
L Transistor_FET:2N7000 Q5
U 1 1 5DD7F05F
P 2650 5300
F 0 "Q5" V 2900 5300 50  0000 C CNN
F 1 "2N7000" V 2991 5300 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline_Wide" H 2850 5225 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N7000.pdf" H 2650 5300 50  0001 L CNN
	1    2650 5300
	0    -1   1    0   
$EndComp
$Comp
L Device:R R5
U 1 1 5DD7F066
P 2250 5200
F 0 "R5" H 2320 5246 50  0000 L CNN
F 1 "10k" H 2320 5155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2180 5200 50  0001 C CNN
F 3 "~" H 2250 5200 50  0001 C CNN
	1    2250 5200
	1    0    0    -1  
$EndComp
Text GLabel 2200 4800 0    50   Input ~ 0
5V
Wire Wire Line
	2250 5050 2250 4800
Wire Wire Line
	2250 4800 2200 4800
$Comp
L Device:R R10
U 1 1 5DD7F070
P 2900 5150
F 0 "R10" H 2970 5196 50  0000 L CNN
F 1 "10k" H 2970 5105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2830 5150 50  0001 C CNN
F 3 "~" H 2900 5150 50  0001 C CNN
	1    2900 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2650 5100 2650 4950
Wire Wire Line
	2650 4950 2900 4950
Wire Wire Line
	2850 5400 2900 5400
Wire Wire Line
	2450 5400 2250 5400
Wire Wire Line
	2250 5400 2250 5350
Connection ~ 2250 5400
Wire Wire Line
	2900 5300 2900 5400
Connection ~ 2900 5400
Wire Wire Line
	2900 5000 2900 4950
Connection ~ 2900 4950
Wire Wire Line
	2900 4950 2900 4800
Wire Wire Line
	2900 4800 2850 4800
Wire Wire Line
	2750 1750 3050 1750
Wire Wire Line
	2800 1850 3050 1850
Wire Wire Line
	3050 1950 2900 1950
Wire Wire Line
	2900 1950 2900 4450
Wire Wire Line
	2300 4450 2900 4450
Wire Wire Line
	4050 2350 4150 2350
Wire Wire Line
	4150 2350 4150 5400
Wire Wire Line
	1250 4450 1650 4450
NoConn ~ 3050 2150
NoConn ~ 4050 1850
NoConn ~ 4050 1950
NoConn ~ 4050 2050
NoConn ~ 4050 2150
NoConn ~ 4050 2250
Wire Wire Line
	1650 2450 1300 2450
Connection ~ 1650 2450
Text GLabel 1300 1450 0    50   Input ~ 0
RF_SCK
Text GLabel 1300 2450 0    50   Input ~ 0
RF_MOSI
Text GLabel 1300 3450 0    50   Input ~ 0
RF_MISO
Text GLabel 1250 4450 0    50   Input ~ 0
RF_NSS
Text GLabel 1500 5400 0    50   Input ~ 0
RF_INT
Text Notes 2550 5700 0    50   ~ 0
RF Transceiver
Wire Notes Line
	750  5850 750  700 
Wire Notes Line
	750  700  5350 700 
Wire Notes Line
	5350 700  5350 5850
Wire Notes Line
	5350 5850 750  5850
NoConn ~ 7450 4650
NoConn ~ 7450 4550
NoConn ~ 7450 4350
NoConn ~ 7450 4250
NoConn ~ 7450 4150
NoConn ~ 7450 4050
NoConn ~ 7450 3950
NoConn ~ 7450 3650
NoConn ~ 7450 3450
NoConn ~ 7450 3250
NoConn ~ 6450 3250
NoConn ~ 6450 3350
Text GLabel 6450 4350 0    50   Input ~ 0
RF_MOSI
Text GLabel 6450 4450 0    50   Input ~ 0
RF_MISO
Text GLabel 6450 4550 0    50   Input ~ 0
RF_SCK
Text GLabel 6450 4250 0    50   Input ~ 0
RF_NSS
Text GLabel 6450 3450 0    50   Input ~ 0
RF_INT
NoConn ~ 6450 3550
NoConn ~ 6450 4150
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5DD8502D
P 7300 5000
F 0 "#FLG0101" H 7300 5075 50  0001 C CNN
F 1 "PWR_FLAG" H 7300 5173 50  0000 C CNN
F 2 "" H 7300 5000 50  0001 C CNN
F 3 "~" H 7300 5000 50  0001 C CNN
	1    7300 5000
	-1   0    0    1   
$EndComp
Wire Wire Line
	7300 5000 7300 4950
NoConn ~ 6850 2850
Wire Wire Line
	7300 4950 7050 4950
Wire Wire Line
	7050 4950 6950 4950
Connection ~ 7050 4950
$Comp
L MCU_Module:Arduino_UNO_R3 A1
U 1 1 5DD6F365
P 6950 3850
F 0 "A1" H 6950 4250 50  0000 C CNN
F 1 "Arduino_UNO_R3" H 6950 5100 50  0000 C CNN
F 2 "Module:Arduino_UNO_R3" H 7100 2800 50  0001 L CNN
F 3 "https://www.arduino.cc/en/Main/arduinoBoardUno" H 6750 4900 50  0001 C CNN
	1    6950 3850
	1    0    0    -1  
$EndComp
NoConn ~ 6450 4050
Wire Wire Line
	2100 5400 2250 5400
NoConn ~ 7450 3850
NoConn ~ 6450 3650
NoConn ~ 6450 3750
NoConn ~ 6450 3850
NoConn ~ 6450 3950
$Comp
L Device:Jumper JP1
U 1 1 5DE028AF
P 2600 1450
F 0 "JP1" H 2600 1714 50  0000 C CNN
F 1 "Jumper" H 2600 1623 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" H 2600 1450 50  0001 C CNN
F 3 "~" H 2600 1450 50  0001 C CNN
	1    2600 1450
	1    0    0    -1  
$EndComp
Connection ~ 2300 1450
Wire Wire Line
	3050 1650 2900 1650
Wire Wire Line
	2900 1650 2900 1450
$Comp
L Device:Jumper JP2
U 1 1 5DE0BD34
P 1800 5400
F 0 "JP2" H 1800 5664 50  0000 C CNN
F 1 "Jumper" H 1800 5573 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P15.24mm_Horizontal" H 1800 5400 50  0001 C CNN
F 3 "~" H 1800 5400 50  0001 C CNN
	1    1800 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 5400 4150 5400
$EndSCHEMATC
