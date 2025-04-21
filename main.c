#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <pthread.h>
#include <malloc.h>

#define DARK_BLUE "\033[38;5;19;48;5;19m▀▀\033[0m"
#define LIGHT_BLUE "\033[38;5;14;48;5;14m▀▀\033[0m"
#define ORANGE "\033[38;5;208;48;5;208m▀▀\033[0m"
#define PURPLE "\033[38;5;57;48;5;57m▀▀\033[0m"
#define YELLOW "\033[38;5;226;48;5;226m▀▀\033[0m"
#define RED "\033[38;5;9;48;5;9m▀▀\033[0m"
#define GREEN "\033[38;5;82;48;5;82m▀▀\033[0m"
#define WHITE "\033[38;5;15;48;5;15m▀▀\033[0m"

#define NEWLINE "\n"
#define TAB "  "

#define gotoxy(x, y) printf("\033[%d;%dH", (y), (x))

int HEIGHT = 40;
int WIDTH = 20;
int TETROMINOS_LIST_LENGTH = 100000;
int MAIN_THREAD_SLEEP_TIME = 300000;
int INPUT_THREAD_SLEEP_TIME = 300000;
pthread_mutex_t mutex;

struct termios orig_termios;

void disable_raw_mode()
{
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode()
{
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disable_raw_mode);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char read_key()
{
  char c;
  read(STDIN_FILENO, &c, 1);

  if (c == '\x1b')
  { // ESC sequence
    char seq[2];
    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return '\x1b';

    if (seq[0] == '[')
    {
      switch (seq[1])
      {
      case 'A':
        return 'U'; // Up
      case 'B':
        return 'D'; // Down
      case 'C':
        return 'R'; // Right
      case 'D':
        return 'L'; // Left
      }
    }
  }
  return c;
}

/*
  CONVENTIONS

  The following straight tetromino is 0 degrees:
    ___
   |__|
   |__|
   |__|
   |__|

  The follwing T tetromino is 0 degrees:
       ___
    __|__|___
   |__|__|__|

  The following L right tetromino is at 0 degrees:
   ___
  |__|
  |__|___
  |__|__|

  The following L left tetromino is at 0 degrees:
      ___
     |__|
   __|__|
  |__|__|

  The following skew right tetromino is at 0 degrees:
      ______
   __|__|__|
  |__|__|

  The following skew left tetromino is at 0 degrees:
   ______
  |__|__|___
     |__|__|
*/

enum SquareColor
{
  LIGHT_BLUE_SQUARE = 1,
  YELLOW_SQUARE = 2,
  PURPLE_SQUARE = 3,
  ORANGE_SQAURE = 4,
  DARK_BLUE_SQAURE = 5,
  GREEN_SQUARE = 6,
  RED_SQUARE = 7,
  WHITE_SQUARE = 8
};

enum TetrominoTypes
{
  STRAIGHT = 1,
  SQUARE = 2,
  T = 3,
  L_RIGHT = 4,
  L_LEFT = 5,
  SKEW_RIGHT = 6,
  SKEW_LEFT = 7
};

typedef struct
{
  int tetrominoType;
  int color;
  int x;
  int y;
  int angle; // 0, 90, 180, or 270
} Tetromino;

typedef struct
{
  int x;
  int y;
} Coordinate;

int *playingGrid;
Tetromino currentTetromino;

char *getColor(int color)
{
  switch (color)
  {
  case DARK_BLUE_SQAURE:
    return DARK_BLUE;
    break;
  case LIGHT_BLUE_SQUARE:
    return LIGHT_BLUE;
    break;
  case ORANGE_SQAURE:
    return ORANGE;
    break;
  case PURPLE_SQUARE:
    return PURPLE;
    break;
  case YELLOW_SQUARE:
    return YELLOW;
    break;
  case RED_SQUARE:
    return RED;
    break;
  case GREEN_SQUARE:
    return GREEN;
    break;
  default:
    return WHITE;
    break;
  }
}

int getPlayingGridIndex(int row, int column)
{
  return column * WIDTH + row;
}

int getRowFromPlayingGridIndex(int playingGridIndex, int column)
{
  return playingGridIndex - (column * WIDTH);
}

int getColumnFromPlayingGridIndex(int playingGridIndex, int row)
{
  return (playingGridIndex - row) / WIDTH;
}

int max(int a, int b)
{
  if (a > b)
    return a;
  return b;
}

void printSquare(char *color)
{
  printf(color);
}

Coordinate *getStraightTetrominoCoordinates(Tetromino tetromino)
{
  Coordinate *coordinates = malloc(4 * sizeof(Coordinate));
  if (tetromino.angle == 0 || tetromino.angle == 180)
    for (int i = tetromino.y; i < tetromino.y + 4; ++i)
      coordinates[i - tetromino.y] = (Coordinate){.x = tetromino.x, .y = i};
  else
    for (int i = tetromino.x; i < tetromino.x + 4; ++i)
      coordinates[i - tetromino.x] = (Coordinate){.x = i, .y = tetromino.y};
  return coordinates;
}

Coordinate *getSquareTetrominoCoordintes(Tetromino tetromino)
{
  Coordinate *coordinates = (Coordinate *)malloc(4 * sizeof(Coordinate));
  coordinates[0] = (Coordinate){.x = tetromino.x, .y = tetromino.y};
  coordinates[1] = (Coordinate){.x = tetromino.x, .y = tetromino.y + 1};
  coordinates[2] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y};
  coordinates[3] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y + 1};
  return coordinates;
}

