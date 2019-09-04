#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

#define TLX_BTREE_ASSERT(x) \
    do { assert(x); } while (0)

#define invalid_rootblock_key  0
/*block_key_t indicate the B plus tree's block key in leveldb*/
typedef unsigned int block_key_t;
uint32_t block_key_t_prefix = 0xdead;

template<typename T>
bool get_block_bykey(const block_key_t &key, T &value) {
	if (key == invalid_rootblock_key)
		return false;
	return ontio::storage_get(ontio::make_key(block_key_t_prefix,key), value);
}

template<typename T>
void put_block_bykey(const block_key_t &key, const T &value) {
	if (key == invalid_rootblock_key)
		return;
	ontio::storage_put(ontio::make_key(block_key_t_prefix, key), value);
}

template <typename Key>
struct btree_default_traits {
    static const int leaf_slots = 150;
    static const int inner_slots = 150;
    static const size_t binsearch_threshold = 32;
};

template <typename Key,
          typename Compare = std::less<Key>,
          typename Traits = btree_default_traits<Key>>
class BTree 
{
public:
    typedef Key key_type;
    typedef Compare key_compare;
    typedef Traits traits;

    static const unsigned short leaf_slotmax = traits::leaf_slots;
    static const unsigned short inner_slotmax = traits::inner_slots;

    static const unsigned short leaf_slotmin = (leaf_slotmax / 10);
    static const unsigned short inner_slotmin = (inner_slotmax / 10);

private:
    struct node {
        unsigned short level;
        unsigned short slotuse;
		block_key_t key_curr_node;

        void initialize(const unsigned short l) {
            level = l;
            slotuse = 0;
			key_curr_node = invalid_rootblock_key;
        }

        bool is_leafnode() const {
            return (level == 0);
        }

		bool nextnode_level_isleaf() const {
			return (level == 1);
		}
    };

    struct InnerNode : public node {
        key_type slotkey[inner_slotmax];
        block_key_t childid[inner_slotmax + 1];

        void initialize(const unsigned short l) {
            node::initialize(l);
        }

        bool is_full() const {
            return (node::slotuse == inner_slotmax);
        }

        const key_type& key(size_t s) const {
            return slotkey[s];
        }

        bool is_few() const {
            return (node::slotuse <= inner_slotmin);
        }

        bool is_underflow() const {
            return (node::slotuse < inner_slotmin);
        }

		template<typename DataStream>
		friend DataStream& operator << ( DataStream& ds, const InnerNode& t ) {
			size_t i = 0;
			ds << t.level;
			ds << t.slotuse;
			ds << t.key_curr_node;

			for (; i < t.slotuse; i++) {
				ds << t.slotkey[i];
				ds << t.childid[i];
			}
			ds << t.childid[i];
			return ds;
		}

		template<typename DataStream>
		friend DataStream& operator >> ( DataStream& ds, InnerNode& t ) {
			size_t i = 0;
			ds >> t.level;
			ds >> t.slotuse;
			ds >> t.key_curr_node;
			for (; i < t.slotuse; i++) {
				ds >> t.slotkey[i];
				ds >> t.childid[i];
			}
			ds >> t.childid[i];
			return ds;
		}
    };

    struct LeafNode : public node {
        block_key_t key_prev_leaf;
        block_key_t key_next_leaf;

        key_type slotkey[leaf_slotmax];

        void initialize() {
            node::initialize(0);
            key_prev_leaf = key_next_leaf = invalid_rootblock_key;
        }

        const key_type& key(size_t s) const {
            return slotkey[s];
        }

        bool is_full() const {
            return (node::slotuse == leaf_slotmax);
        }

        bool is_few() const {
            return (node::slotuse <= leaf_slotmin);
        }

        bool is_underflow() const {
            return (node::slotuse < leaf_slotmin);
        }

        void set_slot(unsigned short slot, const key_type& value) {
            TLX_BTREE_ASSERT(slot < node::slotuse);
            slotkey[slot] = value;
        }

		template<typename DataStream>
		friend DataStream& operator << ( DataStream& ds, const LeafNode& t ) {
			ds << t.level;
			ds << t.slotuse;
			ds << t.key_curr_node;

			ds << t.key_prev_leaf;
			ds << t.key_next_leaf;

			for (size_t i = 0; i < t.slotuse; i++) {
				ds << t.slotkey[i];
			}

			return ds;
		}

		template<typename DataStream>
		friend DataStream& operator >> ( DataStream& ds, LeafNode& t ) {
			ds >> t.level;
			ds >> t.slotuse;
			ds >> t.key_curr_node;

			ds >> t.key_prev_leaf;
			ds >> t.key_next_leaf;

			for (size_t i = 0; i < t.slotuse; i++) {
				ds >> t.slotkey[i];
			}

			return ds;
		}

    };


public:
    class iterator
    {
    public:
        typedef typename BTree::key_type key_type;
        typedef typename BTree::key_type& reference;
        typedef typename BTree::key_type* pointer;
        typedef iterator self;

    private:
        //BTree::LeafNode curr_leaf_t;
        BTree::LeafNode* curr_leaf;
        unsigned short curr_slot;
        friend class BTree<key_type, key_compare, traits>;

    public:
        iterator()
            : curr_slot(0)
        { curr_leaf = BTree::allocate_leaf(); curr_leaf->initialize(); }

        iterator(typename BTree::LeafNode *l, unsigned short s)
            : curr_slot(s)
        { curr_leaf = BTree::allocate_leaf();*curr_leaf = *l; }

        reference operator * () const {
            return curr_leaf->slotkey[curr_slot];
        }

        pointer operator -> () const {
            return &curr_leaf->slotkey[curr_slot];
        }

        iterator& operator ++ () {
            if (curr_slot + 1u < curr_leaf->slotuse) {
                ++curr_slot;
            }
            else if (curr_leaf->key_next_leaf!= NULL) {
				block_key_t key_next_leaf = curr_leaf->key_next_leaf;
				get_block_bykey(key_next_leaf, *curr_leaf);
                curr_slot = 0;
            }
            else {
                curr_slot = curr_leaf->slotuse;
            }

            return *this;
        }

