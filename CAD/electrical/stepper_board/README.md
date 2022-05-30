# Stepper board

A board, based on an ATMega168PA, meant to control up to 3 1.5A bipolar steppers (the most common kind, used in 3D printers for instance). It can also control up to 3 12V pumps.

Its code is in the `code/stepper_board` folder.

## Known issues

The steppers stay actively driven all the same. Over prolonged use of time, the card can get really hot, we recommend to use heatsinks on the stepper drivers.

A workaround to permit the microcontroller to switch off the drivers is the set the fuse in such a way that the RESET pin is a regular I/O of the ATMega and let the ATMega hold RESET down when the steppers are not in use. Beware: doing so will prevent reflashing the board.
