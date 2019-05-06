#include <Esplora.h>
#include <SPI.h>
#include <TFT.h>

const double VERSION = 0.07;
const int HIGH_SCORES_NUM = 8;
enum STATES {MENU, GAME, SCOREBOARD, CREDITS};
byte STATE = MENU;
bool state_changed = true;
byte MenuPos = 50;
unsigned long HighScores[HIGH_SCORES_NUM];
enum Colors {RED, ORANGE, YELLOW, PURPLE, BLUE, GREEN, CYAN, BLACK};
const byte BLOCK_SIZE = 6;
const byte FIELD_OFFSET_X = 50;
const byte FIELD_OFFSET_Y = 5;


void DrawSquare(int x, int y, Colors color)
{
  switch (color)
  {
    case RED:     EsploraTFT.fill(0, 0, 255); break; //(b,g,r)
    case ORANGE:  EsploraTFT.fill(51, 153, 255); break;
    case YELLOW:  EsploraTFT.fill(0, 255, 255); break;
    case PURPLE:  EsploraTFT.fill(255, 51, 153); break;
    case BLUE:    EsploraTFT.fill(255, 102, 51); break;
    case GREEN:   EsploraTFT.fill(51, 255, 102); break;
    case CYAN:    EsploraTFT.fill(255, 255, 51); break;
    case BLACK:   EsploraTFT.fill(0, 0, 0); break;
  }
  byte X = x * BLOCK_SIZE + FIELD_OFFSET_X;
  byte Y = y * BLOCK_SIZE + FIELD_OFFSET_Y;
  EsploraTFT.rect(X,Y, BLOCK_SIZE, BLOCK_SIZE);
}

int waitForInput(bool timed = false, int wait = 500)
{
  int timePassed = 0;
  while (Esplora.readButton(SWITCH_DOWN) == HIGH &&
         Esplora.readButton(SWITCH_LEFT) == HIGH &&
         Esplora.readButton(SWITCH_UP) == HIGH &&
         Esplora.readButton(SWITCH_RIGHT) == HIGH)
  {
    delay(2);
    if (timed)
      timePassed += 2;
    if (timePassed > wait)
      return -1;
  }
  int button = -1;
  if (Esplora.readButton(SWITCH_RIGHT) == LOW)
    button = SWITCH_RIGHT;
  if (Esplora.readButton(SWITCH_LEFT) == LOW)
    button = SWITCH_LEFT;
  if (Esplora.readButton(SWITCH_UP) == LOW)
    button = SWITCH_UP;
  if (Esplora.readButton(SWITCH_DOWN) == LOW)
    button = SWITCH_DOWN;
  unsigned long t = millis() + 200;
  while ((Esplora.readButton(SWITCH_DOWN) == LOW ||
          Esplora.readButton(SWITCH_LEFT) == LOW ||
          Esplora.readButton(SWITCH_UP) == LOW ||
          Esplora.readButton(SWITCH_RIGHT) == LOW) && millis() < t) {
    delay(2);
  }
  return button;
}


struct Block
{
  Colors color = BLACK;
  bool occupied = false;
};

class Playfield
{
  Block field[10][20];
  unsigned long score;

  void PrintScore() const
  {
    EsploraTFT.fill(0, 0, 0);
    EsploraTFT.rect(114, 4, 43, 30);
    EsploraTFT.textSize(1); 
    EsploraTFT.text("Score:",115,5);
    char text[20];
    String(score).toCharArray(text, 20);
    EsploraTFT.text(text,118,13);
  }
  
  public:
    Playfield() {score = 0; PrintScore();}
    void Draw() const
    {
      EsploraTFT.fill(0, 0, 0);
      EsploraTFT.rect(FIELD_OFFSET_X - 1, FIELD_OFFSET_Y - 1, BLOCK_SIZE * 10 + 2, BLOCK_SIZE * 20 + 2);
      for (int y = 0; y < 20; y++)
      {
        for (int x = 0; x < 10; x++)
          if (field[x][y].occupied)
            DrawSquare(x, y, field[x][y].color);
      }
    }

