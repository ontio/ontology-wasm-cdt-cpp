#pragma once
#ifdef __cplusplus
extern "C" {
#endif
uint64_t timestamp(void);
uint32_t block_height(void);
size_t input_length(void);
size_t call_output_length(void);
void self_address(void *address);
void caller_address(void *address);
void entry_address(void *address);
void get_input(void *inputptr);
void get_call_output(void *res);
uint32_t check_witness(void *address);
/*return the blockhash length*/
size_t current_blockhash(void *blockhash);
/*return the txhash length*/	
size_t current_txhash(void *txhash);
void ret(void *res, size_t len);
void notify(void *msg, size_t len);
//void debug(void *msg, size_t len);
void debug( const char* cstr, uint32_t len);
/*return the output length.*/
size_t call_contract(void *contract_address,void *input, size_t len);
size_t storage_read(void *keyptr, size_t keylen, void *valptr, size_t vlen, size_t offset);
void storage_write(void *keyptr, size_t keylen, void *valptr, size_t vlen);
void storage_delete(void *keyptr, size_t keylen);
size_t contract_migrate(void *codeptr,size_t codelen, uint32_t needstorage, void *nameptr, size_t namelen, void *versionptr, size_t versionlen, void *authorptr, size_t authorlen, void *emailptr, size_t emaillen, void *descptr, size_t desclen, void *newaddrptr);
void contract_delete(void);
//void abort(void *msg, size_t len);

#ifdef __cplusplus
}
#endif
