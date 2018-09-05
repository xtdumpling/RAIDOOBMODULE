/*
 * SHA384 hash implementation and interface functions
 * Copyright (c) 2003-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef SHA384_H
#define SHA384_H

#include <commonheader.h>

//int sha384_init(struct hash_state * md);
void sha384_vector(size_t num_elem, const u8 *addr[], const size_t *len,
                   u8 *mac);
//int sha384_done(struct hash_state * md, unsigned char *out);
#endif /* SHA384_H */
