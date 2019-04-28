#include<ontiolib/ontio.hpp>

using namespace ontio;
using std::string;

class hello : public contract {
public:
	using contract::contract;
	string myhello(string s) {
		return s + " hello world, my name is steven";
	}
};

ONTIO_DISPATCH(hello, (myhello))
