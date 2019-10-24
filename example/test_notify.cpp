#include<ontiolib/ontio.hpp>
using std::string;
using namespace ontio;

class hello: public contract {
	public:
	using contract::contract;
	void notify_event_test(void) {
		vector<string> addresspool = {"Ab1z3Sxy7ovn4AuScdmMh4PRMvcwCMzSNV", "AW6WWR5xuMpDU5HNoKcta5vm4PUAkByzB5", "Ady2fjRT42bXJBTnNrw1eDPmsVfZ9dHRAB", "AXUpHEB92DDycS7GKjknbF8ZAekskEXkP2", "ASJwxXuVYTJjs2GYVjoJR3W4fyoM7xMfWM", "AdvyrRVzqeR2dQBxB5WrGj7iGKCV93qDP8", "AJxvw1H9zXivoe8mg9toKDQoHDzCHmc4ir", "ATo1YJsYAaLsZKXdnPakgWmhgLdeWFxmFq"};
		vector<address> addr;
		for(auto si : addresspool) {
			addr.push_back(base58toaddress(si));
		}

		string s("hello world");
		//address t = {0xe9,0x8f,0x49,0x98,0xd8,0x37,0xfc,0xdd,0x44,0xa5,0x05,0x61,0xf7,0xf3,0x21,0x40,0xc7,0xc6,0xc2,0x60};
		address t = addr[0];

		bool b = false;
		int64_t a0= 22;
		int32_t a1= 33;
		int128_t a2= 44;
		H256 a3 = current_blockhash();
		std::vector<uint8_t> arg1{0x11,0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc};
		std::tuple<string, string, address, bool, int64_t, int32_t, int128_t, H256, vector<uint8_t>> x("hahahah", s, t, b, a0, a1, a2, a3, arg1);
		std::vector<string> ttt = {"aaa", "bbb", "ccc"};
		std::list<string> ttt0 = {"aaa0", "bbb0", "ccc0"};
		std::deque<int32_t> ttt1 = {99, 98, 97};
		std::set<int32_t> ttt2 = {990, 980, 970};
		address ttt3[] = {addr[0], addr[1], addr[2]};
		int32_t ttt4[] = {1,2,3,4,5};
		std::array<address,2> ttt5 = {addr[3], addr[4]};
		notify_event("transfer",s,t, b, a0, a1, a2, a3, arg1, x, ttt, ttt0, ttt1, ttt2, ttt3, ttt4, ttt5);
		notify_event("transfer", "my name is steven");
		notify_event(s);
		notify_event(t);
		notify_event(b);
		notify_event(a0);
		notify_event(a1);
		notify_event(a2);
		notify_event(a3);
		notify_event(arg1);
		notify_event(x);
		notify_event(ttt);
		notify_event(ttt1);
		notify_event(ttt2);
		notify_event(ttt3);
		notify_event(ttt4);
		notify_event(ttt5);
	}
};

ONTIO_DISPATCH( hello, (notify_event_test))