        iterator operator ++ (int) {
            iterator tmp = *this;

            if (curr_slot + 1u < curr_leaf->slotuse) {
                ++curr_slot;
            }
            else if (curr_leaf->key_next_leaf != NULL) {
				block_key_t key_next_leaf = curr_leaf->key_next_leaf;
				get_block_bykey(key_next_leaf, *curr_leaf);
                curr_slot = 0;
            }
            else {
                curr_slot = curr_leaf->slotuse;
            }

            return tmp;
        }

        iterator& operator -- () {
            if (curr_slot > 0) {
                --curr_slot;
            }
            else if (curr_leaf->key_prev_leaf != nullptr) {
				block_key_t key_prev_leaf = curr_leaf->key_prev_leaf;
				get_block_bykey(key_prev_leaf, *curr_leaf);
                curr_slot = curr_leaf->slotuse - 1;
            }
            else {
                curr_slot = 0;
            }

            return *this;
        }

        iterator operator -- (int) {
            iterator tmp = *this;

            if (curr_slot > 0) {
                --curr_slot;
            }
            else if (curr_leaf->key_prev_leaf != nullptr) {
				block_key_t key_prev_leaf = curr_leaf->key_prev_leaf;
				get_block_bykey(key_prev_leaf, *curr_leaf);
                curr_slot = curr_leaf->slotuse - 1;
            }
            else {
                curr_slot = 0;
            }

            return tmp;
        }

        bool operator == (const iterator& x) const {
            return (x.curr_leaf->key_curr_node  == curr_leaf->key_curr_node) && (x.curr_slot == curr_slot);
        }

        bool operator != (const iterator& x) const {
            return (x.curr_leaf->key_curr_node != curr_leaf->key_curr_node) || (x.curr_slot != curr_slot);
        }
	};
public:
	BTree() {
		rootblock_isleaf = true;
		key_head_leaf = key_tail_leaf = rootblock_key = invalid_rootblock_key;
		/*start from invalid_rootblock_key*/
		blocknum = invalid_rootblock_key + 1;
		key_num = 0;
	}

	block_key_t get_rootblockkey() {
		return rootblock_key;
	}

	int keynum(void) {
		return key_num;
	}

private:

	block_key_t rootblock_key;
	bool rootblock_isleaf;

	block_key_t blocknum;

    block_key_t key_head_leaf;
    block_key_t key_tail_leaf;
	/*above need serialization*/
	ONTLIB_SERIALIZE( BTree, (rootblock_key) (rootblock_isleaf) (blocknum) (key_head_leaf) (key_tail_leaf))

    key_compare key_less_;
	int key_num;

private:
    bool key_less(const key_type& a, const key_type& b) const {
        return key_less_(a, b);
    }

    bool key_lessequal(const key_type& a, const key_type& b) const {
        return !key_less_(b, a);
    }

    bool key_greater(const key_type& a, const key_type& b) const {
        return key_less_(b, a);
    }

    bool key_greaterequal(const key_type& a, const key_type& b) const {
        return !key_less_(a, b);
    }

    bool key_equal(const key_type& a, const key_type& b) const {
        return !key_less_(a, b) && !key_less_(b, a);
    }

	block_key_t allocate_block_key(void) {
		return blocknum++;
	}

public:
    iterator begin() {
		iterator it;
		get_block_bykey(key_head_leaf, *(it.curr_leaf));
        return it;
    }

    iterator end() {
		iterator it;
        return it;
    }

    std::pair<iterator, bool> insert(const key_type& key) {
		return insert_start(key);
	}

private:
    template <typename node_type>
    int find_lower(const node_type* n, const key_type& key) const {
        if (n->slotuse == 0) return 0;

		if (n->slotuse > traits::binsearch_threshold)
        {
            int lo = 0, hi = n->slotuse;
			while (lo < hi)
			{
				int mid = (lo + hi) >> 1;

				if (key_lessequal(key, n->key(mid))) {
					hi = mid;
				}
				else {
					lo = mid + 1;
				}
			}

            return lo;
		} else {
            int lo = 0;
            while (lo < n->slotuse && key_less(n->key(lo), key)) ++lo;
            return lo;
		}
    }

public:
    iterator find(const key_type& key) {

		if (rootblock_key == invalid_rootblock_key) {
			return end();
		}

		InnerNode *node_inner = allocate_inner();
		LeafNode *node_leaf = allocate_leaf();

		if (rootblock_isleaf) {
			get_block_bykey(rootblock_key, *node_leaf);
		} else {
			get_block_bykey(rootblock_key, *node_inner);

			while (not node_inner->nextnode_level_isleaf()) {
				int slot = find_lower(node_inner, key);
				block_key_t n = node_inner->childid[slot];
				get_block_bykey(n, *node_inner);
			}

			int slot = find_lower(node_inner, key);
			block_key_t n = node_inner->childid[slot];
			get_block_bykey(n, *node_leaf);
		}

		free(node_inner);
		LeafNode *leaf = node_leaf;
        int slot = find_lower(leaf, key);
        return (slot < leaf->slotuse && key_equal(key, leaf->key(slot)))
               ? iterator(leaf, slot) : end();
    }

private:
    std::pair<iterator, bool>
    insert_start(const key_type& key) {
		InnerNode *inn = allocate_inner();
		LeafNode *lfn = allocate_leaf();

        block_key_t  newchild = invalid_rootblock_key;
        key_type newkey = key_type();
		if (rootblock_key == invalid_rootblock_key) {
			rootblock_key = key_head_leaf = key_tail_leaf = allocate_block_key();
		}

        std::pair<iterator, bool> r =
            insert_descend(rootblock_key, key, &newkey, &newchild, rootblock_isleaf);

        if (newchild != invalid_rootblock_key)
        {
			unsigned short rootlevel;
			InnerNode *newroot = allocate_inner();

			if (rootblock_isleaf) {
				get_block_bykey(rootblock_key, *lfn);
				rootlevel = lfn->level;
			} else {
				get_block_bykey(rootblock_key, *inn);
				rootlevel = inn->level;
			}

			newroot->initialize(rootlevel + 1);

			//printf("newkey %u\n", newkey);
            newroot->slotkey[0] = newkey;

            newroot->childid[0] = rootblock_key;
            newroot->childid[1] = newchild;

            newroot->slotuse = 1;

			rootblock_key = allocate_block_key();
			rootblock_isleaf = false;

			newroot->key_curr_node = rootblock_key;
			put_block_bykey(rootblock_key, *newroot);
			free(newroot);
        }

		if (r.second) {
			key_num++;
			//printf("key_num %d\n", key_num);
		}

		free(inn);
		free(lfn);

        return r;
    }

