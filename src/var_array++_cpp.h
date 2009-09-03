/* Copyright (c) 2003-2005
   Forschungszentrum Juelich GmbH, Federal Republic of Germany.

   All rights reserved. Redistribution and use in source and binary forms,
   with or without modification, are permitted provided that the following
   conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
 
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

   * Neither the name of Forschungszentrum Juelich GmbH nor the names of
     its contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY FORSCHUNGSZENTRUM JUELICH GMBH "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL FORSCHUNGSZENTRUM JUELICH GMBH BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  */

/*
 * This file is part of the VarArray library, version 1.0.
 *
 * Authors:
 *  Guenter Egerer, Annika Schiller, Forschungszentrum Juelich
 * Bugs:
 *  Please report any bugs to `G.Egerer@fz-juelich.de'.
 */


#include <iostream>
#include <iomanip>
#include <sstream>

namespace vaArray {
  // forward declaration
  template <int N, typename T>
  struct var_array_io;


  template <int N, typename T>
  var_array_io<N, T> make_var_array_io(T)
  {
    return var_array_io<N, T>();
  }


  // primary class template 
  template <int N, typename T>
  struct var_array_io {

    std::ostream& print(std::ostream& ostr, T array, int indent)
    {
      if (indent > 0)
        ostr << std::string(indent, ' ');
      else
        indent = -indent;

      std::ostringstream os;
      os << (vaSize(array,1)-1);
      int n = os.str().length();
      std::string s("[ ");

      for (int i = 0; i < vaSize(array,1); i++)
      {
        ostr << s << '[' << std::setw(n) << i << "]: ";
        vaArray::make_var_array_io<N-1>(array[i]).print(ostr, array[i], -(indent+n+6));
        s = std::string(indent+2, ' ');
      }
      return ostr << std::string(indent, ' ') << ']' <<std::endl;
    }
  };

  // partial specialization of var_array_io<N, T>
  template <typename T>
  struct var_array_io<1, T> {

    std::ostream& print(std::ostream& ostr, T array, int indent)
    {
      if (indent > 0)
        ostr << std::string(indent, ' ');
      else
        indent = -indent;

      std::ostringstream os;
      os << (vaSize(array,1)-1);
      int n = os.str().length();
      std::string s("[ ");
      for (int i = 0; i < vaSize(array,1); i++)
      {
        ostr << s << '[' << std::setw(n) << i << "]: " << array[i] << std::endl;
        s = std::string(indent+2, ' ');
      }
      return ostr << std::string(indent, ' ') << ']' << std::endl;
    }
  };
}
