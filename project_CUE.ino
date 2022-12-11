#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include <String.h>
#include <ArduinoJson.h>

/*
 * --lcd--
 * SDA - 20
 * SCL - 21
 * 
 * --joystick--
 * VRx - A0
 * VRy - A1
 * SW - 3
 * 
 * --SD card Module--
 * MISO - 50
 * MOSI - 51
 * SCK - 52
 * CS - 53
 */

/*
 * page 1 = 인트로
 * page 2 = 음악 선택
 * page 3 = 음 선택
 * page 4 = 음 설정
 * page 5 = 음악 옵션
 * page 6 = 음악 연주
 */

DynamicJsonDocument doc(4096);
LiquidCrystal_I2C lcd(0x27,20,4);
const int Xin = A0;
const int Yin = A1;
const int KEYin = 3;
const int SDcard = 53;
const int buzzer1 = 8;
const int buzzer2 = 9;
const int buzzer3 = 10;
int page = 1;
int settingMusic;
int pointx = 0;
int pointy = 0;
int settingx, settingy, note_, octave, hashtag, connect_, settingPage;
int time_signature = 4;
int max_page = 10;
const char* filename = "cue_.txt";

File file;
  
void setup() 
{
  pinMode(SDcard, OUTPUT);
  pinMode(KEYin, INPUT_PULLUP);
  pinMode(53, OUTPUT);
  pinMode(buzzer1, OUTPUT);
  pinMode(buzzer2, OUTPUT);
  pinMode(buzzer3, OUTPUT);
  intro();

  Serial.begin(115200);

  if (!SD.begin(53)) 
    {
      while (1);
    }
  
  if(SD.exists(filename))
  {
    file = SD.open(filename);
    deserializeJson(doc, file);
    file.close();
    SD.remove(filename);
  }
  
  file = SD.open(filename, FILE_WRITE);
  serializeJson(doc, file);
  file.close();
}

void loop() 
{
  if (!SD.begin(53)) 
  {
    while (1);
  }
  
  point_Events();
}

void intro() //인트로
{
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CUE");
  lcd.setCursor(3,1);
  lcd.print("press the button");
  lcd.setCursor(0, 3);
  lcd.print("^^^^^^^^^^^^^^^^^^^^");
  delay(100);
}

void point_reset()
{
  pointx = 0;
  pointy = 0;
  lcd.setCursor(0,0);
  lcd.print(">");
}

void point_set(int x, int y)
{
  lcd.setCursor(pointx, pointy);
  lcd.print(" ");
  pointx = x;
  pointy = y;
  lcd.setCursor(pointx, pointy);
  lcd.print(">");
}

int joystick_value()
{
  int xVal = analogRead(Xin);
  int yVal = analogRead(Yin);

  if(xVal >= 1021)
  {
    return 1;
  }
  if(xVal == 0)
  {
    return 2;
  }
  if(yVal >= 1021)
  {
    return 3;
  }
  if(yVal == 0)
  {
    return 4;
  }
}