    std::pair<iterator, bool> insert_descend(
        block_key_t curr_bk, const key_type& key,
        key_type* splitkey, block_key_t* key_splitblock, bool isleaf) {

        if (not isleaf)
        {
			//InnerNode inn;
			InnerNode *inn = allocate_inner();
			InnerNode *inner = inn;
			get_block_bykey(curr_bk, *inner);
			TLX_BTREE_ASSERT(inner->key_curr_node == curr_bk);

            key_type newkey = key_type();
            block_key_t newchild = invalid_rootblock_key;

            int slot = find_lower(inner, key);

            std::pair<iterator, bool> r = insert_descend(inner->childid[slot], key, &newkey, &newchild, inner->nextnode_level_isleaf());

            if (newchild != invalid_rootblock_key)
            {
				InnerNode *new_inn = allocate_inner();
				InnerNode *newinner = nullptr;
                if (inner->is_full())
                {
					newinner = new_inn;
					newinner->initialize(inner->level);

					unsigned int mid = (inner->slotuse >> 1);

					/*this for balance.*/
					if (slot <= mid && mid > inner->slotuse - (mid + 1))
						mid--;

					/*num mid + 1 to old inner. so from index mid + 1 to new inner.*/
					newinner->slotuse = inner->slotuse - (mid + 1);
					/*index [a, b)*/

					std::copy(inner->slotkey + mid + 1, inner->slotkey + inner->slotuse, newinner->slotkey);
					/*child[index] <= key[index]*/
					std::copy(inner->childid + mid + 1, inner->childid + inner->slotuse + 1, newinner->childid);

					/*here must mid not mid + 1. because that inner's child index mid + 1 moved to new_inner. so old_inner max child index is mid. and the child max index equl to the number of slotuse. */
					inner->slotuse = mid;
					*splitkey = inner->key(mid); // and the index mid key moved to uplevel key to be inserted.
					*key_splitblock = allocate_block_key();

                    if (slot == inner->slotuse + 1 &&
                        inner->slotuse < newinner->slotuse)
                    {
                        TLX_BTREE_ASSERT(inner->slotuse + 1 < inner_slotmax);

                        InnerNode* split = newinner;

                        // move the split key and it's datum into the left node
                        inner->slotkey[inner->slotuse] = *splitkey;
                        inner->childid[inner->slotuse + 1] = split->childid[0];
                        inner->slotuse++;

                        // set new split key and move corresponding datum into
                        // right node
                        split->childid[0] = newchild;
                        *splitkey = newkey;

						put_block_bykey(curr_bk, *inner);
						TLX_BTREE_ASSERT(curr_bk == inner->key_curr_node);
						if(newinner) {
							newinner->key_curr_node = *key_splitblock;
							put_block_bykey(*key_splitblock, *newinner);
						}

						free(new_inn);
                        return r;
                    }
                    else if (slot >= inner->slotuse + 1)
                    {
                        slot -= inner->slotuse + 1;
                        inner = newinner;
                    }
                }

                TLX_BTREE_ASSERT(slot >= 0 && slot <= inner->slotuse);

                std::copy_backward( inner->slotkey + slot, inner->slotkey + inner->slotuse, inner->slotkey + inner->slotuse + 1);
                std::copy_backward( inner->childid + slot, inner->childid + inner->slotuse + 1, inner->childid + inner->slotuse + 2);

                inner->slotkey[slot] = newkey;
                inner->childid[slot + 1] = newchild;
                inner->slotuse++;

				put_block_bykey(curr_bk, *inn);
				TLX_BTREE_ASSERT(curr_bk == inn->key_curr_node);
				if(newinner) {
					newinner->key_curr_node = *key_splitblock;
					put_block_bykey(*key_splitblock, *newinner);
				}

				free(new_inn);
				free(inn);

				if (0)
				{
					print_node(curr_bk, false, 0, true);
					print_node(*key_splitblock, false, 0, true);
				}
            }

            return r;
        }
        else
        {

			LeafNode *lfn= allocate_leaf();
			LeafNode *leaf = lfn;
			leaf->initialize();
			leaf->key_curr_node = curr_bk;
			get_block_bykey(curr_bk, *leaf);
			TLX_BTREE_ASSERT(curr_bk == leaf->key_curr_node);

            int slot = find_lower(leaf, key);

            if ( slot < leaf->slotuse && key_equal(key, leaf->key(slot))) {
                return std::pair<iterator, bool>(iterator(leaf, slot), false);
            }

			LeafNode* newleaf = nullptr;
			block_key_t newleafkey = allocate_block_key();
            if (leaf->is_full())
            {
				/*split the node.*/
				unsigned int mid = (leaf->slotuse >> 1);
				newleaf = allocate_leaf();

				newleaf->slotuse = leaf->slotuse - mid;
				newleaf->key_next_leaf = leaf->key_next_leaf;

				if (newleaf->key_next_leaf == invalid_rootblock_key) {
        		    key_tail_leaf = newleafkey;
        		}
        		else {
					LeafNode *t = allocate_leaf();
					if(get_block_bykey(newleaf->key_next_leaf, *t)) {
						t->key_prev_leaf = newleafkey;
						TLX_BTREE_ASSERT(newleaf->key_next_leaf == t->key_curr_node);
						put_block_bykey(newleaf->key_next_leaf, *t);
						free(t);
					}
        		}

				/*bigger to newleaf.*/
				std::copy(leaf->slotkey + mid, leaf->slotkey + leaf->slotuse, newleaf->slotkey);
				leaf->slotuse = mid;
				leaf->key_next_leaf = newleafkey;
				newleaf->key_prev_leaf = curr_bk;
				*splitkey = leaf->key(leaf->slotuse - 1);

                if (slot >= leaf->slotuse)
                {
                    slot -= leaf->slotuse;
                    leaf = newleaf;
                }
            }

			ontio::check(slot >= 0 && slot <= leaf->slotuse, "slot must be in range.");

			if (leaf->slotuse > 0)
				std::copy_backward(leaf->slotkey + slot, leaf->slotkey + leaf->slotuse, leaf->slotkey + leaf->slotuse + 1);

            leaf->slotkey[slot] = key;
            leaf->slotuse++;

			/*this for incase. may not be happend. should insert to the newnode. because the seperate is parallel with newkey insert. but handled at slot >=sleaf->slotuse. sohere should be imposibble. */
			if (newleaf && leaf != newleaf && slot == leaf->slotuse - 1) {
                *splitkey = key;
			}

			/*update two leaf block*/
			put_block_bykey(curr_bk, *lfn);
			TLX_BTREE_ASSERT(lfn->key_curr_node == curr_bk);
			
			if(newleaf) {
				*key_splitblock = newleafkey;
				newleaf->key_curr_node = newleafkey;
				put_block_bykey(newleafkey, *newleaf);
				free(newleaf);
			}
			free(lfn);

			return std::pair<iterator, bool>(iterator(leaf, slot), true);
        }

		return std::pair<iterator, bool> (iterator(nullptr, 0), false);
    }

