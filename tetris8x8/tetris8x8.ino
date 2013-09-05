#include <Colorduino.h>

#define BUTTON_LEFT_PIN  2
#define BUTTON_RIGHT_PIN 5
int button_left_status;
int button_right_status;

unsigned long falling_delay = 500; // millisecond
unsigned long button_delay = 400; // millisecond

#define TURN_MAX 50
int turn;

PixelRGB COLOR_BLACK = {0, 0, 0};
PixelRGB COLOR_RED = {255, 0, 0};
PixelRGB COLOR_GREEN = {0, 255, 0};
PixelRGB COLOR_BLUE = {0, 0, 255};
PixelRGB COLOR_YELLOW = {255, 255, 0};
PixelRGB COLOR_CYAN = {0, 255, 255};
PixelRGB COLOR_MAGENTA = {255, 0, 255};
PixelRGB COLOR_WHITE = {255, 255, 255};

#define COLORS_MAX 3

PixelRGB colors[COLORS_MAX] = {
  COLOR_RED, COLOR_GREEN, COLOR_BLUE, 
};

PixelRGB pic_winner[ColorduinoScreenWidth][ColorduinoScreenHeight] = {
  {COLOR_BLACK, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK}, 
  {COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_GREEN, COLOR_GREEN}, 
  {COLOR_GREEN, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK}, 
  {COLOR_GREEN, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}, 
  {COLOR_GREEN, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}, 
  {COLOR_GREEN, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK}, 
  {COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_GREEN, COLOR_GREEN}, 
  {COLOR_BLACK, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_GREEN, COLOR_BLACK},
};

PixelRGB pic_loser[ColorduinoScreenWidth][ColorduinoScreenHeight] = {
  {COLOR_RED, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_RED, COLOR_BLACK}, 
  {COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_RED, COLOR_RED}, 
  {COLOR_BLACK, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_RED, COLOR_BLACK}, 
  {COLOR_BLACK, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}, 
  {COLOR_BLACK, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}, 
  {COLOR_BLACK, COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_RED, COLOR_BLACK}, 
  {COLOR_BLACK, COLOR_RED, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_RED, COLOR_RED}, 
  {COLOR_RED, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_RED, COLOR_BLACK}, 
};

PixelRGB screen_data[ColorduinoScreenWidth][ColorduinoScreenHeight];

// state machine
typedef enum{
  StateStart,
  StateRunPrepare,
  StateRun,
  StateWinner,
  StateLoser,
} StateEnum;

StateEnum state;

int movingpixel_x;
int movingpixel_y;
PixelRGB movingpixel_color;

boolean colorCompare(const PixelRGB colorA, const PixelRGB colorB)
{
  if(colorA.r == colorB.r && colorA.g == colorB.g && colorA.b == colorB.b){
    return true;
  }
  return false;
}

void move_pixel_left()
{
  if(movingpixel_x > 0 && colorCompare(screen_data[movingpixel_x-1][movingpixel_y], COLOR_BLACK) == true){
    screen_data[movingpixel_x][movingpixel_y] = COLOR_BLACK;
    movingpixel_x--;
    screen_data[movingpixel_x][movingpixel_y] = movingpixel_color;
    displayPixelRGBData(screen_data);
  }
}
void move_pixel_right()
{
  if(movingpixel_x < ColorduinoScreenWidth - 1 && colorCompare(screen_data[movingpixel_x+1][movingpixel_y], COLOR_BLACK) == true){
    screen_data[movingpixel_x][movingpixel_y] = COLOR_BLACK;
    movingpixel_x++;
    screen_data[movingpixel_x][movingpixel_y] = movingpixel_color;
    displayPixelRGBData(screen_data);
  }
}

