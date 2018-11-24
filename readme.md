# Seiba car
This is the code repo for a Raspberry Pi car racing competition in Introduction to Embedded System (Fall 2017) at NJU. 

The following gif shows our car during the final. 

![](https://raw.githubusercontent.com/Hirico/hirico.github.io/master/project/car.gif)

The home of this competition is at [here](https://github.com/miaoxw/EmbeddedSystemNJU2018-Demo), where more detailed information about the underlying hardware & software is provided. The camera on our car is Logitech C920. 

# The Tricky Parts
* For the computer vision, you want a pipeline that can see the bold lanes as clear as possible, filter all other info (the environment is really messy), and can be run fast on a cheap ARM Cortex-A53. Cropping a region, resizing the gray image to 90P, and applying blur and dilation turns out to be pretty good for pre-processing.

* In this competition, time is scarce for tuning parameters, yet good parameters can give you significant advantage. Implementing a manual controller with a parameter file can give you the ability to one-key change any parameter without recompilation, and will save you countless time in iterations. More importantly, you will gain full control of your car and get rid of writing (many) test cases. Just remember to switch to your main autonomous program when the real racing starts.