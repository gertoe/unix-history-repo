/* ip_input.c 1.12 81/11/08 */

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/clock.h"
#include "../h/mbuf.h"
#include "../net/inet_cksum.h"
#include "../net/inet.h"
#include "../net/inet_systm.h"
#include "../net/imp.h"
#include "../net/ip.h"			/* belongs before inet.h */
#include "../net/ip_icmp.h"
#include "../net/tcp.h"

/*
 * Ip initialization.
 */
ip_init()
{

	ipq.next = ipq.prev = &ipq;
	ip_id = time & 0xffff;
}

int	ipcksum = 1;

struct	ip *ip_reass();

/*
 * Ip input routines.
 */

/*
 * Ip input routine.  Checksum and byte swap header.  If fragmented
 * try to reassamble.  If complete and fragment queue exists, discard.
 * Process options.  Pass to next level.
 */
ip_input(m0)
	struct mbuf *m0;
{
	register struct ip *ip;		/* known to be r11 in CKSUM below */
	register struct mbuf *m = m0;
	register int i;
	register struct ipq *q;
	register struct ipq *fp;
	int hlen;

COUNT(IP_INPUT);
	/*
	 * Check header and byteswap.
	 */
	ip = mtod(m, struct ip *);
	if ((hlen = ip->ip_hl << 2) > m->m_len) {
		printf("ip hdr ovflo\n");
		m_freem(m);
		return;
	}
	CKSUM_IPCHK(m, ip, r11, hlen);
	if (ip->ip_sum) {
		printf("ip_sum %x\n", ip->ip_sum);
		netstat.ip_badsum++;
		if (ipcksum) {
			m_freem(m);
			return;
		}
	}
	ip->ip_len = ntohs(ip->ip_len);
	ip->ip_id = ntohs(ip->ip_id);
	ip->ip_off = ntohs(ip->ip_off);

	/*
	 * Check that the amount of data in the buffers
	 * is as at least much as the IP header would have us expect.
	 * Trim mbufs if longer than we expect.
	 * Drop packet if shorter than we expect.
	 */
	i = 0;
	for (; m != NULL; m = m->m_next)
		i += m->m_len;
	m = m0;
	if (i != ip->ip_len) {
		if (i < ip->ip_len) {
			printf("ip_input: short packet\n");
			m_freem(m);
			return;
		}
		m_adj(m, ip->ip_len - i);
	}

	/*
	 * Process options and, if not destined for us,
	 * ship it on.
	 */
	if (hlen > sizeof (struct ip))
		ip_dooptions(ip, hlen);
	if (ip->ip_dst.s_addr != n_lhost.s_addr) {
		if (--ip->ip_ttl == 0) {
			icmp_error(ip, ICMP_TIMXCEED);
			return;
		}
		ip_output(dtom(ip));
		return;
	}

	/*
	 * Look for queue of fragments
	 * of this datagram.
	 */
	for (fp = ipq.next; fp != &ipq; fp = fp->next)
		if (ip->ip_id == fp->ipq_id &&
		    ip->ip_src.s_addr == fp->ipq_src.s_addr &&
		    ip->ip_dst.s_addr == fp->ipq_dst.s_addr &&
		    ip->ip_p == fp->ipq_p)
			goto found;
	fp = 0;
found:

	/*
	 * Adjust ip_len to not reflect header,
	 * set ip_mff if more fragments are expected,
	 * convert offset of this to bytes.
	 */
	ip->ip_len -= hlen;
	ip->ip_mff = 0;
	if (ip->ip_off & IP_MF)
		ip->ip_mff = 1;
	ip->ip_off <<= 3;

	/*
	 * If datagram marked as having more fragments
	 * or if this is not the first fragment,
	 * attempt reassembly; if it succeeds, proceed.
	 */
	if (ip->ip_mff || ip->ip_off) {
		ip = ip_reass(ip, fp);
		if (ip == 0)
			return;
		hlen = ip->ip_hl << 2;
		m = dtom(ip);
	} else
		if (fp)
			(void) ip_freef(fp);

	/*
	 * Switch out to protocol specific routine.
	 * SHOULD GO THROUGH PROTOCOL SWITCH TABLE
	 */
	switch (ip->ip_p) {

	case IPPROTO_ICMP:
		icmp_input(m);
		break;

	case IPPROTO_TCP:
		if (hlen > sizeof (struct ip))
			ip_stripoptions(ip, hlen);
		tcp_input(m);
		break;

	case IPPROTO_UDP:
		if (hlen > sizeof (struct ip))
			ip_stripoptions(ip, hlen);
		udp_input(m);
		break;

	default:
		ri_input(m);
		break;
	}
}

