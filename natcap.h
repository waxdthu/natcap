/*
 * Author: Chen Minqiang <ptpt52@gmail.com>
 *  Date : Wed, 19 Dec 2012 09:52:21 +0800
 *
 * This file is part of the natcap.
 *
 * natcap is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * natcap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with natcap; see the file COPYING. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _NATCAP_H_
#define _NATCAP_H_

#define MODULE_NAME "natcap"
#define NATCAP_VERSION "6.0.0"

#ifdef __KERNEL__
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <asm/byteorder.h>
#include <asm/types.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>

#pragma pack(push)
#pragma pack(1)

#define NATCAP_CLIENT_MODE (1<<0)
#define NATCAP_NEED_ENC    (1<<1)

/* subtype: 7bits 0~127 */
#define SUBTYPE_NATCAP          0
#define SUBTYPE_PEER_SYN        64
#define SUBTYPE_PEER_SYNACK     65
#define SUBTYPE_PEER_ACK        66
#define SUBTYPE_PEER_FSYN       67
#define SUBTYPE_PEER_FACK       68
#define SUBTYPE_PEER_FSYNACK    69
#define SUBTYPE_PEER_XSYN       70
#define SUBTYPE_PEER_SSYN       71

struct natcap_TCPOPT_header {
	u8 opcode;
#define TCPOPT_PEER 0x9A
#define TCPOPT_NATCAP 0x99
	u8 opsize;
	u8 type;
#if defined(__LITTLE_ENDIAN_BITFIELD)
	u8 encryption:1,
	   subtype:7;
#elif defined(__BIG_ENDIAN_BITFIELD)
	u8 subtype:7,
	   encryption:1;
#else
#error	"Adjust your <asm/byteorder.h> defines"
#endif
};
struct natcap_TCPOPT_data {
	u32 u_hash;
	u8 mac_addr[ETH_ALEN];
	__be16 port;
	__be32 ip;
};

struct natcap_TCPOPT_dst {
	__be32 ip;
	__be16 port;
};

struct natcap_TCPOPT_user {
	u32 u_hash;
	u8 mac_addr[ETH_ALEN];
};

struct natcap_TCPOPT_peer {
	u16 icmp_id;
	u16 icmp_sequence;
	union {
		struct {
			u32 ip;
			u8 mac_addr[ETH_ALEN];
		} user;
		u16 map_port;
	};
	u16 icmp_payload_len;
	u8 timeval[0];
};

#define NATCAP_TCPOPT_SYN (1<<7)
#define NATCAP_TCPOPT_TARGET (1<<6)
#define NATCAP_TCPOPT_SPROXY (1<<5)
#define NATCAP_TCPOPT_CONFUSION (1<<4)

#define NATCAP_TCPOPT_TYPE_MASK (0x0F)
#define NATCAP_TCPOPT_TYPE(t) ((t) & NATCAP_TCPOPT_TYPE_MASK)

struct natcap_TCPOPT {
#define NATCAP_TCPOPT_TYPE_NONE 0
	struct natcap_TCPOPT_header header;
	union {
		struct {
#define NATCAP_TCPOPT_TYPE_ALL 1
			struct natcap_TCPOPT_data data;
		} all;
		struct {
#define NATCAP_TCPOPT_TYPE_DST 2
			struct natcap_TCPOPT_dst data;
		} dst;
		struct {
#define NATCAP_TCPOPT_TYPE_USER 3
			struct natcap_TCPOPT_user data;
		} user;
		struct {
#define NATCAP_TCPOPT_TYPE_PEER 6
			struct natcap_TCPOPT_peer data;
		} peer;
	};
#define NATCAP_TCPOPT_TYPE_CONFUSION 4
	char extra_pad[4]; /* sometimes on encode/decode need 4bytes extra space */
#define NATCAP_TCPOPT_TYPE_ADD 5
};

struct cone_nat_session {
	__be32 ip;
	__be16 port;
};

struct cone_snat_session {
	__be32 lan_ip;
	__be32 wan_ip;
	__be16 lan_port;
	__be16 wan_port;
};

#pragma pack(pop)

struct tuple {
	u16 encryption:8,
		tcp_encode:4,
		udp_encode:4;
	__be16 port;
	__be32 ip;
};

