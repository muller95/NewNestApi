#ifndef NA_ERRNO_H
#define NA_ERRNO_H

enum NA_ERRNO {
	NA_SUCCESS,
	NA_ENOMEM,
	NA_EINVAL,
	NA_EFIGFORMAT
};

extern enum NA_ERRNO na_errno;

#endif
