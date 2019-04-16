#include <Esplora.h>
#include <SPI.h>
#include <TFT.h>   

const double VERSION = 0.03;
const int HIGH_SCORES_NUM = 8;
enum STATES {MENU, GAME, SCOREBOARD, CREDITS};
byte STATE = MENU;
bool state_changed = true;
byte MenuPos = 50;
int HighScores[HIGH_SCORES_NUM];
enum Colors {RED,ORANGE,YELLOW,PURPLE,BLUE,GREEN,CYAN};
int timer = 500;
const byte blockSize = 6;

class Block
{
  Colors color;
  bool occupied;
  public:
    Block(Colors color = RED,bool occupied = false) : color(color), occupied(false) {}

    Colors Getcolor() const {return color;}
    bool IsOccupied() const {return occupied;}
    
};

void DrawSquare(byte x, byte y, Colors color)
{
  switch(color)
  {
    case RED:     EsploraTFT.fill(255,0,0); break;
    case ORANGE:  EsploraTFT.fill(255,153,51); break;
    case YELLOW:  EsploraTFT.fill(255,255,0); break;
    case PURPLE:  EsploraTFT.fill(153,51,255); break;
    case BLUE:    EsploraTFT.fill(51,102,255); break;
    case GREEN:   EsploraTFT.fill(102,255,51); break;
    case CYAN:    EsploraTFT.fill(51,255,255); break;
  }
  EsploraTFT.rect(x*blockSize+50,y*blockSize+5,blockSize,blockSize);
}

class Tetromino
{
  protected:
    byte posX, posY;
  public:
    Tetromino() {posX = 5; posY = 0;}
    virtual void Rotate() = 0;
};

class IPiece : public Tetromino
{
  Block blocks[4][4];
  public:
    IPiece() 
    {
      Serial.println("IPiece Created");
    }
    void Rotate() override
    {
      
    }
};

class Piece3 : public Tetromino
{
  protected:
    Block blocks[3][3];
    void Rotate() override
    {
      
    }
};

class OPiece : public Piece3
{
  
};

class TPiece : public Piece3
{
  
};

class SPiece : public Piece3
{
  
};

class ZPiece : public Piece3
{
  
};

class JPiece : public Piece3
{
  
};

class LPiece : public Piece3
{
  
};

class Playfield
{
  public:
    Block field[10][20];
};

void play()
{
  //Block playfield[10][20];
  IPiece piece;
  byte escape_counter = 0;
  byte level = 1;
  byte lines = 0;
  Serial.println(EsploraTFT.width());
  Serial.println(EsploraTFT.height());
  EsploraTFT.background(51,153,51);
  EsploraTFT.fill(0,0,0);
  EsploraTFT.rect(49,4,62,122);
  EsploraTFT.rect(114,4,43,30);
  DrawSquare(0,0,RED);
  DrawSquare(1,1,ORANGE);
  DrawSquare(2,2,YELLOW);
  DrawSquare(3,3,GREEN);
  DrawSquare(4,4,CYAN);
  DrawSquare(5,5,BLUE);
  DrawSquare(6,6,PURPLE);
  DrawSquare(7,7,PURPLE);
  DrawSquare(8,8,PURPLE);
  DrawSquare(9,9,PURPLE);
  DrawSquare(8,10,RED);
  DrawSquare(7,11,ORANGE);
  DrawSquare(6,12,YELLOW);
  DrawSquare(5,13,GREEN);
  DrawSquare(4,14,CYAN);
  DrawSquare(3,15,BLUE);
  DrawSquare(2,16,PURPLE);
  DrawSquare(1,17,PURPLE);
  DrawSquare(0,18,PURPLE);
  DrawSquare(1,19,PURPLE);

  for(byte x=0; x<10; x++)
  {
    for(byte y=0; y<20; y++)
    {
      
    }
  }


  while(true)
  {
    int button_pressed = waitForInput();
    if(button_pressed==SWITCH_RIGHT)
      break;  
  }
  level = 1;
  timer = 500;
  STATE = SCOREBOARD;
}

