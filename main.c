#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
*/

enum SquareColor
{
  DARK_BLUE_SQAURE = 1,
  LIGHT_BLUE_SQUARE = 2,
  ORANGE_SQAURE = 3,
  PURPLE_SQUARE = 4,
  YELLOW_SQUARE = 5,
  RED_SQUARE = 6,
  GREEN_SQUARE = 7,
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
  char *color;
  int x;
  int y;
  int angle; // 0, 90, 180, or 270
} Tetromino;

int HEIGHT = 20;
int WIDTH = 15;
int TETROMINOS_LIST_LENGTH = 100000;

int *playingGrid;
Tetromino *tetrominosList;

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
    return YELLOW;
    break;
  }
}

int getPlayingGridIndex(int row, int column)
{
  return column * WIDTH + row;
}

void printSquare(char *color)
{
  printf(color);
}

void straightTetromino(Tetromino tetromino)
{
  int TETROMINO_LENGTH = 4;
  if (tetromino.angle == 0 || tetromino.angle == 180)
    for (int i = tetromino.y; i < tetromino.y + TETROMINO_LENGTH; ++i)
      playingGrid[getPlayingGridIndex(tetromino.x, i)] = tetromino.color;
  else
    for (int i = tetromino.x; i < tetromino.x + TETROMINO_LENGTH; ++i)
      playingGrid[getPlayingGridIndex(i, tetromino.y)] = tetromino.color;
}

void squareTetromino(Tetromino tetromino)
{
  playingGrid[getPlayingGridIndex(tetromino.x, tetromino.y)] = tetromino.color;
  playingGrid[getPlayingGridIndex(tetromino.x, tetromino.y + 1)] = tetromino.color;
  playingGrid[getPlayingGridIndex(tetromino.x + 1, tetromino.y)] = tetromino.color;
  playingGrid[getPlayingGridIndex(tetromino.x + 1, tetromino.y + 1)] = tetromino.color;
}

void tTetromino(Tetromino tetromino)
{
  if (tetromino.angle == 0)
  {
  }
}

void lTetromino(char *color)
{
  printSquare(color);
  printf(NEWLINE);
  printSquare(color);
  printf(NEWLINE);
  printSquare(color);
  printSquare(color);
  printf(NEWLINE);
}

void skewTetromino(char *color)
{
  printf(TAB);
  printSquare(color);
  printSquare(color);
  printf(NEWLINE);
  printSquare(color);
  printSquare(color);
  printf(NEWLINE);
}

void displayMatrix()
{
  for (int i = 0; i < HEIGHT; ++i)
  {
    for (int j = 0; j < WIDTH; ++j)
      printf("%d ", playingGrid[getPlayingGridIndex(j, i)]);
    printf(NEWLINE);
  }
}

void updatePlayingGrid()
{
  for (int i = 0; i < TETROMINOS_LIST_LENGTH; ++i)
  {
    if (tetrominosList[i].tetrominoType != 0)
    {
    }
  }
}

void drawMatrix()
{
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

int findFirstEmptyIndexInTetrominosList()
{
  for (int i = 0; i < TETROMINOS_LIST_LENGTH; ++i)
    if (tetrominosList[i].tetrominoType == 0)
      return i;
}

void addTetrominoToTetrominosList(Tetromino tetromino)
{
  int firstEmptyIndex = findFirstEmptyIndexInTetrominosList();
  tetrominosList[firstEmptyIndex] = tetromino;
}

int main()
{
  playingGrid = malloc(sizeof(int[WIDTH][HEIGHT]));
  tetrominosList = (Tetromino *)malloc(sizeof(Tetromino) * TETROMINOS_LIST_LENGTH);

  Tetromino tetromino;
  tetromino.x = 0;
  tetromino.y = 0;
  tetromino.angle = 0;
  tetromino.tetrominoType = squareTetromino;
  tetromino.color = LIGHT_BLUE_SQUARE;
  squareTetromino(tetromino);

  drawMatrix(playingGrid);

  free(playingGrid);
  free(tetrominosList);
  return 0;
}
