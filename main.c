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

int HEIGHT = 20;
int WIDTH = 15;

int *playingGrid;

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

enum Tetromino
{
  STRAIGHT = 1,
  SQUARE = 2,
  T = 3,
  L_RIGHT = 4,
  L_LEFT = 5,
  SKEW_RIGHT = 6,
  SKEW_LEFT = 7
};

int getPlayingGridIndex(int row, int column)
{
  return column * WIDTH + row;
}

void printSquare(char *color)
{
  printf(color);
}

void straightTetromino(char *color)
{
  for (int i = 0; i < 4; ++i)
    printSquare(color);
  printf(NEWLINE);
}

void squareTetromino(char *color)
{
  printSquare(color);
  printSquare(color);
  printf(NEWLINE);
  printSquare(color);
  printSquare(color);
  printf(NEWLINE);
}

void tTetromino(char *color)
{
  for (int i = 0; i < 3; ++i)
    printSquare(color);
  printf(NEWLINE);
  printf(TAB);
  printSquare(color);
  printf(NEWLINE);
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

void drawMatrix()
{
  for (int i = 0; i < HEIGHT; ++i)
  {
    for (int j = 0; j < WIDTH; ++j)
      printSquare(getColor(playingGrid[getPlayingGridIndex(j, i)]));
    printf(NEWLINE);
  }
}

void addTetromino(int (*playingGrid)[], int index, int tetrominoNumber)
{
}

int main()
{
  playingGrid = malloc(sizeof(int[WIDTH][HEIGHT]));

  playingGrid[0] = RED_SQUARE;
  playingGrid[1] = DARK_BLUE_SQAURE;

  drawMatrix(playingGrid);

  straightTetromino(&LIGHT_BLUE);
  printf(NEWLINE);
  squareTetromino(&YELLOW);
  printf(NEWLINE);
  tTetromino(&PURPLE);
  printf(NEWLINE);
  lTetromino(&ORANGE);
  printf(NEWLINE);
  skewTetromino(&GREEN);
  printf(NEWLINE);

  free(playingGrid);
  return 0;
}