boolean checkAndClear(int mx, int my)
{
    PixelRGB color = screen_data[mx][my];
    if(colorCompare(color, COLOR_BLACK) == true){
      return false;
    }
    
    //check if anything row-3 or col-3 and clear
    //check horizontal, row
    int pos[8];
    int num;
    int x, y;
    
    num = 0;
    pos[num] = mx;
    num++;
    for(x = mx - 1; x >= 0; x--){
      if(colorCompare(screen_data[x][my], color) == true){
        pos[num] = x;
        num++;
      }
      else{
        break;
      }
    }
    for(x = mx + 1; x < ColorduinoScreenWidth; x++){
      if(colorCompare(screen_data[x][my], color) == true){
        pos[num] = x;
        num++;
      }
      else{
        break;
      }
    }
    if(num >= 3){
      for(int i = 0; i < num; i++){
        screen_data[pos[i]][my] = COLOR_BLACK;
      }
      displayPixelRGBData(screen_data);
      delay(400);
      for(int i = 0; i < num; i++){
        for(y = my + 1; y < ColorduinoScreenHeight; y++){
          screen_data[pos[i]][y - 1] = screen_data[pos[i]][y];
        }
      }
      displayPixelRGBData(screen_data);
      delay(400);
      
      for(int w = 0; w < ColorduinoScreenWidth; w++){
        for(int h = 0; h < ColorduinoScreenHeight; h++){
          if(colorCompare(screen_data[w][h], COLOR_BLACK) == false){
            checkAndClear(w, h);
          }
        }
      }
      
      return true;
    }
    
    //check vertical, col
    num = 0;
    pos[num] = my;
    num++;
    for(y = my - 1; y >= 0; y--){
      if(colorCompare(screen_data[mx][y], color) == true){
        pos[num] = y;
        num++;
      }
      else{
        break;
      }
    }
    if(num >= 3){
      for(int i = 0; i < num; i++){
        screen_data[mx][pos[i]] = COLOR_BLACK;
      }
      displayPixelRGBData(screen_data);
      delay(400);
      return true;
    }
    
    return false;
}
void move_pixel_down()
{
  if(movingpixel_y == 0 || colorCompare(screen_data[movingpixel_x][movingpixel_y - 1], COLOR_BLACK) == false){//to be fixed
    if(checkAndClear(movingpixel_x, movingpixel_y) == true){
      Serial.println("checkAndClear() == true");
      movingpixel_start();
      return;
    }
    if(movingpixel_y == ColorduinoScreenHeight - 1){//at top
      state = StateLoser;
      delay(1000);
      return;
    }
    else{//new moving pixel
      movingpixel_start();
      return;
    }
  }
  
  screen_data[movingpixel_x][movingpixel_y] = COLOR_BLACK;
  movingpixel_y--;
  screen_data[movingpixel_x][movingpixel_y] = movingpixel_color;
  displayPixelRGBData(screen_data);
}

void movingpixel_start()
{
  movingpixel_x = random(ColorduinoScreenWidth);
  movingpixel_y = ColorduinoScreenHeight - 1;
  movingpixel_color = colors[random(COLORS_MAX)];
  
  screen_data[movingpixel_x][movingpixel_y] = movingpixel_color;
  displayPixelRGBData(screen_data);
  
  turn++;
  if(turn == TURN_MAX){
    state = StateWinner;
  }
}

void displayPixelRGBData(PixelRGB data[ColorduinoScreenWidth][ColorduinoScreenHeight])
{
  for(int w = 0; w < ColorduinoScreenWidth; w++){
    for(int h = 0; h < ColorduinoScreenHeight; h++){
      Colorduino.SetPixel(w, h, data[w][h].r, data[w][h].g, data[w][h].b);
    }
  }
  Colorduino.FlipPage();
}
void fillColor(PixelRGB rgb)
{
  for(int w = 0; w < ColorduinoScreenWidth; w++){
    for(int h = 0; h < ColorduinoScreenHeight; h++){
      screen_data[w][h].r = rgb.r;
      screen_data[w][h].g = rgb.g;
      screen_data[w][h].b = rgb.b;
    }
  }
  displayPixelRGBData(screen_data);
}

unsigned long time_previous;
unsigned long time_previous_for_button;
void setup()
{
  randomSeed(analogRead(0));
  
  pinMode(BUTTON_LEFT_PIN, INPUT);
  pinMode(BUTTON_RIGHT_PIN, INPUT);
  
  Serial.begin(57600);
  
  Colorduino.Init();
  // R, G, B, value is between 0~63
  unsigned char whiteBalVal[3] = {36,63,63};
  Colorduino.SetWhiteBal(whiteBalVal);
  
  time_previous = millis();
  
  state = StateStart;
}

void loop()
{
  switch(state){
    case StateStart:
    {
      for(int i = 0; i < COLORS_MAX; i++){
        fillColor(colors[i]);
        delay(1000);
      }
      state = StateRunPrepare;
    }
    break;
    case StateRunPrepare:
    {
      fillColor(COLOR_BLACK);
      delay(100);
      
      movingpixel_start();
      turn = 0;
      state = StateRun;
    }
    break;
    case StateRun:
    {
      if(button_left_status == LOW){
        button_left_status = digitalRead(BUTTON_LEFT_PIN);
      }
      if(button_right_status == LOW){
        button_right_status = digitalRead(BUTTON_RIGHT_PIN);
      }
      
      if(button_left_status == HIGH){
        Serial.println("Left button pressed");
      }
      if(button_right_status == HIGH){
        Serial.println("Right button pressed");
      }
      unsigned long time_now = millis();
      if(time_now - time_previous_for_button > button_delay){
        time_previous_for_button = time_now;
        if(button_left_status == HIGH){
          move_pixel_left();
        }
        if(button_right_status == HIGH){
          move_pixel_right();
        }
        button_left_status = LOW;
        button_right_status = LOW;
      }
      
      if(time_now - time_previous > falling_delay){
        time_previous = time_now;
        move_pixel_down();
      }
    }
    break;
    case StateWinner:
      displayPixelRGBData(pic_winner);
      delay(5000);
      state = StateStart;
    break;
    case StateLoser:
      displayPixelRGBData(pic_loser);
      delay(5000);
      state = StateStart;
    break;
    default: // error, should not be here, reset
      state = StateStart;
    break;
  }
}