	static LeafNode* allocate_leaf(void) {
		LeafNode* t = (LeafNode *) malloc(sizeof(LeafNode));
		return t;
	}

	static InnerNode* allocate_inner(void) {
		InnerNode* t = (InnerNode*) malloc(sizeof(InnerNode));
		return t;
	}
public:
    void print(void) const {
        if (rootblock_key != invalid_rootblock_key) {
            print_node(rootblock_key, rootblock_isleaf, 0, true);
        }
    }

private:
    //! Recursively descend down the tree and print out nodes.
    static void print_node(const block_key_t node, bool isleaf, 
                           unsigned int depth = 0, bool recursive = false) {
        for (unsigned int i = 0; i < depth; i++) printf("  ");

        if (isleaf)
        {
            //const LeafNode* leafnode = static_cast<const LeafNode*>(node);
			LeafNode *leafnode = allocate_leaf();
			get_block_bykey(node, *leafnode);
			TLX_BTREE_ASSERT(node == leafnode->key_curr_node);
			printf("leaf node (%u) level %u slotuse %u\n", node, leafnode->level,leafnode->slotuse);

            for (unsigned int i = 0; i < depth; i++) printf("  ");
			printf("  leaf prev %u next %u\n",leafnode->key_prev_leaf,leafnode->key_next_leaf);

            for (unsigned int i = 0; i < depth; i++) printf("  ");

            for (unsigned int slot = 0; slot < leafnode->slotuse; ++slot)
            {
				printf("%u  ", leafnode->key(slot));
            }
			printf("\n");
        }
        else
        {
			InnerNode *innernode = allocate_inner();
			get_block_bykey(node, *innernode);
			TLX_BTREE_ASSERT(node == innernode->key_curr_node);
			printf("inner node (%u) level %u slotuse %u\n", node, innernode->level, innernode->slotuse);

            for (unsigned int i = 0; i < depth; i++) printf("  ");

            for (unsigned short slot = 0; slot < innernode->slotuse; ++slot)
            {
				printf("(%u) %u ", innernode->childid[slot],innernode->slotkey[slot]);
            }

			printf("(%u)\n", innernode->childid[innernode->slotuse]);

            if (recursive)
            {
                for (unsigned short slot = 0;
                     slot < innernode->slotuse + 1; ++slot)
                {
                    print_node(innernode->childid[slot], innernode->nextnode_level_isleaf(), depth + 1, recursive);
                }
            }
        }
    }

public:
    enum result_flags_t {
        //! Deletion successful and no fix-ups necessary.
        btree_ok = 0,

        //! Deletion not successful because key was not found.
        btree_not_found = 1,

        //! Deletion successful, the last key was updated so parent slotkeys
        //! need updates.
        btree_update_lastkey = 2,

        //! Deletion successful, children nodes were merged and the parent needs
        //! to remove the empty node.
        btree_fixmerge = 4
    };


    struct result_t {
        result_flags_t flags;

        key_type lastkey;

        result_t(result_flags_t f = btree_ok)
            : flags(f), lastkey()
        { }

        result_t(result_flags_t f, const key_type& k)
            : flags(f), lastkey(k)
        { }

        bool has(result_flags_t f) const {
            return (flags & f) != 0;
        }

        result_t& operator |= (const result_t& other) {
            flags = result_flags_t(flags | other.flags);

            // we overwrite existing lastkeys on purpose
            if (other.has(btree_update_lastkey))
                lastkey = other.lastkey;

            return *this;
        }
    };

	bool erase(const key_type &key) {
		if (rootblock_key == invalid_rootblock_key) return false;
		int slot = 0;

        result_t result = erase_one_descend(key, rootblock_key, invalid_rootblock_key, invalid_rootblock_key, invalid_rootblock_key, invalid_rootblock_key, nullptr, slot, rootblock_isleaf);
		if (!result.has(btree_not_found)) {
			key_num--;
			//printf("key_num %d\n", key_num);
		}

        return !result.has(btree_not_found);
	}

