#define WASM_LOCAL_DEBUG
#include<ontiolib/ontio.hpp>
#include<ontiolib/base58.hpp>

using namespace ontio;
using std::vector;


class oep4 : public contract {
    key SUPPLY_KEY = make_key("totalsupply");
    asset total = 1000000000;
    std::string balancePrefix = "balance_";
    std::string approvePrefix = "approve_";
	//uint8_t balancePrefix = 0xaa;
	//uint8_t approvePrefix = 0xbb;

    std::string NAME = "WTK";
    std::string SYMBOL = "SYM";
	address OWNER = base58toaddress("Ad4pjz2bqep4RhQrUAzMuZJkBC3qJ1tZuT");

    int DECIMALS = 9;

    struct transferM {
        address from;
        address to;
        asset amount;
        ONTLIB_SERIALIZE( transferM,  (from)(to)(amount) )
    };
	using mul_trans_args = std::vector<struct transferM>;

    public:
    using contract::contract;

    bool init(){
        ontio_assert(check_witness(OWNER),"checkwitness failed");
        asset totalsupply = 0;

        if (storage_get(SUPPLY_KEY,totalsupply)){
            return false;
        }

        storage_put(SUPPLY_KEY,total);

        asset amount = 0;
		storage_get(SUPPLY_KEY, amount);
		printf("total: %lld\n", amount);
		amount = 0;

        storage_put(make_key(balancePrefix,OWNER),total);
        char buffer [100];
        std::sprintf(buffer, "init %s %s %lld\n","",OWNER.tohexstring().c_str(),total);
        notify(buffer);

        if(storage_get(make_key(balancePrefix,OWNER),amount))
			printf("balanceOf address %s : %lld\n",OWNER.tohexstring().c_str() , amount);
		else
			printf("get nothing\n");

        return true;
    }

    std::string name(){
        return NAME;
    }

    std::string symbol(){
        return SYMBOL;
    }

    asset totalSupply(){
        return total;
    }

    int decimals(){
        return DECIMALS;
    }

    bool transfer(address from, address to , asset amount) {
        ontio_assert(check_witness(from),"checkwitness failed");
        ontio_assert(amount > 0,"amount should greater than 0");
        if (from == to){
            return true;
        }
        asset frombalance = balanceOf(from);
        asset tobalance = balanceOf(to);

        key fromkey = make_key(balancePrefix,from);
        key tokey = make_key(balancePrefix,to);

        ontio_assert(frombalance >= amount,"amount error");
        if (frombalance == amount) {
            storage_delete(fromkey);
        } else {
            asset tmp = frombalance - amount;
            storage_put(fromkey,tmp);
        }
        asset tmp = tobalance+amount;
        storage_put(tokey,tmp);
        printf("transfer from %s to %s amount %lld\n",from.tohexstring().c_str(),to.tohexstring().c_str(),amount);

        tobalance = balanceOf(to);
		printf("to %s, balance: %lld\n", to.tohexstring().c_str(), tobalance);

        return true;
    }

    bool approve(address from, address to , asset amount){
        ontio_assert(check_witness(from),"checkwitness failed");
        ontio_assert(amount > 0,"amount error");
        ontio_assert(balanceOf(from) >= amount,"amount error");
        storage_put(make_key(approvePrefix,from,"_",to),amount);
        return true;
    }

    asset allowance(address from, address to){
        asset b = 0;
        storage_get(make_key(approvePrefix,from,"_",to),b);
        return b;
    }

    bool transferFrom(address owner, address from,address to, asset amount){
        ontio_assert(check_witness(owner),"checkwitness failed");
        asset aw = allowance(from,to);
        ontio_assert(aw >= amount,"amount error");
        asset frombalance = balanceOf(from);
        ontio_assert(frombalance >= amount,"amount error");

        key allownancekey = make_key(approvePrefix,from,"_",to);
        key frombalancekey = make_key(balancePrefix,from);
        if (aw == amount){
            storage_delete(allownancekey);
        }else{
            asset tmp = frombalance - amount;

            storage_put(allownancekey,tmp);
        }
        if (balanceOf(from) == amount){
            storage_delete(frombalancekey);
        }else{
            asset tmp = frombalance - amount;

            storage_put(frombalancekey,tmp);
        }
        asset tmp = balanceOf(to) + amount;
        storage_put(make_key(balancePrefix,to),tmp);
        return true;

    }

    bool transferMulti(mul_trans_args params){
        for (struct transferM p :params){
            ontio_assert(transfer(p.from, p.to, p.amount),"tranfer failed");
        }
        return true;
    }

    asset balanceOf(address acct){
        asset b = 0;
        storage_get(make_key(balancePrefix,acct),b);
        return b;
    }
};
ONTIO_DISPATCH(oep4, (init)(transfer)(balanceOf)(allowance)(transferFrom)(approve)(name)(symbol)(totalSupply)(decimals)(transferMulti))

#ifdef WASM_LOCAL_DEBUG
void call_init() {
	/*prepare method and args.*/
	string action("init");
	auto v = pack(action);
	set_input(v);

	/*call contract test.*/
	apply();
}

asset call_balanceOf(const address &acct) {
	string action("balanceOf");
	auto v = pack(action, acct);
	set_input(v);

	apply();

	asset amount;
	asset *test = &amount;
	get_call_output(*test);
	printf("balanceOf address %s : %lld\n",acct.tohexstring().c_str() , amount.amount);
	return amount;
}

void call_transfer(address &from , address &to, asset &amount) {
	string action("transfer");
	auto v = pack(action, from, to, amount);
	set_input(v);
	apply();
}

extern "C" void invoke(void) {
	asset sum = 0;
	address OWNER = base58toaddress("Ad4pjz2bqep4RhQrUAzMuZJkBC3qJ1tZuT");
	call_init();

	auto amount0 = call_balanceOf(OWNER);
	check(amount0 == 1000000000, "init error");

	vector<string> s = {"Ab1z3Sxy7ovn4AuScdmMh4PRMvcwCMzSNV", "AW6WWR5xuMpDU5HNoKcta5vm4PUAkByzB5", "Ady2fjRT42bXJBTnNrw1eDPmsVfZ9dHRAB", "AXUpHEB92DDycS7GKjknbF8ZAekskEXkP2", "ASJwxXuVYTJjs2GYVjoJR3W4fyoM7xMfWM", "AdvyrRVzqeR2dQBxB5WrGj7iGKCV93qDP8", "AJxvw1H9zXivoe8mg9toKDQoHDzCHmc4ir", "ATo1YJsYAaLsZKXdnPakgWmhgLdeWFxmFq"};
	for(auto si : s) {
		address t = base58toaddress(si);
		asset amount = 9782;
		call_transfer(OWNER, t, amount);
		sum += amount;
	}

	for(auto si :s) {
		auto amount = call_balanceOf(base58toaddress(si));
		check(amount == 9782, "tranfer failed");
	}


	auto amount1 = call_balanceOf(OWNER);
	check(amount0 == (amount1 + sum), "should be equal\n");


	for(auto si : s) {
		address t = base58toaddress(si);
		asset amount = 9782;
		call_transfer(t, OWNER, amount);
	}

	for(auto si: s) {
		auto amount = call_balanceOf(base58toaddress(si));
		check(amount == 0, "transfer back failed. each");
	}


	auto back_amount = call_balanceOf(OWNER);
	check(back_amount == 1000000000, "transfer back failed. Owner");
}
#endif
