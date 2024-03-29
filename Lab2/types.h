/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _TYPES_H_RPCGEN
#define _TYPES_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef char *Request;

typedef char *Response;

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Request (XDR *, Request*);
extern  bool_t xdr_Response (XDR *, Response*);

#else /* K&R C */
extern bool_t xdr_Request ();
extern bool_t xdr_Response ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_TYPES_H_RPCGEN */
