/*-
 * Copyright (c) 1999 Cameron Grant <cg@freebsd.org>
 * Copyright by Hannu Savolainen 1995
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

/*
 * first, include kernel header files.
 */

#ifndef _OS_H_
#define _OS_H_

#ifdef _KERNEL
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/ioccom.h>
#include <sys/filio.h>
#include <sys/sockio.h>
#include <sys/fcntl.h>
#include <sys/tty.h>
#include <sys/proc.h>
#include <sys/kernel.h> /* for DATA_SET */
#include <sys/module.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/uio.h>
#include <sys/syslog.h>
#include <sys/errno.h>
#include <sys/malloc.h>
#include <sys/bus.h>
#if __FreeBSD_version < 500000
#include <sys/buf.h>
#endif
#include <machine/resource.h>
#include <machine/bus.h>
#include <sys/rman.h>
#include <sys/limits.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <sys/sbuf.h>
#include <sys/soundcard.h>
#include <sys/sysctl.h>
#include <sys/kobj.h>
#include <vm/vm.h>
#include <vm/pmap.h>

#undef	USING_MUTEX
#undef	USING_DEVFS

#if __FreeBSD_version > 500000
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/condvar.h>

#define USING_MUTEX
#define USING_DEVFS
#else
#define	INTR_TYPE_AV	INTR_TYPE_TTY
#define	INTR_MPSAFE	0
#endif

#define SND_DYNSYSCTL

struct pcm_channel;
struct pcm_feeder;
struct snd_dbuf;
struct snd_mixer;

#include <dev/sound/pcm/buffer.h>
#include <dev/sound/pcm/channel.h>
#include <dev/sound/pcm/feeder.h>
#include <dev/sound/pcm/mixer.h>
#include <dev/sound/pcm/dsp.h>

#define	PCM_SOFTC_SIZE	512

#define SND_STATUSLEN	64

#define SOUND_MODVER	1

#define SOUND_MINVER	1
#define SOUND_PREFVER	SOUND_MODVER
#define SOUND_MAXVER	1

/*
PROPOSAL:
each unit needs:
status, mixer, dsp, dspW, audio, sequencer, midi-in, seq2, sndproc = 9 devices
dspW and audio are deprecated.
dsp needs min 64 channels, will give it 256

minor = (unit << 20) + (dev << 16) + channel
currently minor = (channel << 16) + (unit << 4) + dev

nomenclature:
	/dev/pcmX/dsp.(0..255)
	/dev/pcmX/dspW
	/dev/pcmX/audio
	/dev/pcmX/status
	/dev/pcmX/mixer
	[etc.]
*/

#define PCMMAXCHAN		0xff
#define PCMMAXDEV		0x0f
#define PCMMAXUNIT		0x0f
#define PCMMINOR(x)		(minor(x))
#define PCMCHAN(x)		((PCMMINOR(x) >> 16) & PCMMAXCHAN)
#define PCMUNIT(x)		((PCMMINOR(x) >> 4) & PCMMAXUNIT)
#define PCMDEV(x)		(PCMMINOR(x) & PCMMAXDEV)
#define PCMMKMINOR(u, d, c)	((((c) & PCMMAXCHAN) << 16) | \
				(((u) & PCMMAXUNIT) << 4) | ((d) & PCMMAXDEV))

#define SD_F_SIMPLEX		0x00000001
#define SD_F_AUTOVCHAN		0x00000002
#define SD_F_SOFTPCMVOL		0x00000004
#define SD_F_PSWAPLR		0x00000008
#define SD_F_RSWAPLR		0x00000010
#define SD_F_PRIO_RD		0x10000000
#define SD_F_PRIO_WR		0x20000000
#define SD_F_PRIO_SET		(SD_F_PRIO_RD | SD_F_PRIO_WR)
#define SD_F_DIR_SET		0x40000000
#define SD_F_TRANSIENT		0xf0000000

/* many variables should be reduced to a range. Here define a macro */
#define RANGE(var, low, high) (var) = \
	(((var)<(low))? (low) : ((var)>(high))? (high) : (var))
#define DSP_BUFFSIZE (8192)

