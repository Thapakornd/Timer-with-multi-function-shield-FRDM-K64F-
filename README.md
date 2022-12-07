# Timer-with-multi-function-shield-FRDM-K64F
Create timer range 10 sec - 99 min with multi function shield use in board FRDM K64F 
<br><br>In my mini project how to controll multi functional shield with FRDM K64F
# What module use in project?
-  7-segments
- Buzzer
- LED (D1-D4)
- Switch (Sw1 - Sw3)
# What function use in my source code?
- Timer with FTM and Systick
- Interrupt PORTB
# How it work
> At start status min is 0 and sec is 0
<p align="center">
  <img width="300" src="https://user-images.githubusercontent.com/89478647/206271479-1b03ed03-654a-4bd4-9d03-888824c7827b.jpg">
</p>

> Switch 1 (Count from left) : Increase +20 second
> <br>Switch 2 : Decrease -20 second
> <br>Switch 3 : Start timer
# Progress timer 
when timer is 0 : 0 buzzer and LED will turn on (while process LED will turn on from bottom to top)

<img width="300" src="https://user-images.githubusercontent.com/89478647/206280515-cb427792-f326-43d1-818e-199c3eacf772.jpg">
<img width="300" src="https://user-images.githubusercontent.com/89478647/206280797-9f4a9d29-0d6d-4ce3-bf21-ac0fad48a5c6.jpg">
<img width="300" src="https://user-images.githubusercontent.com/89478647/206286238-8badf2ff-5111-4181-83a6-fa20dc8f2a5b.jpg">
