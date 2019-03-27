
namespace __hash_string__{

	using std::string;

	typedef uint64_t hash_t;
	constexpr hash_t prime = 0x100000001B3ull;
	constexpr hash_t basis = 0xCBF29CE484222325ull;

	hash_t string_hash(string &s)  
	{
		hash_t ret = basis;
		for(char c : s) {
			ret ^= c;  
			ret *= prime;  
		}
		return ret;  
	}

	constexpr hash_t hash_at_compile_time(char const* str, hash_t last_value = basis)  
	{
	    return *str ? hash_at_compile_time(str+1, (*str ^ last_value) * prime) : last_value;
	}
}
