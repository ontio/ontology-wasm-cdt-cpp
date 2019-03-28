#include<ontiolib/ontio.hpp>
#include<ontiolib/base58.hpp>

using namespace ontio;

class oep5 : public contract {
    address OWNER = Base58toaddress("Ad4pjz2bqep4RhQrUAzMuZJkBC3qJ1tZuT");
    std::string NAME = "OEP5 WASM";
    std::string SYMBOL = "OEP5 SYMBOL";
    std::string BALANCE_PREFIX = "balance_";
    std::string APPROVE_PREFIX = "approve_";

    std::string OWNER_PREFIX = "owner_";
    std::string TK_PREFIX = "token_";
    std::string TOTALSUPPLY = "totalsupply";

    struct transferM {
        address to;
        uint64_t tokenID;
        ONTLIB_SERIALIZE( transferM,  (to)(tokenID) )
    };

    struct tokenStruct{
        std::string name;
        std::string image;
        std::string description;
        std::string othermeta;
        ONTLIB_SERIALIZE( tokenStruct,  (name)(image)(description)(othermeta) )
    };
    public:
    using contract::contract;
    //name function
    std::string name(){
        return NAME;
    }

    //symbol function
    std::string symbol(){
        return SYMBOL;
    }

    //total supply function
    uint64_t totalSupply(){
        uint64_t b = 0;
        storage_get(make_key(TOTALSUPPLY),b);
        return b;
    }

    //balance of function
    asset balanceOf(address acct){
        asset b = 0;
        storage_get(make_key(BALANCE_PREFIX,acct),b);
        return b;
    }

    //owner of function
    address ownerOf(uint64_t tokenid){
        address b ={};
        storage_get(make_key(OWNER_PREFIX,std::to_string(tokenid)),b);
        return b;
    }

    bool init(){
        uint64_t t = 0;
        storage_get(make_key(TOTALSUPPLY),t);
        if (t > 0){
            return false;
        }
        storage_put(make_key(TOTALSUPPLY),0);
        return true;
    }

    //transfer function
    bool transfer(address to , uint64_t tokenid){
        address oldowner;
        key tokenkey = make_key(OWNER_PREFIX ,std::to_string(tokenid));
        storage_get(tokenkey,oldowner);
        ontio_assert(check_witness(oldowner),"checkwitness failed!");
        storage_put(tokenkey,to);
        return true;
    }

    //transferMulti function
    bool transferMulti(std::vector<struct transferM> params){
        for (struct transferM p :params){
            ontio_assert(transfer(p.to, p.tokenID),"tranfer failed");
        }
        return true;
    }

    bool approve(address to,std::string tokenid){
        address oldowner = {};
        key tokenkey = make_key(OWNER_PREFIX,tokenid);
        storage_get(tokenkey,oldowner);
        ontio_assert(check_witness(oldowner),"checkwitness failed!");

        storage_put(make_key(APPROVE_PREFIX,tokenid),to);
        return true;
    }

    bool takeOwnership(address to,uint64_t tokenID){
        address t = {};
        key approvekey = make_key(APPROVE_PREFIX,std::to_string(tokenID));
        storage_get(approvekey,t);
        ontio_assert(check_witness(t),"checkwitness failed!");
        storage_put(make_key(OWNER_PREFIX,std::to_string(tokenID)),to);
        return true;
    }

    std::vector<char> tokenMetadata(uint64_t tokenID){
        std::vector<char> tmp = {};
        key tokenkey = make_key(TK_PREFIX,std::to_string(tokenID));
        storage_get(tokenkey,tmp);
        return tmp;
    }

    bool createToken(std::string name,std::string image,std::string description,std::string other){
        ontio_assert(check_witness(OWNER),"check witness failed");
        struct tokenStruct tk ;
        tk.name = name;
        tk.image = image;
        tk.description = description;
        tk.othermeta = other;
        uint64_t total = 0;
        storage_get(make_key(TOTALSUPPLY),total);

        std::string s = std::to_string(total+1);
        key tkkey = make_key(TK_PREFIX,s);
        storage_put(make_key(TOTALSUPPLY),total+1);
        storage_put(tkkey,tk);
        storage_put(make_key(OWNER_PREFIX,s),OWNER);

        notify(s);
        return true;
    }

    struct tokenStruct getTokenByID(uint64_t id){
        struct tokenStruct tk ;
        key tkkey = make_key(TK_PREFIX,std::to_string(id));
        storage_get(tkkey,tk);
        return tk;
    }
    

};

ONTIO_DISPATCH(oep5, (init)(transfer)(balanceOf)(ownerOf)(takeOwnership)(approve)(name)(symbol)(totalSupply)(createToken)(getTokenByID));