    result_t erase_one_descend(const key_type& key,
                               block_key_t curr,
                               block_key_t left, block_key_t right,
                               block_key_t left_parent, block_key_t right_parent,
                               InnerNode *parent, int &parentslot,bool curr_isleaf) {
        if (curr_isleaf)
        {
			LeafNode* leaf = (LeafNode *) malloc(sizeof(LeafNode) * 6);
			get_block_bykey(curr, *leaf);
			TLX_BTREE_ASSERT(curr == leaf->key_curr_node);
			LeafNode *left_leaf = nullptr;
			LeafNode *right_leaf = nullptr;

            int slot = find_lower(leaf, key);

            if (slot >= leaf->slotuse || !key_equal(key, leaf->key(slot)))
            {
                return btree_not_found;
            }

            std::copy(leaf->slotkey + slot + 1, leaf->slotkey + leaf->slotuse,
                      leaf->slotkey+ slot);

            leaf->slotuse--;

            result_t myres = btree_ok;

            // if the last key of the leaf was changed, the parent is notified
            // and updates the key of this leaf
            if (slot == leaf->slotuse)
            {
                if (parent && parentslot < parent->slotuse)
                {
                    TLX_BTREE_ASSERT(parent->childid[parentslot] == curr);
                    parent->slotkey[parentslot] = leaf->key(leaf->slotuse - 1);
                }
                else
                {
                    if (leaf->slotuse >= 1)
                    {
                        myres |= result_t(
                            btree_update_lastkey, leaf->key(leaf->slotuse - 1));
                    }
                    else
                    {
						if (leaf_slotmin != 1 && inner_slotmin != 1)
							TLX_BTREE_ASSERT(curr == rootblock_key);
                    }
                }
            }


            if (leaf->is_underflow() && !(leaf->key_curr_node == rootblock_key && leaf->slotuse >= 1))
            {
				if (left != invalid_rootblock_key) {
					left_leaf = leaf + 1;
					get_block_bykey(left, *left_leaf);
					TLX_BTREE_ASSERT(left_leaf->key_curr_node == left);
				}

				if (right != invalid_rootblock_key) {
					right_leaf = leaf + 2;
					get_block_bykey(right, *right_leaf);
					TLX_BTREE_ASSERT(right_leaf->key_curr_node == right);
				}

                // case : if this empty leaf is the root, then delete all nodes
                // and set root to nullptr.
                if (left_leaf == nullptr && right_leaf == nullptr)
                {
                    TLX_BTREE_ASSERT(leaf->key_curr_node == rootblock_key);
                    TLX_BTREE_ASSERT(leaf->slotuse == 0);

					rootblock_key = invalid_rootblock_key;
                    key_head_leaf = key_tail_leaf = invalid_rootblock_key;

                    return btree_ok;
                }
                // case : if both left and right leaves would underflow in case
                // of a shift, then merging is necessary. choose the more local
                // merger with our parent
                else if ((left_leaf == nullptr || left_leaf->is_few()) &&
                         (right_leaf == nullptr || right_leaf->is_few()))
                {
					if (left_parent == parent->key_curr_node)
                        myres |= merge_leaves(left_leaf, leaf, parent);
					else {
						TLX_BTREE_ASSERT(right_parent == parent->key_curr_node);
                        myres |= merge_leaves(leaf, right_leaf, parent);
						parentslot++;
					}
                }
                // case : the right leaf has extra data, so balance right with
                // current
                else if ((left_leaf != nullptr && left_leaf->is_few()) &&
                         (right_leaf != nullptr && !right_leaf->is_few()))
                {
                    if (right_parent == parent->key_curr_node)
                        myres |= shift_left_leaf(
                            leaf, right_leaf, parent, parentslot);
					else {
						TLX_BTREE_ASSERT(left_parent == parent->key_curr_node);
                        myres |= merge_leaves(left_leaf, leaf, parent);
					}
                }
                // case : the left leaf has extra data, so balance left with
                // current
                else if ((left_leaf != nullptr && !left_leaf->is_few()) &&
                         (right_leaf != nullptr && right_leaf->is_few()))
                {
                    if (left_parent == parent->key_curr_node)
                        shift_right_leaf(
                            left_leaf, leaf, parent, parentslot - 1);
					else {
						TLX_BTREE_ASSERT(right_parent == parent->key_curr_node);
                        myres |= merge_leaves(leaf, right_leaf, parent);
						parentslot++;
					}
                }
                // case : both the leaf and right leaves have extra data and our
                // parent, choose the leaf with more data
                else if (left_parent == right_parent)
                {
					//TLX_BTREE_ASSERT(right_parent == curr);
					TLX_BTREE_ASSERT(right_parent == parent->key_curr_node);
                    if (left_leaf->slotuse <= right_leaf->slotuse)
                        myres |= shift_left_leaf(
                            leaf, right_leaf, parent, parentslot);
                    else
                        shift_right_leaf(
                            left_leaf, leaf, parent, parentslot - 1);
                }
                else
                {
                    if (left_parent == parent->key_curr_node)
                        shift_right_leaf(
                            left_leaf, leaf, parent, parentslot - 1);
					else {
						TLX_BTREE_ASSERT(right_parent == parent->key_curr_node);
                        myres |= shift_left_leaf(
                            leaf, right_leaf, parent, parentslot);
					}
                }
            }

			TLX_BTREE_ASSERT(curr == leaf->key_curr_node);
			put_block_bykey(curr, *leaf);
			free(leaf);

            return myres;
        }
        else // !curr->is_leafnode()
        {
			InnerNode* inner =  (InnerNode *)malloc(sizeof(InnerNode) * 3);
			get_block_bykey(curr, *inner);
            //InnerNode* right_inner = static_cast<InnerNode*>(right);
			TLX_BTREE_ASSERT(curr == inner->key_curr_node);

            block_key_t myleft, myright;
            block_key_t myleft_parent, myright_parent;

            int slot = find_lower(inner, key);

            if (slot == 0) {
				InnerNode* left_inner = inner + 1;
                myleft = (left == invalid_rootblock_key) ? invalid_rootblock_key: (get_block_bykey(left, *left_inner),(left_inner)->childid[left_inner->slotuse - 1]);
				if (myleft != invalid_rootblock_key) {
					TLX_BTREE_ASSERT(left == left_inner->key_curr_node);
				}
                myleft_parent = left_parent; //acctually here should use left_inner->key_curr_node
            }
            else {
                myleft = inner->childid[slot - 1];
                myleft_parent = curr;
            }

            if (slot == inner->slotuse) {
				InnerNode *right_inner = inner + 2;
                myright = (right == invalid_rootblock_key) ? invalid_rootblock_key: (get_block_bykey(right, *right_inner),(right_inner)->childid[0]);
				if (myright != invalid_rootblock_key) {
					TLX_BTREE_ASSERT(right == right_inner->key_curr_node);
				}
                myright_parent = right_parent; //here use right_inner->key_curr_node
            }
            else {
                myright = inner->childid[slot + 1];
                myright_parent = curr;
            }

            result_t result = erase_one_descend(
                key,
                inner->childid[slot],
                myleft, myright,
                myleft_parent, myright_parent,
                inner, slot, inner->nextnode_level_isleaf());

            result_t myres = btree_ok;

            if (result.has(btree_not_found))
            {
                return result;
            }

            if (result.has(btree_update_lastkey))
            {
                if (parent && parentslot < parent->slotuse)
                {
					TLX_BTREE_ASSERT(parent->childid[parentslot] == curr);
                    parent->slotkey[parentslot] = result.lastkey;
                }
                else
                {
                    myres |= result_t(btree_update_lastkey, result.lastkey);
                }
            }

            if (result.has(btree_fixmerge))
            {
                // either the current node or the next is empty and should be
                // removed
                //if (inner->childid[slot]->slotuse != 0)
                //    slot++;

                // this is the child slot invalidated by the merge
				LeafNode *t = allocate_leaf();
				get_block_bykey(inner->childid[slot], *t);
				TLX_BTREE_ASSERT(t->key_curr_node == inner->childid[slot]);
                TLX_BTREE_ASSERT(t->slotuse == 0);

                std::copy(
                    inner->slotkey + slot, inner->slotkey + inner->slotuse,
                    inner->slotkey + slot - 1);
                std::copy(
                    inner->childid + slot + 1,
                    inner->childid + inner->slotuse + 1,
                    inner->childid + slot);

                inner->slotuse--;

                if (inner->level == 1)
                {
                    // fix split key for children leaves
					LeafNode *child = allocate_leaf();
                    slot--;
					get_block_bykey(inner->childid[slot], *child);
                    inner->slotkey[slot] = child->key(child->slotuse - 1);
                }
            }

			//put_block_bykey(inner->key_curr_node, *inner);

			/*
			if ((inner->key_curr_node == rootblock_key) && (inner->slotuse == 0))
			{
                   rootblock_key  = inner->childid[0];
				   rootblock_isleaf = true;
			}
			*/

            if (inner->is_underflow() &&
                !(inner->key_curr_node == rootblock_key && inner->slotuse >= 1))
            {
				InnerNode *left_inner= nullptr;
				InnerNode *right_inner = nullptr;

				if (left != invalid_rootblock_key) {
					left_inner = allocate_inner();
					get_block_bykey(left, *left_inner);
					TLX_BTREE_ASSERT(left_inner->key_curr_node == left);
				}

				if (right != invalid_rootblock_key) {
					right_inner = allocate_inner();
					get_block_bykey(right, *right_inner);
					TLX_BTREE_ASSERT(right_inner->key_curr_node == right);
				}

                // case: the inner node is the root and has just one child. that
                // child becomes the new root
                if (left_inner == nullptr && right_inner == nullptr)
                {
                    TLX_BTREE_ASSERT(inner->key_curr_node == rootblock_key);
                    TLX_BTREE_ASSERT(inner->slotuse == 0);

					rootblock_key  = inner->childid[0];
					if (inner->nextnode_level_isleaf())
						rootblock_isleaf = true;
					else
						rootblock_isleaf = false;

                    inner->slotuse = 0;
                    return btree_ok;
                }
                // case : if both left and right leaves would underflow in case
                // of a shift, then merging is necessary. choose the more local
                // merger with our parent
                else if ((left_inner == nullptr || left_inner->is_few()) &&
                         (right_inner == nullptr || right_inner->is_few()))
                {
                    if (left_parent == parent->key_curr_node)
                        myres |= merge_inner(
                            left_inner, inner, parent, parentslot - 1);
					else {
						TLX_BTREE_ASSERT(right_parent = parent->key_curr_node);
                        myres |= merge_inner(
                            inner, right_inner, parent, parentslot);
						parentslot++;
					}
                }
                // case : the right leaf has extra data, so balance right with
                // current
                else if ((left_inner != nullptr && left_inner->is_few()) &&
                         (right_inner != nullptr && !right_inner->is_few()))
                {
                    if (right_parent == parent->key_curr_node)
                        shift_left_inner(
                            inner, right_inner, parent, parentslot);
					else {
						TLX_BTREE_ASSERT(left_parent == parent->key_curr_node);
                        myres |= merge_inner(
                            left_inner, inner, parent, parentslot - 1);
					}
                }
                // case : the left leaf has extra data, so balance left with
                // current
                else if ((left_inner != nullptr && !left_inner->is_few()) &&
                         (right_inner != nullptr && right_inner->is_few()))
                {
                    if (left_parent == parent->key_curr_node)
                        shift_right_inner(
                            left_inner, inner, parent, parentslot - 1);
					else {
						TLX_BTREE_ASSERT(right_parent == parent->key_curr_node);
                        myres |= merge_inner(
                            inner, right_inner, parent, parentslot);
						parentslot++;
					}
                }
                // case : both the leaf and right leaves have extra data and our
                // parent, choose the leaf with more data
                else if (left_parent == right_parent)
                {
					TLX_BTREE_ASSERT(right_parent == parent->key_curr_node);
                    if (left_inner->slotuse <= right_inner->slotuse)
                        shift_left_inner(
                            inner, right_inner, parent, parentslot);
                    else
                        shift_right_inner(
                            left_inner, inner, parent, parentslot - 1);
                }
                else
                {
                    if (left_parent == parent->key_curr_node)
                        shift_right_inner(
                            left_inner, inner, parent, parentslot - 1);
					else{
						TLX_BTREE_ASSERT(right_parent == parent->key_curr_node);
                        shift_left_inner(
                            inner, right_inner, parent, parentslot);
					}
                }
            }

			TLX_BTREE_ASSERT(inner->key_curr_node == curr);
			put_block_bykey(inner->key_curr_node, *inner);
			free(inner);

            return myres;
        }
    }

