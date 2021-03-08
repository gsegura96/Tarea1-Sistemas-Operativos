/*
uini - single header-file INI parser

Copyright (c) 2019 Ethan McTague

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/** Trim whitespace from start and end of string. */
static inline char *uini_trim (char *in) {
  if (!in) return NULL;
  while (isspace(*in)) in++;
  char *end = in + strlen(in) - 1;
  while (end > in && isspace(*end)) end--;
  end[1] = '\0';
  return in;
}

/** uini_parse - simple INI file parser
 *  For each property in the INI file `f`, `handler` is invoked, passing:
 *   - The section (no section = empty string) containing the property.
 *   - The name of the property.
 *   - The value of the property.
 *   - The user pointer passed as `user` to parseINI.
 *  Syntax errors are mostly ignored silently and flexibly.
 *  Failure to provide a value for a property will result in `value` being passed as NULL.
 */
static inline void uini_parse(FILE *f, void (*handler)(const char *section, const char *name, const char *value, void *user), void *user)
{
  char *section = strdup(""), *line = NULL, *clean = NULL, *context = NULL;
  size_t read = 0;

  while (getline(&line, &read, f) > 0) {
    char *clean = uini_trim(strtok_r(line, ";\r\n", &context));
    if (!clean || !*clean) continue;
    if (*clean == '[') {
      free(section);
      section = uini_trim(strdup(strtok_r(++clean, "]", &context)));
    } else {
      const char *key = uini_trim(strtok_r(clean, "=", &context));
      if (!*key) continue;
      handler(section, key, uini_trim(strtok_r(NULL, "\r\n", &context)), user);
    }
  }
  free(section);
  free(line);
}