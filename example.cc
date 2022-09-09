#include <stdio.h>
#include <utility> //since C++11 for std::swap
#include <algorithm>
#include <array>
#include <vector>


#include "uperm.h"

int main() {
  const int N = 6;

  //Initialize the data
  std::array<int,N> A;
  for (int i=0;i<A.size();i++) {
    A[i] = i;
  }

  //These are the number of unique permutations at each level
  // Note that these are constexpre'd, so that they can be used as array 
  // template inputs
  printf("N unique L0 %zu \n",uperm::num_unique_permutations(N,0));
  printf("N unique L1 %zu \n",uperm::num_unique_permutations(N,1));
  printf("N unique L2 %zu \n",uperm::num_unique_permutations(N,2));
  printf("N unique L3 %zu \n",uperm::num_unique_permutations(N,3));
  printf("N unique L4 %zu \n",uperm::num_unique_permutations(N,4));
  printf("N unique L5 %zu \n",uperm::num_unique_permutations(N,5));
  
 
  //A demonstration
  printf("\n\n");
  const int L = 5;
  auto const PERMS = uperm::get_all_unique_permutations<N,L>();
  const size_t NL = uperm::num_unique_permutations(N,L);

  printf("Original values:");
  printf("[");
  for (auto const& val : A) {
    printf("%d, ",val);
  } 
  printf("]\n");
   

  printf("Permutations at level %d :  %zu \n",L,NL);
  for (auto const& perm : PERMS) {
    for (int I=0; I < perm.size(); I++) {
      printf("P(%zu,%zu) ",perm[I].first,perm[I].second);
    }
    printf("= [");
    auto NEW = uperm::execute_permutations<std::array<int,N>,L>(perm,A); 
    for (int I=0; I < NEW.size(); I++) {
      printf("%d, ",NEW[I]);
    }
    printf("]\n");
  }


  return 0;
}

