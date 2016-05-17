// Copyright (c) 2016,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./2d_logistic_map.h"
#include "../../libraries/CImg.h"
#include "../../libraries/cryptopp562/gf256.h"


// HELPER
int compare(const void *arg1, const void *arg2)
{
  int const *lhs = static_cast<int const*>(arg1);
  int const *rhs = static_cast<int const*>(arg2);
  return (lhs[0] < rhs[0]) ? -1
      :  ((rhs[0] < lhs[0]) ? 1
      :  (lhs[1] < rhs[1] ? -1
      :  ((rhs[1] < lhs[1] ? 1 : 0))));
}

struct key_values_t {
  double x0;
  double y0;
  double r;
  double T;
  int A[8];
};

// Equation (5)
// x_0: start =   0, end =  52
// y_0: start =  52, end = 104
// r  : start = 104, end = 156
// T  : start = 156, end = 208
double
trans_frac(int *key, int start, int end)
{
  double value = 0.0;
  for(int i = start; i < end; i++)
    value += key[i] * pow(2, (-1)*((i-start)+1));
  return value;
}

int
bits_to_int(int *key, int start, int end)
{
  int value = 0;
  for (int i = start; i < end; i++)
    value |= key[i] << (i - start);
  return value;
}

void
get_key_values(struct key_values_t *kv, struct img_info_t *img_info)
{
  kv->x0 = trans_frac(img_info->key, 0, 52);
  kv->y0 = trans_frac(img_info->key, 52, 104);
  kv->r = trans_frac(img_info->key, 104, 156) * 0.08 + 1.1;
  kv->T = trans_frac(img_info->key, 156, 208);
  kv->A[0] = bits_to_int(img_info->key, 208, 214);
  kv->A[1] = bits_to_int(img_info->key, 214, 220);
  kv->A[2] = bits_to_int(img_info->key, 220, 226);
  kv->A[3] = bits_to_int(img_info->key, 226, 232);
  kv->A[4] = bits_to_int(img_info->key, 232, 238);
  kv->A[5] = bits_to_int(img_info->key, 238, 244);
  kv->A[6] = bits_to_int(img_info->key, 244, 250);
  kv->A[7] = bits_to_int(img_info->key, 250, 256);
}

double
logistic_2d_x(double x, double y, double r)
{
  return r * (3*y+1) * x * (1-x);
}

double
logistic_2d_y(double x, double y, double r)
{
  return r * (3 * (r * (3*y+1) * x * (1-x)) + 1) * y * (1-y);
}

void
get_permutation_vector(std::vector<int> & permutation, std::vector<double> & data)
{
  for (int i = 0 ; i != permutation.size() ; i++)
    permutation[i] = i;

  sort(permutation.begin(), permutation.end(),
    [&](const int& a, const int& b) {
      return (data[a] < data[b]);
    }
  );
}

void
multiply_matrices(int result[kS][kS], const int a[kS][kS], const int b[kS][kS])
{
  for(int i = 0; i < kS; i++)
    for(int j = 0; j < kS; j++)
      for (int k = 0; k < kS; k++)
        result[i][j] = (result[i][j] + a[i][k] * b[k][j]) % 256;
}

void
multiply_matrices_galois(int result[kS][kS], const int a[kS][kS], const int b[kS][kS])
{
  CryptoPP::GF256 gf(29);
  for(int i = 0; i < kS; i++)
    for(int j = 0; j < kS; j++)
      for (int k = 0; k < kS; k++)
        result[i][j] = gf.Add(result[i][j], gf.Multiply(a[i][k], b[k][j]));
}

cimg_library::CImg<unsigned int>
logistic_permutation_enc(cimg_library::CImg<unsigned int> P, double *x_seq, double *y_seq)
{
  int M = P.width();
  int N = P.height();
  // M .. width, N .. height, 1 .. depth, 1 .. spectrum, 0 .. default value
  cimg_library::CImg<unsigned int> C0(M, N, 1, 1, 0);
  cimg_library::CImg<unsigned int> C(M, N, 1, 1, 0);

  // Equation (10) - row permutation
  std::vector<double> x_col_0(N, 0.0);
  std::vector<int> U_x(N,0);
  for(int i = 0; i < M*N; i += M)
    x_col_0[i/M] = x_seq[i];
  get_permutation_vector(U_x, x_col_0);

  for(int y = 0; y < U_x.size(); y++)
    for(int x = 0; x < M; x++)
      C0(x, y) = P(U_x[x], y);

  // Equation (11) - column permutation
  std::vector<double> y_row_0(M, 0.0);
  std::vector<int> U_y(M,0);
  for(int i = 0; i < M; i++)
    y_row_0[i] = y_seq[i];
  get_permutation_vector(U_y, y_row_0);

  for(int x = 0; x < U_y.size(); x++)
    for(int y = 0; y < N; y++)
      C(x, y) = C0(x, U_y[y]);

  return C;
}

