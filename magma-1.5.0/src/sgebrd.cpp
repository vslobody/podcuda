/*
    -- MAGMA (version 1.5.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date September 2014

       @generated from zgebrd.cpp normal z -> s, Tue Sep  2 12:38:24 2014

*/
#include "common_magma.h"

/**
    Purpose
    -------
    SGEBRD reduces a general real M-by-N matrix A to upper or lower
    bidiagonal form B by an orthogonal transformation: Q**H * A * P = B.

    If m >= n, B is upper bidiagonal; if m < n, B is lower bidiagonal.

    Arguments
    ---------
    @param[in]
    m       INTEGER
            The number of rows in the matrix A.  M >= 0.

    @param[in]
    n       INTEGER
            The number of columns in the matrix A.  N >= 0.

    @param[in,out]
    A       REAL array, dimension (LDA,N)
            On entry, the M-by-N general matrix to be reduced.
            On exit,
            if m >= n, the diagonal and the first superdiagonal are
              overwritten with the upper bidiagonal matrix B; the
              elements below the diagonal, with the array TAUQ, represent
              the orthogonal matrix Q as a product of elementary
              reflectors, and the elements above the first superdiagonal,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors;
    \n
            if m < n, the diagonal and the first subdiagonal are
              overwritten with the lower bidiagonal matrix B; the
              elements below the first subdiagonal, with the array TAUQ,
              represent the orthogonal matrix Q as a product of
              elementary reflectors, and the elements above the diagonal,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors.
            See Further Details.

    @param[in]
    lda     INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    @param[out]
    d       real array, dimension (min(M,N))
            The diagonal elements of the bidiagonal matrix B:
            D(i) = A(i,i).

    @param[out]
    e       real array, dimension (min(M,N)-1)
            The off-diagonal elements of the bidiagonal matrix B:
            if m >= n, E(i) = A(i,i+1) for i = 1,2,...,n-1;
            if m < n, E(i) = A(i+1,i) for i = 1,2,...,m-1.

    @param[out]
    tauq    REAL array dimension (min(M,N))
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix Q. See Further Details.

    @param[out]
    taup    REAL array, dimension (min(M,N))
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix P. See Further Details.

    @param[out]
    work    (workspace) REAL array, dimension (MAX(1,LWORK))
            On exit, if INFO = 0, WORK[0] returns the optimal LWORK.

    @param[in]
    lwork   INTEGER
            The length of the array WORK.  LWORK >= (M+N)*NB, where NB
            is the optimal blocksize.
    \n
            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    @param[out]
    info    INTEGER
      -     = 0:  successful exit
      -     < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ---------------
    The matrices Q and P are represented as products of elementary
    reflectors:

    If m >= n,

       Q = H(1) H(2) . . . H(n)  and  P = G(1) G(2) . . . G(n-1)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors;
    v(1:i-1) = 0, v(i) = 1, and v(i+1:m) is stored on exit in A(i+1:m,i);
    u(1:i) = 0, u(i+1) = 1, and u(i+2:n) is stored on exit in A(i,i+2:n);
    tauq is stored in TAUQ(i) and taup in TAUP(i).

    If m < n,

       Q = H(1) H(2) . . . H(m-1)  and  P = G(1) G(2) . . . G(m)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors;
    v(1:i) = 0, v(i+1) = 1, and v(i+2:m) is stored on exit in A(i+2:m,i);
    u(1:i-1) = 0, u(i) = 1, and u(i+1:n) is stored on exit in A(i,i+1:n);
    tauq is stored in TAUQ(i) and taup in TAUP(i).

    The contents of A on exit are illustrated by the following examples:

    @verbatim
    m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n):

      (  d   e   u1  u1  u1 )           (  d   u1  u1  u1  u1  u1 )
      (  v1  d   e   u2  u2 )           (  e   d   u2  u2  u2  u2 )
      (  v1  v2  d   e   u3 )           (  v1  e   d   u3  u3  u3 )
      (  v1  v2  v3  d   e  )           (  v1  v2  e   d   u4  u4 )
      (  v1  v2  v3  v4  d  )           (  v1  v2  v3  e   d   u5 )
      (  v1  v2  v3  v4  v5 )
    @endverbatim

    where d and e denote diagonal and off-diagonal elements of B, vi
    denotes an element of the vector defining H(i), and ui an element of
    the vector defining G(i).

    @ingroup magma_sgesvd_comp
    ********************************************************************/
