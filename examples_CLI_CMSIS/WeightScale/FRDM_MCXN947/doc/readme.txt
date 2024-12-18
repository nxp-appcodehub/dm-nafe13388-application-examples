Hardware requirements
===================
- Mini/micro C USB cable
- FRDM-MCXN947 board
- NAFE13388-UIM board
- Personal Computer

Hardware settings

The needed hardware are:

Power supply of 15 V or 15 V Adapter.
USB cable.
Jumper wires
FRDM-MCXN947 Base Board
NAFE13388-UIM Shield Board (analog front-end)
4-wire RTD Sensor ( RTD 2/4-wire applications).
220 ohm resistor (current sensing application).
Load cell (weight-scale application).
External power supply in order to try different voltage readings (voltage-sensing application).

Hardware connections details:

SGM3209YS8G/TR IC U20 (J90 1x2) The SGM3209 is a charge pump voltage converter which is used to generate a negative supply from a positive input. J90(1*2). It generates -15 V when we apply +15 V using external voltage source.

JUMPER (default) = CLOSED
OUTPUT pin of IC U20
-15 V
AICOM - J91 (CLOSED)

Analog inputs: For analog inputs use jumpers J82 and J83.

External voltage source -> J89 1- GND 2- +15 V

Settings for selection of EDMA/Interrupt Mode:

Note: Set RTE_SPI1_DMA_EN to 1 for EDMA mode and RTE_SPI1_DMA_EN to 0 for Interrupt mode in board/RTE_Device.h

Prepare the Demo
===============
1.  Connect a USB cable between the host PC and the OpenSDA USB port on the target board.
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

Running the demo
===============
When the demo runs successfully, you can see the logs printed on the terminal based on the menu options user chooses.

LOGS:
=============

----------------WELCOME TO NAFE MCXN DEMO---------------

Weight Scale (Load Cell)

********************

Three Step Procedure:

Step1: Offset Calculation

Step2: Calibration Coefficient Calculation

Step3: Actual Weight Calculation

********************

Press any key to start the procedure

********************

-> Calculation of Offset <-

-> Step 1:Offset Calculation <-

Leave the weight scale without weight and click any buttons.

Press any key to start conversion
-----------------------------------
-> Step 1: Done <-

********************

-> Step 2 : Calibration Coefficient Calculation <-

For First Calculation Put Known Weight for Calibration

Put an object of known weight on the scale

Please enter the weight value in grams and then click enter key

The weight on the scale is 303g, do you confirm(y/n)?

-> Step 2: Done <-

********************

->Click any button to start one Conversion sequences<-

Voltage Load Cell=0.525475mV, Weight=302.239899g

********************

Put any item to calculate its weight

->Click any button to start one Conversion sequences<-

Voltage Load Cell=0.525649mV, Weight=302.908478g

********************

Put any item to calculate its weight

->Click any button to start one Conversion sequences<-

Voltage Load Cell=0.525693mV, Weight=303.076996g

********************

Put any item to calculate its weight

->Click any button to start one Conversion sequences<-

Voltage Load Cell=0.525581mV, Weight=302.648407g

********************

Put any item to calculate its weight

->Click any button to start one Conversion sequences<-

Voltage Load Cell=0.448378mV, Weight=6.568249g

********************

Put any item to calculate its weight

->Click any button to start one Conversion sequences<-

Voltage Load Cell=0.448418mV, Weight=6.719488g

********************

Put any item to calculate its weight

->Click any button to start one Conversion sequences<-

Voltage Load Cell=0.450370mV, Weight=14.205552g

********************

Put any item to calculate its weight

->Click any button to start one Conversion sequences<-

Voltage Load Cell=0.576778mV, Weight=498.994690g

********************

Put any item to calculate its weight

->Click any button to start one Conversion sequences<-