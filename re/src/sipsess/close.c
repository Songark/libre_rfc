/**
 * @file close.c  SIP Session Close
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <re_types.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_sa.h>
#include <re_list.h>
#include <re_hash.h>
#include <re_fmt.h>
#include <re_uri.h>
#include <re_tmr.h>
#include <re_msg.h>
#include <re_sip.h>
#include <re_sipsess.h>
#include <re_sipreason.h>
#include "sipsess.h"


static void bye_resp_handler(int err, const struct sip_msg *msg, void *arg)
{
	struct sipsess *sess = arg;

	if (err || sip_request_loops(&sess->ls, msg->scode))
		goto out;

	if (msg->scode < 200) {
		return;
	}
	else if (msg->scode < 300) {
		;
	}
	else {
		if (sess->peerterm)
			goto out;

		switch (msg->scode) {

		case SIP_RESP_UNAUTHORIZED:
		case SIP_RESP_PROXY_AUTH_REQUIRED:
			err = sip_auth_authenticate(sess->auth, msg);
			if (err)
				break;

			err = sipsess_bye(sess, false);
			if (err)
				break;

			return;
		}
	}

 out:
	mem_deref(sess);
}


int sipsess_bye(struct sipsess *sess, bool reset_ls)
{
	if (sess->req)
		return EPROTO;

	if (reset_ls)
		sip_loopstate_reset(&sess->ls);

	struct mbuf * reasonmb = NULL;
	if (sess->reason && sess->reason->type != REASON_NOEXIST) {
		reasonmb = mbuf_alloc(256);

		int err = sip_getreason_str(sess->reason, reasonmb);
		if (!err) {
			mbuf_write_str(reasonmb, "\r\n");
			reasonmb->pos = 0;
			reasonmb->buf[reasonmb->end] = 0;
		}
	}
	
	int err = sip_drequestf(&sess->req, sess->sip, true, "BYE",
			     sess->dlg, 0, sess->auth,
			     NULL, bye_resp_handler, sess,
			     "%s%s"
			     "Content-Length: 0\r\n"
			     "\r\n",
				 reasonmb ? mbuf_buf(reasonmb) : NULL,
			     sess->close_hdrs);

	if (reasonmb)
		mem_deref(reasonmb);

	return err;
}
