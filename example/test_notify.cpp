#include<ontiolib/ontio.hpp>
using std::string;
using namespace ontio;

class hello: public contract {
	public:
	using contract::contract;
	void notify_string(void) {
		NeoString arg1;
		arg1.s.append("hello world");
		auto notifyargs = make_notify_buffer(arg1);
		notify(notifyargs);

		NeoString arg2;
		arg2.s.append("my name is steven");
		auto notifyargs2 = make_notify_buffer(arg2);
		notify(notifyargs2);
	}

	void notify_int(void) {
		NeoInt<int64_t>  arg1{3446898};
		auto notifyargs = make_notify_buffer(arg1);
		notify(notifyargs);
	}

	void notify_bool(void) {
		NeoBoolean arg1{true};
		auto notifyargs = make_notify_buffer(arg1);
		notify(notifyargs);

		NeoBoolean arg2{false};
		auto notifyargs2 = make_notify_buffer(arg2);
		notify(notifyargs2);
	}

	void notify_bytearray(void) {
		NeoByteArray arg1{0x11,0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc};
		auto notifyargs = make_notify_buffer(arg1);
		notify(notifyargs);
	}

	void notify_address(void) {
		NeoAddress arg1;
		arg1.value = {0xe9,0x8f,0x49,0x98,0xd8,0x37,0xfc,0xdd,0x44,0xa5,0x05,0x61,0xf7,0xf3,0x21,0x40,0xc7,0xc6,0xc2,0x60};
		auto notifyargs = make_notify_buffer(arg1);
		notify(notifyargs);
	}

	void notify_h256(void) {
		NeoH256 arg1;
		arg1.value = current_blockhash();
		auto notifyargs = make_notify_buffer(arg1);
		notify(notifyargs);
	}

	void notify_list(void) {
		NeoAddress arg0;
		arg0.value = {0xe9,0x8f,0x49,0x98,0xd8,0x37,0xfc,0xdd,0x44,0xa5,0x05,0x61,0xf7,0xf3,0x21,0x40,0xc7,0xc6,0xc2,0x60};

		NeoInt<int64_t>  arg1{3446898};
		NeoString arg2;
		arg2.s.append("my name is steven");
		NeoByteArray arg3{0x11,0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc};
		NeoH256 arg4;
		NeoBoolean arg5{true};
		arg4.value = current_blockhash();
		auto notifyargs = make_notify_buffer(arg0, arg1, arg2, arg3, arg4, arg5);
		notify(notifyargs);
	}
};

ONTIO_DISPATCH( hello, (notify_string)(notify_int)(notify_bool)(notify_bytearray)(notify_address)(notify_h256)(notify_list))