void point_Events()
{
  int joystick = joystick_value();
  
  if(joystick == 1)                                                        //오른쪽
  {
    if(page == 2)
    {
      if(pointy == 3 && pointx == 0)
      {
        point_set(pointx + 16, pointy);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointx == 0 && pointy == 0)
      {
        point_set(pointx + 15, pointy);
        delay(300);
        joystick = joystick_value();
      }
    }
    
    if(page == 3)
    {
      if(pointx < 18 && pointy != 3)
      {
        point_set(pointx + 2, pointy);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointx == 0 && pointy == 3)
      {
        point_set(4, pointy);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointx == 4 && pointy == 3)
      {
        point_set(16, pointy);
        delay(300);
        joystick = joystick_value();
      }
    }

    if(page == 4)
    {
      if(pointy > 1)
      {
        point_set(17, 3);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointy == 0)
      {
        if(note_ == 7)
        {
          note_ = 0;
        }
        else
        {
          note_ += 1;
        }
        lcd.setCursor(10, 0);
        lcd.print(notereader(note_));
        delay(300);
      }
      else if(pointy == 1)
      {
        if(octave == 7)
        {
          octave = 1;
        }
        else
        {
          octave += 1;
        }
        lcd.setCursor(10, 1);
        lcd.print(octave);
        delay(300);
      }
    }

    if(page == 5)
    {
      if(pointy == 0)
      {
        if(time_signature == 4)
        {
          time_signature = 8;
          lcd.setCursor(16, 0);
          lcd.print("8 ");
          delay(300);
        }
        else if(time_signature == 8)
        {
          time_signature = 16;
          lcd.setCursor(16, 0);
          lcd.print("16");
          delay(300);
        }
      }

      if(pointy == 1)
      {
        if(max_page == 100)
        {
          max_page = 1;
          lcd.setCursor(16, 1);
          lcd.print("1  ");
          delay(300);
        }
        max_page += 1;
        lcd.setCursor(16, 1);
        lcd.print("   ");
        lcd.setCursor(16, 1);
        lcd.print(max_page);
        delay(300);
      }
    }
    
  }else if(joystick == 2)                                                        //왼쪽
  {
    if(page == 2)
    {
      if(pointy == 3 && pointx == 16)
      {
        point_set(pointx - 16, pointy);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointx == 15 && pointy == 0)
      {
        point_set(pointx - 15, pointy);
        delay(300);
        joystick = joystick_value();
      }
    }
    
    if(page == 3)
    {
      if(pointx > 0 && pointy != 3)
      {
        point_set(pointx - 2, pointy);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointx == 16 && pointy == 3)
      {
        point_set(4, pointy);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointx == 4 && pointy == 3)
      {
        point_set(0, pointy);
        delay(300);
        joystick = joystick_value();
      }
    }

    if(page == 4)
    {
      if(pointy == 3)
      {
        point_set(0, 3);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointy == 0)
      {
        if(note_ == 0)
        {
          note_ = 7;
        }
        else
        {
          note_ -= 1;
        }
        lcd.setCursor(10, 0);
        lcd.print(notereader(note_));
        delay(300);
      }
      else if(pointy == 1)
      {
        if(octave <= 1)
        {
          octave = 7;
        }
        else
        {
          octave -= 1;
        }
        lcd.setCursor(10, 1);
        lcd.print(octave);
        delay(300);
      }
    }

    if(page == 5)
    {
      if(pointy == 0)
      {
        if(time_signature == 8)
        {
          time_signature = 4;
          lcd.setCursor(16, 0);
          lcd.print("4 ");
          delay(300);
        }
        else if(time_signature == 16)
        {
          time_signature = 8;
          lcd.setCursor(16, 0);
          lcd.print("8 ");
          delay(300);
        }
      }

      if(pointy == 1)
      {
        if(max_page == 1)
        {
          max_page = 100;
          lcd.setCursor(16, 1);
          lcd.print("100");
          delay(300);
        }
        max_page -= 1;
        lcd.setCursor(16, 1);
        lcd.print("   ");
        lcd.setCursor(16, 1);
        lcd.print(max_page);
        delay(300);
      }
    }
    
  }else if(joystick == 3)                                                        //아래쪽
  {
    if(page == 2)
    {
      if(pointy < 3 && pointx != 16)
      {
        point_set(pointx, pointy + 1);
        delay(300);
        joystick = joystick_value();
      }
    }

    if(page == 3)
    {
      if(pointy < 2)
      {
        point_set(pointx, pointy + 1);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointy == 2)
      {
        point_set(0, 3);
        delay(300);
        joystick = joystick_value();
      }
    }

    if(page == 4)
    {
      if(pointy < 3)
      {
        point_set(pointx, pointy + 1);
        delay(300);
        joystick = joystick_value();
      }
    }

    if(page == 5)
    {
      if(pointy == 0)
      {
        point_set(0, 1);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointy == 1)
      {
        point_set(17, 3);
        delay(300);
        joystick = joystick_value();
      }
    }
    
  }else if(joystick == 4)                                                        //위쪽
  {
    if(page == 2)
    {
      if(pointy > 0 && pointx != 16)
      {
        point_set(pointx, pointy - 1);
        delay(300);
        joystick = joystick_value();
      }
    }

    if(page == 3)
    {
      if(pointy > 0)
      {
        point_set(pointx, pointy - 1);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointy == 2)
      {
        point_set(0, 2);
        delay(300);
        joystick = joystick_value();
      }
    }
    
    if(page == 4)
    {
      if(pointy > 0)
      {
        point_set(pointx, pointy - 1);
        delay(300);
        joystick = joystick_value();
      }
    }

    if(page == 5)
    {
      if(pointy == 3)
      {
        point_set(0, 1);
        delay(300);
        joystick = joystick_value();
      }
      else if(pointy == 1)
      {
        point_set(0, 0);
        delay(300);
        joystick = joystick_value();
      }
    }
  }
  
  int buttonVal = digitalRead(KEYin);                                      //버튼
  if(buttonVal == LOW)
  {
    if(page == 1)
    {
      page = 2;

      Music_select();
    }
    else if(page == 2)
    {
      if(pointx == 0)
      {
        switch(pointy)
        {
          case 0:
            settingPage = 1;
            music_set(1);
            settingMusic = 1;
            page = 3;
            delay(500);
            break;
          case 1:
            settingPage = 1;
            music_set(2);
            settingMusic = 2;
            page = 3;
            delay(500);
            break;
          case 2:
            settingPage = 1;
            music_set(3);
            settingMusic = 3;
            page = 3;
            delay(500);
            break;
          default:
            break;
        }
      }
      else if(pointx == 15)
      {
        switch(pointy)
        {
          case 0:
            music_play(1);
            delay(500);
            break;
          case 1:
            music_play(2);
            delay(500);
            break;
          case 2:
            music_play(3);
            delay(500);
            break;
          default:
            break;
        }
      }
    }
    else if(page == 3)
    {
      if(pointy != 3)
      {
        settingx = pointx+1;
        settingy = pointy;
        note_set();
        delay(500);
      }
      else if(pointx == 0 && pointy == 3 && settingPage == 1)
      {
        Music_select();
        page = 2;
      }
      else if(pointx == 4 && pointy == 3)
      {
        Music_option();
        page = 5;
      }
      else if(pointx == 0 && pointy == 3 && settingPage != 1)
      {
        settingPage -= 1;
        music_set(settingMusic);
        point_set(0, 3);
        delay(500);
      }
      else if(pointx == 16 && pointy == 3)
      {
        if(settingPage == 100)
        {
          settingPage = 1;
        }
        settingPage += 1;
        music_set(settingMusic);
        point_set(16, 3);
      }
    }
    else if(page == 4)
    {
      if(pointx != 0 && pointy == 3)
      {
        music_set(settingMusic);
        lcd.setCursor(settingx, settingy);
        lcd.print(notereader(note_));
        jsonset(addressmake(settingMusic, settingPage, settingy, 1, settingx), note_);
        jsonset(addressmake(settingMusic, settingPage, settingy, 2, settingx), octave);
        jsonset(addressmake(settingMusic, settingPage, settingy, 3, settingx), hashtag);
        jsonset(addressmake(settingMusic, settingPage, settingy, 4, settingx), connect_);
        page = 3;
        point_set(settingx -1, settingy);
        delay(500);
      }
      else if(pointx == 0 && pointy == 2)
      {
        if(hashtag == 0)
        {
          hashtag = 1;
          lcd.setCursor(10, 2);
          lcd.print("yes");
          delay(300);
        }
        else
        {
          hashtag = 0;
          lcd.setCursor(10, 2);
          lcd.print("no ");
          delay(300);
        }
      }
      else if(pointx == 0 && pointy == 3)
      {
        if(connect_ == 0)
        {
          connect_ = 1;
          lcd.setCursor(10, 3);
          lcd.print("yes");
          delay(300);
        }
        else
        {
          connect_ = 0;
          lcd.setCursor(10, 3);
          lcd.print("no ");
          delay(300);
        }
      }
    }
    else if(page == 5)
    {
      if(pointx == 17 && pointy == 3)
      {
        music_set(settingMusic);
        page = 3;
        delay(500);
      }
    }
  }
}

