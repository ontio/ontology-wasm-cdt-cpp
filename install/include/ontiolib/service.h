#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#ifdef WASM_TEST
uint64_t timestamp(void);
uint32_t block_height(void);
size_t input_length(void);
size_t call_output_length(void);
void self_address(const void *address);
void caller_address(const void *address);
void entry_address(const void *address);
void get_input(const void *inputptr);
void get_call_output(const void *res);
uint32_t check_witness(const void *address);
/*return the blockhash length*/
size_t current_blockhash(const void *blockhash);
/*return the txhash length*/	
size_t current_txhash(const void *txhash);
void ret(const void *res, const size_t len);
void notify(const void *msg, const size_t len);
//void debug(void *msg, size_t len);
void debug( const char* cstr, const uint32_t len);
/*return the output length.*/
size_t call_contract(const void *contract_address,const void *input, const size_t len);
size_t storage_read(const void *keyptr, const size_t keylen, const void *valptr, const size_t vlen, const size_t offset);
void storage_write(const void *keyptr, const size_t keylen, const void *valptr, const size_t vlen);
void storage_delete(const void *keyptr, const size_t keylen);
size_t contract_migrate(const void *codeptr,const size_t codelen, const uint32_t needstorage, const void *nameptr, const size_t namelen, const void *versionptr, const size_t versionlen, const void *authorptr, const size_t authorlen, const void *emailptr, const size_t emaillen, const void *descptr, const size_t desclen, const void *newaddrptr);
void contract_delete(void);

#include<string.h>
#include<assert.h>
	struct storage_elt {
		void *keyptr;
		size_t keylen;
		void *valptr;
		size_t vlen;
		struct storage_elt *next;
	};
	//typedef struct storage_elt storage_elt; 

	struct storage_header_h {
		size_t num;
		storage_elt *head;
		storage_elt *tail;
	};

	void print_all(storage_header_h *h) {
		int num = 0;
		int i = 0;
		if (h->head) {
			storage_elt *iter_node = h->head;
			
			while (iter_node) {
				printf("index %d: ", num);
				
				uint8_t *keyptr = (uint8_t *)iter_node->keyptr;
				uint8_t *valptr = (uint8_t *)iter_node->valptr;

				printf("  key: ");
				for(i = 0; i < iter_node->keylen; i++ ) {
					printf("%02x", keyptr[i]);
				}

				printf("  val: ");
				for(i = 0; i < iter_node->vlen; i++ ) {
					printf("%02x", valptr[i]);
				}
				printf("\n");

				num++;
				iter_node = iter_node->next;
			}
		}
	}

	storage_header_h s_header_t{0, nullptr, nullptr};
	storage_header_h *s_header = &s_header_t;

	uint32_t check_witness(const void *address) {
		return uint32_t(1u);
	}

	size_t storage_read(const void *keyptr, const size_t keylen, const void *valptr, const size_t vlen, const size_t offset) {
		if (s_header->head) {
			storage_elt *iter_node = s_header->head;

			assert(iter_node != NULL);
			
			while (iter_node != NULL) {
				if (keylen == iter_node->keylen && memcmp(keyptr, iter_node->keyptr, keylen) == 0) {
					size_t copy_len = vlen <= iter_node->vlen ? vlen : iter_node->vlen;
					//printf("match the key. total %u, vlen: %u, iter_node->vlen: %u, copy_len:%u\n", s_header->num, vlen, iter_node->vlen, copy_len);
					memcpy((void* )valptr, iter_node->valptr, copy_len);
					return iter_node->vlen;
				} else
					iter_node = iter_node->next;
			}
		}

		return UINT32_MAX;
	}

	void storage_write(const void *keyptr, const size_t keylen, const void *valptr, const size_t vlen) {
		storage_elt *prev_node = NULL;
		storage_elt *h = (storage_elt *) malloc(sizeof(storage_elt));
		h->keyptr = (void *) malloc(keylen);
		h->valptr = (void *) malloc(vlen);
		h->keylen = keylen;
		h->vlen = vlen;
		h->next = NULL;
		memcpy(h->keyptr, keyptr, keylen);
		memcpy(h->valptr, valptr, vlen);

		if (s_header->head) {
			assert(s_header->tail);

			storage_elt *iter_node = s_header->head;

			while (iter_node != NULL) {
				if (keylen == iter_node->keylen && memcmp(keyptr, iter_node->keyptr, keylen) == 0) {
					if (prev_node == NULL) {
						h->next = iter_node->next;
						s_header->head = h;
					} else {
						h->next =iter_node->next;
						prev_node->next = h;
					}

					if (h->next == NULL)
						s_header->tail = h;
					free(iter_node);
					return;
				} else {
					prev_node = iter_node;
					iter_node = iter_node->next;
				}
			}

			s_header->tail->next = h;
			s_header->tail = h;
		} else {
			s_header->head = h;
			s_header->tail= h;
			s_header->num = 0;
		}

		s_header->num++;
		//print_all(s_header);
	}

	void storage_delete(const void *keyptr, const size_t keylen) {
		if (s_header->head) {
			storage_elt *prev_node = NULL; 
			storage_elt *iter_node = s_header->head;
			
			while (iter_node != NULL) {
				if (keylen == iter_node->keylen && memcmp(keyptr, iter_node->keyptr, keylen) == 0) {

					if (prev_node == NULL) {
						s_header->head = iter_node->next;
						if (s_header->head == NULL)
							s_header->tail = NULL;
					}
					else {
						prev_node->next = iter_node->next;
						if (prev_node->next == NULL)
							s_header->tail = prev_node;
					}

					free(iter_node->keyptr);
					free(iter_node->valptr);
					free(iter_node);
					s_header->num--;
					assert(s_header->num >= 0);
				} else
					prev_node = iter_node;
					iter_node = iter_node->next;
			}
		}
	}

	void notify(const void *msg, const size_t len) {
		debug((char *)msg, len);
	}
#else
uint64_t timestamp(void);
uint32_t block_height(void);
size_t input_length(void);
size_t call_output_length(void);
void self_address(const void *address);
void caller_address(const void *address);
void entry_address(const void *address);
void get_input(const void *inputptr);
void get_call_output(const void *res);
uint32_t check_witness(const void *address);
/*return the blockhash length*/
size_t current_blockhash(const void *blockhash);
/*return the txhash length*/	
size_t current_txhash(const void *txhash);
void ret(const void *res, const size_t len);
void notify(const void *msg, const size_t len);
//void debug(void *msg, size_t len);
void debug( const char* cstr, const uint32_t len);
/*return the output length.*/
size_t call_contract(const void *contract_address,const void *input, const size_t len);
size_t storage_read(const void *keyptr, const size_t keylen, const void *valptr, const size_t vlen, const size_t offset);
void storage_write(const void *keyptr, const size_t keylen, const void *valptr, const size_t vlen);
void storage_delete(const void *keyptr, const size_t keylen);
size_t contract_migrate(const void *codeptr,const size_t codelen, const uint32_t needstorage, const void *nameptr, const size_t namelen, const void *versionptr, const size_t versionlen, const void *authorptr, const size_t authorlen, const void *emailptr, const size_t emaillen, const void *descptr, const size_t desclen, const void *newaddrptr);
void contract_delete(void);
//void abort(void *msg, size_t len);
#endif

#ifdef __cplusplus
}
#endif