    static result_t shift_left_leaf(
        LeafNode* left, LeafNode* right,
        InnerNode* parent, unsigned int parentslot) {

        TLX_BTREE_ASSERT(left->is_leafnode() && right->is_leafnode());
        TLX_BTREE_ASSERT(parent->level == 1);

        TLX_BTREE_ASSERT(left->key_next_leaf == right->key_curr_node);
        TLX_BTREE_ASSERT(left->key_curr_node == right->key_prev_leaf);

        TLX_BTREE_ASSERT(left->slotuse < right->slotuse);
        TLX_BTREE_ASSERT(parent->childid[parentslot] == left->key_curr_node);

        unsigned int shiftnum = (right->slotuse - left->slotuse) >> 1;

        TLX_BTREE_ASSERT(left->slotuse + shiftnum < leaf_slotmax);

        // copy the first items from the right node to the last slot in the left
        // node.

        std::copy(right->slotkey, right->slotkey+ shiftnum, left->slotkey+ left->slotuse);

        left->slotuse += shiftnum;

        // shift all slots in the right node to the left

        std::copy(right->slotkey + shiftnum, right->slotkey + right->slotuse,
                  right->slotkey);

        right->slotuse -= shiftnum;

		put_block_bykey(left->key_curr_node, *left);
		put_block_bykey(right->key_curr_node, *right);

        // fixup parent
        if (parentslot < parent->slotuse) {
            parent->slotkey[parentslot] = left->key(left->slotuse - 1);
            return btree_ok;
        }
        else {  // the update is further up the tree
            return result_t(btree_update_lastkey, left->key(left->slotuse - 1));
        }
    }