void music_set(int a)
{
  if(a == 1)
  {
    settingMusic = 1;
    lcd.clear();

    for(int i=0; i<=2; i++)
    {
      for(int i1=1; i1<20; i1+=2)
      {
        int val = jsonget(addressmake(1, settingPage, i, 1, i1));
        //122234-
        //곡, 페이지, 줄, 옵션, 칸
        lcd.setCursor(i1, i);
        lcd.print(notereader(val));
      }
    }
    

    lcd.setCursor(1,3);
    lcd.print("<-");
    lcd.setCursor(5, 3);
    lcd.print("option");
    lcd.setCursor(12, 3);
    lcd.print("   ");
    lcd.setCursor(12, 3);
    lcd.print(settingPage);
    lcd.setCursor(17,3);
    lcd.print("->");
    point_reset();
  }
}

void note_set()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" note     ");
  lcd.print(notereader(jsonget(addressmake(1, settingPage, settingy, 1, settingx))));
  
  lcd.setCursor(0,1);
  lcd.print(" octave   ");
  lcd.print(jsonget(addressmake(1, settingPage, settingy, 2, settingx)));
  
  lcd.setCursor(0,2);
  lcd.print(" #        ");
  if(jsonget(addressmake(1, settingPage, settingy, 3, settingx)) == 1)
  {
    lcd.print("yes");
  }
  else
  {
    lcd.print("no");
  }
  
  lcd.setCursor(0,3);
  lcd.print(" connect  ");
  if(jsonget(addressmake(1, settingPage, settingy, 4, settingx)) == 1)
  {
    lcd.print("yes");
  }
  else
  {
    lcd.print("no");
  }
  
  lcd.setCursor(18,3);
  lcd.print("OK");
  point_reset();
  page = 4;
  note_ = jsonget(addressmake(1, settingPage, settingy, 1, settingx));
  octave = jsonget(addressmake(1, settingPage, settingy, 2, settingx));
  hashtag = jsonget(addressmake(1, settingPage, settingy, 3, settingx));
  connect_ = jsonget(addressmake(1, settingPage, settingy, 4, settingx));
}

