#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *f = NULL;

static void flush_on_signal(int sig) {
  if (f) {
    fflush(f);
  }
  fflush(stdout);
  fflush(stderr);
  signal(sig, flush_on_signal);
}

__attribute__((constructor)) static void init_logging(void) {
  const char *path = "trace.log";
  f = fopen(path, "w");
  if (!f) {
    exit(1);
  }
  static char buf[1 << 20];
  setvbuf(f, buf, _IOFBF, sizeof(buf));
  signal(SIGINT, flush_on_signal);
  signal(SIGTERM, flush_on_signal);
}

__attribute__((destructor)) static void fini_logging(void) {
  if (f)
    fclose(f);
  f = NULL;
}

void logEdge(const char *user, const char *operand) {
  fputs(user, f);
  fputs("<-", f);
  fputs(operand, f);
  fputc('\n', f);
}
