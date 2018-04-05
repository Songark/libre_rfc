/**
* @file re_reason.h  Reason header of SIP message
*
* Copyright (C) 2018 Sqgenius.com
*/

/* Reason Header Define */
struct sipheader_reason {
	uint16_t type;
	uint16_t cause;
};

/* Define Type of reason */
enum reason_type {
	REASON_NOEXIST,
	REASON_PREEMPTION,			
};

/* Define Cause of preemption */
enum reason_cause {
	REASON_CAUSE_INVALID = 0,

	REASON_CAUSE_UAPREEMPTION = 1,
	REASON_CAUSE_REVRESPREEMPTION,
	REASON_CAUSE_GENERICPREEMPTION,
	REASON_CAUSE_NONIPPREEMPTION,	
};

int sip_getreason_str(struct sipheader_reason* reason, struct mbuf *rpstr);
int sip_getreason_obj(struct mbuf *mbstr, struct sipheader_reason* reason);
int sip_set_reason(struct sipheader_reason* reason, uint16_t cause);

