// 아두이노의 남은 램 사이즈 체크. 아두이노 메가 사용시 메모리가 생각보다 부족하니 이상 동작시 수시로 체크 필요.
// To check ram size. you should check frequently too small memory cause wired behavior in arduino mega.
int freeRam() 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


///////////////////////////////////////////////////////////////////
// RTC Time
#define SCK_PIN 50
#define DAT_PIN 51
#define RST_PIN 52

#include <DS1302.h>

// DS1302 라이브러리 초기화
// Initialize DS1302 Library
DS1302 rtc(RST_PIN, DAT_PIN, SCK_PIN);

void InitRTC();
void PrintRTC(int x, int y, int textSize = 1);
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// LED MATRIX
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define OE   9
#define LAT 10
#define CLK 11
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true, 64);

bool usingDeepNightMode = false;
bool isDeepNight = false;
uint16_t deepNightColor = matrix.Color333(0, 1, 0);
void SetTextColor(uint16_t color)
{
  if (isDeepNight)
  {
    matrix.setTextColor(deepNightColor);
    return;
  }

  matrix.setTextColor(color);
}
///////////////////////////////////////////////////////////////////

#define SSID "WIFI ID"
#define PASS "WIFI PASSWORD"
#define DST_IP "WEB SERVER IP"
#define DST_PORT "WEB SERVER PORT"

#define WIFI Serial3

///////////////////////////////////////////////////////////////////
// IntervalUpdator
class IntervalUpdator
{
public:
  IntervalUpdator(unsigned long updateValue)
    : m_updateDelay(0), m_updateValue(updateValue)
  { }

  // 시간을 업데이트 하며, 만료되는 경우 리셋한다. (Update the interval time and when the time expired, reset time.)
  // @return 설정된 시간이 리셋된 경우 true, 그렇지 않은 경우 false (When the interval time reseted, return true if not false.)
  bool Update()
  {
    unsigned long millisTime = millis();
    m_interval = (millisTime - m_updateDelay);
    if (0 < m_interval)
    {
      m_updateDelay = millisTime + m_updateValue;
      return true;
    }
    return false;
  }

  // 시간이 만료되었는지 여부를 리턴. (just return the time interval is expired or not)
  // @return 설정된 시간간격이 아직 완료 되지 않은 경우 true, 그렇지 않으면 false. (When it is in interval time, return value is true, if not false.)
  bool UpdateWithoutReset()
  {
    Serial.println(m_updateValue);
    unsigned long millisTime = millis();
    m_interval = (millisTime - m_updateDelay);
    if (0 < m_interval)
    {
      return true;
    }
    return false;
  }

  // 시간 리셋 (Rest time.)
  void Reset()
  {
    m_updateDelay = millis() + m_updateValue;
  }

  // 1회 업데이트 시 증가 값 (The value which an increase per update)
  unsigned long GetUpdateValue() const
  {
    return m_updateValue;
  }

  // 현재 Interval 값 리턴 (Return the current interval)
  long GetCurrentInterval() const
  {
    return m_interval;
  }

private:
  unsigned long m_updateDelay;
  unsigned long m_updateValue;
  long m_interval;
};

IntervalUpdator intervalUpdator(10);

int maxInformationIndex = 0;
int currentInformationIndex = 0;
const int InformationArraySize = 10;
String informationString[InformationArraySize];
uint16_t informationStringColor[InformationArraySize];
uint16_t dateColor = matrix.Color333(3, 3, 3);
uint16_t timeColor = matrix.Color333(5, 5, 5);
uint16_t dowColor = matrix.Color333(3,0,0);
///////////////////////////////////////////////////////////////////

void ReadWIFISerial(char* readWifiSerial, unsigned int bufferMaxLength, unsigned int period);
void PrintWIFISerialToSerial(unsigned int period);
bool InitWIFI();
bool ConnectWiFi();
bool TryToConnectWIFI();
void GetIP();
void SetConnectionMode();
void ForwardWIFIToSerial();
void ForwardSerialToWIFI();
bool ConnectToServer();
bool SendHttpRequest();
void ReceiveResponseHttp();

#include "img_weather.h"

///////////////////////////////////////////////////////////////////
// Weather Info
const char* WeatherCode[] =
{
  "SKY_D00", "SKY_D01", "SKY_D02", "SKY_D03", "SKY_D04", "SKY_D05", "SKY_D06", "SKY_D07"
};

int GetWeatherIconIndex(const char* code)
{
  const int count = sizeof(WeatherCode) / sizeof(char*);
  for(int i=0;i<count;++i)
  {
    if (0 == strcmp(code, WeatherCode[i]))
    {
      return i;
    }
  }
  return -1;
}