void display_menu()
{
  if(state_changed)
  {
    EsploraTFT.setTextSize(4);
    EsploraTFT.background(0,0,0);
    EsploraTFT.stroke(255,50,50);
    EsploraTFT.text("Tetris",8,0);
    EsploraTFT.stroke(255,255,255);
    EsploraTFT.setTextSize(1);
    EsploraTFT.text("Play",50,50);
    EsploraTFT.text("Scoreboard",50,70);
    EsploraTFT.text("Credits",50,90);
    state_changed = false;
  }
  EsploraTFT.text(">",40,MenuPos);
  int button_pressed = waitForInput();
  EsploraTFT.stroke(0,0,0);
  if(button_pressed==SWITCH_RIGHT)
  {
    switch(MenuPos)
    {
      case 50: STATE = GAME; break;
      case 70: STATE = SCOREBOARD; break;
      case 90: STATE = CREDITS; break;
    }
    state_changed = true;
  }
  if(button_pressed==SWITCH_UP&&MenuPos>50)
  {
    EsploraTFT.text(">",40,MenuPos);
    MenuPos-=20;
  }
  if(button_pressed==SWITCH_DOWN&&MenuPos<90)
  {
    EsploraTFT.text(">",40,MenuPos);
    MenuPos+=20;
  }
  EsploraTFT.stroke(255,255,255);
  Serial.println(button_pressed);
}

void display_scoreboard()
{
  if(state_changed)
  {
    EsploraTFT.background(0,0,0);
    EsploraTFT.setTextSize(2);
    EsploraTFT.text("Scoreboard",0,0);
    EsploraTFT.setTextSize(1);
    char text[20];
    for(byte i=1;i<=HIGH_SCORES_NUM;i++)
    {
      String(i).toCharArray(text,20);
      EsploraTFT.text(text,10,i*10+20); 
      EsploraTFT.text(":",15,i*10+20);
      String(HighScores[i-1]).toCharArray(text,20);
      EsploraTFT.text(text,25,i*10+20);
    }
    EsploraTFT.text("> Back",5,EsploraTFT.height()-10);
    state_changed = false;
  }
  int button_pressed = waitForInput();
  if(button_pressed==SWITCH_RIGHT)
  {
    STATE = MENU;
    state_changed = true;
  }
}

void display_credits()
{
  if(state_changed)
  {
    EsploraTFT.background(0,0,0);
    EsploraTFT.text("Created by",0,0);
    EsploraTFT.text("Ladislav Miklik",0,10);
    EsploraTFT.text("as a semestral work",0,20);
    EsploraTFT.text("in BI-ARD",0,30);
    EsploraTFT.text("Current version: ",0,40);
    char text[5];
    String(VERSION).toCharArray(text,5);
    EsploraTFT.text(text,100,40);
    EsploraTFT.text("> Back",5,EsploraTFT.height()-10);
    state_changed = false;
  }
  int button_pressed = waitForInput();
  if(button_pressed==SWITCH_RIGHT)
  {
    STATE = MENU;
    state_changed = true;
  }
}


int waitForInput()
{
  while(Esplora.readButton(SWITCH_DOWN)==HIGH&&
        Esplora.readButton(SWITCH_LEFT)==HIGH&&
        Esplora.readButton(SWITCH_UP)==HIGH&&
        Esplora.readButton(SWITCH_RIGHT)==HIGH){delay(2);}
  int button = -1;
  if(Esplora.readButton(SWITCH_RIGHT)==LOW)
    button = SWITCH_RIGHT;
  if(Esplora.readButton(SWITCH_LEFT)==LOW)
    button = SWITCH_LEFT;
  if(Esplora.readButton(SWITCH_UP)==LOW)
    button = SWITCH_UP;
  if(Esplora.readButton(SWITCH_DOWN)==LOW)
    button = SWITCH_DOWN;
  unsigned long t = millis()+200;
  while((Esplora.readButton(SWITCH_DOWN)==LOW||
        Esplora.readButton(SWITCH_LEFT)==LOW||
        Esplora.readButton(SWITCH_UP)==LOW||
        Esplora.readButton(SWITCH_RIGHT)==LOW)&&millis()<t){delay(2);}
  return button;
}

void setup()
{
  EsploraTFT.begin();
  Serial.begin(9600);
  for(byte i = 0; i < HIGH_SCORES_NUM; i++)
    HighScores[i] = 0;
}

void loop()
{
  if(STATE==MENU)
    display_menu();
  if(STATE==GAME)
    play();
  if(STATE==SCOREBOARD)
    display_scoreboard();
  if(STATE==CREDITS)
    display_credits();
}
