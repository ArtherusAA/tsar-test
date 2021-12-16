#include <stdio.h>

#define N 10

long A[N][N][2];

int main() {
  long S = 0;
#pragma dvm actual(S)
#pragma dvm region in(A, S)out(A, S)
  {
#pragma dvm parallel([I][J][K]) tie(A[I][J][K])
    for (int I = 0; I < N; ++I)
      for (int J = 0; J < N; ++J)
        for (int K = 0; K < 2; ++K)
          A[I][J][K] = I + J + K;
#pragma dvm parallel([I][J]) tie(A[I][J][]) across(A [1:0] [1:0] [0:0])
    for (int I = 1; I < N; ++I)
      for (int J = 1; J < N; ++J)
        for (int K = 0; K < 2; ++K)
          A[I][J][K] =
              A[I - 1][J][K] + A[I][J - 1][K] + A[I][J][0] + A[I][J][1];
#pragma dvm parallel([I][J][K]) tie(A[I][J][K]) reduction(sum(S))
    for (int I = 0; I < N; ++I)
      for (int J = 0; J < N; ++J)
        for (int K = 0; K < 2; ++K)
          S += A[I][J][K];
  }
#pragma dvm get_actual(A, S)
  printf("Sum = %ld\n", S);
#pragma dvm actual(A)
  return 0;
}
