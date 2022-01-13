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
L Connector_Generic:Conn_02x06_Odd_Even J2
U 1 1 61C32033
P 6400 3100
F 0 "J2" H 6450 3517 50  0000 C CNN
F 1 "Edge slot" H 6450 3426 50  0000 C CNN
F 2 "custom:Roborock lidar connector" H 6400 3100 50  0001 C CNN
F 3 "~" H 6400 3100 50  0001 C CNN
	1    6400 3100
	1    0    0    -1  
$EndComp
NoConn ~ 6200 3400
NoConn ~ 6700 3400
$Comp
L Connector:Conn_01x06_Male J1
U 1 1 61C3356A
P 5500 3100
F 0 "J1" H 5608 3481 50  0000 C CNN
F 1 "Molex KK" H 5608 3390 50  0000 C CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-06A_1x06_P2.54mm_Vertical" H 5500 3100 50  0001 C CNN
F 3 "~" H 5500 3100 50  0001 C CNN
	1    5500 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 2900 6100 2900
Wire Wire Line
	6200 3000 5800 3000
Wire Wire Line
	6200 3100 5900 3100
Wire Wire Line
	6200 3200 6000 3200
Wire Wire Line
	6200 3300 6100 3300
Wire Wire Line
	6700 3300 6800 3300
Wire Wire Line
	6800 3300 6800 3500
Wire Wire Line
	6800 3500 6100 3500
Wire Wire Line
	6100 3500 6100 3300
Connection ~ 6100 3300
Wire Wire Line
	6700 3200 6900 3200
Wire Wire Line
	6900 3200 6900 3600
Wire Wire Line
	6900 3600 6000 3600
Wire Wire Line
	6700 3100 7000 3100
Wire Wire Line
	7000 3100 7000 3700
Wire Wire Line
	7000 3700 5900 3700
Wire Wire Line
	6700 3000 7100 3000
Wire Wire Line
	7100 3000 7100 3800
Wire Wire Line
	7100 3800 5800 3800
Wire Wire Line
	6700 2900 6800 2900
Wire Wire Line
	6800 2900 6800 2600
Wire Wire Line
	6800 2600 6100 2600
Wire Wire Line
	6100 2600 6100 2900
Connection ~ 6100 2900
Wire Wire Line
	6100 2900 5700 2900
NoConn ~ 5700 3400
Wire Wire Line
	5700 3300 6100 3300
Wire Wire Line
	6000 3600 6000 3200
Connection ~ 6000 3200
Wire Wire Line
	6000 3200 5700 3200
Wire Wire Line
	5900 3700 5900 3100
Connection ~ 5900 3100
Wire Wire Line
	5900 3100 5700 3100
Wire Wire Line
	5800 3800 5800 3000
Connection ~ 5800 3000
Wire Wire Line
	5800 3000 5700 3000
$EndSCHEMATC