void Music_select()
{
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Example");      //+제목
  lcd.setCursor(1,1);
  lcd.print("");
  lcd.setCursor(1,2);
  lcd.print("");

  lcd.setCursor(16,0);
  lcd.print("play");

  lcd.setCursor(1,3);
  lcd.print("<-");
  lcd.setCursor(17,3);
  lcd.print("->");
      
  point_reset();
  delay(500);
}

char notereader(int n)
{
  switch(n)
  {
    case 0:
      return 'X';
    case 1:
      return 'C';
    case 2:
      return 'D';
    case 3:
      return 'E';
    case 4:
      return 'F';
    case 5:
      return 'G';
    case 6:
      return 'A';
    case 7:
      return 'B';
    default:
      break;
  }
}

void Music_option()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" time signature");
  lcd.setCursor(16, 0);
  lcd.print(time_signature);
  lcd.setCursor(0, 1);
  lcd.print(" max page");
  lcd.setCursor(16, 1);
  lcd.print(max_page);
  lcd.setCursor(18, 3);
  lcd.print("OK");
  point_reset();
}

void jsonset(String address, int value)
{
  doc[address] = value;

  savejsontofile();
}

int jsonget(String address)
{
  int val = doc[address];
  return val;
}

String addressmake(int i1, int i2, int i3, int i4, int i5)
{
  String address;
  
  if(i1 < 10 && i2 < 1000 && i3 <= 2 && i4 <= 4)
  {
    address = (String) (i1 + (i2 * 10) + (i3 * 10000) + (i4 * 100000) + (i5 * 1000000));
    //122234-
    //곡, 페이지, 줄, 옵션, 칸
  }
  return address;
}

void savejsontofile()
{
  if(SD.exists(filename))
  {
    SD.remove(filename);
  }
  
  File file = SD.open(filename, FILE_WRITE);
  String Json;
  serializeJson(doc, Json);
  file.println(Json);
  file.close();
}