//const unsigned char PROGMEM* currentWeatherDataPtr;
int weatherIconIndex = -1;
int minDegree = 0;
int maxDegree = 0;
bool isNight = false;
int deepNightStart = 22;
int deepNightEnd = 6;
void UpdateNight()
{
  Time time = rtc.getTime();
  isNight = ((6 > time.hour) || (18 < time.hour));
  isDeepNight = (usingDeepNightMode && ((deepNightEnd > time.hour) || (deepNightStart < time.hour)));
}
///////////////////////////////////////////////////////////////////

void DrawImage(int x, int y, int width, int height, char* imageArray)
{
  unsigned int count = 0;
  char first = 0;
  char second = 0;
  byte b = 0;
  byte g = 0;
  byte r = 0;
  byte a = 0;
  float alpha = 0.0f;
  int i, j;
  for(i=0;i<height;++i)
  {
    for(j=0;j<width;++j)
    {
      first = pgm_read_byte(&imageArray[count]);
      second = pgm_read_byte(&imageArray[count+1]);

      b = (first >> 3) & 0x07;
      g = first & 0x07;
      r = (second >> 3) & 0x07;
      a = second & 0x07;
      alpha = a / 7.0f;

      r = alpha * r;
      g = alpha * g;
      b = alpha * b;

      if (a)
      {
        matrix.drawPixel(x + j, y + i, (isDeepNight ? deepNightColor : matrix.Color333(r, g, b)));
      }
      count += 2;
    }
  }
}

void WeatherImageTest()
{
  const unsigned char PROGMEM* temp0 = 0;

  switch(weatherIconIndex)
  {
    case 0: temp0 = IMG_a38; break;
    case 1: temp0 = isNight ? IMG_a08 : IMG_a01 ; break;
    case 2: temp0 = isNight ? IMG_a09 : IMG_a02; break;
    case 3: temp0 = isNight ? IMG_a10 : IMG_a03; break;
    case 4: temp0 = IMG_a18; break;
    case 5: temp0 = IMG_a21; break;
    case 6: temp0 = IMG_a32; break;
    case 7: temp0 = IMG_a04; break;
  }

  if(temp0)
  {
    DrawImage(0, 0, 25, 25, temp0);
  }
}

int informationUpdateFailedCount = 0;
bool succeedWifiConnection = false;
bool InitWiFiConnection()
{
  if (InitWIFI())
  {
    if (TryToConnectWIFI())
    {
      GetIP();
      SetConnectionMode();
      return true;
    }
    else
    {
        Serial.println(F("WIFI Connection Fail"));
    }
  }
  else
  {
    Serial.println(F("WIFI Init Fail"));
  }

  return false;
}
bool UpdateInformation()
{
  if (ConnectToServer())
  {
    if (SendHttpRequest())
    {
      ReceiveResponseHttp();      
      return true;
    }
  }
  return false;
}

void setup() 
{
  for(int i=0;i<InformationArraySize;++i)
  {
    informationString[i].reserve(10);
    informationStringColor[i] = matrix.Color333(1, 3, 1);
  }
  Serial.begin(57600);

  matrix.begin();
  matrix.setTextSize(1);    // size 1 == 8 pixels high
  matrix.fillScreen(matrix.Color333(0,0,0));

  SetTextColor(matrix.Color333(7,7,7));
  matrix.setCursor(1, 2);
  matrix.print("Trying to");
  matrix.setCursor(16, 12);
  matrix.print("Connect");
  matrix.setCursor(36, 22);
  matrix.print("WIFI");
  matrix.swapBuffers(false);
  
  succeedWifiConnection = InitWiFiConnection();
  if (succeedWifiConnection)
  {
    succeedWifiConnection = UpdateInformation();
  }

  InitRTC();
  
  WeatherImageTest();

  Serial.println(F("=============================== end of Setup"));
}

