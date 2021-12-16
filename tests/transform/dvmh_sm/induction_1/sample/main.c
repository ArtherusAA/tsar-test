enum { N = 100 };

double A[N];

int main() {
  int I, J;
  double S = 0;
#pragma dvm region in(A, I)out(A, I)
  {
#pragma dvm parallel([I]) tie(A[I])
    for (I = 0; I < N; ++I)
      A[I] = I;
  }
  J = 1;
#pragma dvm get_actual(I)
  for (I = 0; I < N - 1; ++I) {
#pragma dvm actual(I)
#pragma dvm get_actual(A, I)
    S = S + A[I] + A[J];
    J = J + 1;
  }
#pragma dvm actual(I)
  return S;
}