void music_play(int i)
{
  int note1, note2, note3;
  int octave1, octave2, octave3;
  int hashtag1, hashtag2, hashtag3;
  int connect1, connect2, connect3;
  for(int p=1; p<=max_page; p++)
  {
    for(int i1=1; i1<20; i1+=2)
    {
      for(int i2=0; i2<=2; i2++)
      {
        if(i2 == 0)
        {
          note1 = jsonget(addressmake(1, p, i2, 1, i1));
          octave1 = jsonget(addressmake(1, p, i2, 2, i1));
          hashtag1 = jsonget(addressmake(1, p, i2, 3, i1));
          connect1 = jsonget(addressmake(1, p, i2, 4, i1));
        }
        else if(i2 == 1)
        {
          note2 = jsonget(addressmake(1, p, i2, 1, i1));
          octave2 = jsonget(addressmake(1, p, i2, 2, i1));
          hashtag2 = jsonget(addressmake(1, p, i2, 3, i1));
          connect2 = jsonget(addressmake(1, p, i2, 4, i1));
        }
        else if(i2 == 2)
        {
          note3 = jsonget(addressmake(1, p, i2, 1, i1));
          octave3 = jsonget(addressmake(1, p, i2, 2, i1));
          hashtag3 = jsonget(addressmake(1, p, i2, 3, i1));
          connect3 = jsonget(addressmake(1, p, i2, 4, i1));
        }
        
        //곡, 페이지, 줄, 옵션, 칸
      }
      if(note1 != 0)
      {
        tone(buzzer1, notemake(note1, octave1, hashtag1));
      }
      if(note1 != 0)
      {
        tone(buzzer2, notemake(note2, octave2, hashtag2));
      }
      if(note1 != 0)
      {
        tone(buzzer3, notemake(note3, octave3, hashtag3));
      }
      
      delay(75);
      if(jsonget(addressmake(1, p, 0, 4, i1)) == 0)
      {
        noTone(buzzer1);
      }
      if(jsonget(addressmake(1, p, 1, 4, i1)) == 0)
      {
        noTone(buzzer2);
      }
      if(jsonget(addressmake(1, p, 2, 4, i1)) == 0)
      {
        noTone(buzzer3);
      }
      delay(50);
    }
  }
  
}

