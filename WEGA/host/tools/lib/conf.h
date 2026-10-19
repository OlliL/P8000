/*
 * conf - reader for the small configuration files of the host tools
 * (cc.conf next to cc, later scc.conf, ...).
 *
 * File format, one setting per line:
 *
 *   # comment (also after a value, when preceded by a blank)
 *   key = value
 *   key = "value with # and leading blanks"
 *   _root = /w
 *   cas = ${_root}/bin/cas      # ${name}: an earlier key, else the environment
 *
 * Keys are letters, digits, '_' and '.'.  A later line overrides an earlier one.
 * Keys that begin with '_' are private variables for ${...}; conf_apply ignores them.
 * Blanks around key and value are removed.  Errors are reported with file name
 * and line number.
 */
#ifndef CONF_H
#define CONF_H

struct conf;

/* a program variable that a key sets */
struct conf_var {
	const char	*key;
	char		**var;		/* set to a malloc'ed copy of the value */
};

/*
 * Path of the configuration file that belongs to the running program: the
 * directory of the executable (/proc/self/exe, else argv0) plus "/<ext>".
 * Returns a malloc'ed string, 0 when the directory cannot be found.
 */
char *conf_sibling(const char *argv0, const char *name);

/*
 * Read the file.  Returns 0 with errno ENOENT when the file does not exist (the
 * callers then keep their defaults) and 0 with errno EINVAL after a syntax error
 * (messages are already printed on stderr).
 */
struct conf *conf_read(const char *path);

/* value of a key, 0 when not set */
const char *conf_get(const struct conf *c, const char *key);

/*
 * Set the program variables of vars (terminated by a 0 key) from the file.
 * Keys of the file that are not in vars are reported on stderr.  Returns the
 * number of such keys.
 */
int conf_apply(const struct conf *c, const struct conf_var *vars);

void conf_free(struct conf *c);

#endif