/*
 * Macros for reading/writing PCM sample / int values from bytes array.
 * Since every process is done using signed integer (and to make our life
 * less miserable), unsigned sample will be converted to its signed
 * counterpart and restored during writing back. To avoid overflow,
 * we truncate 32bit (and only 32bit) samples down to 24bit (see below
 * for the reason), unless PCM_USE_64BIT_ARITH is defined.
 */

/*
 * Automatically turn on 64bit arithmetic on suitable archs
 * (amd64 64bit, ia64, etc..) for wider 32bit samples / integer processing.
 */
#if LONG_BIT >= 64
#undef PCM_USE_64BIT_ARITH
#define PCM_USE_64BIT_ARITH	1
#else
#if 0
#undef PCM_USE_64BIT_ARITH
#define PCM_USE_64BIT_ARITH	1
#endif
#endif

#ifdef PCM_USE_64BIT_ARITH
typedef int64_t intpcm_t;
#else
typedef int32_t intpcm_t;
#endif

/* 32bit fixed point shift */
#define	PCM_FXSHIFT	8

#define PCM_S8_MAX	  0x7f
#define PCM_S8_MIN	 -0x80
#define PCM_S16_MAX	  0x7fff
#define PCM_S16_MIN	 -0x8000
#define PCM_S24_MAX	  0x7fffff
#define PCM_S24_MIN	 -0x800000
#ifdef PCM_USE_64BIT_ARITH
#if LONG_BIT >= 64
#define PCM_S32_MAX	  0x7fffffffL
#define PCM_S32_MIN	 -0x80000000L
#else
#define PCM_S32_MAX	  0x7fffffffLL
#define PCM_S32_MIN	 -0x80000000LL
#endif
#else
#define PCM_S32_MAX	  0x7fffffff
#define PCM_S32_MIN	(-0x7fffffff - 1)
#endif

/* Bytes-per-sample definition */
#define PCM_8_BPS	1
#define PCM_16_BPS	2
#define PCM_24_BPS	3
#define PCM_32_BPS	4

#if BYTE_ORDER == LITTLE_ENDIAN
#define PCM_READ_S16_LE(b8)		*((int16_t *)(b8))
#define _PCM_READ_S32_LE(b8)		*((int32_t *)(b8))
#define PCM_READ_S16_BE(b8) \
		((int32_t)((b8)[1] | ((int8_t)((b8)[0])) << 8))
#define _PCM_READ_S32_BE(b8) \
		((int32_t)((b8)[3] | (b8)[2] << 8 | (b8)[1] << 16 | \
			((int8_t)((b8)[0])) << 24))

#define PCM_WRITE_S16_LE(b8, val)	*((int16_t *)(b8)) = (val)
#define _PCM_WRITE_S32_LE(b8, val)	*((int32_t *)(b8)) = (val)
#define PCM_WRITE_S16_BE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[1] = val; \
			b8[0] = val >> 8; \
		} while(0)
#define _PCM_WRITE_S32_BE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[3] = val; \
			b8[2] = val >> 8; \
			b8[1] = val >> 16; \
			b8[0] = val >> 24; \
		} while(0)

#define PCM_READ_U16_LE(b8)		((int16_t)(*((uint16_t *)(b8)) ^ 0x8000))
#define _PCM_READ_U32_LE(b8)		((int32_t)(*((uint32_t *)(b8)) ^ 0x80000000))
#define PCM_READ_U16_BE(b8) \
		((int32_t)((b8)[1] | ((int8_t)((b8)[0] ^ 0x80)) << 8))
#define _PCM_READ_U32_BE(b8) \
		((int32_t)((b8)[3] | (b8)[2] << 8 | (b8)[1] << 16 | \
			((int8_t)((b8)[0] ^ 0x80)) << 24))

#define PCM_WRITE_U16_LE(b8, val)	*((uint16_t *)(b8)) = (val) ^ 0x8000
#define _PCM_WRITE_U32_LE(b8, val)	*((uint32_t *)(b8)) = (val) ^ 0x80000000
#define PCM_WRITE_U16_BE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[1] = val; \
			b8[0] = (val >> 8) ^ 0x80; \
		} while(0)
#define _PCM_WRITE_U32_BE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[3] = val; \
			b8[2] = val >> 8; \
			b8[1] = val >> 16; \
			b8[0] = (val >> 24) ^ 0x80; \
		} while(0)