Coordinate *getTTetrominoCoordinates(Tetromino tetromino)
{
  Coordinate *coordinates = (Coordinate *)malloc(4 * sizeof(Coordinate));
  if (tetromino.angle == 0 || tetromino.angle == 180)
  {
    coordinates[0] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y};
    for (int i = tetromino.x; i < tetromino.x + 3; ++i)
    {
      int yIndex = tetromino.angle == 0 ? tetromino.y + 1 : tetromino.y;
      coordinates[i - tetromino.x + 1] = (Coordinate){.x = i, .y = yIndex};
    }
  }
  else
  {
    int xIndex = tetromino.angle == 90 ? tetromino.x + 1 : tetromino.x;
    coordinates[0] = (Coordinate){.x = xIndex, .y = tetromino.y + 1};
    for (int i = tetromino.y; i < tetromino.y + 3; ++i)
    {
      xIndex = tetromino.angle == 90 ? tetromino.x : tetromino.x + 1;
      coordinates[i - tetromino.y + 1] = (Coordinate){.x = xIndex, .y = i};
    }
  }

  return coordinates;
}

Coordinate *getLRightTetrominoCoordinates(Tetromino tetromino)
{
  Coordinate *coordinates = (Coordinate *)malloc(4 * sizeof(Coordinate));
  if (tetromino.angle == 0)
  {
    coordinates[0] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y + 2};
    for (int i = tetromino.y; i < tetromino.y + 3; ++i)
      coordinates[i - tetromino.y + 1] = (Coordinate){.x = tetromino.x, .y = i};
  }
  else if (tetromino.angle == 180)
  {
    coordinates[0] = (Coordinate){.x = tetromino.x, .y = tetromino.y};
    for (int i = tetromino.y; i < tetromino.y + 3; ++i)
      coordinates[i - tetromino.y + 1] = (Coordinate){.x = tetromino.x + 1, .y = i};
  }
  else if (tetromino.angle == 90)
  {
    coordinates[0] = (Coordinate){.x = tetromino.x, .y = tetromino.y + 1};
    for (int i = tetromino.x; i < tetromino.x + 3; ++i)
      coordinates[i - tetromino.x + 1] = (Coordinate){.x = i, .y = tetromino.y};
  }
  else
  {
    coordinates[0] = (Coordinate){.x = tetromino.x + 2, .y = tetromino.y};
    for (int i = tetromino.x; i < tetromino.x + 3; ++i)
      coordinates[i - tetromino.x + 1] = (Coordinate){.x = i, .y = tetromino.y + 1};
  }

  return coordinates;
}

Coordinate *getLLeftTetrominoCoordinates(Tetromino tetromino)
{
  Coordinate *coordinates = (Coordinate *)malloc(4 * sizeof(Coordinate));
  if (tetromino.angle == 0)
  {
    coordinates[0] = (Coordinate){.x = tetromino.x, .y = tetromino.y + 2};
    for (int i = tetromino.y; i < tetromino.y + 3; ++i)
      coordinates[i - tetromino.y + 1] = (Coordinate){.x = tetromino.x + 1, .y = i};
  }
  else if (tetromino.angle == 180)
  {
    coordinates[0] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y};
    for (int i = tetromino.y; i < tetromino.y + 3; ++i)
      coordinates[i - tetromino.y + 1] = (Coordinate){.x = tetromino.x, .y = i};
  }
  else if (tetromino.angle == 90)
  {
    coordinates[0] = (Coordinate){.x = tetromino.x, .y = tetromino.y};
    for (int i = tetromino.x; i < tetromino.x + 3; ++i)
      coordinates[i - tetromino.x + 1] = (Coordinate){.x = i, .y = tetromino.y + 1};
  }
  else
  {
    coordinates[0] = (Coordinate){.x = tetromino.x + 2, .y = tetromino.y + 1};
    for (int i = tetromino.x; i < tetromino.x + 3; ++i)
      coordinates[i - tetromino.x + 1] = (Coordinate){.x = i, .y = tetromino.y};
  }
  return coordinates;
}

