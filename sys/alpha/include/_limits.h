/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)limits.h	8.3 (Berkeley) 1/4/94
 *	From: NetBSD: limits.h,v 1.3 1997/04/06 08:47:31 cgd Exp
 * $FreeBSD$
 */

#ifndef _MACHINE__LIMITS_H_
#define	_MACHINE__LIMITS_H_

/*
 * According to ANSI (section 2.2.4.2), the values below must be usable by
 * #if preprocessing directives.  Additionally, the expression must have the
 * same type as would an expression that is an object of the corresponding
 * type converted according to the integral promotions.  The subtraction for
 * INT_MIN, etc., is so the value is not unsigned; e.g., 0x80000000 is an
 * unsigned int for 32-bit two's complement ANSI compilers (section 3.1.3.2).
 * These numbers are for the default configuration of gcc.  They work for
 * some other compilers as well, but this should not be depended on.
 */

#define	__CHAR_BIT	8		/* number of bits in a char */

#define	__SCHAR_MAX	0x7f		/* max value for a signed char */
#define	__SCHAR_MIN	(-0x7f-1)	/* min value for a signed char */

#define	__UCHAR_MAX	0xffU		/* max value for an unsigned char */
#define	__CHAR_MAX	0x7f		/* max value for a char */
#define	__CHAR_MIN	(-0x7f-1)	/* min value for a char */

#define	__USHRT_MAX	0xffffU		/* max value for an unsigned short */
#define	__SHRT_MAX	0x7fff		/* max value for a short */
#define	__SHRT_MIN	(-0x7fff-1)	/* min value for a short */

#define	__UINT_MAX	0xffffffffU	/* max value for an unsigned int */
#define	__INT_MAX	0x7fffffff	/* max value for an int */
#define	__INT_MIN	(-0x7fffffff-1)	/* min value for an int */

#define	__ULONG_MAX	0xffffffffffffffffUL	/* max for an unsigned long */
#define	__LONG_MAX	0x7fffffffffffffffL	/* max for a long */
#define	__LONG_MIN	(-0x7fffffffffffffffL-1) /* min for a long */

/* Long longs and longs are the same size on the alpha. */
					/* max for an unsigned long long */
#define	__ULLONG_MAX	0xffffffffffffffffULL
#define	__LLONG_MAX	0x7fffffffffffffffLL	/* max for a long long */
#define	__LLONG_MIN	(-0x7fffffffffffffffLL-1) /* min for a long long */

#define	__SSIZE_MAX	__LONG_MAX	/* max value for a ssize_t */

#define	__SIZE_T_MAX	__ULONG_MAX	/* max value for a size_t */

#define	__OFF_MAX	__LONG_MAX	/* max value for a off_t */
#define	__OFF_MIN	__LONG_MIN	/* min value for a off_t */

/* Quads and longs are the same on the alpha. Ensure they stay in sync. */
#define	__UQUAD_MAX	(__ULONG_MAX)	/* max value for a uquad_t */
#define	__QUAD_MAX	(__LONG_MAX)	/* max value for a quad_t */
#define	__QUAD_MIN	(__LONG_MIN)	/* min value for a quad_t */

#define	__LONG_BIT	64
#define	__WORD_BIT	32

#endif /* !_MACHINE__LIMITS_H_ */