cimg_library::CImg<unsigned int>
logistic_permutation_dec(cimg_library::CImg<unsigned int> C, double *x_seq, double *y_seq)
{
  int M = C.width();
  int N = C.height();
  // M .. width, N .. height, 1 .. depth, 1 .. spectrum, 0 .. default value
  cimg_library::CImg<unsigned int> C0(M, N, 1, 1, 0);
  cimg_library::CImg<unsigned int> P(M, N, 1, 1, 0);

  // Equation (11) - column permutation
  std::vector<double> y_row_0(M, 0.0);
  std::vector<int> U_y(M,0);
  for(int i = 0; i < M; i++)
    y_row_0[i] = y_seq[i];
  get_permutation_vector(U_y, y_row_0);

  for(int x = 0; x < U_y.size(); x++)
    for(int y = 0; y < N; y++)
      C0(x, U_y[y]) = C(x, y);

  // Equation (10) - row permutation
  std::vector<double> x_col_0(N, 0.0);
  std::vector<int> U_x(N,0);
  for(int i = 0; i < M*N; i += M)
    x_col_0[i/M] = x_seq[i];
  get_permutation_vector(U_x, x_col_0);

  for(int y = 0; y < U_x.size(); y++)
    for(int x = 0; x < M; x++)
      P(U_x[x], y) = C0(x, y);

  return P;
}

int
trun(double b)
{
  int low = 9;
  int high = 16;
  return floor(b*pow(10,high)) - floor(b*pow(10,(low-1))) * pow(10,(high-low+1));
}

cimg_library::CImg<unsigned int>
logistic_transposition_enc(cimg_library::CImg<unsigned int> P, double *x_seq, double *y_seq)
{
  int M = P.width();
  int N = P.height();

  cimg_library::CImg<unsigned int> C(M, N, 1, 1, 0);

  // Equation (16): zd equals the z matrix but with T(d) applied.
  int zd[M][N];
  for(int i = 0; i < N*M; i++)
    zd[i/M][i%M] = trun(x_seq[i] + y_seq[i]);

  int I[M][N];
  for(int y = 0; y < N; y += 4)
    for(int x = 0; x < M; x += 4)
      for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
          switch(kGX[i][j])
          {
            case kGN:
              I[x+j][y+i] = zd[x+j][y+i] % F;
              break;
            case kGR:
              I[x+j][y+i] = (int) fmod(floor(sqrt(zd[x+j][y+i])), F);
              break;
            case kGS:
              I[x+j][y+i] = (int) fmod(pow(zd[x+j][y+i],2), F);
              break;
            case kGD:
              I[x+j][y+i] = (2*zd[x+j][y+i]) % F;
              break;
            default:
              exit(1);
          }

  cimg_forXY(P, x, y) {
    C(x,y) = (P(x,y) + I[x][y]) % F;
  }

  return C;
}

cimg_library::CImg<unsigned int>
logistic_transposition_dec(cimg_library::CImg<unsigned int> C, double *x_seq, double *y_seq)
{

  int M = C.width();
  int N = C.height();

  cimg_library::CImg<unsigned int> P(M, N, 1, 1, 0);

  // Equation (16): zd equals the z matrix but with T(d) applied.
  int zd[M][N];
  for(int i = 0; i < N*M; i++)
    zd[i/M][i%M] = trun(x_seq[i] + y_seq[i]);

  int I[M][N];
  for(int y = 0; y < N; y += 4)
    for(int x = 0; x < M; x += 4)
      for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
          switch(kGX[i][j])
          {
            case kGN:
              I[x+j][y+i] = zd[x+j][y+i] % F;
              break;
            case kGR:
              I[x+j][y+i] = (int) fmod(floor(sqrt(zd[x+j][y+i])), F);
              break;
            case kGS:
              I[x+j][y+i] = (int) fmod(pow(zd[x+j][y+i],2), F);
              break;
            case kGD:
              I[x+j][y+i] = (2*zd[x+j][y+i]) % F;
              break;
            default:
              exit(1);
          }

  cimg_forXY(P, x, y) {
    P(x,y) = (C(x,y) - I[x][y]) % F;
  }

  return P;
}

cimg_library::CImg<unsigned int>
logistic_diffusion_enc(cimg_library::CImg<unsigned int> P)
{
  int M = P.width();
  int N = P.height();
  int Cb[kS][kS];
  int Pb[kS][kS];
  int tmp[kS][kS];

  cimg_library::CImg<unsigned int> C(M, N, 1, 1, 0);

  for(int y = 0; y < N; y += kS)
    for(int x = 0; x < M; x += kS)
    {
      std::fill(tmp[0], tmp[0] + kS*kS, 0);
      std::fill(Cb[0], Cb[0] + kS*kS, 0);
      std::fill(Pb[0], Pb[0] + kS*kS, 0);

      for(int i = 0; i < kS; i++)
        for (int j = 0; j < kS; j++)
          Pb[i][j] = P(x+j, y+i);

      multiply_matrices_galois(tmp, kLENC, Pb);
      multiply_matrices_galois(Cb, tmp, kLENC);

      for(int i = 0; i < kS; i++)
        for (int j = 0; j < kS; j++)
          C(x+j, y+i) = Cb[i][j];
    }

  return C;
}

