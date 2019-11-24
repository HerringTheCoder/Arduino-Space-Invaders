#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

/* global variables */
LiquidCrystal_I2C lcd(0x3f,16,2);  // set the LCD address to 0x3f for a 16 chars and 2 line display
char gameMap[32][2]; /*declaring game map*/
int gameTurns=0;
bool bulletFired = false;

 /*sound section*/
#define NOTE_G4 391 //G4
#define NOTE_A4 440 //A4
#define NOTE_C5 523 //C5
#define NOTE_D5 587 //D5
#define NOTE_E5_FLAT 622 //E flat (Eb)
#define NOTE_E5 659  //E5
#define BUZZER_DIO 7 /*Selects the output pin for piezzo buzzer */

/*analog input secton*/
#define JOYS_VRY_DEADZONE_LEFT 750
#define JOYS_VRY_DEADZONE_RIGHT 150
#define JOYS_VRX_DIO A0    /* Selects the input pin for the joysticks X-Axis */
#define JOYS_VRY_DIO A1    /* Selects the input pin for the joysticks Y-Axis */
#define JOYS_SW_DIO 8      /* Selects the input pin for the joysticks push button */
/*ascii section*/
#define SHIP_ASCII 0b10101010 /* Katakana エ sign */
#define ALIEN_ASCII 0b01011000 /* Upper Case 'X' */
#define BULLET 0b10100101 /* - sign */
#define EXPLOSION 0b00101010 /*   * character */

/*Speed control*/
#define READ_DELAY 20
#define TIMESCALE 5 /*sets the time scale of objects (enemies, bullets) moving on the map, TIMESCALE*READ_DELAY = turn time */ 
/*Sound speed*/
#define TEMPO 200 /*sets the sustain parameter of each note/pause, higher value = slower */

void setup()
{
  randomSeed(analogRead(5)); // using unconnected A5 pin to generate random numbers 
  pinMode(JOYS_SW_DIO, INPUT_PULLUP);
  pinMode(BUZZER_DIO, OUTPUT);
  digitalWrite(JOYS_SW_DIO, HIGH);
  lcd.init();       // initialize the lcd  
  lcd.backlight(); //turn on backlight
  loadLevel(); 
  playMusic();        
}

void loop()
{
  for (int cycleCounter = 0; cycleCounter < TIMESCALE; cycleCounter++)
  { 
  
  if(analogRead(JOYS_VRY_DIO) > JOYS_VRY_DEADZONE_LEFT)
  { 
    turnLeft();
   }
  else if(analogRead(JOYS_VRY_DIO) < JOYS_VRY_DEADZONE_RIGHT)
  {
    turnRight();
   }
   if(!digitalRead(JOYS_SW_DIO) && bulletFired==false)
   {
   fire();
   }
delay(READ_DELAY);
} 
noTone(BUZZER_DIO);
  moveBullets();
 moveEnemies();
 bulletFired= false;
 gameTurns++;
 if(gameTurns>32)
 {
  loadLevel();
  gameTurns=0;
  playMusicBreak();
 }
 }

void turnLeft()
{
  gameMap[0][0] = char(SHIP_ASCII);
  gameMap[0][1] = char(0);
  lcd.setCursor(0,0);
  lcd.print(char(SHIP_ASCII));
  lcd.setCursor(0,1);
  lcd.print(" ");
  
  
}
void turnRight()
{
  gameMap[0][1] = char(SHIP_ASCII);
  gameMap[0][0] = char(0);
   lcd.setCursor(0,1);
  lcd.print(char(SHIP_ASCII)); 
  lcd.setCursor(0,0);
  lcd.print(" "); 
}

