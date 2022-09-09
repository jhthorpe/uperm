/*  uperm.h
	JHT, September 6, 2022 

  Generates a vector or array of permutations and the number of permutations performed

  Consider permutations of the following numbers:
  0, 1, 2, 3

  There are 4! = 4*3*2*1 = 24 total permutations

  The set of unique permutations can be determined for each "level", the number of 
  swaps of two indices required to generate the new pattern 

  L0 : {0,1,2,3}

  L1 : P01 P02 P03 P12 P13 P23 

  L2 : P12(P01 P02 P03)
       P13(P01 P02 P03)
       P23(P01 P02 P03 P12 P13)

  L3 : P23(P12(P01 P02 P03) P13(P01 P02 P03))

  Where P12 means that the 1,2 index pair is swapped:
    P13(0123) = 0321

  Note that for each level, the leftmost index of the permutations 
  is restricted such that, reading left to right, the leftmost indices
  are monatonically decreasing. 

  Actual loops should occur from inner to outer, though, for better caching.  
  
  //Something of a proof is below
  To generate all unique permuations (ignoring replicas), define:
    Current set of permutations : {b_x}, where x is the total permutation number

  b_0 = ( 0 1 2 3 )

  then 
  for i=0,N-2
    b_i = b_(i-1)
    for j=i+1,N-1
      add Pij{b_(i-1)} to {b_i}

    
  so...

  b_0 = 0123
  b_1 = b_0 + P01(b_0) + P02(b_0) + P03(b_0) 
  b_2 = b_1 + P12(b_1) + P13(b_1) 
  b_3 = b_2 + P23(b_2)        

  At each stage:
  {b0} =  0123

  {b1} =  0123  			<- b0 
         -1023				<- P01(b0)
         -2103  			<- P02(b0)
         -3120				<- P03(b0)
  The {b1} contains all unique 0th inices

  {b2} =  0123  -1023  -2103  -3120  <- b1
         -0213   1203   2013   3210  <- P12(b1)
         -0321   1320   2301   3012  <- P13(b1)
  The {b2} contains all unique 0,1 index pairs

  {b3} =  0123  -1023  -2103  -3120  -0213   1203   2013   3210  -0321   1320   2301   3012  <- b2
         -0132   1032   2130   3102   0231  -1230  -2031  -3201   0312   1302   2310   3021  <- P23(b2)
  and {b3} contains all unique 0,1,2 (and thus 4) index pairs (and is hence the proof why this works)

  An algorithm that gathers all these terms at one "level" (number of swaps) is presented at the top

  
*/

#include <stdio.h>
#include <array>
#include <vector>
#include <cstddef>