	//right==>left
    result_t merge_leaves(LeafNode* left, LeafNode* right,
                          InnerNode* parent) {
        TLX_BTREE_ASSERT(left->is_leafnode() && right->is_leafnode());
        TLX_BTREE_ASSERT(parent->level == 1);

        TLX_BTREE_ASSERT(left->slotuse + right->slotuse < leaf_slotmax);
		LeafNode *t = allocate_leaf();

        std::copy(right->slotkey, right->slotkey + right->slotuse,
                  left->slotkey + left->slotuse);

        left->slotuse += right->slotuse;

        left->key_next_leaf = right->key_next_leaf;

		get_block_bykey(left->key_next_leaf, *t);

		if (left->key_next_leaf != invalid_rootblock_key) {
            t->key_prev_leaf = left->key_curr_node;
			put_block_bykey(left->key_next_leaf, *t);
		}
        else
            key_tail_leaf = left->key_curr_node;

        right->slotuse = 0;

		put_block_bykey(left->key_curr_node, *left);
		put_block_bykey(right->key_curr_node, *right);

        return btree_fixmerge;
    }

	//left==>right
    static void shift_right_leaf(LeafNode* left, LeafNode* right,
                                 InnerNode* parent, unsigned int parentslot) {
        TLX_BTREE_ASSERT(left->is_leafnode() && right->is_leafnode());
        TLX_BTREE_ASSERT(parent->level == 1);

        TLX_BTREE_ASSERT(left->key_next_leaf == right->key_curr_node);
        TLX_BTREE_ASSERT(left->key_curr_node == right->key_prev_leaf);
        TLX_BTREE_ASSERT(parent->childid[parentslot] == left->key_curr_node);

        TLX_BTREE_ASSERT(left->slotuse > right->slotuse);

        unsigned int shiftnum = (left->slotuse - right->slotuse) >> 1;

        TLX_BTREE_ASSERT(right->slotuse + shiftnum < leaf_slotmax);

        std::copy_backward(right->slotkey, right->slotkey + right->slotuse,
                           right->slotkey + right->slotuse + shiftnum);

        right->slotuse += shiftnum;

        // copy the last items from the left node to the first slot in the right
        // node.
        std::copy(left->slotkey + left->slotuse - shiftnum,
                  left->slotkey + left->slotuse,
                  right->slotkey);

        left->slotuse -= shiftnum;

        parent->slotkey[parentslot] = left->key(left->slotuse - 1);

		put_block_bykey(left->key_curr_node, *left);
		put_block_bykey(right->key_curr_node, *right);
    }

    static void shift_right_inner(InnerNode* left, InnerNode* right,
                                  InnerNode* parent, unsigned int parentslot) {
        TLX_BTREE_ASSERT(left->level == right->level);
        TLX_BTREE_ASSERT(parent->level == left->level + 1);

        TLX_BTREE_ASSERT(left->slotuse > right->slotuse);
        TLX_BTREE_ASSERT(parent->childid[parentslot] == left->key_curr_node);

        unsigned int shiftnum = (left->slotuse - right->slotuse) >> 1;

        if (1)
        {
            // find the left node's slot in the parent's children
            unsigned int leftslot = 0;
            while (leftslot <= parent->slotuse &&
                   parent->childid[leftslot] != left->key_curr_node)
                ++leftslot;

            TLX_BTREE_ASSERT(leftslot < parent->slotuse);
            TLX_BTREE_ASSERT(parent->childid[leftslot] == left->key_curr_node);
            TLX_BTREE_ASSERT(parent->childid[leftslot + 1] == right->key_curr_node);

            TLX_BTREE_ASSERT(leftslot == parentslot);
        }

        // shift all slots in the right node

        TLX_BTREE_ASSERT(right->slotuse + shiftnum < inner_slotmax);

        std::copy_backward(
            right->slotkey, right->slotkey + right->slotuse,
            right->slotkey + right->slotuse + shiftnum);
        std::copy_backward(
            right->childid, right->childid + right->slotuse + 1,
            right->childid + right->slotuse + 1 + shiftnum);

        right->slotuse += shiftnum;

        // copy the parent's decision slotkey and childid to the last new key on
        // the right
        right->slotkey[shiftnum - 1] = parent->slotkey[parentslot];

        // copy the remaining last items from the left node to the first slot in
        // the right node.
        std::copy(left->slotkey + left->slotuse - shiftnum + 1,
                  left->slotkey + left->slotuse,
                  right->slotkey);
        std::copy(left->childid + left->slotuse - shiftnum + 1,
                  left->childid + left->slotuse + 1,
                  right->childid);

        // copy the first to-be-removed key from the left node to the parent's
        // decision slot
        parent->slotkey[parentslot] = left->slotkey[left->slotuse - shiftnum];

        left->slotuse -= shiftnum;

		put_block_bykey(left->key_curr_node, *left);
		put_block_bykey(right->key_curr_node, *right);
    }

