#include <iostream>
#include <var_array++.h>

int main(void)
{
  char ctab_[2][2] = {
    { 'a', 'b' },
    { 'c', 'd' }
  };
  // <em>the following code line makes the build-in array</em> ctab_
  // <em>accessable as VarArray</em> ctab<em>:</em>
  vaArray_2d(char) ctab = vaCreate_2d(2, 2, char, ctab_);

  std::cout << "Matrix:" << std::endl;
  vaPrint(std::cout, ctab);
  std::cout << "First line of matrix:" << std::endl;
  vaPrint(std::cout, ctab[0], 3);

  vaDelete(ctab);
}
