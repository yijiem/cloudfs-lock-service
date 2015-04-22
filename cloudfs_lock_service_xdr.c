/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "cloudfs_lock_service.h"

bool_t
xdr_keyname (XDR *xdrs, keyname *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, objp, MAXNAMELEN))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_status (XDR *xdrs, status *objp)
{
	register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_type (XDR *xdrs, type *objp)
{
	register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_lock_params (XDR *xdrs, lock_params *objp)
{
	register int32_t *buf;

	 if (!xdr_keyname (xdrs, &objp->key))
		 return FALSE;
	 if (!xdr_type (xdrs, &objp->operation))
		 return FALSE;
	return TRUE;
}