/*
 * Take incoming datagram fragment and try to
 * reassamble it into whole datagram.  If a chain for
 * reassembly of this datagram already exists, then it
 * is given as fp; otherwise have to make a chain.
 */
struct ip *
ip_reass(ip, fp)
	register struct ip *ip;
	register struct ipq *fp;
{
	register struct mbuf *m = dtom(ip);
	register struct ip *q;
	struct mbuf *t;
	int hlen = ip->ip_hl << 2;
	int i, next;

	/*
	 * Presence of header sizes in mbufs
	 * would confuse code below.
	 */
	m->m_off += hlen;
	m->m_len -= hlen;

	/*
	 * If first fragment to arrive, create a reassembly queue.
	 */
	if (fp == 0) {
		if ((t = m_get(1)) == NULL)
			goto dropfrag;
		t->m_off = MMINOFF;
		fp = mtod(t, struct ipq *);
		insque(fp, &ipq);
		fp->ipq_ttl = IPFRAGTTL;
		fp->ipq_p = ip->ip_p;
		fp->ipq_id = ip->ip_id;
		fp->ipq_next = fp->ipq_prev = (struct ip *)fp;
		fp->ipq_src = ip->ip_src;
		fp->ipq_dst = ip->ip_dst;
	}

	/*
	 * Find a segment which begins after this one does.
	 */
	for (q = fp->ipq_next; q != (struct ip *)fp; q = q->ip_next)
		if (q->ip_off > ip->ip_off)
			break;

	/*
	 * If there is a preceding segment, it may provide some of
	 * our data already.  If so, drop the data from the incoming
	 * segment.  If it provides all of our data, drop us.
	 */
	if (q->ip_prev != (struct ip *)fp) {
		i = q->ip_prev->ip_off + q->ip_prev->ip_len - ip->ip_off;
		if (i > 0) {
			if (i >= ip->ip_len)
				goto dropfrag;
			m_adj(dtom(ip), i);
			ip->ip_off += i;
			ip->ip_len -= i;
		}
	}

	/*
	 * While we overlap succeeding segments trim them or,
	 * if they are completely covered, dequeue them.
	 */
	while (q != (struct ip *)fp && ip->ip_off + ip->ip_len > q->ip_off) {
		i = (ip->ip_off + ip->ip_len) - q->ip_off;
		if (i < q->ip_len) {
			q->ip_len -= i;
			m_adj(dtom(q), i);
			break;
		}
		q = q->ip_next;
		m_freem(dtom(q->ip_prev));
		ip_deq(q->ip_prev);
	}

	/*
	 * Stick new segment in its place;
	 * check for complete reassembly.
	 */
	ip_enq(ip, q->ip_prev);
	next = 0;
	for (q = fp->ipq_next; q != (struct ip *)fp; q = q->ip_next) {
		if (q->ip_off != next)
			return (0);
		next += q->ip_len;
	}
	if (q->ip_prev->ip_mff)
		return (0);

	/*
	 * Reassembly is complete; concatenate fragments.
	 */
	q = fp->ipq_next;
	m = dtom(q);
	t = m->m_next;
	m->m_next = 0;
	m_cat(m, t);
	while ((q = q->ip_next) != (struct ip *)fp)
		m_cat(m, dtom(q));

	/*
	 * Create header for new ip packet by
	 * modifying header of first packet;
	 * dequeue and discard fragment reassembly header.
	 * Make header visible.
	 */
	ip = fp->ipq_next;
	ip->ip_len = next;
	ip->ip_src = fp->ipq_src;
	ip->ip_dst = fp->ipq_dst;
	remque(fp);
	m_free(dtom(fp));
	m = dtom(ip);
	m->m_len += sizeof (struct ip);
	m->m_off -= sizeof (struct ip);
	return (ip);

dropfrag:
	m_freem(m);
	return (0);
}

/*
 * Free a fragment reassembly header and all
 * associated datagrams.
 */
struct ipq *
ip_freef(fp)
	struct ipq *fp;
{
	register struct ip *q;
	struct mbuf *m;