namespace uperm {

//contains the indices to permute, starting from zero
struct index_permutation {
  size_t first;
  size_t second;
  
};

//alias for array of indices to permute
template<int L> //L is level or # of permutations
using index_permutation_list = std::array<index_permutation,L>;


//Number of pairs for a given total elements (N) and minimum 
// LHS index MIN
constexpr size_t num_unique_pairs_ge_min(const size_t N, 
                                  const size_t MIN) {
  return (N-MIN > 0 && MIN <= N-2) ? (N-MIN)*(N-MIN-1)/2 : 0;
}

//Number of pairs for a given total elements (N) 
constexpr size_t num_unique_pairs(const size_t N) {
  return ( N > 0 ) ? (N)*(N-1)/2 : 0;
}

//Number of pairs (N) with maximum LHS index (MAX)
constexpr size_t num_unique_pairs_lt_max(const size_t N,
                                          const size_t MAX) {
  return ( N > 0 && MAX >= 0) ? (2*N*MAX - MAX*MAX - MAX)/2 : 0; 
}

/*Number of unique permutations of N elements generated from 
    level L, with LHS index >= MIN */
constexpr size_t num_unique_permutations_ge_min(const size_t N,
                                                const size_t L,
                                                const size_t MIN) {
  if (L==0) {
    return 1;
  } else if ( L < 0 || MIN > N-2) {
    return 0;
  }

  size_t total = 0;
  for (size_t I=MIN+1; I <= N-2 ; I++) {
    total += (N-I - 1) * num_unique_permutations_ge_min(N,L-1,I);
  }
  return total; 
}


/* Number of unique permutations of N elements generated from 
 level L (number of allowed pair swaps) */ 
constexpr size_t num_unique_permutations(const size_t N, 
                                         const size_t L) {

  if (L == 0) {
    return 1;
  } else if (L > N - 1) {
    return 0; 
  }

  size_t total = 0;
  for (size_t I=0; I <= N-2; I++) {
    total += (N - I - 1) * num_unique_permutations_ge_min(N,L-1,I);
  }
  return total;
} 


/*For a given class, execute a permutation list of length L
 This requires the class to have a .begin() iterator

 TODO: optimize permutions to remove redundencies:
	ex: P(0,1) P (0,1) -> No permutation

	Could reuse intermediate permutations when evaluating a 
	list of permutations to improve permorfmance.
	ex: P(1,2) P(0,1) and P(1,3) P(0,1) can reuse the 
	    P(0,1) result (however this requires copying)*/ 
template<class T, int L>
T execute_permutations(const index_permutation_list<L>& PLIST,
                       const T& IN) { 
  T OUT = IN;
  for (auto const& perm : PLIST) {
    std::iter_swap(OUT.begin()+perm.first,OUT.begin()+perm.second);
  }
  return OUT;
}


/*
  inner permutation construction loop 
  N is number of elements
  L is level of permutations
  X is index we are working on 
  MIN is minimum index
  LIST is the index permutation list
*/
template<int N, int LMAX, typename Iterator>
void inner_permutation_loop(const int L, const int X, const int MIN, index_permutation_list<LMAX>& TMP, 
  Iterator& LIST_ELEMENT) {
  if (X > LMAX-1 || L < 0) {
    std::copy(TMP.begin(),TMP.end(),(*LIST_ELEMENT).begin());
    LIST_ELEMENT++; 
    return;
  }

  for (size_t I=MIN;I < std::min(N - L,N - 1) ; I++) {
    for (size_t J=I+1; J < N ; J++) {
      TMP[X] = {I,J}; 
      inner_permutation_loop<N,LMAX,Iterator>(L-1,X+1,I+1,TMP,LIST_ELEMENT);
    }
  } 
  
} 

/* 
  returns all permutations of N indices a particular level L as a vector
*/

/*
template<int N, int L>
using index_permutation_list_array = std::array<index_permutation_list<L>,num_unique_permutations(N,L)>;

template <int N, int L>
index_permutation_list_array<N,L> get_all_unique_permutations() {
  index_permutation_list_array<N,L> OUT;

  if (L == 0) {
    OUT[0][0] = {0,0};
    return OUT;
  }

  index_permutation_list<L> TMP;

  auto ELEMENT = OUT.begin();
  for (size_t I=0; I < std::min(N - L,N-1) ; I++) {
    for (size_t J=I+1; J < N ; J++) {
      TMP[0] = {I,J}; 
      inner_permutation_loop<N,L>(L-1,0+1,I+1,TMP,ELEMENT);
    }
  }

  return OUT;
}
*/

template<int N, int L>
using index_permutation_list_vector = std::vector<index_permutation_list<L>>;
template <int N, int L>
index_permutation_list_vector<N,L> get_all_unique_permutations() {
  index_permutation_list_vector<N,L> OUT(num_unique_permutations(N,L));

  if (L == 0) {
    OUT[0][0] = {0,0};
    return OUT;
  }

  index_permutation_list<L> TMP;

  auto ELEMENT = OUT.begin();
  for (size_t I=0; I < std::min(N - L,N-1) ; I++) {
    for (size_t J=I+1; J < N ; J++) {
      TMP[0] = {I,J}; 
      inner_permutation_loop<N,L>(L-1,0+1,I+1,TMP,ELEMENT);
    }
  }

  return OUT;
}


} //end of namespace