cimg_library::CImg<unsigned int>
logistic_diffusion_dec(cimg_library::CImg<unsigned int> C)
{
  int M = C.width();
  int N = C.height();
  int Cb[kS][kS];
  int Pb[kS][kS];
  int tmp[kS][kS];

  cimg_library::CImg<unsigned int> P(M, N, 1, 1, 0);

  for(int y = 0; y < N; y += kS)
    for(int x = 0; x < M; x += kS)
    {
      std::fill(tmp[0], tmp[0] + kS*kS, 0);
      std::fill(Cb[0], Cb[0] + kS*kS, 0);
      std::fill(Pb[0], Pb[0] + kS*kS, 0);

      for(int i = 0; i < kS; i++)
        for (int j = 0; j < kS; j++)
          Cb[i][j] = C(x+j, y+i);

      multiply_matrices_galois(tmp, kLDEC, Cb);
      multiply_matrices_galois(Pb, tmp, kLDEC);

      for(int i = 0; i < kS; i++)
        for (int j = 0; j < kS; j++)
          P(x+j, y+i) = Pb[i][j];
    }

  return P;
}

void
run_enc_2d_logistic_map(struct img_info_t *img_info)
{
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted(origin);

  struct key_values_t kv = {};
  get_key_values(&kv, img_info);

  int M = encrypted.width();
  int N = encrypted.height();
  int nr_rounds_min = ceil(log2((double)M*N) / (2*log2((double)kS)));

  for(int i = 0; i < nr_rounds_min; i++)
  {
    // Equation (6)
    double i_x0 = fmod(kv.T + kv.x0 * kv.A[i%8], 1.0);
    double i_y0 = fmod(kv.T + kv.y0 * kv.A[i%8], 1.0);

    double x_seq[M*N];
    double y_seq[M*N];

    x_seq[0] = logistic_2d_x(i_x0, i_y0, kv.r);
    y_seq[0] = logistic_2d_y(i_x0, i_y0, kv.r);
    for(int j = 1; j < M*N; j++)
    {
      x_seq[j] = logistic_2d_x(x_seq[j-1], y_seq[j-1], kv.r);
      y_seq[j] = logistic_2d_y(x_seq[j-1], y_seq[j-1], kv.r);
    }
    encrypted = logistic_permutation_enc(encrypted, x_seq, y_seq);
    encrypted = logistic_diffusion_enc(encrypted);
    encrypted = logistic_transposition_enc(encrypted, x_seq, y_seq);
  }
  encrypted.save(img_info->encrypted.c_str());
}

void
run_dec_2d_logistic_map(struct img_info_t *img_info)
{
  cimg_library::CImg<unsigned int> encrypted(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted(encrypted);

  struct key_values_t kv = {};
  get_key_values(&kv, img_info);

  int M = decrypted.width();
  int N = decrypted.height();
  int nr_rounds_min = ceil(log2((double)M*N) / (2*log2((double)kS)));

  for(int i = nr_rounds_min-1; i >= 0 ; i--)
  {
    // Equation (6)
    double i_x0 = fmod(kv.T + kv.x0 * kv.A[i%8], 1.0);
    double i_y0 = fmod(kv.T + kv.y0 * kv.A[i%8], 1.0);

    double x_seq[M*N];
    double y_seq[M*N];

    x_seq[0] = logistic_2d_x(i_x0, i_y0, kv.r);
    y_seq[0] = logistic_2d_y(i_x0, i_y0, kv.r);
    for(int j = 1; j < M*N; j++)
    {
      x_seq[j] = logistic_2d_x(x_seq[j-1], y_seq[j-1], kv.r);
      y_seq[j] = logistic_2d_y(x_seq[j-1], y_seq[j-1], kv.r);
    }
    decrypted = logistic_transposition_dec(decrypted, x_seq, y_seq);
    decrypted = logistic_diffusion_dec(decrypted);
    decrypted = logistic_permutation_dec(decrypted, x_seq, y_seq);
  }
  decrypted.save(img_info->decrypted.c_str());
}

void
set_key_2d_logistic_map(struct img_info_t *img_info)
{
  // required a key-array of size 256
  img_info->key_length = kMaxKeyLength;
  unsigned int seed = time(NULL);
  for(int i = 0; i < kMaxKeyLength; i++)
    img_info->key[i] = (rand_r(&seed) % 2) ? 0 : 1;
}

void
set_key_modified_2d_logistic_map(struct img_info_t *img_info)
{
  unsigned int seed = time(NULL);
  int i = rand_r(&seed) % kMaxKeyLength;
  img_info->key[i] = (img_info->key[i] == 0) ? 1 : 0;
}
