#include <stdio.h>
#include <var_array.h>


void printMatrix(void *m_, int rowc, int colc)
{
  int *m = (int *) m_;
  int i, j;

  for (i = 0; i < rowc; i++)
  {
    for (j = 0; j < colc; j++)
      printf(" %4d", m[colc*i+j]);
    printf("\n");
  }
}


int main(void)
{
  const int bounds[] = { 25, 16, 1000, 750, 5, 5, 100, 200, 0 };
  vaArray_2d(int) a;
  int n = 0;
  int nrow, ncol, i, j;

  while (
         ((nrow = bounds[n++]) > 0) && ((ncol = bounds[n++]) > 0)
      /* scanf("%d%d", &nrow, &ncol) == 2 */
        )
  {
    a = vaCreate_2d(nrow, ncol, int, NULL);
    printf("Matrix with %d rows and %d columns created.\n", nrow, ncol);

    for (i = 0; i < nrow; i++)
      for (j = 0; j < ncol; j++)
        a[i][j] = (i != j) ? 0 : 1;

    if ((nrow <= 10) && (ncol <= 10))
      printMatrix(&a[0][0], nrow, ncol);

    vaDelete(a);
    printf("Matrix deleted.\n");
  }

  return 0;
}
