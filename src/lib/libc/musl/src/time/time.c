#include <time.h>
const char __utc[] = "UTC";

time_t time(time_t *t)
{
	time_t ut = timestamp();
	if (t) *t = ut;
	return ut;
}
