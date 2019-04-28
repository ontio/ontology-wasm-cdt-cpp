#include<ontiolib/ontio.hpp>
#include<ontiolib/base58.hpp>
#include <regex>

using namespace ontio;
using std::string;
class oep7 : public contract{

    address ADMIN_ADDRESS = base58toaddress("Ad4pjz2bqep4RhQrUAzMuZJkBC3qJ1tZuT");
    string DOMAIN_PREFIX = "domain_";
    string OWNER_PREFIX = "owner_";

    std::vector<string> black_list = {"admin","foundation","official"};

    public:
    using contract::contract;
    bool init(){
        return true;
    }

    bool registerDomain(address acct, string domainName){
        printf("1");
        check(check_witness(acct),"check witness failed");
        printf("2");

        for (string b:black_list){
            check(domainName.compare(b),"domain in black list");
        }
        printf("3");

        bool chk = std::regex_match(domainName, std::regex("[0-9a-z]{3,10}")) ;
        check(chk,"domainName format error");
        printf("4");
        address owner = {};
        key domainkey = make_key(OWNER_PREFIX,domainName);
        bool exist = storage_get(domainkey,owner);
        check(!exist,"owner is registered");
        printf("5");
        storage_put(domainkey,acct);
        // char buffer [100];
        // std::sprintf(buffer, "registered %s %s ",owner,domainName.data); 
        // notify(buffer);
        notify("register succeed!");
        return true;
    }

    address ownerOf(string domainName){
        address owner = {};
        key domainkey = make_key(OWNER_PREFIX,domainName);
        storage_get(domainkey,owner);
        return owner;
    }

    bool transfer(address to, string domainName){
        address owner = {};
        key domainkey = make_key(OWNER_PREFIX,domainName);
        storage_get(domainkey,owner);

        check(check_witness(owner),"check witness failed");

        storage_put(domainkey,to);

        // char buffer [100];
        // std::sprintf(buffer, "transfer %s %s %s","",owner,to,domainName.data);       
        // notify(buffer);
        return true;
    }
};
ONTIO_DISPATCH(oep7, (init)(transfer)(ownerOf)(registerDomain));