    //! Balance two inner nodes. The function moves key/data pairs from right to
    //! left so that both nodes are equally filled. The parent node is updated
    //! if possible.
    static void shift_left_inner(InnerNode* left, InnerNode* right,
                                 InnerNode* parent, unsigned int parentslot) {
        TLX_BTREE_ASSERT(left->level == right->level);
        TLX_BTREE_ASSERT(parent->level == left->level + 1);

        TLX_BTREE_ASSERT(left->slotuse < right->slotuse);
        TLX_BTREE_ASSERT(parent->childid[parentslot] == left->key_curr_node);

        unsigned int shiftnum = (right->slotuse - left->slotuse) >> 1;

        TLX_BTREE_ASSERT(left->slotuse + shiftnum < inner_slotmax);

        if (1)
        {
            // find the left node's slot in the parent's children and compare to
            // parentslot

            unsigned int leftslot = 0;
            while (leftslot <= parent->slotuse &&
                   parent->childid[leftslot] != left->key_curr_node)
                ++leftslot;

            TLX_BTREE_ASSERT(leftslot < parent->slotuse);
            TLX_BTREE_ASSERT(parent->childid[leftslot] == left->key_curr_node);
            TLX_BTREE_ASSERT(parent->childid[leftslot + 1] == right->key_curr_node);

            TLX_BTREE_ASSERT(leftslot == parentslot);
        }

        // copy the parent's decision slotkey and childid to the first new key
        // on the left
        left->slotkey[left->slotuse] = parent->slotkey[parentslot];
        left->slotuse++;

        // copy the other items from the right node to the last slots in the
        // left node.
        std::copy(right->slotkey, right->slotkey + shiftnum - 1,
                  left->slotkey + left->slotuse);
        std::copy(right->childid, right->childid + shiftnum,
                  left->childid + left->slotuse);

        left->slotuse += shiftnum - 1;

        // fixup parent
        parent->slotkey[parentslot] = right->slotkey[shiftnum - 1];

        // shift all slots in the right node
        std::copy(
            right->slotkey + shiftnum, right->slotkey + right->slotuse,
            right->slotkey);
        std::copy(
            right->childid + shiftnum, right->childid + right->slotuse + 1,
            right->childid);

        right->slotuse -= shiftnum;

		put_block_bykey(left->key_curr_node, *left);
		put_block_bykey(right->key_curr_node, *right);
    }

    //! Merge two inner nodes. The function moves all key/childid pairs from
    //! right to left and sets right's slotuse to zero. The right slot is then
    //! removed by the calling parent node.
    static result_t merge_inner(InnerNode* left, InnerNode* right,
                                InnerNode* parent, unsigned int parentslot) {

        TLX_BTREE_ASSERT(left->level == right->level);
        TLX_BTREE_ASSERT(parent->level == left->level + 1);

        TLX_BTREE_ASSERT(parent->childid[parentslot] == left->key_curr_node);

        TLX_BTREE_ASSERT(left->slotuse + right->slotuse < inner_slotmax);

        if (1)
        {
            // find the left node's slot in the parent's children
            unsigned int leftslot = 0;
            while (leftslot <= parent->slotuse &&
                   parent->childid[leftslot] != left->key_curr_node)
                ++leftslot;

            TLX_BTREE_ASSERT(leftslot < parent->slotuse);
            TLX_BTREE_ASSERT(parent->childid[leftslot] == left->key_curr_node);
            TLX_BTREE_ASSERT(parent->childid[leftslot + 1] == right->key_curr_node);

            TLX_BTREE_ASSERT(parentslot == leftslot);
        }

        // retrieve the decision key from parent
        left->slotkey[left->slotuse] = parent->slotkey[parentslot];
        left->slotuse++;

        // copy over keys and children from right
        std::copy(right->slotkey, right->slotkey + right->slotuse,
                  left->slotkey + left->slotuse);
        std::copy(right->childid, right->childid + right->slotuse + 1,
                  left->childid + left->slotuse);

        left->slotuse += right->slotuse;
        right->slotuse = 0;

		put_block_bykey(left->key_curr_node, *left);
		put_block_bykey(right->key_curr_node, *right);

        return btree_fixmerge;
    }


public:
    void verify_leaflinks() const {
		if (key_head_leaf == invalid_rootblock_key)
			return;

		LeafNode *n = allocate_leaf();
		LeafNode *next = allocate_leaf();
		get_block_bykey(key_head_leaf, *n);
		TLX_BTREE_ASSERT(key_head_leaf == n->key_curr_node);

        TLX_BTREE_ASSERT(n->level == 0);
        TLX_BTREE_ASSERT(!n || n->key_prev_leaf == invalid_rootblock_key);

        unsigned int testcount = 0;

        while (1)
        {
            TLX_BTREE_ASSERT(n->level == 0);
            TLX_BTREE_ASSERT(n->slotuse > 0);

            for (unsigned short slot = 0; slot < n->slotuse - 1; ++slot)
            {
                TLX_BTREE_ASSERT(key_lessequal(n->key(slot), n->key(slot + 1)));
            }

            testcount += n->slotuse;

            if (n->key_next_leaf != invalid_rootblock_key)
            {

				next = get_block_bykey(n->key_next_leaf, *next);
				TLX_BTREE_ASSERT(n->key_next_leaf == next->key_curr_node);
                TLX_BTREE_ASSERT(key_lessequal(n->key(n->slotuse - 1), next->key(0)));

                TLX_BTREE_ASSERT(n->key_curr_node == next->key_prev_leaf);

				LeafNode *t = n;
				n = next;
				next = t;
            }
            else
            {
                TLX_BTREE_ASSERT(key_tail_leaf == n->key_curr_node);
				break;
            }
        }

        TLX_BTREE_ASSERT(testcount == keynum());
    }
};
