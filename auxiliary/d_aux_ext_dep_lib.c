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
#if 0
#include <malloc.h>
#endif

#include "../include/blasfeo_common.h"
#include "../include/blasfeo_block_size.h"
#include "../include/blasfeo_d_aux.h"
#include "../include/blasfeo_v_aux_ext_dep.h"




#if ! defined(OS_WINDOWS)
int posix_memalign(void **memptr, size_t alignment, size_t size);
#endif



/* creates a zero matrix */
void d_zeros(double **pA, int row, int col)
	{
	*pA = malloc((row*col)*sizeof(double));
	double *A = *pA;
	int i;
	for(i=0; i<row*col; i++) A[i] = 0.0;
	}



/* creates a zero matrix aligned to a cache line */
void d_zeros_align(double **pA, int row, int col)
	{
#if defined(OS_WINDOWS)
	*pA = (double *) _aligned_malloc( (row*col)*sizeof(double), 64 );
#else
	void *temp;
	int err = posix_memalign(&temp, 64, (row*col)*sizeof(double));
	if(err!=0)
		{
		printf("Memory allocation error");
		exit(1);
		}
	*pA = temp;
#endif
	double *A = *pA;
	int i;
	for(i=0; i<row*col; i++) A[i] = 0.0;
	}



/* frees matrix */
void d_free(double *pA)
	{
	free( pA );
	}



/* frees aligned matrix */
void d_free_align(double *pA)
	{
#if defined(OS_WINDOWS)
	_aligned_free( pA );
#else
	free( pA );
#endif
	}



/* prints a matrix in column-major format */
void d_print_mat(int m, int n, double *A, int lda)
	{
	int i, j;
	for(i=0; i<m; i++)
		{
		for(j=0; j<n; j++)
			{
			printf("%9.5f ", A[i+lda*j]);
			}
		printf("\n");
		}
	printf("\n");
	}	



/* prints the transposed of a matrix in column-major format */
void d_print_tran_mat(int row, int col, double *A, int lda)
	{
	int i, j;
	for(j=0; j<col; j++)
		{
		for(i=0; i<row; i++)
			{
			printf("%9.5f ", A[i+lda*j]);
			}
		printf("\n");
		}
	printf("\n");
	}	



/* prints a matrix in column-major format */
void d_print_to_file_mat(FILE *file, int row, int col, double *A, int lda)
	{
	int i, j;
	for(i=0; i<row; i++)
		{
		for(j=0; j<col; j++)
			{
			fprintf(file, "%9.5f ", A[i+lda*j]);
			}
		fprintf(file, "\n");
		}
	fprintf(file, "\n");
	}	



/* prints the transposed of a matrix in column-major format */
void d_print_tran_to_file_mat(FILE *file, int row, int col, double *A, int lda)
	{
	int i, j;
	for(j=0; j<col; j++)
		{
		for(i=0; i<row; i++)
			{
			fprintf(file, "%9.5f ", A[i+lda*j]);
			}
		fprintf(file, "\n");
		}
	fprintf(file, "\n");
	}	



/* prints a matrix in column-major format (exponential notation) */
void d_print_e_mat(int m, int n, double *A, int lda)
	{
	int i, j;
	for(i=0; i<m; i++)
		{
		for(j=0; j<n; j++)
			{
			printf("%e\t", A[i+lda*j]);
			}
		printf("\n");
		}
	printf("\n");
	}	



/* prints the transposed of a matrix in column-major format (exponential notation) */
void d_print_e_tran_mat(int row, int col, double *A, int lda)
	{
	int i, j;
	for(j=0; j<col; j++)
		{
		for(i=0; i<row; i++)
			{
			printf("%e\t", A[i+lda*j]);
			}
		printf("\n");
		}
	printf("\n");
	}	



/****************************
* strmat interface
****************************/

#if defined(LA_HIGH_PERFORMANCE)



// create a matrix structure for a matrix of size m*n by dynamically allocating the memory
struct d_strmat * d_allocate_strmat(int m, int n)
	{
	int size = d_memsize_strmat(m, n);
	void *mem;
	v_zeros_align(&mem, size);
	struct d_strmat *sA = d_create_strmat(m, n, mem);
	return sA;
	}



