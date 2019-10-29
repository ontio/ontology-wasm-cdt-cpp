#include<ontiolib/ontio.hpp>

using namespace ontio;
using std::vector;


class oep4 : public contract {
    key SUPPLY_KEY = make_key("totalsupply");
    asset total = 1000000000;
    std::string balancePrefix = "balance_";
    std::string approvePrefix = "approve_";

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
        check(check_witness(OWNER),"checkwitness failed");
        asset totalsupply = 0;

        if (storage_get(SUPPLY_KEY,totalsupply)){
            return false;
        }

        storage_put(SUPPLY_KEY,total);

        asset amount = 0;
		storage_get(SUPPLY_KEY, amount);
		amount = 0;

        storage_put(make_key(balancePrefix,OWNER),total);

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
        check(check_witness(from),"checkwitness failed");
        check(amount > 0,"amount should greater than 0");
        if (from == to){
            return true;
        }
        asset frombalance = balanceOf(from);
        asset tobalance = balanceOf(to);

        key fromkey = make_key(balancePrefix,from);
        key tokey = make_key(balancePrefix,to);

        check(frombalance >= amount,"amount error");
        if (frombalance == amount) {
            storage_delete(fromkey);
        } else {
            asset tmp = frombalance - amount;
            storage_put(fromkey,tmp);
        }
        asset tmp = tobalance+amount;
        storage_put(tokey,tmp);

        tobalance = balanceOf(to);

		notify_event(std::tuple<string, address, address, asset>("transfer", from, to, amount ));

        return true;
    }

    bool approve(address owner, address spender , asset amount) {
        check(check_witness(owner),"checkwitness failed");
        check(amount > 0,"amount error");
        check(balanceOf(owner) >= amount,"amount error");
        storage_put(make_key(approvePrefix,owner,"_",spender),amount);

		notify_event(std::tuple<string, address, address, asset>("transfer", owner, spender, amount));
        return true;
    }

    asset allowance(address from, address to) {
        asset b = 0;
        storage_get(make_key(approvePrefix,from,"_",to),b);
        return b;
    }

    bool transferFrom(address owner, address from,address to, asset amount){
        check(check_witness(owner),"checkwitness failed");
        asset aw = allowance(from,to);
        check(aw >= amount,"amount error");
        asset frombalance = balanceOf(from);
        check(frombalance >= amount,"amount error");

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
            check(transfer(p.from, p.to, p.amount),"tranfer failed");
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
