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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/blasfeo_common.h"



#if defined(LA_REFERENCE) | defined(LA_BLAS)



// return memory size (in bytes) needed for a strmat
int s_memsize_strmat(int m, int n)
	{
	int diag_size = m<n ? m : n;

	int str_size = 0;
	str_size += sizeof(struct s_strmat);

	int mem_size = 0;
	mem_size += m*n*sizeof(float);
	mem_size += diag_size*sizeof(float);

	int size = 0;
	size += (str_size+BLASFEO_ALIGN-1)/BLASFEO_ALIGN*BLASFEO_ALIGN;
	size += (mem_size+BLASFEO_ALIGN-1)/BLASFEO_ALIGN*BLASFEO_ALIGN;

	return size;
	}



// create a matrix structure for a matrix of size m*n by using memory passed by a pointer
// XXX assumes memory to be aligned to BLASFEO_ALIGN-bytes
struct s_strmat * s_create_strmat(int m, int n, void *memory)
	{
	int diag_size = m<n ? m : n; // al(min(m,n)) // XXX max ???

	char *c_ptr = memory;

	struct s_strmat *sA = (struct s_strmat *) c_ptr;
	c_ptr += (sizeof(struct s_strmat)+BLASFEO_ALIGN-1)/BLASFEO_ALIGN*BLASFEO_ALIGN;

	//
	sA->pA = (float *) c_ptr;
	c_ptr += m*n*sizeof(float);
	//
	sA->dA = (float *) c_ptr;
	c_ptr += diag_size*sizeof(float);
	//
	sA->m = m;
	//
	sA->n = n;
	//
	sA->use_dA = 0;
	//
	sA->memsize = s_memsize_strmat(m, n);

	return sA;
	}



// return memory size (in bytes) needed for a strvec
int s_memsize_strvec(int m)
	{
	int str_size = 0;
	str_size += sizeof(struct s_strvec);

	int mem_size = 0;
	mem_size = m*sizeof(float);

	int size = 0;
	size += (str_size+BLASFEO_ALIGN-1)/BLASFEO_ALIGN*BLASFEO_ALIGN;
	size += (mem_size+BLASFEO_ALIGN-1)/BLASFEO_ALIGN*BLASFEO_ALIGN;

	return size;
	}



// create a matrix structure for a matrix of size m*n by using memory passed by a pointer
// XXX assumes memory to be aligned to BLASFEO_ALIGN-bytes
struct s_strvec * s_create_strvec(int m, void *memory)
	{
	char *c_ptr = memory;

	struct s_strvec *sa = (struct s_strvec *) c_ptr;
	c_ptr += (sizeof(struct s_strvec)+BLASFEO_ALIGN-1)/BLASFEO_ALIGN*BLASFEO_ALIGN;

	//
	sa->pa = (float *) c_ptr;
	c_ptr += m*sizeof(float);
	//
	sa->m = m;
	//
	sa->memsize = s_memsize_strvec(m);

	return sa;
	}



// convert a matrix into a matrix structure
void s_cvt_mat2strmat(int m, int n, float *A, int lda, struct s_strmat *sA, int ai, int aj)
	{
	int ii, jj;
	int lda2 = sA->m;
	float *pA = sA->pA + ai + aj*lda2;
	for(jj=0; jj<n; jj++)
		{
		ii = 0;
		for(; ii<m-3; ii+=4)
			{
			pA[ii+0+jj*lda2] = A[ii+0+jj*lda];
			pA[ii+1+jj*lda2] = A[ii+1+jj*lda];
			pA[ii+2+jj*lda2] = A[ii+2+jj*lda];
			pA[ii+3+jj*lda2] = A[ii+3+jj*lda];
			}
		for(; ii<m; ii++)
			{
			pA[ii+0+jj*lda2] = A[ii+0+jj*lda];
			}
		}
	return;
	}