Coordinate *getSkewRightTetrominoCoordinates(Tetromino tetromino)
{
  Coordinate *coordinates = (Coordinate *)malloc(4 * sizeof(Coordinate));
  if (tetromino.angle == 0 || tetromino.angle == 180)
  {
    coordinates[0] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y};
    coordinates[1] = (Coordinate){.x = tetromino.x + 2, .y = tetromino.y};
    coordinates[2] = (Coordinate){.x = tetromino.x, .y = tetromino.y + 1};
    coordinates[3] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y + 1};
  }
  else if (tetromino.angle == 90 || tetromino.angle == 270)
  {
    coordinates[0] = (Coordinate){.x = tetromino.x, .y = tetromino.y};
    coordinates[1] = (Coordinate){.x = tetromino.x, .y = tetromino.y + 1};
    coordinates[2] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y + 1};
    coordinates[3] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y + 2};
  }
  return coordinates;
}

Coordinate *getSkewLeftTetrominoCoordinates(Tetromino tetromino)
{
  Coordinate *coordinates = (Coordinate *)malloc(4 * sizeof(Coordinate));
  if (tetromino.angle == 0 || tetromino.angle == 180)
  {
    coordinates[0] = (Coordinate){.x = tetromino.x, .y = tetromino.y};
    coordinates[1] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y};
    coordinates[2] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y + 1};
    coordinates[3] = (Coordinate){.x = tetromino.x + 2, .y = tetromino.y + 1};
  }
  else if (tetromino.angle == 90 || tetromino.angle == 270)
  {
    coordinates[0] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y};
    coordinates[1] = (Coordinate){.x = tetromino.x, .y = tetromino.y + 1};
    coordinates[2] = (Coordinate){.x = tetromino.x + 1, .y = tetromino.y + 1};
    coordinates[3] = (Coordinate){.x = tetromino.x, .y = tetromino.y + 2};
  }
  return coordinates;
}

Coordinate *getTetrominoCoordinates(Tetromino tetromino)
{
  switch (tetromino.tetrominoType)
  {
  case STRAIGHT:
    return getStraightTetrominoCoordinates(tetromino);
  case SQUARE:
    return getSquareTetrominoCoordintes(tetromino);
  case T:
    return getTTetrominoCoordinates(tetromino);
  case L_RIGHT:
    return getLRightTetrominoCoordinates(tetromino);
  case L_LEFT:
    return getLLeftTetrominoCoordinates(tetromino);
  case SKEW_RIGHT:
    return getSkewRightTetrominoCoordinates(tetromino);
  case SKEW_LEFT:
    return getSkewLeftTetrominoCoordinates(tetromino);
  default:
    break;
  }
}

int maxLength(Tetromino tetromino)
{
  if (tetromino.tetrominoType == STRAIGHT)
    return tetromino.angle == 0 || tetromino.angle == 180 ? 0 : 3;
  else if (tetromino.tetrominoType == SQUARE)
    return 1;
  else if (tetromino.tetrominoType == T)
    return tetromino.angle == 0 || tetromino.angle == 180 ? 2 : 1;
  else if (tetromino.tetrominoType == L_LEFT || tetromino.tetrominoType == L_RIGHT)
    return tetromino.angle == 0 || tetromino.angle == 180 ? 1 : 2;
  else if (tetromino.tetrominoType == SKEW_LEFT || tetromino.tetrominoType == SKEW_RIGHT)
    return tetromino.angle == 0 || tetromino.angle == 180 ? 2 : 1;
}

int maxHeight(Tetromino tetromino)
{
  if (tetromino.tetrominoType == STRAIGHT)
    return tetromino.angle == 0 || tetromino.angle == 180 ? 3 : 0;
  else if (tetromino.tetrominoType == SQUARE)
    return 1;
  else if (tetromino.tetrominoType == T)
    return tetromino.angle == 0 || tetromino.angle == 180 ? 1 : 2;
  else if (tetromino.tetrominoType == L_LEFT || tetromino.tetrominoType == L_RIGHT)
    return tetromino.angle == 0 || tetromino.angle == 180 ? 2 : 1;
  else if (tetromino.tetrominoType == SKEW_LEFT || tetromino.tetrominoType == SKEW_RIGHT)
    return tetromino.angle == 0 || tetromino.angle == 180 ? 1 : 2;
}