#else /* !LITTLE_ENDIAN */
#define PCM_READ_S16_LE(b8) \
		((int32_t)((b8)[0] | ((int8_t)((b8)[1])) << 8))
#define _PCM_READ_S32_LE(b8) \
		((int32_t)((b8)[0] | (b8)[1] << 8 | (b8)[2] << 16 | \
			((int8_t)((b8)[3])) << 24))
#define PCM_READ_S16_BE(b8)		*((int16_t *)(b8))
#define _PCM_READ_S32_BE(b8)		*((int32_t *)(b8))

#define PCM_WRITE_S16_LE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[0] = val; \
			b8[1] = val >> 8; \
		} while(0)
#define _PCM_WRITE_S32_LE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[0] = val; \
			b8[1] = val >> 8; \
			b8[2] = val >> 16; \
			b8[3] = val >> 24; \
		} while(0)
#define PCM_WRITE_S16_BE(b8, val)	*((int16_t *)(b8)) = (val)
#define _PCM_WRITE_S32_BE(b8, val)	*((int32_t *)(b8)) = (val)

#define PCM_READ_U16_LE(b8) \
		((int32_t)((b8)[0] | ((int8_t)((b8)[1] ^ 0x80)) << 8))
#define _PCM_READ_U32_LE(b8) \
		((int32_t)((b8)[0] | (b8)[1] << 8 | (b8)[2] << 16 | \
			((int8_t)((b8)[3] ^ 0x80)) << 24))
#define PCM_READ_U16_BE(b8) ((int16_t)(*((uint16_t *)(b8)) ^ 0x8000))
#define _PCM_READ_U32_BE(b8) ((int32_t)(*((uint32_t *)(b8)) ^ 0x80000000))

#define PCM_WRITE_U16_LE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[0] = val; \
			b8[1] = (val >> 8) ^ 0x80; \
		} while(0)
#define _PCM_WRITE_U32_LE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[0] = val; \
			b8[1] = val >> 8; \
			b8[2] = val >> 16; \
			b8[3] = (val >> 24) ^ 0x80; \
		} while(0)
#define PCM_WRITE_U16_BE(b8, val)	*((uint16_t *)(b8)) = (val) ^ 0x8000
#define _PCM_WRITE_U32_BE(b8, val)	*((uint32_t *)(b8)) = (val) ^ 0x80000000
#endif

#define PCM_READ_S24_LE(b8) \
		((int32_t)((b8)[0] | (b8)[1] << 8 | ((int8_t)((b8)[2])) << 16))
#define PCM_READ_S24_BE(b8) \
		((int32_t)((b8)[2] | (b8)[1] << 8 | ((int8_t)((b8)[0])) << 16))

#define PCM_WRITE_S24_LE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[0] = val; \
			b8[1] = val >> 8; \
			b8[2] = val >> 16; \
		} while(0)
#define PCM_WRITE_S24_BE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[2] = val; \
			b8[1] = val >> 8; \
			b8[0] = val >> 16; \
		} while(0)

#define PCM_READ_U24_LE(b8) \
		((int32_t)((b8)[0] | (b8)[1] << 8 | \
			((int8_t)((b8)[2] ^ 0x80)) << 16))
#define PCM_READ_U24_BE(b8) \
		((int32_t)((b8)[2] | (b8)[1] << 8 | \
			((int8_t)((b8)[0] ^ 0x80)) << 16))

#define PCM_WRITE_U24_LE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[0] = val; \
			b8[1] = val >> 8; \
			b8[2] = (val >> 16) ^ 0x80; \
		} while(0)
#define PCM_WRITE_U24_BE(bb8, vval) do { \
			int32_t val = (vval); \
			uint8_t *b8 = (bb8); \
			b8[2] = val; \
			b8[1] = val >> 8; \
			b8[0] = (val >> 16) ^ 0x80; \
		} while(0)

#ifdef PCM_USE_64BIT_ARITH
#define PCM_READ_S32_LE(b8)		_PCM_READ_S32_LE(b8)
#define PCM_READ_S32_BE(b8)		_PCM_READ_S32_BE(b8)
#define PCM_WRITE_S32_LE(b8, val)	_PCM_WRITE_S32_LE(b8, val)
#define PCM_WRITE_S32_BE(b8, val)	_PCM_WRITE_S32_BE(b8, val)