// convert and transpose a matrix into a matrix structure
void s_cvt_tran_mat2strmat(int m, int n, float *A, int lda, struct s_strmat *sA, int ai, int aj)
	{
	int ii, jj;
	int lda2 = sA->m;
	float *pA = sA->pA + ai + aj*lda2;
	for(jj=0; jj<n; jj++)
		{
		ii = 0;
		for(; ii<m-3; ii+=4)
			{
			pA[jj+(ii+0)*lda2] = A[ii+0+jj*lda];
			pA[jj+(ii+1)*lda2] = A[ii+1+jj*lda];
			pA[jj+(ii+2)*lda2] = A[ii+2+jj*lda];
			pA[jj+(ii+3)*lda2] = A[ii+3+jj*lda];
			}
		for(; ii<m; ii++)
			{
			pA[jj+(ii+0)*lda2] = A[ii+0+jj*lda];
			}
		}
	return;
	}



// convert a vector into a vector structure
void s_cvt_vec2strvec(int m, float *a, struct s_strvec *sa, int ai)
	{
	float *pa = sa->pa + ai;
	int ii;
	for(ii=0; ii<m; ii++)
		pa[ii] = a[ii];
	return;
	}



// convert a matrix structure into a matrix
void s_cvt_strmat2mat(int m, int n, struct s_strmat *sA, int ai, int aj, float *A, int lda)
	{
	int ii, jj;
	int lda2 = sA->m;
	float *pA = sA->pA + ai + aj*lda2;
	for(jj=0; jj<n; jj++)
		{
		ii = 0;
		for(; ii<m-3; ii+=4)
			{
			A[ii+0+jj*lda] = pA[ii+0+jj*lda2];
			A[ii+1+jj*lda] = pA[ii+1+jj*lda2];
			A[ii+2+jj*lda] = pA[ii+2+jj*lda2];
			A[ii+3+jj*lda] = pA[ii+3+jj*lda2];
			}
		for(; ii<m; ii++)
			{
			A[ii+0+jj*lda] = pA[ii+0+jj*lda2];
			}
		}
	return;
	}



// convert and transpose a matrix structure into a matrix
void s_cvt_tran_strmat2mat(int m, int n, struct s_strmat *sA, int ai, int aj, float *A, int lda)
	{
	int ii, jj;
	int lda2 = sA->m;
	float *pA = sA->pA + ai + aj*lda2;
	for(jj=0; jj<n; jj++)
		{
		ii = 0;
		for(; ii<m-3; ii+=4)
			{
			A[jj+(ii+0)*lda] = pA[ii+0+jj*lda2];
			A[jj+(ii+1)*lda] = pA[ii+1+jj*lda2];
			A[jj+(ii+2)*lda] = pA[ii+2+jj*lda2];
			A[jj+(ii+3)*lda] = pA[ii+3+jj*lda2];
			}
		for(; ii<m; ii++)
			{
			A[jj+(ii+0)*lda] = pA[ii+0+jj*lda2];
			}
		}
	return;
	}



// convert a vector structure into a vector
void s_cvt_strvec2vec(int m, struct s_strvec *sa, int ai, float *a)
	{
	float *pa = sa->pa + ai;
	int ii;
	for(ii=0; ii<m; ii++)
		a[ii] = pa[ii];
	return;
	}



// insert element into strmat
void sgein1_libstr(float a, struct s_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	pA[0] = a;
	return;
	}



// extract element from strmat
float sgeex1_libstr(struct s_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	return pA[0];
	}



// insert element into strvec
void svecin1_libstr(float a, struct s_strvec *sx, int xi)
	{
	float *x = sx->pa + xi;
	x[0] = a;
	return;
	}



// extract element from strvec
float svecex1_libstr(struct s_strvec *sx, int xi)
	{
	float *x = sx->pa + xi;
	return x[0];
	}



// set all elements of a strmat to a value
void sgese_libstr(int m, int n, float alpha, struct s_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	int ii, jj;
	for(jj=0; jj<n; jj++)
		{
		for(ii=0; ii<m; ii++)
			{
			pA[ii+lda*jj] = alpha;
			}
		}
	return;
	}



// set all elements of a strvec to a value
void svecse_libstr(int m, float alpha, struct s_strvec *sx, int xi)
	{
	float *x = sx->pa + xi;
	int ii;
	for(ii=0; ii<m; ii++)
		x[ii] = alpha;
	return;
	}



// extract diagonal to vector
void sdiaex_libstr(int kmax, float alpha, struct s_strmat *sA, int ai, int aj, struct s_strvec *sx, int xi)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	float *x = sx->pa + xi;
	int ii;
	for(ii=0; ii<kmax; ii++)
		x[ii] = alpha*pA[ii*(lda+1)];
	return;
	}