    unsigned long GetScore() {return score;}

    bool IsOccupied(byte x, byte y) const
    {
      return field[x][y].occupied;
    }

    void PlaceBlock(byte x, byte y, const Block & bl, Colors color)
    {
      field[x][y] = bl;
      field[x][y].color = color;
    }

    void CheckRows()
    {
      byte changed = 0;
      for(byte row=0;row<20;row++)
      {
        for(byte col=0;col<10;col++)
        {
          if(!field[col][row].occupied)
            break;
          if(col==9)
          {
            changed++;
            byte y = row;
            while(y>0)
            {
              for(byte x = 0; x<10;x++)
                field[x][y] = field[x][y-1];
              y--;
            }
          }
        }
      }
      if(changed)
      {
        switch(changed)
        {
          case 1: score += 40;    break;
          case 2: score += 100;   break;
          case 3: score += 300;   break;
          case 4: score += 1200;  break;
        }
        Draw();
        PrintScore();
      }
    }
};

class Tetromino
{
  protected:
    Colors color;
    byte posX, posY;
    bool placed;
    virtual void Erase() const {}
    virtual void Place(const Playfield & field) {}
  public:
    Tetromino() {
      posX = 5;
      posY = 0;
      color = BLACK;
      placed = false;
    }
    virtual bool CheckCollisions(const Playfield & field) {
      return false;
    }
    virtual void Rotate(const Playfield &) {};
    virtual void Draw() const {}
    virtual void Preview() const {};
    void MoveLeft(const Playfield & field)
    {
      posX--;
      if (!CheckCollisions(field))
      {
        posX++;
        Erase();
        posX--;
        Draw();
      }
      else
        posX++;
    }
    void MoveRight(const Playfield & field)
    {
      posX++;
      if (!CheckCollisions(field))
      {
        posX--;
        Erase();
        posX++;
        Draw();
      }
      else
        posX--;
    }
    void Fall(const Playfield & field)
    {
      posY++;
      if (!CheckCollisions(field))
      {
        posY--;
        Erase();
        posY++;
        Draw();
      }
      else
      {
        posY--;
        Place(field);
      }
    }
    bool IsPlaced() 
    {
      return placed;
    }
};

class Piece4 : public Tetromino
{
  protected:
    Block blocks[4][4];
    void Erase() const override
    {
      EsploraTFT.stroke(0, 0, 0);
      for (int y = 0; y < 4; y++)
      {
        for (int x = 0; x < 4; x++)
          if (blocks[x][y].occupied)
            DrawSquare(x + posX, y + posY, BLACK);
      }
      EsploraTFT.stroke(255, 255, 255);
    }

    void Place(const Playfield & field) override
    {
      for (int y = 0; y < 4; y++)
      {
        for (int x = 0; x < 4; x++)
          if (blocks[x][y].occupied)
            field.PlaceBlock(x + posX, y + posY, blocks[x][y], color);
      }
      placed = true;
    }

  public:
    Piece4() : Tetromino() {}

    virtual bool CheckCollisions(const Playfield & field) override
    {
      for (int y = 0; y < 4; y++)
      {
        for (int x = 0; x < 4; x++)
        {
          byte X = x + posX;
          byte Y = y + posY;
          if (blocks[x][y].occupied && (field.IsOccupied(X, Y) || X < 0 || X > 9 || Y > 19))
            return true;
        }
      }
      return false;
    }

    void Draw() const override
    {
      for (int y = 0; y < 4; y++)
      {
        for (int x = 0; x < 4; x++)
          if (blocks[x][y].occupied)
            DrawSquare(x + posX, y + posY, color);
      }
    }

