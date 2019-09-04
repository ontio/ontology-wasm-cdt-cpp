#pragma once
#ifdef __cplusplus
extern "C" {
#endif
uint64_t ontio_timestamp(void);
uint32_t ontio_block_height(void);
size_t ontio_input_length(void);
size_t ontio_call_output_length(void);
void ontio_self_address(const void *address);
void ontio_caller_address(const void *address);
void ontio_entry_address(const void *address);
void ontio_get_input(const void *inputptr);
void ontio_get_call_output(const void *res);
uint32_t ontio_check_witness(const void *address);
/*return the blockhash length*/
size_t ontio_current_blockhash(const void *blockhash);
/*return the txhash length*/	
size_t ontio_current_txhash(const void *txhash);
void ontio_return(const void *res, const size_t len);
void ontio_notify(const void *msg, const size_t len);
//void debug(void *msg, size_t len);
void ontio_debug( const char* cstr, const uint32_t len);
/*return the output length.*/
size_t ontio_call_contract(const void *contract_address,const void *input, const size_t len);
size_t ontio_storage_read(const void *keyptr, const size_t keylen, const void *valptr, const size_t vlen, const size_t offset);
void ontio_storage_write(const void *keyptr, const size_t keylen, const void *valptr, const size_t vlen);
void ontio_storage_delete(const void *keyptr, const size_t keylen);
size_t ontio_contract_migrate(const void *codeptr,const size_t codelen, const uint32_t needstorage, const void *nameptr, const size_t namelen, const void *versionptr, const size_t versionlen, const void *authorptr, const size_t authorlen, const void *emailptr, const size_t emaillen, const void *descptr, const size_t desclen, const void *newaddrptr);
void ontio_contract_delete(void);
//void abort(void *msg, size_t len);
#ifdef __cplusplus
}
#endif
