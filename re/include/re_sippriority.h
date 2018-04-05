/**
* @file re_sippriority.h  Resource-Priority header of SIP message
*
* Copyright (C) 2018 Sqgenius.com
*/

/* Resource-Priority Header Define */
struct rl;
struct sipheader_rpriority;
extern struct list accept_rp_list;

enum resource_priority_namespace {
	RESOURCE_PRIORITY_DNS,			// Defense Switched Network
	RESOURCE_PRIORITY_DRNS,			// Defense RED Switched Network
	RESOURCE_PRIORITY_Q735,			// Q.735
	RESOURCE_PRIORITY_ETS,			// Emergency Telecommunications Service
	RESOURCE_PRIORITY_WPS,			// Wireless Priority Service
};

struct sipheader_rpriority* sip_getpriority_ptr(const char *rp, uint16_t order);
int sip_getpriority_str(struct sipheader_rpriority* rp, struct mbuf *rpstr);
bool sip_bigger_than_first_rp(struct sipheader_rpriority * first, struct sipheader_rpriority * second);
void sip_accept_rplist_init();
int sip_accept_rplist_append(char *rp, uint16_t order);
bool sip_acceptable_rpvalue(struct sipheader_rpriority * rp);
int sip_get_acceptable_rpvalue(struct mbuf * acceptable_rp_buf);
uint16_t sip_rplist_order(struct sipheader_rpriority* rp);