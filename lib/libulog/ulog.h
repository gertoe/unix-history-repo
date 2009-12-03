/*-
 * Copyright (c) 2009 Ed Schouten <ed@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef _ULOG_H_
#define	_ULOG_H_

#include <sys/cdefs.h>
#include <sys/_timeval.h>

/*
 * libulog.
 *
 * This library is provided as a migratory tool towards <utmpx.h>.  We
 * cannot yet implement <utmpx.h>, because our on-disk file format lacks
 * various fields.  <utmpx.h> also has some shortcomings.  Ideally we
 * want to allow logging of user login records generated by unprivileged
 * processes as well, provided that they hold a file descriptor to a
 * pseudo-terminal master device.
 *
 * Unlike struct utmpx, the buffers containing the strings are not
 * stored inside struct ulog_utmpx itself.  Processes should never
 * handcraft these structures anyway.
 *
 * This library (or at least parts of it) will hopefully deprecate over
 * time, when we provide the <utmpx.h> API.
 */

#define	_UTX_USERDISPSIZE	16
#define	_UTX_LINEDISPSIZE	8
#define	_UTX_HOSTDISPSIZE	16

struct ulog_utmpx {
	char		*ut_user;
#if 0
	char		*ut_id;
#endif
	char		*ut_line;
	char		*ut_host;
#if 0
	pid_t		 ut_pid;
	short		 ut_type;
#endif
	struct timeval	 ut_tv;
};

__BEGIN_DECLS
void	ulog_endutxent(void);
struct ulog_utmpx *ulog_getutxent(void);
#if 0
struct ulog_utmpx *ulog_getutxid(const struct ulog_utmpx *id);
struct ulog_utmpx *ulog_getutxline(const struct ulog_utmpx *line);
struct ulog_utmpx *ulog_pututxline(const struct ulog_utmpx *utmpx);
#endif
void	ulog_setutxent(void);

void	ulog_login(const char *, const char *, const char *);
void	ulog_login_pseudo(int, const char *);
void	ulog_logout(const char *);
void	ulog_logout_pseudo(int);
__END_DECLS

#endif /* !_ULOG_H_ */
