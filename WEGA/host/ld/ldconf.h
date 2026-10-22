/*
 * ldconf - settings of the host loaders ld, sld and gld from "ld.conf" next to the executable:
 *
 *   root    = /path/to/wega31_root       the file tree of the original system (where /lib/crt0.o is)
 *   libpath = /lib:/usr/lib              the directories of -l (default /lib:/usr/lib:/z/bin)
 *
 * The environment ($WEGA_ROOT, $WEGA_LIBPATH) wins over the file. Format: see ../tools/lib/conf.h.
 */
void ldconf_load(const char *argv0);