    virtual void Preview() const override
    {
      
      for (int y = 0; y < 4; y++)
      {
        for (int x = 0; x < 4; x++)
          if (blocks[x][y].occupied)
             DrawSquare(x-6, y+1, color);
      }
    }

    void Rotate(const Playfield & field) override
    {
      Block rotated[4][4];
      Block copy[4][4];
      Erase();
      for (int y = 0; y < 4; y++)
      {
        int Y = map(y,0,3,3,0);
        for (int x = 0; x < 4; x++)
          rotated[x][Y] = blocks[y][x];
      }
      for (int y = 0; y < 4; y++)
      {
        for (int x = 0; x < 4; x++)
        {
          copy[x][y] = blocks[x][y];
          blocks[x][y] = rotated[x][y]; 
        }
      }
      if(CheckCollisions(field))
      {
        for (int y = 0; y < 4; y++)
        {
          for (int x = 0; x < 4; x++)
            blocks[x][y] = copy[x][y];
        }   
      } 
      Draw();
    }
};

class Piece3 : public Tetromino
{
  protected:
    Block blocks[3][3];
    void Erase() const override
    {
      EsploraTFT.stroke(0, 0, 0);
      for (int y = 0; y < 3; y++)
      {
        for (int x = 0; x < 3; x++)
          if (blocks[x][y].occupied)
            DrawSquare(x + posX, y + posY, BLACK);
      }
      EsploraTFT.stroke(255, 255, 255);
    }

    void Place(const Playfield & field) override
    {
      for (int y = 0; y < 3; y++)
      {
        for (int x = 0; x < 3; x++)
        {
          if (blocks[x][y].occupied)
            field.PlaceBlock(x + posX, y + posY, blocks[x][y], color);
        }
      }
      placed = true;
    }
  public:
    Piece3() : Tetromino() {}

    virtual bool CheckCollisions(const Playfield & field) override
    {
      for (int y = 0; y < 3; y++)
      {
        for (int x = 0; x < 3; x++)
        {
          byte X = x + posX;
          byte Y = y + posY;
          if (blocks[x][y].occupied && (field.IsOccupied(X, Y) || X < 0 || X > 9 || Y > 19))
            return true;
        }
      }
      return false;
    }

    void Draw() const override
    {
      for (int y = 0; y < 3; y++)
      {
        for (int x = 0; x < 3; x++)
          if (blocks[x][y].occupied)
            DrawSquare(x + posX, y + posY, color);
      }
    }
    
    virtual void Preview() const override
    {
      for (int y = 0; y < 3; y++)
      {
        for (int x = 0; x < 3; x++)
          if (blocks[x][y].occupied)
             DrawSquare(x-6, y+1, color);
      }
    }
 
    void Rotate(const Playfield & field) override
    {
      Block rotated[3][3];
      Block copy[3][3];
      Erase();
      for (int y = 0; y < 3; y++)
      {
        int Y = map(y,0,2,2,0);
        for (int x = 0; x < 3; x++)
          rotated[x][Y] = blocks[y][x];
      }
      for (int y = 0; y < 3; y++)
      {
        for (int x = 0; x < 3; x++)
        {
          copy[x][y] = blocks[x][y];
          blocks[x][y] = rotated[x][y]; 
        }
      }
      if(CheckCollisions(field))
      {
        for (int y = 0; y < 3; y++)
        {
          for (int x = 0; x < 3; x++)
            blocks[x][y] = copy[x][y];
        }   
      } 
      Draw();
    }
};

class IPiece : public Piece4
{
  public:
    IPiece() : Piece4()
    {
      color = CYAN;
      blocks[0][1].occupied = true;
      blocks[1][1].occupied = true;
      blocks[2][1].occupied = true;
      blocks[3][1].occupied = true;
    }
};

class OPiece : public Piece4
{
  public:
    OPiece() : Piece4()
    {
      color = YELLOW;
      blocks[1][1].occupied = true;
      blocks[1][2].occupied = true;
      blocks[2][1].occupied = true;
      blocks[2][2].occupied = true;
    }
};