// insert a vector into diagonal
void sdiain_libstr(int kmax, float alpha, struct s_strvec *sx, int xi, struct s_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	float *x = sx->pa + xi;
	int ii;
	for(ii=0; ii<kmax; ii++)
		pA[ii*(lda+1)] = alpha*x[ii];
	return;
	}



// extract a row into a vector
void srowex_libstr(int kmax, float alpha, struct s_strmat *sA, int ai, int aj, struct s_strvec *sx, int xi)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	float *x = sx->pa + xi;
	int ii;
	for(ii=0; ii<kmax; ii++)
		x[ii] = alpha*pA[ii*lda];
	return;
	}



// insert a vector  into a row
void srowin_libstr(int kmax, float alpha, struct s_strvec *sx, int xi, struct s_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	float *x = sx->pa + xi;
	int ii;
	for(ii=0; ii<kmax; ii++)
		pA[ii*lda] = alpha*x[ii];
	return;
	}



// add a vector to a row
void srowad_libstr(int kmax, float alpha, struct s_strvec *sx, int xi, struct s_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	float *x = sx->pa + xi;
	int ii;
	for(ii=0; ii<kmax; ii++)
		pA[ii*lda] += alpha*x[ii];
	return;
	}



// swap two rows of a matrix struct
void srowsw_libstr(int kmax, struct s_strmat *sA, int ai, int aj, struct s_strmat *sC, int ci, int cj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	int ldc = sC->m;
	float *pC = sC->pA + ci + cj*lda;
	int ii;
	float tmp;
	for(ii=0; ii<kmax; ii++)
		{
		tmp = pA[ii*lda];
		pA[ii*lda] = pC[ii*ldc];
		pC[ii*ldc] = tmp;
		}
	return;
	}



// permute the rows of a matrix struct
void srowpe_libstr(int kmax, int *ipiv, struct s_strmat *sA)
	{
	int ii;
	for(ii=0; ii<kmax; ii++)
		{
		if(ipiv[ii]!=ii)
			srowsw_libstr(sA->n, sA, ii, 0, sA, ipiv[ii], 0);
		}
	return;
	}



// insert a vector  into a rcol
void scolin_libstr(int kmax, struct s_strvec *sx, int xi, struct s_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	float *x = sx->pa + xi;
	int ii;
	for(ii=0; ii<kmax; ii++)
		pA[ii] = x[ii];
	return;
	}



// swap two cols of a matrix struct
void scolsw_libstr(int kmax, struct s_strmat *sA, int ai, int aj, struct s_strmat *sC, int ci, int cj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	int ldc = sC->m;
	float *pC = sC->pA + ci + cj*lda;
	int ii;
	float tmp;
	for(ii=0; ii<kmax; ii++)
		{
		tmp = pA[ii];
		pA[ii] = pC[ii];
		pC[ii] = tmp;
		}
	return;
	}



// permute the cols of a matrix struct
void scolpe_libstr(int kmax, int *ipiv, struct s_strmat *sA)
	{
	int ii;
	for(ii=0; ii<kmax; ii++)
		{
		if(ipiv[ii]!=ii)
			scolsw_libstr(sA->m, sA, 0, ii, sA, 0, ipiv[ii]);
		}
	return;
	}



// copy a generic strmat into a generic strmat
void sgecp_libstr(int m, int n, struct s_strmat *sA, int ai, int aj, struct s_strmat *sC, int ci, int cj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	int ldc = sC->m;
	float *pC = sC->pA + ci + cj*ldc;
	int ii, jj;
	for(jj=0; jj<n; jj++)
		{
		ii = 0;
		for(; ii<m-3; ii+=4)
			{
			pC[ii+0+jj*ldc] = pA[ii+0+jj*lda];
			pC[ii+1+jj*ldc] = pA[ii+1+jj*lda];
			pC[ii+2+jj*ldc] = pA[ii+2+jj*lda];
			pC[ii+3+jj*ldc] = pA[ii+3+jj*lda];
			}
		for(; ii<m; ii++)
			{
			pC[ii+0+jj*ldc] = pA[ii+0+jj*lda];
			}
		}
	return;
	}



