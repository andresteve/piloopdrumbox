#!/bin/sh


echo starting Looper
cd /home/pi/piLooper/python
xterm -T "Looper" -geometry 100x70+10+35 -hold -e sudo /usr/bin/python3 main.py