struct natcap_session {
	unsigned int magic;
#define NS_NATCAP_CONFUSION_BIT 0
#define NS_NATCAP_CONFUSION (1 << NS_NATCAP_CONFUSION_BIT)
#define NS_NATCAP_DST_BIT 1
#define NS_NATCAP_DST (1 << NS_NATCAP_DST_BIT)
#define NS_NATCAP_TCPUDPENC_BIT 2
#define NS_NATCAP_TCPUDPENC (1 << NS_NATCAP_TCPUDPENC_BIT)
#define NS_NATCAP_AUTH_BIT 3
#define NS_NATCAP_AUTH (1 << NS_NATCAP_AUTH_BIT)
#define NS_NATCAP_DROP_BIT 4
#define NS_NATCAP_DROP (1 << NS_NATCAP_DROP_BIT)
#define NS_NATCAP_NOLIMIT_BIT 5
#define NS_NATCAP_NOLIMIT (1 << NS_NATCAP_NOLIMIT_BIT)

#define NS_NATCAP_CONECFM_BIT 6
#define NS_NATCAP_CONECFM (1 << NS_NATCAP_CONECFM_BIT)
#define NS_NATCAP_CONESNAT_BIT 7
#define NS_NATCAP_CONESNAT (1 << NS_NATCAP_CONESNAT_BIT)

#define NS_NATCAP_TCPENC_BIT 13
#define NS_NATCAP_TCPENC (1 << NS_NATCAP_TCPENC_BIT)
#define NS_NATCAP_UDPENC_BIT 14
#define NS_NATCAP_UDPENC (1 << NS_NATCAP_UDPENC_BIT)
#define NS_NATCAP_ENC_BIT 15
#define NS_NATCAP_ENC (1 << NS_NATCAP_ENC_BIT)

#define NS_PEER_SSYN_BIT 11
#define NS_PEER_SSYN (1 << NS_PEER_SSYN_BIT)
#define NS_PEER_KNOCK_BIT 12
#define NS_PEER_KNOCK (1 << NS_PEER_KNOCK_BIT)
	union {
		struct {
			unsigned short status;
			unsigned short _pad[1];
			__be16 new_source;
			__be16 target_port;
			__be32 target_ip;
			union {
				int tcp_seq_offset; //used on HTTP confusion
				unsigned int current_seq; //used on UDP pack to TCP
			};
			union {
				int tcp_ack_offset; //used on HTTP confusion
				unsigned int foreign_seq; //used on UDP pack to TCP
			};
		} n;
		struct {
			unsigned short status;
			unsigned short cache_index;
			u16 remote_mss; //for peer used on server side
			__be16 peer_sport; //for peer used on server side
			__be32 peer_sip; //for peer used on server side
			unsigned int local_seq; //for peer used on both side
			unsigned int remote_seq; //for peer used on both side
			int tcp_seq_offset;
		} p;
	};
};

#define NATCAP_MAGIC 0x43415099

/*XXX refer to drivers/nos/src/nos.h */
#define IPS_NATFLOW_FF_STOP_BIT 18
#define IPS_NATFLOW_FF_STOP (1 << IPS_NATFLOW_FF_STOP_BIT)

// test t1 < t2 return 1
static inline int tuple_lt(const struct tuple *t1, const struct tuple *t2)
{
	if (ntohl(t1->ip) < ntohl(t2->ip))
		return 1;
	else if (ntohl(t1->ip) > ntohl(t2->ip))
		return 0;
	else if (ntohs(t1->port) < ntohs(t2->port))
		return 1;
	else if (ntohs(t1->port) > ntohs(t2->port))
		return 0;
	else if (t1->encryption < t2->encryption)
		return 1;
	else if (t1->encryption > t2->encryption)
		return 0;
	else if (t1->tcp_encode < t2->tcp_encode)
		return 1;
	else if (t1->tcp_encode > t2->tcp_encode)
		return 0;
	else if (t1->udp_encode < t2->udp_encode)
		return 1;
	else if (t1->udp_encode > t2->udp_encode)
		return 0;
	else
		return 0;
}
// test t1 == t2
static inline int tuple_eq(const struct tuple *t1, const struct tuple *t2)
{
	return (t1->ip == t2->ip &&
			t1->port == t2->port &&
			t1->encryption == t2->encryption &&
			t1->tcp_encode == t2->tcp_encode &&
			t1->udp_encode == t2->udp_encode);
}

