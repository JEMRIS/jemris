// ---------------------------------------------------------------------
// Example program that uses the RNG class
// To compile this example:
// 1.  Download this file along with rng.C and rng.h
// 2.  Compile rng.C and randtest.C.  For example, with g++, type:
//     g++ randtest.C rng.C -o randtest
// 3.  Run the program "randtest"

#include <iostream>
#include "rng.h"

using namespace std;

int main()
{

 RNG x;  // Not seeded explicitly, so it will be given a random seed.
 double testsum = 0;
 for (int i = 0; i < 2; ++i) {
   cout << x.exponential(5) << endl;
   cout << x.gamma(.5,2) << endl;
   cout << x.beta(3,6) << endl;
   cout << x.binomial(.2,1000) << endl;
   cout << x.chi_square(20) << endl;
   testsum += x.normal();
   testsum += x.uniform() - .5;
   testsum += x.exponential(5) - .2;
   testsum += x.gamma(1,1);
 }

 cout << "Filling a vector with 10 N(2, 5) variates\n";
 vector<double> v(10);
 x.normal(v, 2, 5);
 for (unsigned int i = 0; i < v.size(); ++i)
   cout << v[i] << endl;

 cout << "Filling a vector with a sample of size 200000 from "
      << "Multinom(c(.2,.5,.0001,.2))\n";
 // Note that probabilities are normalized automatically
 vector<double> probs(4);
 probs[0] = 2;
 probs[1] = 5;
 probs[2] = .001;
 probs[3] = 2;
 vector<uint> res;
 x.multinom(200000, probs, res);
 for (unsigned int i = 0; i < res.size(); ++i)
   cout << res[i] << endl;

  return 0;
}

