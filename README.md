# Pi-loop-drumbox!


## Install requirements
### PureData
To install puredata: `sudo apt-get install puredata`. PD version used: 0.51-1.  
No extensions are required.  
In order to get the soundcard working and avoiding the  
`ALSA input error (snd_pcm_open): No such file or directory`  
error, you need to make sure that the USB-soundcard is the only audio device and has index 0. Disable the Pi's jack input via this file: `/etc/modprobe.d/alsa-blacklist.conf` and add the following line: `blacklist snd_bcm2835`.   
Then, reconfigure the index of audio devices: `/lib/modprobe.d/aliases.conf` and change the index of `options snd-usb-audio index=0`.  
Check if the only audio device is the USB-soundcard through `cat /proc/asound/cards`, which should give the index 0 to the soundcard. 

### Python
The Python code is based on Python3. 