class TPiece : public Piece3
{
  public:
    TPiece() : Piece3()
    {
      color = PURPLE;
      blocks[0][1].occupied = true;
      blocks[1][0].occupied = true;
      blocks[1][1].occupied = true;
      blocks[2][1].occupied = true;
    }
};

class SPiece : public Piece3
{
  public:
    SPiece() : Piece3()
    {
      color = GREEN;
      blocks[0][1].occupied = true;
      blocks[1][0].occupied = true;
      blocks[1][1].occupied = true;
      blocks[2][0].occupied = true;
    }
};

class ZPiece : public Piece3
{
  public:
    ZPiece() : Piece3()
    {
      color = RED;
      blocks[0][0].occupied = true;
      blocks[1][0].occupied = true;
      blocks[1][1].occupied = true;
      blocks[2][1].occupied = true;
    }
};

class JPiece : public Piece3
{
  public:
    JPiece() : Piece3()
    {
      color = BLUE;
      blocks[1][0].occupied = true;
      blocks[1][1].occupied = true;
      blocks[1][2].occupied = true;
      blocks[0][2].occupied = true;
    }
};

class LPiece : public Piece3
{
  public:
    LPiece() : Piece3()
    {
      color = ORANGE;
      blocks[1][0].occupied = true;
      blocks[1][1].occupied = true;
      blocks[1][2].occupied = true;
      blocks[2][2].occupied = true;
    }
};

class Tetromino * NewPiece()
{
  long rnd = random(7);
  switch (rnd)
  {
    case 0: return new IPiece();
    case 1: return new OPiece();
    case 2: return new LPiece();
    case 3: return new JPiece();
    case 4: return new TPiece();
    case 5: return new SPiece();
    case 6: return new ZPiece();
  }
}

void play()
{
  Serial.println("-----------------");

  int tick = 200;
  byte escape_counter = 0;
  EsploraTFT.background(204, 153, 0);
  Playfield field;
  field.Draw();
  Tetromino * current = NewPiece();
  Tetromino * next = NewPiece();
  unsigned long timer = millis();
  current->Draw();
  EsploraTFT.fill(0,0,0);
  EsploraTFT.rect(4,4,42,30);
  next->Preview();
  while (true)
  {
    if (current->IsPlaced())
    {
      delete current;
      current = next;
      next = NewPiece();
      field.CheckRows();
      if(!current->CheckCollisions(field))
      {
        current->Draw();
        EsploraTFT.fill(0,0,0);
        EsploraTFT.rect(4,4,43,30);
        next->Preview(); 
      }
      else
      {
        EsploraTFT.textSize(3);
        EsploraTFT.stroke(0,0,255);
        EsploraTFT.text("GAME",45,40);
        EsploraTFT.text("OVER",45,80);
        EsploraTFT.stroke(255,255,255);
        delay(2000);   
        break; 
      }
    }
    int button_pressed = waitForInput(true, tick);
    if (button_pressed == SWITCH_RIGHT)
      current->MoveRight(field);
    if (button_pressed == SWITCH_LEFT)
      current->MoveLeft(field);
    if (button_pressed == SWITCH_DOWN)
      current->Fall(field);
    if (button_pressed == SWITCH_UP)
    {
      escape_counter++;
      current->Rotate(field);
    }
    else
      escape_counter = 0;
    if (millis() > timer + tick)
    {
      timer = millis();
      current->Fall(field);
    }
    if (escape_counter == 8)
      break;
    Serial.println(button_pressed);
  }
  delete current;
  delete next;
  unsigned long score = field.GetScore();         //Saving score
  if(score>HighScores[HIGH_SCORES_NUM])
    HighScores[HIGH_SCORES_NUM] = score;
  for(int i=HIGH_SCORES_NUM;i>=0;i--)
  {
    if(i!=0&&HighScores[i]>HighScores[i-1])
    {
      HighScores[i] = HighScores[i-1];
      HighScores[i-1] = score;
    }
  }
  STATE = SCOREBOARD;
}