extern "C" magma_int_t
magma_sgebrd(magma_int_t m, magma_int_t n,
             float *A, magma_int_t lda, float *d, float *e,
             float *tauq, float *taup,
             float *work, magma_int_t lwork,
             magma_int_t *info)
{
#define  A(i, j) (A  + (j)*lda  + (i))
#define dA(i, j) (dA + (j)*ldda + (i))

    float c_neg_one = MAGMA_S_NEG_ONE;
    float c_one     = MAGMA_S_ONE;
    float *dA, *dwork;

    magma_int_t ncol, nrow, jmax, nb, ldda;

    magma_int_t i, j, nx;
    magma_int_t iinfo;

    magma_int_t minmn;
    magma_int_t ldwrkx, ldwrky, lwkopt;
    magma_int_t lquery;

    nb   = magma_get_sgebrd_nb(n);
    ldda = m;

    lwkopt = (m + n) * nb;
    work[0] = MAGMA_S_MAKE( lwkopt, 0. );
    lquery = (lwork == -1);
    
    /* Check arguments */
    *info = 0;
    if (m < 0) {
        *info = -1;
    } else if (n < 0) {
        *info = -2;
    } else if (lda < max(1,m)) {
        *info = -4;
    } else if (lwork < lwkopt && (! lquery) ) {
        *info = -10;
    }
    if (*info < 0) {
        magma_xerbla( __func__, -(*info) );
        return *info;
    }
    else if (lquery)
        return *info;

    /* Quick return if possible */
    minmn = min(m,n);
    if (minmn == 0) {
        work[0] = c_one;
        return *info;
    }

    if (MAGMA_SUCCESS != magma_smalloc( &dA, n*ldda + (m + n)*nb )) {
        fprintf (stderr, "!!!! device memory allocation error in sgebrd\n" );
        *info = MAGMA_ERR_DEVICE_ALLOC;
        return *info;
    }
    dwork = dA + n*ldda;

    ldwrkx = m;
    ldwrky = n;

    /* Set the block/unblock crossover point NX. */
    nx = 128;

    /* Copy the matrix to the GPU */
    if (minmn - nx >= 1) {
        magma_ssetmatrix( m, n, A, lda, dA, ldda );
    }
    
    for (i=0; i < (minmn - nx); i += nb) {
        /*  Reduce rows and columns i:i+nb-1 to bidiagonal form and return
            the matrices X and Y which are needed to update the unreduced
            part of the matrix */
        nrow = m - i;
        ncol = n - i;

        /*   Get the current panel (no need for the 1st iteration) */
        if ( i > 0 ) {
            magma_sgetmatrix( nrow, nb, dA(i, i), ldda, A( i, i), lda );
            magma_sgetmatrix( nb, ncol - nb,
                              dA(i, i+nb), ldda,
                              A( i, i+nb), lda );
        }

        magma_slabrd_gpu(nrow, ncol, nb,
                         A(i, i),          lda,    dA(i, i),          ldda,
                         d+i, e+i, tauq+i, taup+i,
                         work,             ldwrkx, dwork,             ldwrkx,  // x, dx
                         work+(ldwrkx*nb), ldwrky, dwork+(ldwrkx*nb), ldwrky); // y, dy

        /*  Update the trailing submatrix A(i+nb:m,i+nb:n), using an update
            of the form  A := A - V*Y' - X*U' */
        nrow = m - i - nb;
        ncol = n - i - nb;

        // Send Y back to the GPU
        magma_ssetmatrix( nrow, nb, work  + nb, ldwrkx, dwork + nb, ldwrkx );
        magma_ssetmatrix( ncol, nb,
                          work  + (ldwrkx+1)*nb, ldwrky,
                          dwork + (ldwrkx+1)*nb, ldwrky );

        magma_sgemm( MagmaNoTrans, MagmaConjTrans,
                     nrow, ncol, nb,
                     c_neg_one, dA(i+nb, i   ),      ldda,
                                dwork+(ldwrkx+1)*nb, ldwrky,
                     c_one,     dA(i+nb, i+nb),      ldda);

        magma_sgemm( MagmaNoTrans, MagmaNoTrans,
                     nrow, ncol, nb,
                     c_neg_one, dwork+nb,         ldwrkx,
                                dA( i,    i+nb ), ldda,
                     c_one,     dA( i+nb, i+nb ), ldda);

        /* Copy diagonal and off-diagonal elements of B back into A */
        if (m >= n) {
            jmax = i + nb;
            for (j = i; j < jmax; ++j) {
                *A(j, j  ) = MAGMA_S_MAKE( d[j], 0. );
                *A(j, j+1) = MAGMA_S_MAKE( e[j], 0. );
            }
        } else {
            jmax = i + nb;
            for (j = i; j < jmax; ++j) {
                *A(j,   j ) = MAGMA_S_MAKE( d[j], 0. );
                *A(j+1, j ) = MAGMA_S_MAKE( e[j], 0. );
            }
        }
    }

    /* Use unblocked code to reduce the remainder of the matrix */
    nrow = m - i;
    ncol = n - i;

    if ( 0 < minmn - nx ) {
        magma_sgetmatrix( nrow, ncol, dA(i, i), ldda, A(i, i), lda );
    }
    
    lapackf77_sgebrd( &nrow, &ncol,
                      A(i, i), &lda, d+i, e+i,
                      tauq+i, taup+i, work, &lwork, &iinfo);
    work[0] = MAGMA_S_MAKE( lwkopt, 0. );

    magma_free( dA );
    return *info;
} /* magma_sgebrd */