int notemake(int n, int o, int h)
{
  int note_;

  switch(n)
  {
    case 0: {
      break;
    }
    case 1: {
      switch(o)
      {
        case 1: {
          switch(h)
          {
            case 0: {
              note_ = 33;
              break;
            }
            case 1:
            {
              note_ = 35;
              break;
            }
          }
          break;
        }
        case 2: {
          switch(h)
          {
            case 0: {
              note_ = 65;
              break;
            }
            case 1:
            {
              note_ = 69;
              break;
            }
          }
          break;
        }
        case 3: {
          switch(h)
          {
            case 0: {
              note_ = 131;
              break;
            }
            case 1:
            {
              note_ = 139;
              break;
            }
          }
          break;
        }
        case 4: {
          switch(h)
          {
            case 0: {
              note_ = 262;
              break;
            }
            case 1:
            {
              note_ = 277;
              break;
            }
          }
          break;
        }
        case 5: {
          switch(h)
          {
            case 0: {
              note_ = 523;
              break;
            }
            case 1:
            {
              note_ = 554;
              break;
            }
          }
          break;
        }
        case 6: {
          switch(h)
          {
            case 0: {
              note_ = 1047;
              break;
            }
            case 1:
            {
              note_ = 1109;
              break;
            }
          }
          break;
        }
        case 7: {
          switch(h)
          {
            case 0: {
              note_ = 2093;
              break;
            }
            case 1:
            {
              note_ = 2217;
              break;
            }
          }
          break;
        }
      }
      break;
    }
    case 2: {
      switch(o)
      {
        case 1: {
          switch(h)
          {
            case 0: {
              note_ = 37;
              break;
            }
            case 1:
            {
              note_ = 39;
              break;
            }
          }
          break;
        }
        case 2: {
          switch(h)
          {
            case 0: {
              note_ = 73;
              break;
            }
            case 1:
            {
              note_ = 78;
              break;
            }
          }
          break;
        }
        case 3: {
          switch(h)
          {
            case 0: {
              note_ = 147;
              break;
            }
            case 1:
            {
              note_ = 156;
              break;
            }
          }
          break;
        }
        case 4: {
          switch(h)
          {
            case 0: {
              note_ = 294;
              break;
            }
            case 1:
            {
              note_ = 311;
              break;
            }
          }
          break;
        }
        case 5: {
          switch(h)
          {
            case 0: {
              note_ = 587;
              break;
            }
            case 1:
            {
              note_ = 622;
              break;
            }
          }
          break;
        }
        case 6: {
          switch(h)
          {
            case 0: {
              note_ = 1175;
              break;
            }
            case 1:
            {
              note_ = 1245;
              break;
            }
          }
          break;
        }
        case 7: {
          switch(h)
          {
            case 0: {
              note_ = 2349;
              break;
            }
            case 1:
            {
              note_ = 2489;
              break;
            }
          }
          break;
        }
      }
      break;
    }
    case 3: {
      switch(o)
      {
        case 1: {
          switch(h)
          {
            case 0: {
              note_ = 41;
              break;
            }
            case 1:
            {
              note_ = 44;
              break;
            }
          }
          break;
        }
        case 2: {
          switch(h)
          {
            case 0: {
              note_ = 82;
              break;
            }
            case 1:
            {
              note_ = 87;
              break;
            }
          }
          break;
        }
        case 3: {
          switch(h)
          {
            case 0: {
              note_ = 165;
              break;
            }
            case 1:
            {
              note_ = 175;
              break;
            }
          }
          break;
        }
        case 4: {
          switch(h)
          {
            case 0: {
              note_ = 330;
              break;
            }
            case 1:
            {
              note_ = 349;
              break;
            }
          }
          break;
        }
        case 5: {
          switch(h)
          {
            case 0: {
              note_ = 659;
              break;
            }
            case 1:
            {
              note_ = 698;
              break;
            }
          }
          break;
        }
        case 6: {
          switch(h)
          {
            case 0: {
              note_ = 1319;
              break;
            }
            case 1:
            {
              note_ = 1397;
              break;
            }
          }
          break;
        }
        case 7: {
          switch(h)
          {
            case 0: {
              note_ = 2637;
              break;
            }
            case 1:
            {
              note_ = 2794;
              break;
            }
          }
          break;
        }
      }
      break;
    }
    case 4: {
      switch(o)
      {
        case 1: {
          switch(h)
          {
            case 0: {
              note_ = 44;
              break;
            }
            case 1:
            {
              note_ = 46;
              break;
            }
          }
          break;
        }
        case 2: {
          switch(h)
          {
            case 0: {
              note_ = 87;
              break;
            }
            case 1:
            {
              note_ = 93;
              break;
            }
          }
          break;
        }
        case 3: {
          switch(h)
          {
            case 0: {
              note_ = 175;
              break;
            }
            case 1:
            {
              note_ = 185;
              break;
            }
          }
          break;
        }
        case 4: {
          switch(h)
          {
            case 0: {
              note_ = 349;
              break;
            }
            case 1:
            {
              note_ = 370;
              break;
            }
          }
          break;
        }
        case 5: {
          switch(h)
          {
            case 0: {
              note_ = 698;
              break;
            }
            case 1:
            {
              note_ = 740;
              break;
            }
          }
          break;
        }
        case 6: {
          switch(h)
          {
            case 0: {
              note_ = 1397;
              break;
            }
            case 1:
            {
              note_ = 1480;
              break;
            }
          }
          break;
        }
        case 7: {
          switch(h)
          {
            case 0: {
              note_ = 2794;
              break;
            }
            case 1:
            {
              note_ = 2960;
              break;
            }
          }
          break;
        }
      }
      break;
    }
    case 5: {
      switch(o)
      {
        case 1: {
          switch(h)
          {
            case 0: {
              note_ = 49;
              break;
            }
            case 1:
            {
              note_ = 52;
              break;
            }
          }
          break;
        }
        case 2: {
          switch(h)
          {
            case 0: {
              note_ = 98;
              break;
            }
            case 1:
            {
              note_ = 104;
              break;
            }
          }
          break;
        }
        case 3: {
          switch(h)
          {
            case 0: {
              note_ = 196;
              break;
            }
            case 1:
            {
              note_ = 208;
              break;
            }
          }
          break;
        }
        case 4: {
          switch(h)
          {
            case 0: {
              note_ = 392;
              break;
            }
            case 1:
            {
              note_ = 415;
              break;
            }
          }
          break;
        }
        case 5: {
          switch(h)
          {
            case 0: {
              note_ = 784;
              break;
            }
            case 1:
            {
              note_ = 831;
              break;
            }
          }
          break;
        }
        case 6: {
          switch(h)
          {
            case 0: {
              note_ = 1568;
              break;
            }
            case 1:
            {
              note_ = 1661;
              break;
            }
          }
          break;
        }
        case 7: {
          switch(h)
          {
            case 0: {
              note_ = 3136;
              break;
            }
            case 1:
            {
              note_ = 3322;
              break;
            }
          }
          break;
        }
      }
      break;
    }
    case 6: {
      switch(o)
      {
        case 1: {
          switch(h)
          {
            case 0: {
              note_ = 55;
              break;
            }
            case 1:
            {
              note_ = 58;
              break;
            }
          }
          break;
        }
        case 2: {
          switch(h)
          {
            case 0: {
              note_ = 110;
              break;
            }
            case 1:
            {
              note_ = 117;
              break;
            }
          }
          break;
        }
        case 3: {
          switch(h)
          {
            case 0: {
              note_ = 220;
              break;
            }
            case 1:
            {
              note_ = 233;
              break;
            }
          }
          break;
        }
        case 4: {
          switch(h)
          {
            case 0: {
              note_ = 440;
              break;
            }
            case 1:
            {
              note_ = 466;
              break;
            }
          }
          break;
        }
        case 5: {
          switch(h)
          {
            case 0: {
              note_ = 880;
              break;
            }
            case 1:
            {
              note_ = 932;
              break;
            }
          }
          break;
        }
        case 6: {
          switch(h)
          {
            case 0: {
              note_ = 1760;
              break;
            }
            case 1:
            {
              note_ = 1865;
              break;
            }
          }
          break;
        }
        case 7: {
          switch(h)
          {
            case 0: {
              note_ = 3520;
              break;
            }
            case 1:
            {
              note_ = 3729;
              break;
            }
          }
          break;
        }
      }
      break;
    }
    case 7: {
      switch(o)
      {
        case 1: {
          switch(h)
          {
            case 0: {
              note_ = 62;
              break;
            }
            case 1:
            {
              note_ = 65;
              break;
            }
          }
          break;
        }
        case 2: {
          switch(h)
          {
            case 0: {
              note_ = 123;
              break;
            }
            case 1:
            {
              note_ = 131;
              break;
            }
          }
          break;
        }
        case 3: {
          switch(h)
          {
            case 0: {
              note_ = 247;
              break;
            }
            case 1:
            {
              note_ = 262;
              break;
            }
          }
          break;
        }
        case 4: {
          switch(h)
          {
            case 0: {
              note_ = 494;
              break;
            }
            case 1:
            {
              note_ = 523;
              break;
            }
          }
          break;
        }
        case 5: {
          switch(h)
          {
            case 0: {
              note_ = 988;
              break;
            }
            case 1:
            {
              note_ = 1047;
              break;
            }
          }
          break;
        }
        case 6: {
          switch(h)
          {
            case 0: {
              note_ = 1976;
              break;
            }
            case 1:
            {
              note_ = 2093;
              break;
            }
          }
          break;
        }
        case 7: {
          switch(h)
          {
            case 0: {
              note_ = 3951;
              break;
            }
            case 1:
            {
              note_ = 4186;
              break;
            }
          }
          break;
        }
      }
      break;
    }
  }

  return note_;  
}
