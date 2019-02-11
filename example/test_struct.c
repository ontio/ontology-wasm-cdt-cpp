typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned int size_t;
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

unsigned int invoke()
{
	int a = 1;
	int b = 2;
	struct addr myaddr0 = myaddr(a, b, 1);
	if (myaddr0.a == 1)
	{
		return myaddr0.a;
	}
	else
	{
		return myaddr0.b;
	}
	return 0;
}
