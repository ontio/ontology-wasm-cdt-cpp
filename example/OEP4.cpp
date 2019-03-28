#include<ontiolib/ontio.hpp>
#include<ontiolib/base58.hpp>

using namespace ontio;

class oep4 : public contract {
    key SUPPLY_KEY = make_key("totalsupply");
    address OWNER = Base58toaddress("Ad4pjz2bqep4RhQrUAzMuZJkBC3qJ1tZuT");
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

    public:
    using contract::contract;
    bool init(){
        ontio_assert(check_witness(OWNER),"checkwitness failed");
        asset totalsupply = 0;
        if (storage_get(SUPPLY_KEY,totalsupply)){
            return false;
        }

        storage_put(SUPPLY_KEY,total);
        storage_put(make_key(balancePrefix,OWNER),total);
        char buffer [100];
        std::sprintf(buffer, "%s %s %d","",OWNER,total);
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
        if (frombalance == amount){
            storage_delete(fromkey);
        }else{
            asset tmp = frombalance - amount;
            storage_put(fromkey,tmp);
        }
        asset tmp = tobalance+amount;
        storage_put(tokey,tmp);
        char buffer [100];
        std::sprintf(buffer, "%s %s %d",from,to,amount);
        notify(buffer);
        notify("transfer accomplised");

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

    bool transferMulti(std::vector<struct transferM> params){
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
ONTIO_DISPATCH(oep4, (init)(transfer)(balanceOf)(allowance)(transferFrom)(approve)(name)(symbol)(totalSupply)(decimals));