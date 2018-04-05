/*
 * This file is part of the relog Library.
 *
 * Copyright (C) 2018 by Songark <arksong123@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __zlog_record_table_h
#define __zlog_record_table_h

#include "zc_defs.h"
#include "record.h"

zc_hashtable_t *zlog_record_table_new(void);
void zlog_record_table_del(zc_hashtable_t * records);
void zlog_record_table_profile(zc_hashtable_t * records, int flag);

#endif
