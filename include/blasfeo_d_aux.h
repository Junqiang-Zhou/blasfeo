/**************************************************************************************************
*                                                                                                 *
* This file is part of BLASFEO.                                                                   *
*                                                                                                 *
* BLASFEO -- BLAS For Embedded Optimization.                                                      *
* Copyright (C) 2016-2017 by Gianluca Frison.                                                     *
* Developed at IMTEK (University of Freiburg) under the supervision of Moritz Diehl.              *
* All rights reserved.                                                                            *
*                                                                                                 *
* HPMPC is free software; you can redistribute it and/or                                          *
* modify it under the terms of the GNU Lesser General Public                                      *
* License as published by the Free Software Foundation; either                                    *
* version 2.1 of the License, or (at your option) any later version.                              *
*                                                                                                 *
* HPMPC is distributed in the hope that it will be useful,                                        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                                            *
* See the GNU Lesser General Public License for more details.                                     *
*                                                                                                 *
* You should have received a copy of the GNU Lesser General Public                                *
* License along with HPMPC; if not, write to the Free Software                                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA                  *
*                                                                                                 *
* Author: Gianluca Frison, giaf (at) dtu.dk                                                       *
*                          gianluca.frison (at) imtek.uni-freiburg.de                             *
*                                                                                                 *
**************************************************************************************************/

#include <stdio.h>