#define PCM_READ_U32_LE(b8)		_PCM_READ_U32_LE(b8)
#define PCM_READ_U32_BE(b8)		_PCM_READ_U32_BE(b8)
#define PCM_WRITE_U32_LE(b8, val)	_PCM_WRITE_U32_LE(b8, val)
#define PCM_WRITE_U32_BE(b8, val)	_PCM_WRITE_U32_BE(b8, val)
#else /* !PCM_USE_64BIT_ARITH */
/*
 * 24bit integer ?!? This is quite unfortunate, eh? Get the fact straight:
 * Dynamic range for:
 *	1) Human =~ 140db
 *	2) 16bit = 96db (close enough)
 *	3) 24bit = 144db (perfect)
 *	4) 32bit = 196db (way too much)
 *	5) Bugs Bunny = Gazillion!@%$Erbzzztt-EINVAL db
 * Since we're not Bugs Bunny ..uh..err.. avoiding 64bit arithmetic, 24bit
 * is pretty much sufficient for our signed integer processing.
 */
#define PCM_READ_S32_LE(b8)		(_PCM_READ_S32_LE(b8) >> PCM_FXSHIFT)
#define PCM_READ_S32_BE(b8)		(_PCM_READ_S32_BE(b8) >> PCM_FXSHIFT)
#define PCM_WRITE_S32_LE(b8, val)	_PCM_WRITE_S32_LE(b8, (val) << PCM_FXSHIFT)
#define PCM_WRITE_S32_BE(b8, val)	_PCM_WRITE_S32_BE(b8, (val) << PCM_FXSHIFT)

#define PCM_READ_U32_LE(b8)		(_PCM_READ_U32_LE(b8) >> PCM_FXSHIFT)
#define PCM_READ_U32_BE(b8)		(_PCM_READ_U32_BE(b8) >> PCM_FXSHIFT)
#define PCM_WRITE_U32_LE(b8, val)	_PCM_WRITE_U32_LE(b8, (val) << PCM_FXSHIFT)
#define PCM_WRITE_U32_BE(b8, val)	_PCM_WRITE_U32_BE(b8, (val) << PCM_FXSHIFT)
#endif

/*
 * 8bit sample is pretty much useless since it doesn't provide
 * sufficient dynamic range throughout our filtering process.
 * For the sake of completeness, declare it anyway.
 */
#define PCM_READ_S8(b8)			*((int8_t *)(b8))
#define PCM_READ_S8_NE(b8)		PCM_READ_S8(b8)
#define PCM_READ_U8(b8)			((int8_t)(*((uint8_t *)(b8)) ^ 0x80))
#define PCM_READ_U8_NE(b8)		PCM_READ_U8(b8)

#define PCM_WRITE_S8(b8, val)		*((int8_t *)(b8)) = (val)
#define PCM_WRITE_S8_NE(b8, val)	PCM_WRITE_S8(b8, val)
#define PCM_WRITE_U8(b8, val)		*((uint8_t *)(b8)) = (val) ^ 0x80
#define PCM_WRITE_U8_NE(b8, val)	PCM_WRITE_U8(b8, val)

#define PCM_CLAMP_S8(val) \
		(((val) > PCM_S8_MAX) ? PCM_S8_MAX : \
			(((val) < PCM_S8_MIN) ? PCM_S8_MIN : (val)))
#define PCM_CLAMP_S16(val) \
		(((val) > PCM_S16_MAX) ? PCM_S16_MAX : \
			(((val) < PCM_S16_MIN) ? PCM_S16_MIN : (val)))
#define PCM_CLAMP_S24(val) \
		(((val) > PCM_S24_MAX) ? PCM_S24_MAX : \
			(((val) < PCM_S24_MIN) ? PCM_S24_MIN : (val)))

#ifdef PCM_USE_64BIT_ARITH
#define PCM_CLAMP_S32(val) \
		(((val) > PCM_S32_MAX) ? PCM_S32_MAX : \
			(((val) < PCM_S32_MIN) ? PCM_S32_MIN : (val)))
#else
#define PCM_CLAMP_S32(val) \
		(((val) > PCM_S24_MAX) ? PCM_S32_MAX : \
			(((val) < PCM_S24_MIN) ? PCM_S32_MIN : \
			((val) << PCM_FXSHIFT)))
