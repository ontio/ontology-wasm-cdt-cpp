namespace ontio {
template<typename Key, typename Value, uint8_t prefix>
class KeyTable {
public:
    typedef Key key_type;
    typedef Value value_type;

	class iterator {
	public:
        typedef typename KeyTable::key_type key_type;
        typedef typename KeyTable::value_type value_type;
        typedef value_type* pointer;
        typedef value_type& reference;
		bool is_end;
		key_type key;
		value_type value;
		iterator():is_end(true) {}
		iterator(const key_type &k, const value_type &v): key(k), value(v), is_end(false) {}
		iterator(const key_type &k): key(k), is_end(false) {}
        friend class KeyTable<key_type, value_type, prefix>;

        reference operator * () {
			return value;
		}

        pointer operator -> () {
			return &value;
		}

		void commit() {
			storage_put(make_key(prefix,key), value);
		}

		bool operator == (const iterator& x) const {
			if (is_end != x.is_end) {
				return false;
			}else if (is_end == true) {
				return true;
			}

			return key == x.key;
		}

		bool operator != (const iterator& x) const {
			if (is_end != x.is_end) {
				return true ;
			}else if (is_end == true) {
				return false;
			}

			return key != x.key;
		}
	};

	iterator find(const key_type &key) {
		iterator it(key);
		if (storage_get(make_key(prefix, key), it.value))
			return it;
		else 
			return end();
	}

	iterator end() {
		iterator it;
		return it;
	}

	const iterator cend() {
		iterator it;
		return it;
	}

	template <typename T>
	iterator emplace(const key_type k, T func) {
		value_type v;
		func(v);
		storage_put(make_key(prefix, k), v);
		return iterator(k, v);
	}

	template <typename T>
	void modify(iterator &iter, T func) {
		func(iter.value);
		storage_put(make_key(prefix,iter.key), iter.value);
	}
};
};
