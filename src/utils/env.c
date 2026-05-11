#include "env.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
void set_env(const char *name, const char *value)
{
    char buf[512];
    snprintf(buf, sizeof(buf), "%s=%s", name, value);
    _putenv(buf);
}
#else
void set_env(const char *name, const char *value)
{
    setenv(name, value, 1);
}
#endif