#include <stdio.h>
#include <var_array.h>
/* Don't forget to link this program with the flag -lvar_array. */


void printMatrix(vaArray_2d(int) m)
{
  int rowc = vaSize(m,1);     // number of rows
  int colc = vaSize(m,2);     // number of columns
  int i, j;
  
  for (i = 0; i < rowc; i++)
  {
    for (j = 0; j < colc; j++)
      printf(" %4d", m[i][j]);
    printf("\n");
  }
}


int main(void)
{
  vaArray_2d(int) a1 = vaCreate_2d( 2,  3, int, NULL);
  vaArray_2d(int) a2 = vaCreate_2d(10,  3, int, NULL);
  vaArray_2d(int) a3 = vaCreate_2d( 6, 10, int, NULL);
  int n, i, j;

  n = 0;
  for (i = 0; i < 2; i++)
    for (j = 0; j < 3; j++)
      a1[i][j] = ++n;
  for (i = 0; i < 10; i++)
    for (j = 0; j < 3; j++)
      a2[i][j] = 10*i;
  for (i = 0; i < 6; i++)
    for (j = 0; j < 10; j++)
      a3[i][j] = 0;

  printMatrix(a1);
  printMatrix(a2);
  printMatrix(a3);

  vaDelete(a1);
  vaDelete(a2);
  vaDelete(a3);
  return 0;
}
