/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "types.h"

bool_t
xdr_Request (XDR *xdrs, Request *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, objp, 50))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_Response (XDR *xdrs, Response *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, objp, 50))
		 return FALSE;
	return TRUE;
}
