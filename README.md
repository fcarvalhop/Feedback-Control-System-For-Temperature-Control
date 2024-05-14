
### Hi there👋

This project was developed as part of the Feedback Control Systems Lab course during my undergraduate studies. It was a collaborative effort, carried out in pairs <a href="https://github.com/luizpedrobt" target="_blank">Luiz Pedro Bittencourt</a>, with the objective of designing a digital PID controller to regulate temperature within a controlled environment.

<div align="center">
  <img src="https://github.com/luizpedrobt/FeedbackControlSystems_FinalProject/blob/main/images/8ec62bac-992b-421e-b064-076684428e57.jpeg" width="500">
</div>

## Physical assembly

The project entailed designing a PCB to integrate all the necessary circuits, which was accomplished using KiCad 7.0 software.

The core concept of the project was to utilize a diode as a temperature sensor. To achieve this, the diode was biased with a constant current source, allowing it to vary its voltage linearly in response to changes in ambient temperature. An analog-to-digital converter was then employed to sample the diode's voltage, with the sampled signal undergoing conditioning and filtering for accurate measurement.

<div align="center">
  <img src="https://github.com/luizpedrobt/FeedbackControlSystems_FinalProject/blob/main/images/esquemático.png" width="500">
</div>

We had the flexibility to choose any microcontroller unit (MCU) for implementing the controller. Therefore, my partner and I selected the STM32F411CEU6 blackpill development board. Initially, the project's prototype was developed using an Arduino UNO board.

The project's concept is straightforward: the MCU measures the temperature, compares it with a setpoint (default at 50°C but user-adjustable via Arduino serial communication), and generates a pulse-width modulation (PWM) signal with a duty cycle that varies based on the temperature-setpoint difference.

<div align="center">
  <img src="https://github.com/luizpedrobt/FeedbackControlSystems_FinalProject/blob/main/images/resposta_temperatura.png" width="500">
</div>

<div align="center">
  <img src="https://github.com/luizpedrobt/FeedbackControlSystems_FinalProject/blob/main/images/resposta_controlador.png" width="500">
</div>

Both the heating and cooling systems are controlled by the MCU using a BJT (bipolar junction transistor), although alternative drivers like MOSFETs can also be used. For heating, we used three 33-ohm resistors in parallel (10 W each), resulting in an equivalent resistance of 11 ohms. The project operates at a current close to 1.1 A, assuming a 12 V power supply. A LM7805 regulator was used to power both the Arduino board and other low-power circuits on the PCB.

## Characterization and equation formulation

For system equation formulation, we ran the heater at maximum power while keeping the cooler off to determine the upper heating limit of the plant. Then, upon reaching steady-state maximum accommodation, we activated the cooler at maximum to determine the lower heating limit of the plant. Using MATLAB, we determined the plant transfer function G(s), enabling us to design a suitable PID controller to meet all desired requirements.

<div align="center">
  <img src="https://github.com/luizpedrobt/FeedbackControlSystems_FinalProject/blob/main/images/planta.png" width="500">
</div>

The equation of the system in Laplace's domain is $`G(s) = \frac{-0.17}{74s + 1}`$.

## Firmware development

The code is very simple except for the 10th-order digital filter used. However, we utilized the code provided by our instructors and merely adapted it to our needs. Additionally, it includes two active interrupts: one for signal sampling every 25 ms, and another for changing the setpoint via serial, by rewriting the weak Serial.event() function from the Arduino API. The sampling time of 25 ms was chosen based on the anti-aliasing filter on-board with the cut frequency of 19.8 Hz.

## Conclusions

In conclusion, the project is perfectly working with the arduino, but it'll be implemented in the STM32 lately, star this repository if you don't want do lose any updates!😘