void loop() {
  /*
  ForwardWIFIToSerial();
  ForwardSerialToWIFI();
  */

  // wifi update
  
  static IntervalUpdator wifiIntervalUpdator(1800000);     // 30분(1000 * 60 * 30ms) 마다 한번씩 업데이트 (Update per 30 min)
  if (wifiIntervalUpdator.UpdateWithoutReset())
  {
    Serial.println(succeedWifiConnection ? "connected" : "failed");
    if (!succeedWifiConnection)
    {
      succeedWifiConnection = InitWiFiConnection();
      informationUpdateFailedCount = 0;
    }
    
    if (succeedWifiConnection)
    {
      if (!UpdateInformation())
      {
        ++informationUpdateFailedCount;
      }

      // 실패를 3번 이상 하는 경우 실패했다는 정보 표시 해줌 (If it is failed 3 times, show the information about that)
      if (3 <= informationUpdateFailedCount)
      {
        succeedWifiConnection = false;
        //maxInformationIndex = 1;
        //currentInformationIndex = 0;
        //informationString[0] = "FailWIFI";
      }
    }

    if (succeedWifiConnection)
    {
      wifiIntervalUpdator.Reset();
    }
  }
  
  UpdateNight();
  matrix.fillScreen(matrix.Color333(0,0,0));
  WeatherImageTest();
  PrintRTC(32, 2);
  matrix.setCursor(0, 0);
  SetTextColor(matrix.Color333(3,0,0));

  Serial.print(F("free ram : "));
  Serial.println(freeRam());

  {
    static int scrollMinValue = -informationString[currentInformationIndex].length() * 8;
    static int scrollMaxValue = 4;
    static int scrollValue = scrollMinValue;
    static bool dir = true;

    Serial.println(informationString[currentInformationIndex]);
    Serial.println(scrollValue);

    if (intervalUpdator.Update())
    {
      int scrollPixel = 1;
      if (0 < intervalUpdator.GetUpdateValue())
      {
        scrollPixel = intervalUpdator.GetCurrentInterval() / intervalUpdator.GetUpdateValue();
        if (0 >= scrollPixel)
        {
          scrollPixel = 1;
        }
      }
      if (scrollPixel > 3)
      {
        scrollPixel = 3;
      }

      static long scrollDelay = 0;
      if (millis() > scrollDelay)
      {
        if (dir)
        {
          scrollValue += scrollPixel;
          if (scrollValue > scrollMaxValue)
          {
            scrollValue = scrollMaxValue;
            dir = !dir;
            scrollDelay = 3000 + millis();
          }
        }
        else
        {
          scrollValue -= scrollPixel;
          if (scrollValue < scrollMinValue)
          {
            scrollValue = scrollMinValue;
            dir = !dir;

            ++currentInformationIndex;
            if (currentInformationIndex >= maxInformationIndex)
            {
              currentInformationIndex = 0;
              scrollMinValue = -informationString[currentInformationIndex].length() * 8;
              scrollMaxValue = 4;
              scrollValue = scrollMinValue;
            }
          }
        }
      }
    }
    SetTextColor(informationStringColor[currentInformationIndex]);
    matrix.setCursor(scrollValue, 24);
    matrix.print(informationString[currentInformationIndex]);
  }
  matrix.swapBuffers(false);
}

void ReadWIFISerial(char* readWifiSerial, unsigned int bufferMaxLength, unsigned int period)
{
  int index = 0;
  readWifiSerial[0] = 0;
  unsigned long timeout = millis() + period;
  Serial.println(F("============= direct start "));
  Serial.print(F("free ram : "));
  Serial.println(freeRam());
  do
  {
    while (WIFI.available())
    {
      char temp = (char)WIFI.read();
      if ((bufferMaxLength-1) > index)
      {
        readWifiSerial[index++] = temp;
      }
      Serial.print(temp);
    }
  }while(timeout > millis());
  Serial.println(F("============= direct finish "));
  Serial.print(F("index : "));
  Serial.println(index);
  if (0 < index)
  {
    readWifiSerial[index-1] = 0;
  }
}

void ReadWIFISerial(String& readWifiSerial, unsigned int bufferMaxLength, unsigned int period)
{
  int index = 0;
  unsigned long timeout = millis() + period;
  Serial.println(F("============= direct start "));
  Serial.print(F("free ram : "));
  Serial.println(freeRam());
  do
  {
    while (WIFI.available())
    {
      char temp = (char)WIFI.read();
      if ((bufferMaxLength-1) > index++)
      {
        readWifiSerial += temp;
      }
      Serial.print(temp);
    }
  }while(timeout > millis());
  Serial.println(F("============= direct finish "));
  Serial.print(F("index : "));
  Serial.println(index);
}

void PrintWIFISerialToSerial(unsigned int period)
{
  char szTemp[512] = {0,};
  ReadWIFISerial(szTemp, 512, period);
  Serial.println(F("============= print function start "));
  if (szTemp[0])
    Serial.println(szTemp);
  Serial.println(F("============= print function finish "));
}

bool FindWIFISerialToSerial(unsigned int period, const char* searchData)
{
  char szTemp[512] = {0,};
  ReadWIFISerial(szTemp, 512, period);
  if (szTemp[0])
    return (0 != strstr(szTemp, searchData));
  return false;
}

