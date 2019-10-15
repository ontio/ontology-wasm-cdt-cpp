#include<ontiolib/ontio.hpp>

using namespace ontio;
using std::string;

class hello : public contract {
public:
	using contract::contract;
	string myhello(string s) {

	}
};

ONTIO_DISPATCH(hello, (myhello))