// free memory of a matrix structure
void d_free_strmat(struct d_strmat *sA)
	{
	v_free_align(sA);
	return;
	}



// create a vector structure for a vector of size m by dynamically allocating the memory
struct d_strvec * d_allocate_strvec(int m)
	{
	int size = d_memsize_strvec(m);
	void *mem;
	v_zeros_align(&mem, size);
	struct d_strvec *sa = d_create_strvec(m, mem);
	return sa;
	}



// free memory of a matrix structure
void d_free_strvec(struct d_strvec *sa)
	{
	v_free_align(sa);
	return;
	}



// print a matrix structure
void d_print_strmat(int m, int n, struct d_strmat *sA, int ai, int aj)
	{
	const int ps = D_PS;
	int sda = sA->cn;
	double *pA = sA->pA + aj*ps + ai/ps*ps*sda + ai%ps;
	int ii, i, j, tmp;
	ii = 0;
	if(ai%ps>0)
		{
		tmp = ps-ai%ps;
		tmp = m<tmp ? m : tmp;
		for(i=0; i<tmp; i++)
			{
			for(j=0; j<n; j++)
				{
				printf("%9.5f ", pA[i+ps*j]);
				}
			printf("\n");
			}
		pA += tmp + ps*(sda-1);
		m -= tmp;
		}
	for( ; ii<m-(ps-1); ii+=ps)
		{
		for(i=0; i<ps; i++)
			{
			for(j=0; j<n; j++)
				{
				printf("%9.5f ", pA[i+ps*j+sda*ii]);
				}
			printf("\n");
			}
		}
	if(ii<m)
		{
		tmp = m-ii;
		for(i=0; i<tmp; i++)
			{
			for(j=0; j<n; j++)
				{
				printf("%9.5f ", pA[i+ps*j+sda*ii]);
				}
			printf("\n");
			}
		}
	printf("\n");
	return;
	}



// print a vector structure
void d_print_strvec(int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_mat(m, 1, pa, m);
	return;
	}



// print the transposed of a vector structure
void d_print_tran_strvec(int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_mat(1, m, pa, 1);
	return;
	}



// print a matrix structure
void d_print_to_file_strmat(FILE * file, int m, int n, struct d_strmat *sA, int ai, int aj)
	{
	const int ps = D_PS;
	int sda = sA->cn;
	double *pA = sA->pA + aj*ps + ai/ps*ps*sda + ai%ps;
	int ii, i, j, tmp;
	ii = 0;
	if(ai%ps>0)
		{
		tmp = ps-ai%ps;
		tmp = m<tmp ? m : tmp;
		for(i=0; i<tmp; i++)
			{
			for(j=0; j<n; j++)
				{
				fprintf(file, "%9.5f ", pA[i+ps*j]);
				}
			fprintf(file, "\n");
			}
		pA += tmp + ps*(sda-1);
		m -= tmp;
		}
	for( ; ii<m-(ps-1); ii+=ps)
		{
		for(i=0; i<ps; i++)
			{
			for(j=0; j<n; j++)
				{
				fprintf(file, "%9.5f ", pA[i+ps*j+sda*ii]);
				}
			fprintf(file, "\n");
			}
		}
	if(ii<m)
		{
		tmp = m-ii;
		for(i=0; i<tmp; i++)
			{
			for(j=0; j<n; j++)
				{
				fprintf(file, "%9.5f ", pA[i+ps*j+sda*ii]);
				}
			fprintf(file, "\n");
			}
		}
	fprintf(file, "\n");
	return;
	}



// print a vector structure
void d_print_to_file_strvec(FILE * file, int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_to_file_mat(file, m, 1, pa, m);
	return;
	}



// print the transposed of a vector structure
void d_print_tran_to_file_strvec(FILE * file, int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_to_file_mat(file, 1, m, pa, 1);
	return;
	}



