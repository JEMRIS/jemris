/**************************************************************************
** Copyright (c) 2003-2005                                               **
** Forschungszentrum Juelich GmbH, Federal Republic of Germany.          **
**                                                                       **
** See the file `var_array.c',                                           **
**              `list_c.h', or                                           **
**              `var_array++_cpp.h' for details                          **
**************************************************************************/

/*
 * This file is part of the VarArray library, version 1.0.
 *
 * Authors:
 *  Guenter Egerer, Annika Schiller, Forschungszentrum Juelich
 * Bugs:
 *  Please report any bugs to `G.Egerer@fz-juelich.de'.
 */

#ifndef VAR_ARRAYplusplus_H_INCLUDED
#define VAR_ARRAYplusplus_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
#  include "var_array.h"
#ifdef __cplusplus
}

#include <string>
#include "var_array++_cpp.h"


template <typename T>
inline std::ostream& vaPrint(std::ostream& ostr, vaArray_1d(T) array, int indent = 0)
{
  return vaPrint_1d(ostr, array, indent);
}

template <typename T>
inline std::ostream& vaPrint(std::ostream& ostr, vaArray_2d(T) array, int indent = 0)
{
  return vaPrint_2d(ostr, array, indent);
}

template <typename T>
inline std::ostream& vaPrint(std::ostream& ostr, vaArray_3d(T) array, int indent = 0)
{
  return vaPrint_3d(ostr, array, indent);
}

template <typename T>
inline std::ostream& vaPrint(std::ostream& ostr, vaArray_4d(T) array, int indent = 0)
{
  return vaPrint_4d(ostr, array, indent);
}

template <typename T>
inline std::ostream& vaPrint(std::ostream& ostr, vaArray_5d(T) array, int indent = 0)
{
  return vaPrint_5d(ostr, array, indent);
}


template <typename T>
std::ostream& vaPrint_1d(std::ostream& ostr, vaArray_1d(T) array, int indent = 0)
{
  if (vaSize(array, 1) == -1)
    std::cerr << "vaPrint: error: no valid variable size array specified."
	      << std::endl;
  else
    vaArray::var_array_io<1, vaArray_1d(T)>().print(ostr, array, indent);
  return ostr;
}

template <typename T>
std::ostream& vaPrint_2d(std::ostream& ostr, vaArray_2d(T) array, int indent = 0)
{
  if (vaSize(array, 2) == -1)
    std::cerr << "vaPrint: error: no valid variable size array specified."
	      << std::endl;
  else
    vaArray::var_array_io<2, vaArray_2d(T)>().print(ostr, array, indent);
  return ostr;
}

template <typename T>
std::ostream& vaPrint_3d(std::ostream& ostr, vaArray_3d(T) array, int indent = 0)
{
  if (vaSize(array, 3) == -1)
    std::cerr << "vaPrint: error: no valid variable size array specified."
	      << std::endl;
  else
    vaArray::var_array_io<3, vaArray_3d(T)>().print(ostr, array, indent);
  return ostr;
}

template <typename T>
std::ostream& vaPrint_4d(std::ostream& ostr, vaArray_4d(T) array, int indent = 0)
{
  if (vaSize(array, 4) == -1)
    std::cerr << "vaPrint: error: no valid variable size array specified."
	      << std::endl;
  else
    vaArray::var_array_io<4, vaArray_4d(T)>().print(ostr, array, indent);
  return ostr;
}

template <typename T>
std::ostream& vaPrint_5d(std::ostream& ostr, vaArray_5d(T) array, int indent = 0)
{
  if (vaSize(array, 5) == -1)
    std::cerr << "vaPrint: error: no valid variable size array specified."
	      << std::endl;
  else
    vaArray::var_array_io<5, vaArray_5d(T)>().print(ostr, array, indent);
  return ostr;
}


//template <typename T>
//inline std::ostream& operator<<(std::ostream& ostr, vaArray_1d(T) rhs)
//{
//  return var_array_io<1, vaArray_1d(T)>().print(ostr, rhs);
//}

//template <typename T>
//inline std::ostream& operator<<(std::ostream& ostr, vaArray_2d(T) rhs)
//{
//  return var_array_io<2, vaArray_2d(T)>().print(ostr, rhs);
//}
#endif

#endif
