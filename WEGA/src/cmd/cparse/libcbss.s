// libcbss.s: the commons of the libc part in the order of /lib/cparse (__sobuf before __sibuf); linked in front of
// the libc modules. No text, no data.
	.comm	512, __sobuf
	.comm	512, __sibuf
