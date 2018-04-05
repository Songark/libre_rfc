/**
* @file sipreason/reason.c  Reason header of SIP 
*
* Copyright (C) 2018 Sqgenius.com
*/

#include <re_types.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_sipreason.h>
#include <re_fmt.h>
#include <string.h>

int sip_getreason_str(struct sipheader_reason* reason, struct mbuf *rpstr)
{
	if (!reason || !rpstr)
		return EINVAL;

	if (reason->type != REASON_PREEMPTION)
		return EINVAL;
	
	int err = mbuf_printf(rpstr, "Reason: preemption; cause=%d; text=", reason->cause);
	switch (reason->cause)
	{
	case REASON_CAUSE_UAPREEMPTION:
		err = mbuf_write_str(rpstr, "\"UA Preemption\"");
		break;
	case REASON_CAUSE_REVRESPREEMPTION:
		err = mbuf_write_str(rpstr, "\"Reserved Resources Preempted\"");
		break;
	case REASON_CAUSE_GENERICPREEMPTION:
		err = mbuf_write_str(rpstr, "\"Generic Preemption\"");
		break;
	case REASON_CAUSE_NONIPPREEMPTION:
		err = mbuf_write_str(rpstr, "\"Non-IP Preemption\"");
		break;
	default:
		break;
	}	
	
	if (!err)
		rpstr->buf[rpstr->end] = 0;

	return err;
}

int sip_getreason_obj(struct mbuf *mbstr, struct sipheader_reason* reason)
{
	if (!mbstr || !mbstr->buf)
		return -1;


	if (!reason)
		reason = malloc(sizeof(struct sipheader_reason));

	if (!reason)
		return -1;

	char * reason_header;
	reason_header = strstr(mbstr->buf, "Reason: ");

	if (!reason_header)
		return -1;

	reason_header = strtok(reason_header, " ;");
	char * reason_type = strtok(NULL, " ;");
	char * reason_cause = strtok(NULL, " ;");

	if (!reason_type || !reason_cause)
		return 1;

	if (!strcmp(reason_type, "preemption"))
		reason->type = REASON_PREEMPTION;
	else
		reason->type = REASON_NOEXIST;

	struct pl cause;
	if (re_regex(reason_cause, str_len(reason_cause), "cause=[0-9]+", &cause))
		return 1;

	reason->cause = (uint16_t)atoi(cause.p);
	return 0;
}

int sip_set_reason(struct sipheader_reason* reason, uint16_t cause)
{
	if (!reason)
		return EINVAL;
	
	if (cause) {
		reason->type = REASON_PREEMPTION;
		reason->cause = cause;
	}
	return 0;
}