	for (q = fp->ipq_next; q != (struct ip *)fp; q = q->ip_next)
		m_freem(dtom(q));
	m = dtom(fp);
	fp = fp->next;
	remque(fp->prev);
	m_free(m);
	return (fp);
}

/*
 * Put an ip fragment on a reassembly chain.
 * Like insque, but pointers in middle of structure.
 */
ip_enq(p, prev)
	register struct ip *p;
	register struct ip *prev;
{
COUNT(IP_ENQ);

	p->ip_prev = prev;
	p->ip_next = prev->ip_next;
	prev->ip_next->ip_prev = p;
	prev->ip_next = p;
}

/*
 * To ip_enq as remque is to insque.
 */
ip_deq(p)
	register struct ip *p;
{
COUNT(IP_DEQ);

	p->ip_prev->ip_next = p->ip_next;
	p->ip_next->ip_prev = p->ip_prev;
}

/*
 * IP timer processing;
 * if a timer expires on a reassembly
 * queue, discard it.
 */
ip_slowtimo()
{
	register struct ip *q;
	register struct ipq *fp;
	int s = splnet();
COUNT(IP_SLOWTIMO);

	for (fp = ipq.next; fp != &ipq; )
		if (--fp->ipq_ttl == 0)
			fp = ip_freef(fp);
		else
			fp = fp->next;
	splx(s);
}

ip_drain()
{

}
/*
 * Do option processing on a datagram,
 * possibly discarding it if bad options
 * are encountered.
 */
ip_dooptions(ip)
	struct ip *ip;
{
	register u_char *cp;
	int opt, optlen, cnt, s;
	struct ip_addr *sp;
	register struct ip_timestamp *ipt;
	int x;

	cp = (u_char *)(ip + 1);
	cnt = (ip->ip_hl << 2) - sizeof (struct ip);
	for (; cnt > 0; cnt -= optlen, cp += optlen) {
		opt = cp[0];
		if (opt == IPOPT_EOL)
			break;
		if (opt == IPOPT_NOP)
			optlen = 1;
		else
			optlen = cp[1];
		switch (opt) {

		default:
			break;

		case IPOPT_LSRR:
		case IPOPT_SSRR:
			if (cp[2] < 4 || cp[2] > optlen - (sizeof (long) - 1))
				break;
			sp = (struct ip_addr *)(cp + cp[2]);
			if (n_lhost.s_addr == *(u_long *)sp) {
				if (opt == IPOPT_SSRR) {
					/* MAKE SURE *SP DIRECTLY ACCESSIBLE */
				}
				ip->ip_dst = *sp;
				*sp = n_lhost;
				cp[2] += 4;
			}
			break;

		case IPOPT_TS:
			ipt = (struct ip_timestamp *)cp;
			if (ipt->ipt_len < 5)
				goto bad;
			if (ipt->ipt_ptr > ipt->ipt_len - sizeof (long)) {
				if (++ipt->ipt_oflw == 0)
					goto bad;
				break;
			}
			sp = (struct ip_addr *)(cp+cp[2]);
			switch (ipt->ipt_flg) {

			case IPOPT_TS_TSONLY:
				break;

			case IPOPT_TS_TSANDADDR:
				if (ipt->ipt_ptr + 8 > ipt->ipt_len)
					goto bad;
				*(struct ip_addr *)sp++ = n_lhost;
				break;

			case IPOPT_TS_PRESPEC:
				if (*(u_long *)sp != n_lhost.s_addr)
					break;
				if (ipt->ipt_ptr + 8 > ipt->ipt_len)
					goto bad;
				ipt->ipt_ptr += 4;
				break;

			default:
				goto bad;
			}
			*(n_time *)sp = ip_time();
			ipt->ipt_ptr += 4;
		}
	}
	return (0);
bad:
	/* SHOULD FORCE ICMP MESSAGE */
	return (-1);
}

/*
 * Strip out IP options, e.g. before passing
 * to higher level protocol in the kernel.
 */
ip_stripoptions(ip)
	struct ip *ip;
{
	register int i;
	register struct mbuf *m;
	char *op;
	int olen;
COUNT(IP_OPT);

	olen = (ip->ip_hl<<2) - sizeof (struct ip);
	op = (caddr_t)ip + olen;
	m = dtom(++ip);
	i = m->m_len - (sizeof (struct ip) + olen);
	bcopy((caddr_t)ip+olen, (caddr_t)ip, i);
	m->m_len -= i;
}
