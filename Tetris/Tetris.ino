#include <Esplora.h>
#include <SPI.h>
#include <TFT.h>   

const double VERSION = 0.04;
const int HIGH_SCORES_NUM = 8;
enum STATES {MENU, GAME, SCOREBOARD, CREDITS};
byte STATE = MENU;
bool state_changed = true;
byte MenuPos = 50;
int HighScores[HIGH_SCORES_NUM];
enum Colors {RED,ORANGE,YELLOW,PURPLE,BLUE,GREEN,CYAN,BLACK};
const byte BLOCK_SIZE = 6;
const byte FIELD_OFFSET_X = 50;
const byte FIELD_OFFSET_Y = 5;


void DrawSquare(byte x, byte y, Colors color)
{
  switch(color)
  {
    case RED:     EsploraTFT.fill(0,0,255); break;   //(b,g,r)
    case ORANGE:  EsploraTFT.fill(51,153,255); break;
    case YELLOW:  EsploraTFT.fill(0,255,255); break;
    case PURPLE:  EsploraTFT.fill(255,51,153); break;
    case BLUE:    EsploraTFT.fill(255,102,51); break;
    case GREEN:   EsploraTFT.fill(51,255,102); break;
    case CYAN:    EsploraTFT.fill(255,255,51); break;
    case BLACK:   EsploraTFT.fill(0,0,0); break;
  }
  EsploraTFT.rect(x*BLOCK_SIZE+FIELD_OFFSET_X,y*BLOCK_SIZE+FIELD_OFFSET_Y,BLOCK_SIZE,BLOCK_SIZE);
}

int waitForInput(bool timed = false,int wait = 500)
{
  int timePassed = 0;
  while(Esplora.readButton(SWITCH_DOWN)==HIGH&&
        Esplora.readButton(SWITCH_LEFT)==HIGH&&
        Esplora.readButton(SWITCH_UP)==HIGH&&
        Esplora.readButton(SWITCH_RIGHT)==HIGH)
  {
    delay(2);
    if(timed)
      timePassed+=2;
    if(timePassed>wait)
      return -1;
  }
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


struct Block
{
  Colors color = BLACK;
  bool occupied = false;
  //Block(Colors color = RED,bool occupied = false) : color(color), occupied(occupied) {} 
};

class Playfield
{
  Block field[10][20];
  public:
    void Draw() const
    {
      EsploraTFT.fill(0,0,0);
      EsploraTFT.rect(FIELD_OFFSET_X-1,FIELD_OFFSET_Y-1,BLOCK_SIZE*10+2,BLOCK_SIZE*20+2);
      for(int y=0;y<20;y++)
      {
        for(int x=0;x<10;x++)
          if(field[x][y].occupied)
            DrawSquare(x,y,field[x][y].color);        
      }
    }
    
    bool IsOccupied(byte x, byte y) const
    {
      return field[x][y].occupied;
    }

    void PlaceBlock(byte x, byte y, const Block & bl)
    {
      field[x][y] = bl;
    }
};

class Tetromino
{
  protected:
    Colors color;
    byte posX, posY;
    virtual void Erase() const {}
    virtual void Draw() const {}
    virtual void Place(const Playfield & field) const {}
    virtual bool CheckCollisions(const Playfield & field) {}
  public:
    Tetromino() {posX = 5; posY = 0; color = BLACK;}
    virtual void Rotate() {};
    void MoveLeft()  {Erase(); posX--; Draw();}
    void MoveRight() {Erase(); posX++; Draw();}
    void Fall()      {Erase(); posY++; Draw();}
};

class Piece4 : public Tetromino
{
  protected:
    Block blocks[4][4];
    void Erase() const override
    {
      EsploraTFT.stroke(0,0,0);
      for(int y=0;y<4;y++)
      {
        for(int x=0;x<4;x++)   
          if(blocks[x][y].occupied)
            DrawSquare(x+posX,y+posY,BLACK);
      }
      EsploraTFT.stroke(255,255,255);
    }
    
    void Draw() const override
    {
      for(int y=0;y<4;y++)
      {
        for(int x=0;x<4;x++)   
          if(blocks[x][y].occupied)
            DrawSquare(x+posX,y+posY,color);
      }
    }

    void Place(const Playfield & field) const override
    {
      for(int y=0;y<4;y++)
      {
        for(int x=0;x<4;x++)   
          if(blocks[x][y].occupied)
            field.PlaceBlock(x+posX,y+posY,blocks[x][y]);
      }
    }

    virtual bool CheckCollisions(const Playfield & field) override
    {
      for(int y=0;y<4;y++)
      {
        for(int x=0;x<4;x++)   
          if(blocks[x][y].occupied&&field.IsOccupied(x+posX,y+posY))
            return true;
      }
      return false;
    }
  public:
    Piece4() : Tetromino(){}
    
    void Rotate() override
    {
      
    }
};

class Piece3 : public Tetromino
{
  protected:
    Block blocks[3][3];
    void Erase() const override
    {
      EsploraTFT.stroke(0,0,0);
      for(int y=0;y<3;y++)
      {
        for(int x=0;x<3;x++)   
          if(blocks[x][y].occupied)
            DrawSquare(x+posX,y+posY,BLACK);
      }
      EsploraTFT.stroke(255,255,255);
    }
    
    void Draw() const override
    {
      for(int y=0;y<3;y++)
      {
        for(int x=0;x<3;x++)   
          if(blocks[x][y].occupied)
            DrawSquare(x+posX,y+posY,color);
      }
    }

    void Place(const Playfield & field) const override
    {
      for(int y=0;y<3;y++)
      {
        for(int x=0;x<3;x++)   
          if(blocks[x][y].occupied)
            field.PlaceBlock(x+posX,y+posY,blocks[x][y]);
      }
    }
  public:
    Piece3() : Tetromino() {}
    void Rotate() override
    {
      
    }
};

class IPiece : public Piece4
{
  public:
    IPiece() : Piece4()
    {
      color = YELLOW;
      blocks[0][1].occupied = true;
      blocks[1][1].occupied = true;
      blocks[2][1].occupied = true;
      blocks[3][1].occupied = true;
      Draw();
    }
};

class OPiece : public Piece4
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

void play()
{
  Serial.println("-----------------");
  Playfield field;
  int tick = 200;
  byte escape_counter = 0;
  byte level = 1;
  byte lines = 0;
  EsploraTFT.background(204,153,0);
  EsploraTFT.fill(0,0,0);
  EsploraTFT.rect(114,4,43,30);
  field.Draw();

  Tetromino * current = new IPiece();
  unsigned long timer = millis();
  while(true)
  {    
    //unsigned long t = millis();
    int button_pressed = waitForInput(true,tick);
    if(button_pressed==SWITCH_RIGHT)
    {
      current->MoveRight();
    }
    if(button_pressed==SWITCH_LEFT)
    {
      current->MoveLeft();
    }
    if(button_pressed==SWITCH_UP)
    {
      
    }
    if(button_pressed==SWITCH_DOWN)
    {
      
    }
    if(button_pressed==SWITCH_DOWN)
    {
      escape_counter++;
    }
    else
      escape_counter = 0;
    if(millis()>timer+tick)
    {
      timer = millis();
      current->Fall();
    }
    if(escape_counter==8)
      break;
    Serial.println(button_pressed);
  }
  delete current;
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