#endif

#define PCM_CLAMP_U8(val)	PCM_CLAMP_S8(val)
#define PCM_CLAMP_U16(val)	PCM_CLAMP_S16(val)
#define PCM_CLAMP_U24(val)	PCM_CLAMP_S24(val)
#define PCM_CLAMP_U32(val)	PCM_CLAMP_S32(val)

/* make figuring out what a format is easier. got AFMT_STEREO already */
#define AFMT_32BIT (AFMT_S32_LE | AFMT_S32_BE | AFMT_U32_LE | AFMT_U32_BE)
#define AFMT_24BIT (AFMT_S24_LE | AFMT_S24_BE | AFMT_U24_LE | AFMT_U24_BE)
#define AFMT_16BIT (AFMT_S16_LE | AFMT_S16_BE | AFMT_U16_LE | AFMT_U16_BE)
#define AFMT_8BIT (AFMT_MU_LAW | AFMT_A_LAW | AFMT_U8 | AFMT_S8)
#define AFMT_SIGNED (AFMT_S32_LE | AFMT_S32_BE | AFMT_S24_LE | AFMT_S24_BE | \
			AFMT_S16_LE | AFMT_S16_BE | AFMT_S8)
#define AFMT_BIGENDIAN (AFMT_S32_BE | AFMT_U32_BE | AFMT_S24_BE | AFMT_U24_BE | \
			AFMT_S16_BE | AFMT_U16_BE)

struct pcm_channel *fkchan_setup(device_t dev);
int fkchan_kill(struct pcm_channel *c);

/* XXX Flawed definition. I'll fix it someday. */
#define	SND_MAXVCHANS	PCMMAXCHAN

/*
 * Minor numbers for the sound driver.
 *
 * Unfortunately Creative called the codec chip of SB as a DSP. For this
 * reason the /dev/dsp is reserved for digitized audio use. There is a
 * device for true DSP processors but it will be called something else.
 * In v3.0 it's /dev/sndproc but this could be a temporary solution.
 */

#define SND_DEV_CTL	0	/* Control port /dev/mixer */
#define SND_DEV_SEQ	1	/* Sequencer /dev/sequencer */
#define SND_DEV_MIDIN	2	/* Raw midi access */
#define SND_DEV_DSP	3	/* Digitized voice /dev/dsp */
#define SND_DEV_AUDIO	4	/* Sparc compatible /dev/audio */
#define SND_DEV_DSP16	5	/* Like /dev/dsp but 16 bits/sample */
#define SND_DEV_STATUS	6	/* /dev/sndstat */
				/* #7 not in use now. */
#define SND_DEV_SEQ2	8	/* /dev/sequencer, level 2 interface */
#define SND_DEV_SNDPROC 9	/* /dev/sndproc for programmable devices */
#define SND_DEV_PSS	SND_DEV_SNDPROC /* ? */
#define SND_DEV_NORESET	10
#define SND_DEV_DSPHW	11	/* specific channel request */

#define DSP_DEFAULT_SPEED	8000

#define ON		1
#define OFF		0

extern int pcm_veto_load;
extern int snd_unit;
extern int snd_maxautovchans;
extern int snd_verbose;
extern devclass_t pcm_devclass;
extern struct unrhdr *pcmsg_unrhdr;

/*
 * some macros for debugging purposes
 * DDB/DEB to enable/disable debugging stuff
 * BVDDB   to enable debugging when bootverbose
 */
#define BVDDB(x) if (bootverbose) x

#ifndef DEB
#define DEB(x)
#endif

SYSCTL_DECL(_hw_snd);

struct pcm_channel *pcm_getfakechan(struct snddev_info *d);
int pcm_chnalloc(struct snddev_info *d, struct pcm_channel **ch, int direction, pid_t pid, int chnum);
int pcm_chnrelease(struct pcm_channel *c);
int pcm_chnref(struct pcm_channel *c, int ref);
int pcm_inprog(struct snddev_info *d, int delta);

struct pcm_channel *pcm_chn_create(struct snddev_info *d, struct pcm_channel *parent, kobj_class_t cls, int dir, void *devinfo);
int pcm_chn_destroy(struct pcm_channel *ch);
int pcm_chn_add(struct snddev_info *d, struct pcm_channel *ch);
int pcm_chn_remove(struct snddev_info *d, struct pcm_channel *ch);

