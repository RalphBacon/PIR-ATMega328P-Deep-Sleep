# PIR ATMega328P Deep-Sleep project
## See https://www.youtube.com/ralphbacon
### (Direct link to video: https://youtu.be/7ChTq7ZHVTI)

2-sided PCB just $5 for 10 pieces https://www.pcbway.com

This relatively simple project, ideal for a beginner, is a movement triggered alarm, as long as it is dark. 

<img src="/images/smallpir.JPG" align="left" width="40%" title="Small PIR">
    
It runs on 3 x AA batteries so the ATMega328P I'm using is put into Deep Sleep and consumes just 0.3µA, whereas the PIR consumes 16µA - quite a difference!

It's a easy project and I'll design a PCB for it using through-hole technology making it even easier for beginners to assemble without worrying about SMD components!

One of the benefits of a simple project like this is that it can be configured in a number of ways: instead of it having to be **dark** to trigger the alarm it can be made to be **light** instead. And the sensitivity is adjustable, anyway.

Don't want the beeper? Just use the LED and singel resistor then. You want the alarm to sound for more than 5 seconds? That a simple one-line code change!

You want to a different module to trigger the ATMega328P? Easily done.

The video contains a structured walkthrough, circuit diagram and various verbal meanderings from me including showing you the current consumption both with and without the PIR unit.

