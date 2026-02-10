# kmdproject
taica-softness-device

This device is a haptic device designed to capture softness of gel. It maps the relationship between displacement (angle) and pressure to calculate a stiffness index (K).

-----------------------------------------------

Short Press Button A: Zero - Resets the current P and A values to (0,0).

Long Press Button A (2s): Power Off

Short Press Button B: Switch Modes between 1.Raw Data+Wave Monitor and 2.XY Plot Analysis.

--

!!!!!IMPORTANT!!!!! 

At the exact moment you begin pressing the material, short-press Button A to zero the baseline. 
Maintain steady pressure for at least 2–3 seconds to generate a clear and accurate analysis curve.

--

#Hardware:

Controller: M5StickC Plus

Sensor: AS5600 (I2C) + Pressure Sensor (Analog Pin 36)

--

#How to Build:

Install Arduino IDE.

Install library: M5StickCPlus.

Flash softness_data.ino to the device.
