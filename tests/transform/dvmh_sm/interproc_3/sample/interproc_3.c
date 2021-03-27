#include <stdio.h>

enum { NX = 100, NY = 100 };

double A[NX][NY][5];
double Q[NY][5];
double P[NY][5];

void baz(double B[][5], int N) {
  for (int J = 0; J < 5; ++J) {
    B[0][J] = 0;
    B[N][J] = 0;
  }
  B[0][1] = 1;
  B[N][1] = 1;
}

void bar(double C[5], double B[5], double A[5]) {
  A[0] = A[0] + B[0] * C[0];
  A[1] = A[1] + B[1] * C[1];
  A[2] = A[2] + B[2] * C[2];
  A[3] = A[3] + B[3] * C[3];
  A[4] = A[4] + B[4] * C[4];
}

void foo() {
#pragma dvm actual(A)
#pragma dvm region in(A)out(A) local(P, Q)
  {
#pragma dvm parallel([I]) tie(A[I][][]) private(P, Q)
    for (int I = 0; I < NX; ++I) {
      for (int J = 0; J < NY; ++J) {
        Q[J][0] = J;
        Q[J][1] = J + 1;
        Q[J][2] = J + 2;
        Q[J][3] = J + 3;
        Q[J][4] = J + 4;
      }
      baz(P, NY - 1);
      for (int J = 1; J < NY - 1; ++J)
        for (int M = 0; M < 5; ++M)
          P[J][M] = P[0][M] + Q[J - 1][M] * (M + 1) + Q[J + 1][M] * (M + 3) +
                    P[NY - 1][M];
      for (int J = 1; J < NY; ++J)
        bar(P[J], A[I][J - 1], A[I][J]);
      for (int J = 0; J < NY - 1; ++J)
        for (int M = 0; M < 5; ++M)
          A[I][J][M] = A[I][J][M] + A[I][J + 1][M];
    }
  }
#pragma dvm get_actual(A)
}

int main() {
#pragma dvm region in(A)out(A)
  {
#pragma dvm parallel([I][J][M]) tie(A[I][J][M])
    for (int I = 0; I < NX; ++I)
      for (int J = 0; J < NY; ++J)
        for (int M = 0; M < 5; ++M)
          A[I][J][M] = 1;
  }
#pragma dvm get_actual(A)
  foo();
#pragma dvm actual(A)
  double S = 0;
#pragma dvm actual(S)
#pragma dvm region in(A, S)out(S)
  {
#pragma dvm parallel([I][J][M]) tie(A[I][J][M]) reduction(sum(S))
    for (int I = 0; I < NX; ++I)
      for (int J = 0; J < NY; ++J)
        for (int M = 0; M < 5; ++M)
          S = S + A[I][J][M];
  }
#pragma dvm get_actual(S)
#pragma dvm get_actual(A)
  printf("S = %e\n", S);
#pragma dvm actual(A)
  return 0;
}