bool InitWIFI()
{
  WIFI.end();
  
  WIFI.begin(115200);                 // 원래 와이파이 모듈의 기본 보드레이트인 115200을 설정해줌 (Set basic wifi module's baud rate 115200)
  WIFI.setTimeout(5000);

  WIFI.println("AT+CIOBAUD=57600");   // 115200 같이 너무 빠른 보드레이트를 쓰면, 하드웨어버퍼가 넘쳐서 일부 스트링이 유실되는 듯 하여 57600으로 변경 (I think when I use too fast baud rate, hardware buffer flood and you lost some of string data. so I set 57600 for this.)
  PrintWIFISerialToSerial(200);

  WIFI.begin(57600);                  // 바꾼 보드레이트로 시리얼통신 보드레이트도 변경해줌 (Set the serial baud rate 57600 too)

  WIFI.println(F("AT+GMR"));
  Serial.println(F("AT+GMR"));
  PrintWIFISerialToSerial(200);


  WIFI.println(F("AT+CWMODE?"));
  Serial.println(F("AT+CWMODE?"));
  if (!FindWIFISerialToSerial(200, "+CWMODE:1"))
  {
    WIFI.println(F("AT+CWMODE=1"));
    Serial.println(F("AT+CWMODE=1"));
    PrintWIFISerialToSerial(200);
  }

  return true;
}

bool ConnectWiFi()
{
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  WIFI.println(cmd);  
  if(FindWIFISerialToSerial(10000, "OK"))
  {
    Serial.println(F("OK, Connected to WiFi."));
    return true;
  }

  Serial.println(F("Can not connect to the WiFi."));
  return false;
}

bool TryToConnectWIFI()
{
  for(int i=0;i<5;i++)
  {
    if(ConnectWiFi())
    {
      return true;
    }
  }

  return false;
}

void GetIP()
{
  WIFI.println(F("AT+CIFSR"));
  PrintWIFISerialToSerial(200);
}

void SetConnectionMode()
{
  WIFI.println(F("AT+CIPMUX=0"));    //set the single connection mode
  FindWIFISerialToSerial(200, "OK");
}

void ForwardWIFIToSerial()
{
  if (WIFI.available()) 
  {
    int inByte = WIFI.read();
    Serial.write(inByte);
  }
}

void ForwardSerialToWIFI()
{
  if (Serial.available()) 
  {
    int inByte = Serial.read();
    WIFI.write(inByte);
  }
}

bool ConnectToServer()
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",";
  cmd += DST_PORT;
  //cmd += "\"";
  WIFI.println(cmd);

  if(FindWIFISerialToSerial(1000, "Error"))
  {
    return false;
  }
  PrintWIFISerialToSerial(0);
  
  return true;
}

bool SendHttpRequest()
{
  String cmd = "GET /weather.php HTTP/1.0\r\nHost : ";
  cmd += DST_IP;
  cmd += "\r\n\r\n";
  cmd += "Host : ";
  cmd += DST_IP;
  cmd += "\r\n\r\n";

  Serial.print(F("send length : "));
  Serial.println(cmd.length());

  String cipsendCmd = "AT+CIPSEND=";
  cipsendCmd += cmd.length();
  WIFI.println(cipsendCmd);
  
  if(FindWIFISerialToSerial(1000, ">"))
  {
    Serial.print(F(">"));
  }
  else
  {
    WIFI.println(F("AT+CIPCLOSE"));
    Serial.println(F("connect timeout"));
    return false;
  }

  WIFI.print(cmd);
  Serial.print(cmd);

  return true;
}

