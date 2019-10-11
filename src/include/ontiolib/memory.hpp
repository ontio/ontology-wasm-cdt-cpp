/**
 *  @file
 *  @copyright defined in ont/LICENSE
 */
#pragma once

extern "C" void* alloca(size_t);

void* sbrk(size_t num_bytes);
extern "C" {
void* malloc(size_t size);
void* calloc(size_t count, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);
}
