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

#define NEWLINE "\n"
#define TAB "  "

int HEIGHT = 10;
int WIDTH = 5;

enum SquareColor
{
  DARK_BLUE_SQAURE = 1,
  LIGHT_BLUE_SQUARE = 2,
  ORANGE_SQAURE = 3,
  PURPLE_SQUARE = 4,
  YELLOW_SQUARE = 5,
  RED_SQUARE = 6,
  GREEN_SQUARE = 7
};

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

void displayMatrix(int (*playingGrid)[])
{
  int total = 0;
  for (int i = 0; i < HEIGHT; ++i)
  {
    for (int j = 0; j < WIDTH; ++j)
    {
      printf("%d ", (*playingGrid)[total]);
      ++total;
    }
    printf(NEWLINE);
  }
}

int main()
{
  int (*playingGrid)[HEIGHT] = malloc(sizeof(int[HEIGHT][WIDTH]));

  playingGrid[0][0] = 10;
  playingGrid[1][0] = 20;

  displayMatrix(playingGrid);

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
