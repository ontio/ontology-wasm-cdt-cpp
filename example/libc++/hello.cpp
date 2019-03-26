#define WASM_LOCAL_DEBUG
#include<ontiolib/ontio.hpp>


using namespace ontio;

class hello : public contract {
public:
	using contract::contract;
	std::string hi(std::string &s) {
		printf("hello world %s\n", s.c_str());
		return s;
	}

	void hii(std::string s) {
		printf("hello world steven %s\n", s.c_str());
	}
};


ONTIO_DISPATCH( hello, (hi)(hii))



extern "C"{

void invoke()
{
	std::string action("hi");
	std::string s("now try string.");
	auto input = pack(action, s);
	save_input_arg(input.data(), input.size());
	apply();
	std::string t;
	get_call_output(t);
	printf("result: %s\n", t.c_str());
}
}
