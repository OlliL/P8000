/* conf - see conf.h */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include "conf.h"

struct entry {
	char		*key;
	char		*val;
	int		line;
	struct entry	*next;
};

struct conf {
	char		*path;
	struct entry	*head, *tail;
};

static char *
xstrdup(const char *s)
{
	char *p = malloc(strlen(s) + 1);

	if (p == 0) {
		fprintf(stderr, "conf: out of memory\n");
		exit(1);
	}
	return strcpy(p, s);
}

char *
conf_sibling(const char *argv0, const char *name)
{
	char exe[PATH_MAX], *slash, *path;
	ssize_t n;

	n = readlink("/proc/self/exe", exe, sizeof exe - 1);
	if (n > 0)
		exe[n] = 0;
	else if (argv0 && strlen(argv0) < sizeof exe)
		strcpy(exe, argv0);
	else
		return 0;
	slash = strrchr(exe, '/');
	if (slash)
		*slash = 0;
	else
		strcpy(exe, ".");
	path = malloc(strlen(exe) + strlen(name) + 2);
	if (path)
		sprintf(path, "%s/%s", exe, name);
	return path;
}

const char *
conf_get(const struct conf *c, const char *key)
{
	const struct entry *e;
	const char *v = 0;

	for (e = c->head; e; e = e->next)
		if (strcmp(e->key, key) == 0)
			v = e->val;		/* the last one wins */
	return v;
}

/* append the expansion of name (len bytes) to buf; 0 when undefined */
static int
expand(const struct conf *c, const char *name, int len, char **buf, int *n, int *cap)
{
	char key[128];
	const char *v;
	int l;

	if (len <= 0 || len >= (int)sizeof key)
		return 0;
	memcpy(key, name, len);
	key[len] = 0;
	v = conf_get(c, key);
	if (v == 0)
		v = getenv(key);
	if (v == 0)
		return 0;
	l = strlen(v);
	while (*n + l + 1 > *cap)
		*buf = realloc(*buf, *cap *= 2);
	memcpy(*buf + *n, v, l);
	*n += l;
	return 1;
}

/* the value part of a line: quotes, trailing comment, ${name} */
static char *
value(const struct conf *c, const char *s, const char *path, int line)
{
	char *buf;
	int n = 0, cap = 64, quoted = 0;

	buf = malloc(cap);
	if (buf == 0)
		return 0;
	if (*s == '"') {
		quoted = 1;
		s++;
	}
	for (; *s; s++) {
		if (quoted && *s == '"') {
			quoted = 2;
			s++;
			break;
		}
		if (!quoted && *s == '#' && n > 0 && isspace((unsigned char)s[-1]))
			break;
		if (*s == '$' && s[1] == '{') {
			const char *e = strchr(s + 2, '}');

			if (e == 0 || !expand(c, s + 2, e - s - 2, &buf, &n, &cap)) {
				fprintf(stderr, "%s:%d: bad or undefined ${...}\n", path, line);
				free(buf);
				return 0;
			}
			s = e;
			continue;
		}
		if (n + 2 > cap)
			buf = realloc(buf, cap *= 2);
		buf[n++] = *s;
	}
	if (quoted == 1) {
		fprintf(stderr, "%s:%d: missing closing quote\n", path, line);
		free(buf);
		return 0;
	}
	if (quoted == 2) {
		while (isspace((unsigned char)*s))
			s++;
		if (*s && *s != '#') {
			fprintf(stderr, "%s:%d: text after closing quote\n", path, line);
			free(buf);
			return 0;
		}
	} else
		while (n > 0 && isspace((unsigned char)buf[n - 1]))
			n--;
	buf[n] = 0;
	return buf;
}

struct conf *
conf_read(const char *path)
{
	FILE *fp;
	struct conf *c;
	char line[2048];
	int ln = 0, bad = 0;

	fp = fopen(path, "r");
	if (fp == 0)
		return 0;
	c = calloc(1, sizeof *c);
	c->path = xstrdup(path);
	while (fgets(line, sizeof line, fp)) {
		char *s = line, *k, *ke, *eq, *v;
		struct entry *e;

		ln++;
		if (strchr(line, '\n') == 0 && !feof(fp)) {
			fprintf(stderr, "%s:%d: line too long\n", path, ln);
			bad = 1;
			break;
		}
		while (isspace((unsigned char)*s))
			s++;
		if (*s == 0 || *s == '#')
			continue;
		eq = strchr(s, '=');
		if (eq == 0) {
			fprintf(stderr, "%s:%d: expected 'key = value'\n", path, ln);
			bad = 1;
			continue;
		}
		for (k = s; k < eq && (isalnum((unsigned char)*k) || *k == '_' || *k == '.'); k++)
			;
		ke = k;
		while (k < eq && isspace((unsigned char)*k))
			k++;
		if (ke == s || k != eq) {
			fprintf(stderr, "%s:%d: bad key\n", path, ln);
			bad = 1;
			continue;
		}
		*ke = 0;
		eq++;
		while (isspace((unsigned char)*eq))
			eq++;
		v = value(c, eq, path, ln);
		if (v == 0) {
			bad = 1;
			continue;
		}
		e = calloc(1, sizeof *e);
		e->key = xstrdup(s);
		e->val = v;
		e->line = ln;
		if (c->tail)
			c->tail->next = e;
		else
			c->head = e;
		c->tail = e;
	}
	fclose(fp);
	if (bad) {
		conf_free(c);
		errno = EINVAL;
		return 0;
	}
	return c;
}

int
conf_apply(const struct conf *c, const struct conf_var *vars)
{
	const struct entry *e;
	const struct conf_var *v;
	int unknown = 0;

	for (e = c->head; e; e = e->next) {
		for (v = vars; v->key; v++)
			if (strcmp(v->key, e->key) == 0)
				break;
		if (v->key == 0 && e->key[0] != '_') {
			fprintf(stderr, "%s:%d: unknown key '%s'\n", c->path, e->line, e->key);
			unknown++;
		}
	}
	for (v = vars; v->key; v++) {
		const char *val = conf_get(c, v->key);

		if (val)
			*v->var = xstrdup(val);
	}
	return unknown;
}

void
conf_free(struct conf *c)
{
	struct entry *e, *n;

	for (e = c->head; e; e = n) {
		n = e->next;
		free(e->key);
		free(e->val);
		free(e);
	}
	free(c->path);
	free(c);
}
