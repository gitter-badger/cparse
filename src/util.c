#include "config.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <cparse/util.h>
#include <cparse/json.h>
#include <cparse/object.h>
#include "log.h"
#include "protocol.h"
#include "private.h"

inline int cparse_str_empty(const char *str)
{
    return !str || !*str;
}

time_t cparse_date_time(const char *str)
{
    int y, M, d, h, m;
    float s;
    int tzh, tzm;
    struct tm time;

    if (8 > sscanf(str, "%d-%d-%dT%d:%d:%f%d:%dZ", &y, &M, &d, &h, &m, &s, &tzh, &tzm))
    {
        sscanf(str, "%d-%d-%dT%d:%d:%fZ", &y, &M, &d, &h, &m, &s);
    }

    time.tm_year = y - 1900; /* Year since 1900 */
    time.tm_mon = M - 1;     /* 0-11 */
    time.tm_mday = d;        /* 1-31 */
    time.tm_hour = h;        /* 0-23 */
    time.tm_min = m;         /* 0-59 */
    time.tm_sec = (int) s;    /* 0-61 */
    time.tm_isdst = 0;     /* auto check daylight savings time */

    return mktime(&time) - timezone;
}

void cparse_replace_str(char **a, const char *b)
{
    if (!a) { return; }

    if (*a) {
        free(*a);
    }

    (*a) = strdup(b);
}

inline int cparse_str_cmp(const char *a, const char *b)
{
    if (cparse_str_empty(a) || cparse_str_empty(b)) { return 0; }

    return strcmp(a, b);
}

int cparse_str_prefix(const char *astr, const char *bstr)
{
    if (astr == NULL || !*astr)
    {
        return 1;
    }
    if (bstr == NULL)
    {
        return 1;
    }
    for (; *astr; astr++, bstr++)
    {
        if (*astr != *bstr) {
            return 1;
        }
    }

    return 0;

}

void cparse_json_add_reference(cParseJson *data, cParseObject *ref)
{
    if (!data || !ref) {
        cparse_log_errno(EINVAL);
        return;
    }

    /* set type to pointer */
    cparse_json_set_string(data, CPARSE_KEY_TYPE, CPARSE_TYPE_POINTER);

    /* add class name */
    if (!cparse_str_empty(ref->className))
    {
        cparse_json_set_string(data, CPARSE_KEY_CLASS_NAME, ref->className);
    }

    /* add object id */
    if (!cparse_str_empty(ref->objectId)) {
        cparse_json_set_string(data, CPARSE_KEY_OBJECT_ID, ref->objectId);
    }

}