// scale a generic strmat
void sgesc_libstr(int m, int n, float alpha, struct s_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	int ii, jj;
	for(jj=0; jj<n; jj++)
		{
		ii = 0;
		for(; ii<m-3; ii+=4)
			{
			pA[ii+0+jj*lda] *= alpha;
			pA[ii+1+jj*lda] *= alpha;
			pA[ii+2+jj*lda] *= alpha;
			pA[ii+3+jj*lda] *= alpha;
			}
		for(; ii<m; ii++)
			{
			pA[ii+0+jj*lda] *= alpha;
			}
		}
	return;
	}



// copy a strvec into a strvec
void sveccp_libstr(int m, struct s_strvec *sa, int ai, struct s_strvec *sc, int ci)
	{
	float *pa = sa->pa + ai;
	float *pc = sc->pa + ci;
	int ii;
	ii = 0;
	for(; ii<m-3; ii+=4)
		{
		pc[ii+0] = pa[ii+0];
		pc[ii+1] = pa[ii+1];
		pc[ii+2] = pa[ii+2];
		pc[ii+3] = pa[ii+3];
		}
	for(; ii<m; ii++)
		{
		pc[ii+0] = pa[ii+0];
		}
	return;
	}



// scale a strvec
void svecsc_libstr(int m, float alpha, struct s_strvec *sa, int ai)
	{
	float *pa = sa->pa + ai;
	int ii;
	ii = 0;
	for(; ii<m-3; ii+=4)
		{
		pa[ii+0] *= alpha;
		pa[ii+1] *= alpha;
		pa[ii+2] *= alpha;
		pa[ii+3] *= alpha;
		}
	for(; ii<m; ii++)
		{
		pa[ii+0] *= alpha;
		}
	return;
	}



// copy a lower triangular strmat into a lower triangular strmat
void strcp_l_libstr(int m, struct s_strmat *sA, int ai, int aj, struct s_strmat *sC, int ci, int cj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	int ldc = sC->m;
	float *pC = sC->pA + ci + cj*ldc;
	int ii, jj;
	for(jj=0; jj<m; jj++)
		{
		ii = jj;
		for(; ii<m; ii++)
			{
			pC[ii+0+jj*ldc] = pA[ii+0+jj*lda];
			}
		}
	return;
	}



// scale and add a generic strmat into a generic strmat
void sgead_libstr(int m, int n, float alpha, struct s_strmat *sA, int ai, int aj, struct s_strmat *sC, int ci, int cj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	int ldc = sC->m;
	float *pC = sC->pA + ci + cj*ldc;
	int ii, jj;
	for(jj=0; jj<n; jj++)
		{
		ii = 0;
		for(; ii<m-3; ii+=4)
			{
			pC[ii+0+jj*ldc] += alpha*pA[ii+0+jj*lda];
			pC[ii+1+jj*ldc] += alpha*pA[ii+1+jj*lda];
			pC[ii+2+jj*ldc] += alpha*pA[ii+2+jj*lda];
			pC[ii+3+jj*ldc] += alpha*pA[ii+3+jj*lda];
			}
		for(; ii<m; ii++)
			{
			pC[ii+0+jj*ldc] += alpha*pA[ii+0+jj*lda];
			}
		}
	return;
	}



// scales and adds a strvec into a strvec
void svecad_libstr(int m, float alpha, struct s_strvec *sa, int ai, struct s_strvec *sc, int ci)
	{
	float *pa = sa->pa + ai;
	float *pc = sc->pa + ci;
	int ii;
	ii = 0;
	for(; ii<m-3; ii+=4)
		{
		pc[ii+0] += alpha*pa[ii+0];
		pc[ii+1] += alpha*pa[ii+1];
		pc[ii+2] += alpha*pa[ii+2];
		pc[ii+3] += alpha*pa[ii+3];
		}
	for(; ii<m; ii++)
		{
		pc[ii+0] += alpha*pa[ii+0];
		}
	return;
	}



