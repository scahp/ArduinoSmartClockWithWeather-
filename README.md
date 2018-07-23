Hi,
This is arduino project for smart clock which shows realtime weather information
from OpenAPI web site.

this is the result. below two links
https://youtu.be/exXg7b-zNbs
https://youtu.be/WMAwHNbVTZ8

-------------------------------------------------------------
Software info
-------------------------------------------------------------
This is just software part for arduino.
arduino smart weather source code, and biatmap to arduino image converter (c#).

This smart clock is supposed to be connected with wifi to contact web server.
The server gives arduino current time and information from Apach web server.
and also you can customize smart weather infomation line which shows bottom in the dot matrix.

The web site format is simple. just print text and color RGB. see the below.
(RGB color value is 0~7 respectively)

[SKY_D03][131]      // [Fixed]ImageName, RGB

[20180724][333]     // [Fixed]Data, RGB

[030157][555]       // [Fixed]Time, RGB

[2][330]            // [Fixed]Day of week by apach server expression

[1][000]            // [Fixed]NightMode Color

[25/36C][700]       // Informations... this and below line you can add dynamically.

[PM:25][242]

[29C/74%][330]

[D+107][433]


I just open this code for anyone interesting to this project.
I wish you could just get what you need in this code.

-------------------------------------------------------------
Hardware info
-------------------------------------------------------------
Board
- Arduino Mega2560

Modules
- Led Matrix 64 x 32 (SMD HD P5 Inddor)
- Clock Module (DS1302)
- ESP8266 WIFI Module
- PHP Webserver('SK Planet' Weather Open API)
