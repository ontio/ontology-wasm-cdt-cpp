##### BlockChainApi

| api name          | args                                                         | return  |
| ----------------- | ------------------------------------------------------------ | ------- |
| self_address      | void                                                         | address |
| caller_address    | void                                                         | address |
| entry_address     | void                                                         | address |
| check_witness     | address                                                      | bool    |
| current_blockhash | void                                                         | H256    |
| current_txhash    | void                                                         | H256    |
| notify            | string                                                       | void    |
| call_contract     | 1. address<br />2. vector<char><br />3. any type, save the return result of calling contract | void    |
| contract_migrage  | 1. vector<char> code<br />2. uint32 vmtype<br />3. string name <br />4. string version<br />5. string autoher<br />6. string email <br />7. string description | address |
| storage_get       | 1. key: acctullay is vector<char><br />2. any type obj contranct have stored. | bool    |
| storage_put       | 1. key: acctually is vector<char><br />2. any type obj can searialize | void    |
| storage_delete    | key                                                          | void    |

#### OtherApi

##### a. apilist

​	ontology wasm cdt, support libc/libc++/crypto。blow will list some api not included in this lib.

| api name                            | args                                                         | return       | description                                                  |
| ----------------------------------- | ------------------------------------------------------------ | ------------ | ------------------------------------------------------------ |
| base58toaddress                     | string: base58 with string                                   | address      | address acctually bytes[20]. and have the interface tohexstring |
| sha256                              | 1. vector<char><br />2. H256: return result                  | void         |                                                              |
| hash256                             | 1. vector<char><br />2. H256:return result                   | void         |                                                              |
| pack                                | any types, any arg num support                               | vector<char> |                                                              |
| check                               | 1. bool<br />2. char */string: will print when debug if false |              |                                                              |
| make_key                            | any types, any arg num support                               | vector<char> |                                                              |
| ONTIO_DISPATCH                      | ONTIO_DISPATCH(oep4, (init)(transfer)(balanceOf)(allowance)) |              | this api just the macro. oep4 here is a contract class name, other are user defined api. and must coding at the last of the main contract file. |
| ont::transfer/ong::transfer         | 1. address from<br />2. address to <br />3. asset amount     | bool         |                                                              |
| ont::transferfrom/ong::transferfrom | 1. address owner<br />2. address  from<br />3. address to<br />4. amount | bool         |                                                              |
| ont::approve/ong::approve           | 1. address from<br />2. address to<br />3. asset amount      | bool         |                                                              |
| ont::balanceof/ong::balanceof       | address                                                      | asset        | asset is a wrapper for int64. and it can check the overflow issue. |
| ont::allowance/ong::allowance       | 1. address from<br />2. address to                           | asset        |                                                              |

##### b. how to searialize

​	the searialize interface api is pack ,ontology wasm cdt can searialize many type. like some type in std. array, vector, map, set, list, dequeue,tuple, string, variant. the code style like below.

```
std::vector<char> vec{1,2,3,4,5,6,7,8};
auto v = pack(vec);
```

​	variable v above is std::vector<char> accaully. also you can searialize many args. 

```
std::vector<char> vec{1,2,3,4,5,6,7,8};
std::set<std::string> st{"a", "b", "c", "d", "e"};
auto v = pack(vec, st);
```

​	you can pack any number of args that the type data support searialize. and the args type can be different.

##### c.  how to make key.

​	the key construct interface is make_key. acctually, make_key just a pack define. 

```
using std::string
using std::vector
string prefix = "balanceprefix"
address owner;
key res = make_Key(prefix, owner);
```

##### d.  how to serialize self defined structure.

​	ONTLIB_SERIALIZE can help define the serialization of self defined structure。 expamle blow

```
    struct transferM {
        address from;
        address to;
        asset amount;
        ONTLIB_SERIALIZE( transferM,  (from)(to)(amount) )
    };
```

​	so if you want to serialize transferM。coding like blow

```
transferM tfsm;
/*init tfsm code ...*/
auto v = pack(tfsm);
```

#### Structure

​	ontology wasm cdt support the libc++ std struture like list, array, vector, map etc. and some else prefined structure.

| structure | method                                           | description                                                  |
| --------- | ------------------------------------------------ | ------------------------------------------------------------ |
| address   | void debugout(void)<br />string tohextring(void) | address is acctually fixed len of bytes[20].  std::array<uint8, 20>. and support == and != operation. |
| asset     | NA                                               | asset is acctuallly int64 with scope [-1<<62 - 1, 1<<62-1]. and support overflow check. you can do arithmatic like normal int64 like +-*/ and arith compare. |
| key       | NA                                               | key is typedef of std::vector<char>                          |
| H160      | NA                                               | H160 is typefdef of std::array<uint8, 20>                    |
| H256      | NA                                               | typedef of std::array<uint8, 32>                             |

#### Library support.

​	currently, ontology wasm cdt support libc/libc++/base58/crypto/.  so you can coding include this library like tranditionl way. the  most import maybe the libc++'s std. like map, list, vecotr, array etc. and when you compile failed with function not find at the link step. may be you can add lib like blow.  and the libc/libc++ already added auto.

```
ont_cpp yourfile.cpp -lbase58 -lcyrpto -lbuiltin
```

#### Codestype

​	blow is the hello world code. 

```
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
```

​	include ontio.hpp first. this include most of the include file include std. so if you want vector. just declare with using std::vector is ok.  class hello is your contract name. myhello is the interface support. any intferface expose to user must add to ONTIO_DISPATCH(contract_name, (intferface0), (intferface1, (intferface2), ....)

#### Example