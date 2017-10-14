# ESP_RC
ESP8266 mobile UDP stream to PPM

I wanted my Q100 SP3 EVO brushed to be controlled via mobile app (a Joystick).
forked the PPM routine from https://github.com/emente/evalink
it is timer driven PPM generated written wonderfully. 
Just load the pluse array with the received data from ESP8266 and it will sent out PPM on the defined PIN.

Forked the UDP with mobile app from https://github.com/QuadTinnakon/Quad230_v5_0_esp8266_androidWifi_GY521

ESP8266 soft app connected to mobile joystick app and gets the AETR data with 2 constant AUX values for ARMing and Angle mode selection.

Will upload the video once quad flies stable.
But can confirm that it works. But try it with great safty care.