void display_menu()
{
  if (state_changed)
  {
    EsploraTFT.setTextSize(4);
    EsploraTFT.background(0, 0, 0);
    EsploraTFT.stroke(255, 50, 50);
    EsploraTFT.text("Tetris", 8, 0);
    EsploraTFT.stroke(255, 255, 255);
    EsploraTFT.setTextSize(1);
    EsploraTFT.text("Play", 50, 50);
    EsploraTFT.text("Scoreboard", 50, 70);
    EsploraTFT.text("Credits", 50, 90);
    state_changed = false;
  }
  EsploraTFT.text(">", 40, MenuPos);
  int button_pressed = waitForInput();
  EsploraTFT.stroke(0, 0, 0);
  if (button_pressed == SWITCH_RIGHT)
  {
    switch (MenuPos)
    {
      case 50: STATE = GAME; break;
      case 70: STATE = SCOREBOARD; break;
      case 90: STATE = CREDITS; break;
    }
    state_changed = true;
  }
  if (button_pressed == SWITCH_UP && MenuPos > 50)
  {
    EsploraTFT.text(">", 40, MenuPos);
    MenuPos -= 20;
  }
  if (button_pressed == SWITCH_DOWN && MenuPos < 90)
  {
    EsploraTFT.text(">", 40, MenuPos);
    MenuPos += 20;
  }
  EsploraTFT.stroke(255, 255, 255);
  Serial.println(button_pressed);
}

void display_scoreboard()
{
  if (state_changed)
  {
    EsploraTFT.background(0, 0, 0);
    EsploraTFT.setTextSize(2);
    EsploraTFT.text("Scoreboard", 0, 0);
    EsploraTFT.setTextSize(1);
    char text[20];
    for (byte i = 1; i <= HIGH_SCORES_NUM; i++)
    {
      String(i).toCharArray(text, 20);
      EsploraTFT.text(text, 10, i * 10 + 20);
      EsploraTFT.text(":", 15, i * 10 + 20);
      String(HighScores[i - 1]).toCharArray(text, 20);
      EsploraTFT.text(text, 25, i * 10 + 20);
    }
    EsploraTFT.text("> Back", 5, EsploraTFT.height() - 10);
    state_changed = false;
  }
  int button_pressed = waitForInput();
  if (button_pressed == SWITCH_RIGHT)
  {
    STATE = MENU;
    state_changed = true;
  }
}

void display_credits()
{
  if (state_changed)
  {
    EsploraTFT.background(0, 0, 0);
    EsploraTFT.text("Created by", 0, 0);
    EsploraTFT.text("Ladislav Miklik", 0, 10);
    EsploraTFT.text("as a semestral work", 0, 20);
    EsploraTFT.text("in BI-ARD", 0, 30);
    EsploraTFT.text("Current version: ", 0, 40);
    char text[5];
    String(VERSION).toCharArray(text, 5);
    EsploraTFT.text(text, 100, 40);
    EsploraTFT.text("> Back", 5, EsploraTFT.height() - 10);
    state_changed = false;
  }
  int button_pressed = waitForInput();
  if (button_pressed == SWITCH_RIGHT)
  {
    STATE = MENU;
    state_changed = true;
  }
}

void setup()
{
  EsploraTFT.begin();
  Serial.begin(9600);
  for (byte i = 0; i < HIGH_SCORES_NUM; i++)
    HighScores[i] = 0;
}

void loop()
{
  if (STATE == MENU)
    display_menu();
  if (STATE == GAME)
    play();
  if (STATE == SCOREBOARD)
    display_scoreboard();
  if (STATE == CREDITS)
    display_credits();
}
