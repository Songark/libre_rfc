/**
* @file sippriority/respriority.c  Resource-Priority header of SIP 
*
* Copyright (C) 2018 Sqgenius.com
*/

#include <re_types.h>
#include <re_list.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_sippriority.h>
#include <re_fmt.h>
#include <string.h>

struct list accept_rp_list;
/** Defines a priority string type */
struct rl {
	char *p;		/**< Pointer to string */
	size_t l;       /**< Length of string  */
};

/** Defines a priority value type */
struct sipheader_rpriority {
	struct le le;       /**< Linked list element                  */
	struct rl namesp;	/** Namespace of Resource-Priority header*/
	struct rl rvalue;	/** R-value of Resource-Priority header*/
	uint16_t order;
};

static void rpbuf_destructor(void *data)
{
	struct sipheader_rpriority *rp = data;

	if (rp) {
		mem_deref(rp->namesp.p);
		mem_deref(rp->rvalue.p);
	}
}

/** 
	Function for getting rp in string format from structure object's pointer.

	Params:
		rp - (in) resource-priority in structured object's pointer.
		rpstr - (out) resource-priority in string format.

	Return value:
		if Success, return Zero,
		else, return Non-zero. 
*/
int sip_getpriority_str(struct sipheader_rpriority* rp, struct mbuf *rpstr)
{
	int err = 0;

	if (!rp || !rpstr)
		return EINVAL;
	
	err |= mbuf_printf(rpstr, "%s.%s\r\n", rp->namesp.p, rp->rvalue.p);
	
	if (!err)
		rpstr->buf[rpstr->end] = 0;

	return err;
}

/**
	Function for getting rp in structure object's pointer from string format.

	Params:
		rp - (in) resource-priority in string format
		
	Return value:
		if Success, return rp in structure object's pointer.
		else, return NULL.
*/
struct sipheader_rpriority* sip_getpriority_ptr(const char *rp, uint16_t order)
{
	char *ns, *rpriority;

	if (!rp)
		return NULL;

	char * sbuff = malloc(strlen(rp) + 1);
	strcpy(sbuff, rp);

	ns = strtok(sbuff, ".");
	rpriority = strtok(NULL, ".");

	if (!ns || !rpriority) {
		free(sbuff);
		return NULL;
	}
		
	struct sipheader_rpriority * respriority;

	respriority  = mem_zalloc(sizeof(*respriority), rpbuf_destructor);
	if (!respriority) {
		free(sbuff);
		return NULL;
	}		
	
	respriority->namesp.l = strlen(ns) + 1;
	respriority->namesp.p = malloc(respriority->namesp.l);
	strcpy(respriority->namesp.p, ns);

	respriority->rvalue.l = strlen(rpriority) + 1;
	respriority->rvalue.p = malloc(respriority->rvalue.l);
	strcpy(respriority->rvalue.p, rpriority);

	respriority->order = order;

	free(sbuff);
	return respriority;
}

/**
	Function for compareing whether second rp is bigger than first rp, or not.

	Params:
		first - first rp
		second - second rp

	Return value:
		if second is bigger than first, return true,
		else, return false.

	For example, 
		(dns.2, dns.3) -> return false
		(dns.3, dns.1) -> return true

	In additional, this function will handle Multiple Concurrent Namespaces.
	For example, 
		Foo.3        Foo.3       Bar.C    (highest priority)
		Foo.2        Bar.C       Foo.3
		Foo.1   or   Foo.2   or  Foo.2
		Bar.C        Bar.B       Foo.1
		Bar.B        Foo.1       Bar.B
		Bar.A        Bar.A       Bar.A    (lowest priority)
*/
bool sip_bigger_than_first_rp(struct sipheader_rpriority * first, struct sipheader_rpriority * second)
{
	if (!first || !second)
		false;

	uint16_t first_order = sip_rplist_order(first);
	uint16_t second_order = sip_rplist_order(second);

	if (first_order > second_order)
		return true;

	return false;
}

void sip_accept_rplist_init()
{
	struct list * rplist = &accept_rp_list;
	list_init(rplist);
}

int sip_accept_rplist_append(char *rp, uint16_t order)
{
	if (!rp)
		return EINVAL;

	struct sipheader_rpriority* rpentity = sip_getpriority_ptr(rp, order);
	if (!rpentity)
		return EINVAL;

	list_unlink(&rpentity->le);
	list_append(&accept_rp_list, &rpentity->le, rpentity);

	return 0;
}

uint16_t sip_rplist_order(struct sipheader_rpriority* rp)
{
	if (!rp)
		return 0;

	struct le *le;
	for (le = list_head(&accept_rp_list); le; le = le->next) {
		struct sipheader_rpriority *active_rp = le->data;

		if (!active_rp)
			continue;

		if (strcmp(active_rp->namesp.p, rp->namesp.p) == 0 &&
			strcmp(active_rp->rvalue.p, rp->rvalue.p) == 0)
			return active_rp->order;
	}

	return 0;
}

/**
Function for checking valid R-P value from Accept-Resource-Priority value list.

Params:
	rp - (in) Resource-Priority value for checking valid or invalid format.

Return value:
	if rp format is valid, return Zero,
	else, return Non-zero.
*/
bool sip_acceptable_rpvalue(struct sipheader_rpriority * rp)
{
	if (!rp)
		false;

	struct le *le;
	for (le = list_head(&accept_rp_list); le; le = le->next) {
		struct sipheader_rpriority *active_rp = le->data;

		if (!active_rp)
			continue;

		if (strcmp(active_rp->namesp.p, rp->namesp.p) == 0 &&
			strcmp(active_rp->rvalue.p, rp->rvalue.p) == 0)
			return true;
	}

	return false;
}

/**
Function for getting Accept-Resource-Priority value list in string format in order to response 417 message.

Params:
	acceptable_rp_buf - (out) Accept-Resource-Priority list in string format.

Return value:
	if Success, return Zero,
	else, return Non-zero.
*/
int sip_get_acceptable_rpvalue(struct mbuf * acceptable_rp_buf)
{
	if (!acceptable_rp_buf)
		return ENOMEM;

	mbuf_write_str(acceptable_rp_buf, "\r\nAccept-Resource-Priority: ");

	struct le *le;
	for (le = list_head(&accept_rp_list); le; le = le->next) {
		struct sipheader_rpriority *active_rp = le->data;

		if (!active_rp)
			continue;

		mbuf_write_str(acceptable_rp_buf, active_rp->namesp.p);
		mbuf_write_str(acceptable_rp_buf, ".");
		mbuf_write_str(acceptable_rp_buf, active_rp->rvalue.p);

		if (le->next)
			mbuf_write_str(acceptable_rp_buf, ",");
	}

	acceptable_rp_buf->buf[acceptable_rp_buf->end] = 0;
	acceptable_rp_buf->pos = 0;

	return 0;
}