static inline void tuple_copy(struct tuple *to, const struct tuple *from)
{
	to->encryption = from->encryption;
	to->tcp_encode = from->tcp_encode;
	to->udp_encode = from->udp_encode;
	to->port = from->port;
	to->ip = from->ip;
}

#define XT_MARK_NATCAP 0x99
#define XT_MARK_NATCAP_MASK 0xFF
#define xt_mark_natcap_set(mark, at) *(unsigned int *)(at) = ((*(unsigned int *)(at)) & (~XT_MARK_NATCAP_MASK)) | ((mark) & XT_MARK_NATCAP_MASK)
#define xt_mark_natcap_get(at) ((*(unsigned int *)(at)) & XT_MARK_NATCAP_MASK)

/* @linux/netfilter/nf_conntrack_common.h */
/* ct->status use bits:[31-24] for ecap status */

#define IPS_NATCAP_PRE_BIT 21
#define IPS_NATCAP_PRE (1 << IPS_NATCAP_PRE_BIT)

#define IPS_NATCAP_PEER_BIT 22
#define IPS_NATCAP_PEER (1 << IPS_NATCAP_PEER_BIT)

#define IPS_NATCAP_CFM_BIT 23
#define IPS_NATCAP_CFM (1 << IPS_NATCAP_CFM_BIT)

#define IPS_NATCAP_ACK_BIT 24
#define IPS_NATCAP_ACK (1 << IPS_NATCAP_ACK_BIT)

#define IPS_NATCAP_BIT 25
#define IPS_NATCAP (1 << IPS_NATCAP_BIT)

#define IPS_NATCAP_BYPASS_BIT 26
#define IPS_NATCAP_BYPASS (1 << IPS_NATCAP_BYPASS_BIT)

#define IPS_NATCAP_SERVER_BIT 27
#define IPS_NATCAP_SERVER (1 << IPS_NATCAP_SERVER_BIT)

#define IPS_NATCAP_CONE_BIT 28
#define IPS_NATCAP_CONE (1 << IPS_NATCAP_CONE_BIT)

#define IPS_NATCAP_DUAL_BIT 29
#define IPS_NATCAP_DUAL (1 << IPS_NATCAP_DUAL_BIT)

#define IPS_NATCAP_SYN1_BIT 30
#define IPS_NATCAP_SYN1 (1 << IPS_NATCAP_SYN1_BIT)

#define IPS_NATCAP_SYN2_BIT 31
#define IPS_NATCAP_SYN2 (1 << IPS_NATCAP_SYN2_BIT)

#define NATCAP_UDP_GET_TYPE(x) (__constant_htons(0x00FF) & (x))
#define NATCAP_UDP_GET_ENC(x) (__constant_htons(0xFF00) & (x))

#define NATCAP_UDP_TYPE1 __constant_htons(0x0001)
#define NATCAP_UDP_TYPE2 __constant_htons(0x0002)
#define NATCAP_UDP_ENC __constant_htons(0x0100)

enum {
	E_NATCAP_OK = 0,
	E_NATCAP_AUTH_FAIL,
	E_NATCAP_INVAL,
};

#endif /* __KERNEL__ */

#define SO_NATCAP_DST 153

static inline int short_test_bit(int nr, const unsigned short *addr)
{
	return 1U & (addr[nr/16] >> (nr & (16-1)));
}

static inline void short_clear_bit(int nr, unsigned short *addr)
{
	unsigned short mask = (1U << ((nr) % 16));
	unsigned short *p = ((unsigned short *)addr) + nr/16;
	*p &= ~mask;
}

static inline void short_set_bit(int nr, unsigned short *addr)
{
	unsigned short mask = (1U << ((nr) % 16));
	unsigned short *p = ((unsigned short *)addr) + nr/16;
	*p |= mask;
}

static inline int short_test_and_set_bit(int nr, unsigned short *addr)
{
	unsigned short mask = (1U << ((nr) % 16));
	unsigned short *p = ((unsigned short *)addr) + nr/16;
	unsigned short old;
	old = *p;
	*p |= mask;
	return (old & mask) != 0;
}

static inline unsigned long ulongdiff(unsigned long a, unsigned long b)
{
	if (a > b)
		return a - b;
	return b - a;
}

static inline unsigned int uintdiff(unsigned int a, unsigned int b)
{
	if (a > b)
		return a - b;
	return b - a;
}

#define MAX_IOCTL_LEN 256

#endif /* _NATCAP_H_ */
