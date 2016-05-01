# Oberstbrau
This is a brewing automaton project for the Oberstbräu brewery.
It's controlled by a WeMos D1 mini running Arduino code, and the UI is Blynk.

The brewery will consist of:
- 98 litre MLT/BK with 3x 3500W heating elements
- 130 litre HLT with a 3500W heating element
- HERMS in a 2000W kettle
- circulation pump, mash water pump and sparging water pump
- mash bag hanging from rails in the ceiling
- mash stirrer
- lift/sink table
- copper spiral chiller

Implemented code:
- Blynk integration
- DS18B20 temperature readings
- PID control for HLT
- PID control for HERMS

To do:
- Lift control
- Water volume pump control
- Stirrer control