void loadLevel()
{
  int rng=0;
   for(int i=0; i<16; i++)  
 {
  gameMap[i][0] = char(32);    /*initializing game map*/
 gameMap[i][1] = char(32);
 lcd.setCursor(i,0);
 lcd.print(char(32));
 lcd.setCursor(i,1);
 lcd.print(char(32));
 }
 for(int i=16; i<32;i++)
  {
  rng=random(0,2);
  if(rng)
  {gameMap[i][0]=char(ALIEN_ASCII);}
  else
  {gameMap[i][0]=char(32);}
  rng=random(0,2);
  if(rng)
  {gameMap[i][1]=char(ALIEN_ASCII);}
  else
  {gameMap[i][1]=char(32);}
 }
 lcd.setCursor(0,0); //set initial ship position
  lcd.print(char(SHIP_ASCII)); //draw ship
  gameMap[0][0]= char(SHIP_ASCII);
}

void fire()
{
  if(gameMap[0][0] == char(SHIP_ASCII))
  {
    gameMap[1][0] = char(BULLET);
  lcd.setCursor(1,0);
  lcd.print(char(BULLET));
  }
  else
  {
    gameMap[1][1] = char(BULLET);
    lcd.setCursor(1,1);
  lcd.print(char(BULLET));    
  }
  bulletFired=true;
  tone(BUZZER_DIO, NOTE_A4*9);
}

void moveBullets(){
  int row;
  int column;
 for(row=15; row>0; row--)
  { 
    for(column=0; column<2; column++)
    {
      if(gameMap[row][column]== char(BULLET))
      {
      lcd.setCursor(row+1,column);
      if(gameMap[row+1][column]==char(ALIEN_ASCII))
      {
      lcd.print(char(EXPLOSION));
      gameMap[row+1][column]=char(EXPLOSION);
      }
      else
      {
        lcd.print(char(BULLET));
        gameMap[row+1][column]=char(BULLET);  
      }
      lcd.setCursor(row,column);
      gameMap[row][column]=char(32);
      lcd.print(char(32));
       } 
    }   
  }
}

void moveEnemies(){
  int row=0,column=0;
  for(row=0;row<32;row++)
  {
    for(column=0;column<2;column++)
    {
      if(gameMap[row][column]==char(ALIEN_ASCII) && gameMap[row-1][column]==char(BULLET))
      { lcd.setCursor(row-1,column);
      lcd.print(char(EXPLOSION));
         gameMap[row-1][column]=char(EXPLOSION);
      gameMap[row][column]=char(32);
       lcd.setCursor(row,column);
    lcd.print(char(32));
      }
    else if(gameMap[row][column]==char(ALIEN_ASCII))
    { lcd.setCursor(row-1,column);
      lcd.print(char(ALIEN_ASCII));     
      gameMap[row-1][column]=char(ALIEN_ASCII);
        gameMap[row][column]=char(32);    
         lcd.setCursor(row,column);
    lcd.print(char(32));
    }
   
    }
  }
  }

void playMusic()
{
  int tempo = 200; //sets note sustain and general tempo (higher = slower)
  tone(BUZZER_DIO, NOTE_A4); // Sends 440Hz 'A'
  delay(TEMPO);        // note sustain
  noTone(BUZZER_DIO); /*Setting up a short pause between repeating notes*/
  delay(TEMPO/16); 
  tone(BUZZER_DIO, NOTE_A4); // Sends 440Hz 'A'
  delay(TEMPO);        
  tone(BUZZER_DIO, NOTE_C5); 
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_D5);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_E5_FLAT);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_E5);
  delay(TEMPO);
  noTone(BUZZER_DIO); /*repeating notes pause*/
  delay(TEMPO/16);
  tone(BUZZER_DIO, NOTE_E5);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_D5);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_C5);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_A4);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_G4);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_A4);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_C5);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_E5);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_A4*4);
  delay(TEMPO*4);
  noTone(BUZZER_DIO);
  delay(TEMPO);
}

void playMusicBreak()
{
  tone(BUZZER_DIO, NOTE_A4);
  delay(TEMPO);
  noTone(BUZZER_DIO);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_C5);
  delay(TEMPO);
  noTone(BUZZER_DIO);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_G4);
  delay(TEMPO);
  tone(BUZZER_DIO, NOTE_A4);
  delay(TEMPO);
  noTone(BUZZER_DIO);
}