// copy and transpose a generic strmat into a generic strmat
void sgetr_libstr(int m, int n, struct s_strmat *sA, int ai, int aj, struct s_strmat *sC, int ci, int cj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	int ldc = sC->m;
	float *pC = sC->pA + ci + cj*ldc;
	int ii, jj;
	for(jj=0; jj<n; jj++)
		{
		ii = 0;
		for(; ii<m-3; ii+=4)
			{
			pC[jj+(ii+0)*ldc] = pA[ii+0+jj*lda];
			pC[jj+(ii+1)*ldc] = pA[ii+1+jj*lda];
			pC[jj+(ii+2)*ldc] = pA[ii+2+jj*lda];
			pC[jj+(ii+3)*ldc] = pA[ii+3+jj*lda];
			}
		for(; ii<m; ii++)
			{
			pC[jj+(ii+0)*ldc] = pA[ii+0+jj*lda];
			}
		}
	return;
	}



// copy and transpose a lower triangular strmat into an upper triangular strmat
void strtr_l_libstr(int m, struct s_strmat *sA, int ai, int aj, struct s_strmat *sC, int ci, int cj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	int ldc = sC->m;
	float *pC = sC->pA + ci + cj*ldc;
	int ii, jj;
	for(jj=0; jj<m; jj++)
		{
		ii = jj;
		for(; ii<m; ii++)
			{
			pC[jj+(ii+0)*ldc] = pA[ii+0+jj*lda];
			}
		}
	return;
	}



// copy and transpose an upper triangular strmat into a lower triangular strmat
void strtr_u_libstr(int m, struct s_strmat *sA, int ai, int aj, struct s_strmat *sC, int ci, int cj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	int ldc = sC->m;
	float *pC = sC->pA + ci + cj*ldc;
	int ii, jj;
	for(jj=0; jj<m; jj++)
		{
		ii = 0;
		for(; ii<=jj; ii++)
			{
			pC[jj+(ii+0)*ldc] = pA[ii+0+jj*lda];
			}
		}
	return;
	}



// insert a strvec to the diagonal of a strmat, sparse formulation
void sdiain_sp_libstr(int kmax, float alpha, struct s_strvec *sx, int xi, int *idx, struct s_strmat *sD, int di, int dj)
	{
	float *x = sx->pa + xi;
	int ldd = sD->m;
	float *pD = sD->pA + di + dj*ldd;
	int ii, jj;
	for(jj=0; jj<kmax; jj++)
		{
		ii = idx[jj];
		pD[ii*(ldd+1)] = alpha * x[jj];
		}
	return;
	}



// extract the diagonal of a strmat from a strvec , sparse formulation
void sdiaex_sp_libstr(int kmax, float alpha, int *idx, struct s_strmat *sD, int di, int dj, struct s_strvec *sx, int xi)
	{
	float *x = sx->pa + xi;
	int ldd = sD->m;
	float *pD = sD->pA + di + dj*ldd;
	int ii, jj;
	for(jj=0; jj<kmax; jj++)
		{
		ii = idx[jj];
		x[jj] = alpha * pD[ii*(ldd+1)];
		}
	return;
	}



// add a vector to diagonal
void sdiaad_libstr(int kmax, float alpha, struct s_strvec *sx, int xi, struct s_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	float *pA = sA->pA + ai + aj*lda;
	float *x = sx->pa + xi;
	int ii;
	for(ii=0; ii<kmax; ii++)
		pA[ii*(lda+1)] += alpha*x[ii];
	return;
	}



// add scaled strvec to another strvec and insert to diagonal of strmat, sparse formulation
void sdiaad_sp_libstr(int kmax, float alpha, struct s_strvec *sx, int xi, int *idx, struct s_strmat *sD, int di, int dj)
	{
	float *x = sx->pa + xi;
	int ldd = sD->m;
	float *pD = sD->pA + di + dj*ldd;
	int ii, jj;
	for(jj=0; jj<kmax; jj++)
		{
		ii = idx[jj];
		pD[ii*(ldd+1)] += alpha * x[jj];
		}
	return;
	}



// add scaled strvec to another strvec and insert to diagonal of strmat, sparse formulation
void sdiaadin_sp_libstr(int kmax, float alpha, struct s_strvec *sx, int xi, struct s_strvec *sy, int yi, int *idx, struct s_strmat *sD, int di, int dj)
	{
	float *x = sx->pa + xi;
	float *y = sy->pa + yi;
	int ldd = sD->m;
	float *pD = sD->pA + di + dj*ldd;
	int ii, jj;
	for(jj=0; jj<kmax; jj++)
		{
		ii = idx[jj];
		pD[ii*(ldd+1)] = y[jj] + alpha * x[jj];
		}
	return;
	}