void ReceiveResponseHttp()
{
  String response;
  response.reserve(512);
  ReadWIFISerial(response, 512, 2000);
  {
    Serial.println(F("======================================="));
    Serial.println(response.length());
    Serial.println(response);
    Serial.println(F("======================================="));

    int loopCnt = 0;
    int loopIndex = 0;
    unsigned int start = response.indexOf("[");
    unsigned int end = response.indexOf("]", start+1);
    
    maxInformationIndex = 0;
    String data;
    data.reserve(50);
    while ((start <= response.length()) && (end <= response.length()))
    {
      data = response.substring(start+1, end);

      Serial.println(data);
      if (0 == (loopCnt % 2))
      {
        // 반드시 여기 있어야 함. text와 color가 한쌍이기 때문 (this code should be here, text and color is pair)
        if (InformationArraySize <= maxInformationIndex)
        {
          break;
        }
        
        switch(loopIndex)
        {
          case 0:   // Weather Icon
          {
            weatherIconIndex = GetWeatherIconIndex(data.c_str());
            Serial.print("weather index : ");
            Serial.println(weatherIconIndex);
          }
          break;
          case 1:   // Date
          {
            if (8 <= data.length())
            {
              char szYear[5] = {0,0,0,0,0,};
              char szMonth[3] = {0,0,0,};
              char szDay[3] = {0,0,0,};
              strncpy(szYear, &data[0], 4);
              strncpy(szMonth, &data[4], 2);
              strncpy(szDay, &data[6], 2);
              rtc.setDate(atoi(szDay), atoi(szMonth), atoi(szYear));
            }
          }
          break;
          case 2:   // Time
          {
            if (6 <= data.length())
            {
              char szHour[3] = {0,0,0,};
              char szMin[3] = {0,0,0,};
              char szSec[3] = {0,0,0,};
              strncpy(szHour, &data[0], 2);
              strncpy(szMin, &data[2], 2);
              strncpy(szSec, &data[4], 2);
              rtc.setTime(atoi(szHour), atoi(szMin), atoi(szSec));
            }
          }
          break;
          case 3:   // Dow
          {
            if (1 == data.length())
            {
              int currentDow = data[0] - '0';
              if (0 == currentDow)      // php 에서는 0이 일요일이기 때문 (php using 0 for sunday)
              {
                currentDow = SUNDAY;
              }
              rtc.setDOW(currentDow);
            }
            break;
          }
          case 4:   // DeepNight Mode
          {
            if (4 <= data.length())
            {
              char startTime[3] = {0,0,0,};
              char endTime[3] = {0,0,0,};
              strncpy(startTime, &data[0], 2);
              strncpy(endTime, &data[2], 2);
              deepNightStart = atoi(startTime);
              deepNightEnd = atoi(endTime);

              usingDeepNightMode = (deepNightStart != deepNightEnd);
            }
            else
            {
              usingDeepNightMode = false;
            }
            break;
          }
          default:
          {
            if (InformationArraySize > maxInformationIndex)
            {
              informationString[maxInformationIndex] = data;
            }
            break; 
          }
        }
      }
      else
      {
        if (3 == data.length())
        {
          uint16_t tempColor = matrix.Color333(data[0] - '0', data[1] - '0', data[2] - '0');
          switch(loopIndex)
          {
            case 0: break;
            case 1: dateColor = tempColor; break;
            case 2: timeColor = tempColor; break;
            case 3: dowColor = tempColor; break;
            case 4: deepNightColor = tempColor; break;
            default:
            {
              informationStringColor[maxInformationIndex] = tempColor; 
              ++maxInformationIndex; 
              break;
            }
          }
        }
        ++loopIndex;
      }
      
      ++loopCnt;
    
      start = response.indexOf("[", end + 1);
      end = response.indexOf("]", start+1);
      Serial.println(start);
      Serial.println(end);
    }
  }
  Serial.println(F("========Finished Tokenize======"));
  Serial.println(F("========Finished Request======="));
}

void InitRTC()
{
  rtc.halt(false);
  rtc.writeProtect(false);

  UpdateNight();
}

void PrintRTC(int x, int y, int textSize/* = 1*/)
{    
  static unsigned long sec = millis();
  static bool secBlink = false;
  if (500 < (millis() - sec))
  {
    sec = millis();
    secBlink = !secBlink;
  }

  char szTime[20] = {0,};
  matrix.setTextSize(textSize);

  const Time time = rtc.getTime();
  const int lineDotCount = 8 * textSize;
  const int lineExt = 2;

  matrix.setCursor(x + 12, y + (lineDotCount + lineExt) * 2);
  SetTextColor(dowColor);
  switch(time.dow)
  {
    case 1: matrix.print(F("MON"));  break;
    case 2: matrix.print(F("TUE"));  break;
    case 3: matrix.print(F("WED"));  break;
    case 4: matrix.print(F("THU"));  break;
    case 5: matrix.print(F("FRI"));  break;
    case 6: matrix.print(F("SAT"));  break;
    case 7: matrix.print(F("SUN"));  break;
  }

  SetTextColor(dateColor);
  matrix.setCursor(x, y);
  sprintf(szTime, "%02d/%02d", time.mon, time.date);
  matrix.print(szTime);
  
  SetTextColor(timeColor);
  matrix.setCursor(x, y + (lineDotCount + lineExt));
  if (secBlink)
  {
    sprintf(szTime, "%02d:%02d", time.hour, time.min);
  }
  else
  {
    sprintf(szTime, "%02d %02d", time.hour, time.min);
  }
  matrix.print(szTime);
}

