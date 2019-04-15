#include <Esplora.h>
#include <SPI.h>
#include <TFT.h>   

const double VERSION = 0.02;
const int HIGH_SCORES_NUM = 8;
enum STATES {MENU, GAME, SCOREBOARD, CREDITS};
int STATE = MENU;
bool state_changed = true;
int MenuPos = 50;
int HighScores[HIGH_SCORES_NUM];
enum Colors {RED,ORANGE,YELLOW,PURPLE,BLUE,GREEN};

class Block
{
  Colors color;
  bool occupied;
  public:
    Block(Colors color = RED,bool occupied = false) : color(color), occupied(false) {}

    Colors Getcolor() const {return color;}
    bool IsOccupied() const {return occupied;}
    
};

class Tetromino
{
  protected:
    byte posX, posY;
  public:
    Tetromino(byte x = 5, byte y = 0) : posX(x), posY(y) {}
};

void play()
{
  Block playfield[10][20];
  byte escape_counter = 0;
  byte level = 0;
  byte lines = 0;
  state_changed=false;

  
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
    {
      STATE = MENU;
      state_changed = true;
    }    
  }

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