void displayPlayingGrid()
{
  for (int i = 0; i < HEIGHT; ++i)
  {
    for (int j = 0; j < WIDTH; ++j)
      printf("%d ", playingGrid[getPlayingGridIndex(j, i)]);
    printf(NEWLINE);
  }
}

bool isTetrominoOutOfBounds(int newX, int newY, int newAngle)
{
  Tetromino newTetromino;
  newTetromino.angle = newAngle;
  newTetromino.x = newX;
  newTetromino.y = newY;
  newTetromino.color = currentTetromino.color;
  newTetromino.tetrominoType = currentTetromino.tetrominoType;

  Coordinate *newTetrominoCoordinates = getTetrominoCoordinates(newTetromino);

  if (newX < 0 || newY >= HEIGHT)
    return true;
  // Use the max length of the tetromino at it's current angle and check if it is out of bounds
  else if (newX + maxLength(newTetromino) >= WIDTH)
    return true;
  else if (newY + maxHeight(newTetromino) >= HEIGHT)
    return true;

  bool isOutOfBounds = false;
  for (int i = 0; i < 4; ++i)
  {
    // Check if the current square is occupied by another tetromino
    if (playingGrid[getPlayingGridIndex(newTetrominoCoordinates[i].x, newTetrominoCoordinates[i].y)] != 0)
    {
      isOutOfBounds = true;
      break;
    }
  }

  free(newTetrominoCoordinates);
  return isOutOfBounds;
}

void drawTetromino()
{
  Coordinate *coordinates = getTetrominoCoordinates(currentTetromino);
  for (int i = 0; i < 4; ++i)
    playingGrid[getPlayingGridIndex(coordinates[i].x, coordinates[i].y)] = currentTetromino.color;
  free(coordinates);
}

void clearCurrentTetromino()
{
  Coordinate *coordinates = getTetrominoCoordinates(currentTetromino);
  for (int i = 0; i < 4; ++i)
    playingGrid[getPlayingGridIndex(coordinates[i].x, coordinates[i].y)] = 0;
  free(coordinates);
}

void drawPlayingGrid()
{
  gotoxy(0, 0);
  for (int i = 0; i < HEIGHT; ++i)
  {
    printf("\033[38;5;15m| \033[0m");
    for (int j = 0; j < WIDTH; ++j)
      printSquare(getColor(playingGrid[getPlayingGridIndex(j, i)]));
    printf("\033[38;5;15m |\033[0m");
    printf(NEWLINE);
  }
  printf(TAB);
  for (int i = 0; i < WIDTH * 2; ++i)
    printf("\033[38;5;15m=\033[0m");
  printf(NEWLINE);
}

Tetromino getRandomTetromino()
{
  srand(time(NULL));
  // Random number in the range [1, 7]
  int randomTetrominoNumber = rand() % 7 + 1;

  srand(time(NULL));
  // Random number in the range [0, WIDTH - 3]
  int randomX = rand() % (WIDTH - 3) + 1;

  Tetromino randomTetromino;
  randomTetromino.x = randomX;
  randomTetromino.y = 1;
  randomTetromino.angle = 0;

  switch (randomTetrominoNumber)
  {
  case STRAIGHT:
    randomTetromino.tetrominoType = STRAIGHT;
    randomTetromino.color = LIGHT_BLUE_SQUARE;
    break;
  case SQUARE:
    randomTetromino.tetrominoType = SQUARE;
    randomTetromino.color = YELLOW_SQUARE;
    break;
  case T:
    randomTetromino.tetrominoType = T;
    randomTetromino.color = PURPLE_SQUARE;
    break;
  case L_RIGHT:
    randomTetromino.tetrominoType = L_RIGHT;
    randomTetromino.color = ORANGE_SQAURE;
    break;
  case L_LEFT:
    randomTetromino.tetrominoType = L_LEFT;
    randomTetromino.color = DARK_BLUE_SQAURE;
    break;
  case SKEW_RIGHT:
    randomTetromino.tetrominoType = SKEW_RIGHT;
    randomTetromino.color = GREEN_SQUARE;
    break;
  case SKEW_LEFT:
    randomTetromino.tetrominoType = SKEW_LEFT;
    randomTetromino.color = RED_SQUARE;
    break;
  default:
    break;
  }

  return randomTetromino;
}