int pcm_addchan(device_t dev, int dir, kobj_class_t cls, void *devinfo);
unsigned int pcm_getbuffersize(device_t dev, unsigned int minbufsz, unsigned int deflt, unsigned int maxbufsz);
int pcm_register(device_t dev, void *devinfo, int numplay, int numrec);
int pcm_unregister(device_t dev);
int pcm_setstatus(device_t dev, char *str);
u_int32_t pcm_getflags(device_t dev);
void pcm_setflags(device_t dev, u_int32_t val);
void *pcm_getdevinfo(device_t dev);


int snd_setup_intr(device_t dev, struct resource *res, int flags,
		   driver_intr_t hand, void *param, void **cookiep);

void *snd_mtxcreate(const char *desc, const char *type);
void snd_mtxfree(void *m);
void snd_mtxassert(void *m);
#define	snd_mtxlock(m) mtx_lock(m)
#define	snd_mtxunlock(m) mtx_unlock(m)

int sysctl_hw_snd_vchans(SYSCTL_HANDLER_ARGS);

typedef int (*sndstat_handler)(struct sbuf *s, device_t dev, int verbose);
int sndstat_acquire(void);
int sndstat_release(void);
int sndstat_register(device_t dev, char *str, sndstat_handler handler);
int sndstat_registerfile(char *str);
int sndstat_unregister(device_t dev);
int sndstat_unregisterfile(char *str);

#define SND_DECLARE_FILE(version) \
	_SND_DECLARE_FILE(__LINE__, version)

#define _SND_DECLARE_FILE(uniq, version) \
	__SND_DECLARE_FILE(uniq, version)

#define __SND_DECLARE_FILE(uniq, version) \
	static char sndstat_vinfo[] = version; \
	SYSINIT(sdf_ ## uniq, SI_SUB_DRIVERS, SI_ORDER_MIDDLE, sndstat_registerfile, sndstat_vinfo); \
	SYSUNINIT(sdf_ ## uniq, SI_SUB_DRIVERS, SI_ORDER_MIDDLE, sndstat_unregisterfile, sndstat_vinfo);

/* usage of flags in device config entry (config file) */
#define DV_F_DRQ_MASK	0x00000007	/* mask for secondary drq */
#define	DV_F_DUAL_DMA	0x00000010	/* set to use secondary dma channel */

/* ought to be made obsolete but still used by mss */
#define	DV_F_DEV_MASK	0x0000ff00	/* force device type/class */
#define	DV_F_DEV_SHIFT	8		/* force device type/class */

#define	PCM_DEBUG_MTX

/*
 * this is rather kludgey- we need to duplicate these struct def'ns from sound.c
 * so that the macro versions of pcm_{,un}lock can dereference them.
 * we also have to do this now makedev() has gone away.
 */

struct snddev_channel {
	SLIST_ENTRY(snddev_channel) link;
	struct pcm_channel *channel;
	int chan_num;
	struct cdev *dsp_devt;
	struct cdev *dspW_devt;
	struct cdev *audio_devt;
	struct cdev *dspHW_devt;
};

struct snddev_info {
	SLIST_HEAD(, snddev_channel) channels;
	struct pcm_channel *fakechan;
	unsigned devcount, playcount, reccount, vchancount;
	unsigned flags;
	int inprog;
	unsigned int bufsz;
	void *devinfo;
	device_t dev;
	char status[SND_STATUSLEN];
	struct mtx *lock;
	struct cdev *mixer_dev;

};

void	sound_oss_sysinfo(oss_sysinfo *);

#ifdef	PCM_DEBUG_MTX
#define	pcm_lock(d) mtx_lock(((struct snddev_info *)(d))->lock)
#define	pcm_unlock(d) mtx_unlock(((struct snddev_info *)(d))->lock)
#else
void pcm_lock(struct snddev_info *d);
void pcm_unlock(struct snddev_info *d);
#endif

#ifdef KLD_MODULE
#define PCM_KLDSTRING(a) ("kld " # a)
#else
#define PCM_KLDSTRING(a) ""
#endif

#endif /* _KERNEL */

#endif	/* _OS_H_ */