// print a matrix structure
void d_print_e_strmat(int m, int n, struct d_strmat *sA, int ai, int aj)
	{
	const int ps = D_PS;
	int sda = sA->cn;
	double *pA = sA->pA + aj*ps + ai/ps*ps*sda + ai%ps;
	int ii, i, j, tmp;
	ii = 0;
	if(ai%ps>0)
		{
		tmp = ps-ai%ps;
		tmp = m<tmp ? m : tmp;
		for(i=0; i<tmp; i++)
			{
			for(j=0; j<n; j++)
				{
				printf("%e\t", pA[i+ps*j]);
				}
			printf("\n");
			}
		pA += tmp + ps*(sda-1);
		m -= tmp;
		}
	for( ; ii<m-(ps-1); ii+=ps)
		{
		for(i=0; i<ps; i++)
			{
			for(j=0; j<n; j++)
				{
				printf("%e\t", pA[i+ps*j+sda*ii]);
				}
			printf("\n");
			}
		}
	if(ii<m)
		{
		tmp = m-ii;
		for(i=0; i<tmp; i++)
			{
			for(j=0; j<n; j++)
				{
				printf("%e\t", pA[i+ps*j+sda*ii]);
				}
			printf("\n");
			}
		}
	printf("\n");
	return;
	}



// print a vector structure
void d_print_e_strvec(int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_e_mat(m, 1, pa, m);
	return;
	}



// print the transposed of a vector structure
void d_print_e_tran_strvec(int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_e_mat(1, m, pa, 1);
	return;
	}



#elif defined(LA_BLAS) | defined(LA_REFERENCE)



// create a matrix structure for a matrix of size m*n by dynamically allocating the memory
struct d_strmat * d_allocate_strmat(int m, int n)
	{
	int size = d_memsize_strmat(m, n);
	void *mem;
	v_zeros_align(&mem, size);
	struct d_strmat *sA = d_create_strmat(m, n, mem);
	return sA;
	}



// free memory of a matrix structure
void d_free_strmat(struct d_strmat *sA)
	{
	v_free_align(sA);
	return;
	}



// create a vector structure for a vector of size m by dynamically allocating the memory
struct d_strvec * d_allocate_strvec(int m)
	{
	int size = d_memsize_strvec(m);
	void *mem;
	v_zeros_align(&mem, size);
	struct d_strvec *sa = d_create_strvec(m, mem);
	return sa;
	}



// free memory of a matrix structure
void d_free_strvec(struct d_strvec *sa)
	{
	v_free_align(sa);
	return;
	}



// print a matrix structure
void d_print_strmat(int m, int n, struct d_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	double *pA = sA->pA + ai + aj*lda;
	d_print_mat(m, n, pA, lda);
	return;
	}



// print a vector structure
void d_print_strvec(int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_mat(m, 1, pa, m);
	return;
	}



// print and transpose a vector structure
void d_print_tran_strvec(int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_mat(1, m, pa, 1);
	return;
	}



// print a matrix structure
void d_print_to_file_strmat(FILE *file, int m, int n, struct d_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	double *pA = sA->pA + ai + aj*lda;
	d_print_to_file_mat(file, m, n, pA, lda);
	return;
	}



// print a vector structure
void d_print_to_file_strvec(FILE *file, int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_to_file_mat(file, m, 1, pa, m);
	return;
	}



// print and transpose a vector structure
void d_print_to_file_tran_strvec(FILE *file, int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_to_file_mat(file, 1, m, pa, 1);
	return;
	}



// print a matrix structure
void d_print_e_strmat(int m, int n, struct d_strmat *sA, int ai, int aj)
	{
	int lda = sA->m;
	double *pA = sA->pA + ai + aj*lda;
	d_print_e_mat(m, n, pA, lda);
	return;
	}



// print a vector structure
void d_print_e_strvec(int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_e_mat(m, 1, pa, m);
	return;
	}



// print and transpose a vector structure
void d_print_e_tran_strvec(int m, struct d_strvec *sa, int ai)
	{
	double *pa = sa->pa + ai;
	d_print_e_mat(1, m, pa, 1);
	return;
	}



#else

#error : wrong LA choice

#endif