// add scaled strvec to row of strmat, sparse formulation
void srowad_sp_libstr(int kmax, float alpha, struct s_strvec *sx, int xi, int *idx, struct s_strmat *sD, int di, int dj)
	{
	float *x = sx->pa + xi;
	int ldd = sD->m;
	float *pD = sD->pA + di + dj*ldd;
	int ii, jj;
	for(jj=0; jj<kmax; jj++)
		{
		ii = idx[jj];
		pD[ii*ldd] += alpha * x[jj];
		}
	return;
	}




void svecad_sp_libstr(int m, float alpha, struct s_strvec *sx, int xi, int *idx, struct s_strvec *sz, int zi)
	{
	float *x = sx->pa + xi;
	float *z = sz->pa + zi;
	int ii;
	for(ii=0; ii<m; ii++)
		z[idx[ii]] += alpha * x[ii];
	return;
	}



void svecin_sp_libstr(int m, float alpha, struct s_strvec *sx, int xi, int *idx, struct s_strvec *sz, int zi)
	{
	float *x = sx->pa + xi;
	float *z = sz->pa + zi;
	int ii;
	for(ii=0; ii<m; ii++)
		z[idx[ii]] = alpha * x[ii];
	return;
	}



void svecex_sp_libstr(int m, float alpha, int *idx, struct s_strvec *sx, int xi, struct s_strvec *sz, int zi)
	{
	float *x = sx->pa + xi;
	float *z = sz->pa + zi;
	int ii;
	for(ii=0; ii<m; ii++)
		z[ii] = alpha * x[idx[ii]];
	return;
	}


// clip without mask return
void sveccl_libstr(int m, struct s_strvec *sxm, int xim, struct s_strvec *sx, int xi, struct s_strvec *sxp, int xip, struct s_strvec *sz, int zi)
	{
	float *xm = sxm->pa + xim;
	float *x  = sx->pa + xi;
	float *xp = sxp->pa + xip;
	float *z  = sz->pa + zi;
	int ii;
	for(ii=0; ii<m; ii++)
		{
		if(x[ii]>=xp[ii])
			{
			z[ii] = xp[ii];
			}
		else if(x[ii]<=xm[ii])
			{
			z[ii] = xm[ii];
			}
		else
			{
			z[ii] = x[ii];
			}
		}
	return;
	}



// clip with mask return
void sveccl_mask_libstr(int m, struct s_strvec *sxm, int xim, struct s_strvec *sx, int xi, struct s_strvec *sxp, int xip, struct s_strvec *sz, int zi, struct s_strvec *sm, int mi)
	{
	float *xm = sxm->pa + xim;
	float *x  = sx->pa + xi;
	float *xp = sxp->pa + xip;
	float *z  = sz->pa + zi;
	float *mask  = sm->pa + mi;
	int ii;
	for(ii=0; ii<m; ii++)
		{
		if(x[ii]>=xp[ii])
			{
			z[ii] = xp[ii];
			mask[ii] = 1.0;
			}
		else if(x[ii]<=xm[ii])
			{
			z[ii] = xm[ii];
			mask[ii] = -1.0;
			}
		else
			{
			z[ii] = x[ii];
			mask[ii] = 0.0;
			}
		}
	return;
	}


// zero out components using mask
void svecze_libstr(int m, struct s_strvec *sm, int mi, struct s_strvec *sv, int vi, struct s_strvec *se, int ei)
	{
	float *mask = sm->pa + mi;
	float *v = sv->pa + vi;
	float *e = se->pa + ei;
	int ii;
	for(ii=0; ii<m; ii++)
		{
		if(mask[ii]==0)
			{
			e[ii] = v[ii];
			}
		else
			{
			e[ii] = 0;
			}
		}
	return;
	}



void svecnrm_inf_libstr(int m, struct s_strvec *sx, int xi, float *ptr_norm)
	{
	int ii;
	float *x = sx->pa + xi;
	float norm = 0.0;
	for(ii=0; ii<m; ii++)
		norm = fmax(norm, fabs(x[ii]));
	*ptr_norm = norm;
	return;
	}



#else

#error : wrong LA choice

#endif

