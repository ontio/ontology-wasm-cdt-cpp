#include<ontiolib/ontio.hpp>
#include<ontiolib/base58.hpp>

using namespace ontio;

class oep4 : public contract {
    key SUPPLY_KEY = make_key("totalsupply");
    address OWNER = base58toaddress("Ab1z3Sxy7ovn4AuScdmMh4PRMvcwCMzSNV");
    asset total = 1000000000;
    std::string balancePrefix = "balance_";
    std::string approvePrefix = "approve_";
    std::string NAME = "WTK";
    std::string SYMBOL = "SYM";

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
        storage_put(make_key(balancePrefix,OWNER),total);
        char buffer [100];
        sprintf(buffer, "%s %s %d","init",OWNER.tohexstring().c_str(),total);
        notify(buffer);
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

    bool transfer(address from, address to , asset amount){
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
        if (frombalance == amount){
            storage_delete(fromkey);
        }else{
            asset tmp = frombalance - amount;
            storage_put(fromkey,tmp);
        }
        asset tmp = tobalance+amount;
        storage_put(tokey,tmp);
        char buffer [128];
        sprintf(buffer, "transfer from %s to %s . amount %d",from.tohexstring().c_str(),to.tohexstring().c_str(),amount);
        notify(buffer);
        notify("transfer accomplised");

        return true;
    }

    bool approve(address from, address to , asset amount){
        check(check_witness(from),"checkwitness failed");
        check(amount > 0,"amount error");
        check(balanceOf(from) >= amount,"amount error");
        storage_put(make_key(approvePrefix,from,"_",to),amount);
        return true;
    }

    asset allowance(address from, address to){
        asset b = 0;
        storage_get(make_key(approvePrefix,from,"_",to),b);
        return b;
    }

    bool transferFrom(address owner, address from,address to, asset amount){
        check(check_witness(owner),"checkwitness failed");
        asset aw = allowance(from,owner);
        check(aw >= amount,"amount error");
        asset frombalance = balanceOf(from);
        check(frombalance >= amount,"amount error");

        key allownancekey = make_key(approvePrefix,from,"_",owner);
        key frombalancekey = make_key(balancePrefix,from);

        if (aw == amount){
            storage_delete(allownancekey);
        }else{
            storage_put(allownancekey, aw - amount);
        }

        if (balanceOf(from) == amount){
            storage_delete(frombalancekey);
        }else{
            storage_put(frombalancekey,frombalance - amount);
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
ONTIO_DISPATCH(oep4, (init)(transfer)(balanceOf)(allowance)(transferFrom)(approve)(name)(symbol)(totalSupply)(decimals)(transferMulti));