#ifdef __cplusplus
extern "C" {
#endif



/************************************************
* old
************************************************/

void dgecp_lib(int m, int n, double alpha, int offsetA, double *A, int sda, int offsetB, double *B, int sdb);
void dtrcp_l_lib(int m, double alpha, int offsetA, double *A, int sda, int offsetB, double *B, int sdb);
void dgead_lib(int m, int n, double alpha, int offsetA, double *A, int sda, int offsetB, double *B, int sdb);
// TODO remove ???
void ddiain_sqrt_lib(int kmax, double *x, int offset, double *pD, int sdd);
// TODO ddiaad1
void ddiareg_lib(int kmax, double reg, int offset, double *pD, int sdd);

/************************************************
* d_aux_lib.c
************************************************/

// 1) d(double) / s(single)
// 2) ge(general) / tr(triangular) / vec(vector) / row(row) / col(column) / dia(diagonal)
// 3) se(set) / cp(copy) / sc(scale) / ad(add) / tr(transpose) / in(insert) / ex(extract) / pe(premute) / sw(swap)
// 4) _l(lower) / _u(upper)
// 5) _sp(sparse)

// returns the memory size (in bytes) needed for a strmat
int d_memsize_strmat(int m, int n);
// returns the memory size (in bytes) needed for a strvec
int d_memsize_strvec(int m);
// create a strmat for a matrix of size m*n by using memory passed by a pointer (pointer is not updated)
struct d_strmat * d_create_strmat(int m, int n, void *memory);
// create a strvec for a vector of size m by using memory passed by a pointer (pointer is not updated)
struct d_strvec * d_create_strvec(int m, void *memory);
void d_cvt_mat2strmat(int m, int n, double *A, int lda, struct d_strmat *sA, int ai, int aj);
void d_cvt_vec2strvec(int m, double *a, struct d_strvec *sa, int ai);
void d_cvt_tran_mat2strmat(int m, int n, double *A, int lda, struct d_strmat *sA, int ai, int aj);
void d_cvt_strmat2mat(int m, int n, struct d_strmat *sA, int ai, int aj, double *A, int lda);
void d_cvt_strvec2vec(int m, struct d_strvec *sa, int ai, double *a);
void d_cvt_tran_strmat2mat(int m, int n, struct d_strmat *sA, int ai, int aj, double *A, int lda);
void dgein1_libstr(double a, struct d_strmat *sA, int ai, int aj);
double dgeex1_libstr(struct d_strmat *sA, int ai, int aj);
void dvecin1_libstr(double a, struct d_strvec *sx, int xi);
double dvecex1_libstr(struct d_strvec *sx, int xi);
// A <= alpha
void dgese_libstr(int m, int n, double alpha, struct d_strmat *sA, int ai, int aj);
// a <= alpha
void dvecse_libstr(int m, double alpha, struct d_strvec *sx, int xi);
// B <= A
void dgecp_libstr(int m, int n, struct d_strmat *sA, int ai, int aj, struct d_strmat *sC, int ci, int cj);
// A <= alpha*A
void dgesc_libstr(int m, int n, double alpha, struct d_strmat *sA, int ai, int aj);
// A <= alpha*B
void dgecpsc_libstr(int m, int n, double alpha, struct d_strmat *sA, int ai, int aj, struct d_strmat *sB, int bi, int bj); // TODO
// y <= x
void dveccp_libstr(int m, struct d_strvec *sa, int ai, struct d_strvec *sc, int ci);
// x <= alpha*x
void dvecsc_libstr(int m, double alpha, struct d_strvec *sa, int ai);
// B <= A, A lower triangular
void dtrcp_l_libstr(int m, struct d_strmat *sA, int ai, int aj, struct d_strmat *sC, int ci, int cj);
// B <= B + alpha*A
void dgead_libstr(int m, int n, double alpha, struct d_strmat *sA, int ai, int aj, struct d_strmat *sC, int ci, int cj);
// y <= y + alpha*x
void dvecad_libstr(int m, double alpha, struct d_strvec *sa, int ai, struct d_strvec *sc, int ci);
void dgetr_lib(int m, int n, double alpha, int offsetA, double *pA, int sda, int offsetC, double *pC, int sdc);
void dgetr_libstr(int m, int n, struct d_strmat *sA, int ai, int aj, struct d_strmat *sC, int ci, int cj);
void dtrtr_l_lib(int m, double alpha, int offsetA, double *pA, int sda, int offsetC, double *pC, int sdc);
void dtrtr_l_libstr(int m, struct d_strmat *sA, int ai, int aj, struct d_strmat *sC, int ci, int cj);
void dtrtr_u_lib(int m, double alpha, int offsetA, double *pA, int sda, int offsetC, double *pC, int sdc);
void dtrtr_u_libstr(int m, struct d_strmat *sA, int ai, int aj, struct d_strmat *sC, int ci, int cj);
void ddiare_libstr(int kmax, double alpha, struct d_strmat *sA, int ai, int aj);
void ddiain_libstr(int kmax, double alpha, struct d_strvec *sx, int xi, struct d_strmat *sA, int ai, int aj);
void ddiaex_lib(int kmax, double alpha, int offset, double *pD, int sdd, double *x);
void ddiaad_lib(int kmax, double alpha, double *x, int offset, double *pD, int sdd);
void ddiain_libsp(int kmax, int *idx, double alpha, double *x, double *pD, int sdd);
void ddiain_sp_libstr(int kmax, double alpha, struct d_strvec *sx, int xi, int *idx, struct d_strmat *sD, int di, int dj);
void ddiaex_libsp(int kmax, int *idx, double alpha, double *pD, int sdd, double *x);
void ddiaex_libstr(int kmax, double alpha, struct d_strmat *sA, int ai, int aj, struct d_strvec *sx, int xi);
void ddiaex_sp_libstr(int kmax, double alpha, int *idx, struct d_strmat *sD, int di, int dj, struct d_strvec *sx, int xi);
void ddiaad_libstr(int kmax, double alpha, struct d_strvec *sx, int xi, struct d_strmat *sA, int ai, int aj);
void ddiaad_libsp(int kmax, int *idx, double alpha, double *x, double *pD, int sdd);
void ddiaad_sp_libstr(int kmax, double alpha, struct d_strvec *sx, int xi, int *idx, struct d_strmat *sD, int di, int dj);
void ddiaadin_libsp(int kmax, int *idx, double alpha, double *x, double *y, double *pD, int sdd);
void ddiaadin_sp_libstr(int kmax, double alpha, struct d_strvec *sx, int xi, struct d_strvec *sy, int yi, int *idx, struct d_strmat *sD, int di, int dj);
void drowin_lib(int kmax, double alpha, double *x, double *pD);
void drowin_libstr(int kmax, double alpha, struct d_strvec *sx, int xi, struct d_strmat *sA, int ai, int aj);
void drowex_lib(int kmax, double alpha, double *pD, double *x);
void drowex_libstr(int kmax, double alpha, struct d_strmat *sA, int ai, int aj, struct d_strvec *sx, int xi);
void drowad_lib(int kmax, double alpha, double *x, double *pD);
void drowad_libstr(int kmax, double alpha, struct d_strvec *sx, int xi, struct d_strmat *sA, int ai, int aj);
void drowin_libsp(int kmax, double alpha, int *idx, double *x, double *pD);
void drowad_libsp(int kmax, int *idx, double alpha, double *x, double *pD);
void drowad_sp_libstr(int kmax, double alpha, struct d_strvec *sx, int xi, int *idx, struct d_strmat *sD, int di, int dj);
void drowadin_libsp(int kmax, int *idx, double alpha, double *x, double *y, double *pD);
void drowsw_lib(int kmax, double *pA, double *pC);
void drowsw_libstr(int kmax, struct d_strmat *sA, int ai, int aj, struct d_strmat *sC, int ci, int cj);
void drowpe_libstr(int kmax, int *ipiv, struct d_strmat *sA);
void dcolex_libstr(int kmax, struct d_strmat *sA, int ai, int aj, struct d_strvec *sx, int xi);
void dcolin_lib(int kmax, double *x, int offset, double *pD, int sdd);
void dcolin_libstr(int kmax, struct d_strvec *sx, int xi, struct d_strmat *sA, int ai, int aj);
void dcolad_lib(int kmax, double alpha, double *x, int offset, double *pD, int sdd);
void dcolin_libsp(int kmax, int *idx, double *x, double *pD, int sdd);
void dcolad_libsp(int kmax, double alpha, int *idx, double *x, double *pD, int sdd);
void dcolsw_lib(int kmax, int offsetA, double *pA, int sda, int offsetC, double *pC, int sdc);
void dcolsw_libstr(int kmax, struct d_strmat *sA, int ai, int aj, struct d_strmat *sC, int ci, int cj);
void dcolpe_libstr(int kmax, int *ipiv, struct d_strmat *sA);
void dvecin_libsp(int kmax, int *idx, double *x, double *y);
void dvecad_libsp(int kmax, int *idx, double alpha, double *x, double *y);
void dvecad_sp_libstr(int m, double alpha, struct d_strvec *sx, int xi, int *idx, struct d_strvec *sz, int zi);
void dvecin_sp_libstr(int m, double alpha, struct d_strvec *sx, int xi, int *idx, struct d_strvec *sz, int zi);
void dvecex_sp_libstr(int m, double alpha, int *idx, struct d_strvec *sx, int x, struct d_strvec *sz, int zi);
void dveccl_libstr(int m, struct d_strvec *sxm, int xim, struct d_strvec *sx, int xi, struct d_strvec *sxp, int xip, struct d_strvec *sz, int zi);
void dveccl_mask_libstr(int m, struct d_strvec *sxm, int xim, struct d_strvec *sx, int xi, struct d_strvec *sxp, int xip, struct d_strvec *sz, int zi, struct d_strvec *sm, int mi);
void dvecze_libstr(int m, struct d_strvec *sm, int mi, struct d_strvec *sv, int vi, struct d_strvec *se, int ei);
void dvecnrm_inf_libstr(int m, struct d_strvec *sx, int xi, double *ptr_norm);



#ifdef __cplusplus
}
#endif
