#include <Esplora.h>
#include <SPI.h>
#include <TFT.h>   

enum States {MENU, GAME, SCOREBOARD};
int STATE = MENU;
bool state_changed = true;

class Block
{
  
};

class Tetromino
{
  
};

void display_menu()
{
  if(state_changed)
  {
    EsploraTFT.background(0,0,0);
    EsploraTFT.stroke(255,50,50);
    EsploraTFT.text("Tetris",0,0);
    state_changed = false;
  }
  int button_pressed = waitForInput();
  if(button_pressed==SWITCH_UP)
    Esplora.writeRGB(255,255,0);
  if(button_pressed==SWITCH_DOWN)
    Esplora.writeRGB(255,0,255);
  Serial.println(button_pressed);
}

void play()
{
  
}

void display_scoreboard()
{
  
}

void setup()
{
  EsploraTFT.begin();
  Serial.begin(9600);
}

int waitForInput()
{
  while(Esplora.readButton(SWITCH_DOWN)==HIGH&&
        Esplora.readButton(SWITCH_LEFT)==HIGH&&
        Esplora.readButton(SWITCH_UP)==HIGH&&
        Esplora.readButton(SWITCH_RIGHT)==HIGH){delay(2);}
  int button = 50;
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

void loop()
{
  if(STATE==MENU)
    display_menu();
  if(STATE==GAME)
    play();
  if(STATE==SCOREBOARD)
    display_scoreboard();
}
