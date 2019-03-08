#include "stdio_impl.h"
//include <sys/uio.h>
#include <ontlib/print.h>

size_t __stdio_write(FILE *f, const unsigned char *buf, size_t len)
{
   debug((char*)(f->wbase), f->wpos-f->wbase);
   debug((void*)buf, len);
   return f->wpos-f->wbase + len;
}
