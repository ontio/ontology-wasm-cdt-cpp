typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned int size_t;
void* malloc(unsigned int);
struct addr {
	unsigned int a;
	unsigned int b;
};

struct addr myaddr(int a, int b, int c)
{
	struct addr t;
	t.a = a + b + c;
	t.b = b + b + c + 1;
	return t;
}

void setmyaddr(struct addr *addr_t)
{
	int a = 9;
	int b = 10;
	addr_t->a = a + b;
	addr_t->b = b - a + 32;
}

unsigned int invoke()
{
	int a = 1;
	int b = 2;
	struct addr *myaddr0 = malloc(sizeof(struct addr));
	setmyaddr(myaddr0);

	if (myaddr0->a == 1)
	{
		return myaddr0->a;
	}
	else
	{
		return myaddr0->b;
	}
	return 0;
}
