#include <iostream>
using std::cout;
using std::endl;
#include <var_array++.h>


vaArray_2d(int) transpose(vaArray_2d(int) m)
{
  vaArray_2d(int) t = vaCreate_2d(vaSize(m,2), vaSize(m,1), int, NULL);

  for(int i = 0; i < vaSize(m,1); i++)
    for(int j = 0; j < vaSize(m,2); j++)
      t[j][i] = m[i][j];

  return t;
}


int main(void)
{
  int a[2][3] = { {1, 2, 3},
                  {4, 5, 6}
                };
  vaArray_2d(int) a1 = vaCreate_2d( 2,  3, int, a);

  vaPrint(cout, a1);

  vaArray_2d(int) a1_trans = transpose(a1);
  cout << "transposed " << endl;
  vaPrint(cout, a1_trans);

  vaDelete(a1);
  vaDelete(a1_trans);
  return 0;
}