void update()
{
  clearCurrentTetromino();
  drawPlayingGrid();
}

char getch()
{
  char buf = 0;
  struct termios old = {0};
  if (tcgetattr(0, &old) < 0)
    perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");
  if (read(0, &buf, 1) < 0)
    perror("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0)
    perror("tcsetattr ~ICANON");
  return (buf);
}

void *inputThreadCallback(void *arg)
{
  while (true)
  {
    char key = read_key();
    pthread_mutex_lock(&mutex);
    clearCurrentTetromino();
    switch (key)
    {
    case 'U':
      currentTetromino.angle += 90;
      if (currentTetromino.angle == 360)
        currentTetromino.angle = 0;
      break;
    case 'D':
      if (!isTetrominoOutOfBounds(currentTetromino.x, currentTetromino.y + 1, currentTetromino.angle))
        ++currentTetromino.y;
      break;
    case 'L':
      if (!isTetrominoOutOfBounds(currentTetromino.x - 1, currentTetromino.y, currentTetromino.angle))
        --currentTetromino.x;
      break;
    case 'R':
      if (!isTetrominoOutOfBounds(currentTetromino.x + 1, currentTetromino.y, currentTetromino.angle))
        ++currentTetromino.x;
      break;
    }
    drawTetromino();
    drawPlayingGrid();
    pthread_mutex_unlock(&mutex);
    usleep(INPUT_THREAD_SLEEP_TIME);
  }

  return NULL;
}

int *linesToDestroy()
{
  int *lineIndexes = malloc(HEIGHT * sizeof(int));
  for (int i = HEIGHT - 1; i >= 0; --i)
  {
    bool completeLine = true;
    for (int j = 0; j < WIDTH; ++j)
    {
      // There is a gap
      if (playingGrid[getPlayingGridIndex(j, i)] == 0)
      {
        // If there were already line(s) found before this then don't continue trying to find more
        if (i < HEIGHT - 1 && lineIndexes[i + 1])
          return lineIndexes;
        completeLine = false;
        break;
      }
    }

    if (completeLine)
      lineIndexes[i] = 1;
  }
  return lineIndexes;
}

void destroyLines(int *lineIndexes)
{
  int highestIndex = __INT_MAX__;
  int lowestIndex = 0;
  for (int i = HEIGHT - 1; i >= 0; --i)
  {
    if (lineIndexes[i] == 1)
    {
      for (int j = 0; j < WIDTH; ++j)
        playingGrid[getPlayingGridIndex(j, i)] = 0;
      if (i < highestIndex)
        highestIndex = i;
      if (i > lowestIndex)
        lowestIndex = i;
    }
  }

  int linesToMoveDown = lowestIndex - highestIndex + 1;
  if (linesToMoveDown < 0)
    return;
  for (int i = highestIndex - 1; i >= 0; --i)
    for (int j = 0; j < WIDTH; ++j)
      playingGrid[getPlayingGridIndex(j, i + linesToMoveDown)] = playingGrid[getPlayingGridIndex(j, i)];
}

int main()
{
  playingGrid = malloc(sizeof(int[WIDTH][HEIGHT]));

  // Tetromino tetromino;
  // tetromino.x = 0;
  // tetromino.y = 0;
  // tetromino.angle = 0;
  // tetromino.tetrominoType = SKEW_RIGHT;
  // tetromino.color = GREEN_SQUARE;
  // drawTetromino(tetromino);

  enable_raw_mode();

  playingGrid[getPlayingGridIndex(3, 1)] = 0;

  pthread_t inputThread;
  pthread_mutex_init(&mutex, NULL);
  pthread_create(&inputThread, NULL, inputThreadCallback, NULL);

  currentTetromino = getRandomTetromino();
  system("clear");

  while (true)
  {
    pthread_mutex_lock(&mutex);

    clearCurrentTetromino();
    if (isTetrominoOutOfBounds(currentTetromino.x, currentTetromino.y + 1, currentTetromino.angle))
    {
      drawTetromino();
      destroyLines(linesToDestroy());
      drawPlayingGrid();
      currentTetromino = getRandomTetromino();
      if (isTetrominoOutOfBounds(currentTetromino.x, currentTetromino.y + 1, currentTetromino.angle))
      {
        drawTetromino();
        drawPlayingGrid();
        return 0;
      }
    }
    else
      ++currentTetromino.y;

    drawTetromino();
    drawPlayingGrid();

    pthread_mutex_unlock(&mutex);
    usleep(MAIN_THREAD_SLEEP_TIME);
  }

  free(playingGrid);
  return 0;
}
