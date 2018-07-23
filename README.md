안녕하세요,
이 프로젝트는 실시간 날씨 정보를 오픈API 웹사이트를 통해 
출력하는 아두이노 스마트 시계 프로젝트입니다. 

프로젝트의 결과물 모습은 아래의 두 링크를 통해 보실수있습니다.
https://youtu.be/exXg7b-zNbs
https://youtu.be/WMAwHNbVTZ8

-------------------------------------------------------------
Software info
-------------------------------------------------------------
여기서 공개된 코드는 아두이노의 소프트웨어 파트 입니다.
아두이노 스마트 날씨 소스 코드와 비트맵을 아두이노에서 쓸수있게 컨버팅 해주는 c# 코드입니다.

이 시계는 와이파이와 연결되어있어 웹서버와 연결되어 있어야 합니다.
서버는 아두이노에게 현재시간과 정보를 아파치 웹서버를 통해 전달합니다.
그리고 도트매트릭스 하단에 표시되는 스마트 날씨 정보 줄을 커스터마이즈 할 수 있습니다.

웹서버 사이트의 포맷은 간단합니다. 어떤 텍스트를 표시할지와 RGB 값입니다. 아래를 보면,
(RGB 컬러값의 범위는 각각 0~7 입니다.)

[SKY_D03][131]      // [고정]ImageName, RGB

[20180724][333]     // [고정]Data, RGB

[030157][555]       // [고정]Time, RGB

[2][330]            // [고정]Day of week by apach server expression

[1][000]            // [고정]NightMode Color

[25/36C][700]       // 추가정보들... 이 라인 부터는 자유롭게 커스터마이즈 가능

[PM:25][242]

[29C/74%][330]

[D+107][433]


이 프로젝트에 관심있는 분들을 위해 코드를 공개합니다.
이 코드에서 필요한 부분을 얻어가셨으면 좋겠습니다.


-------------------------------------------------------------


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
