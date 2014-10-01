#ifndef RUST_PINYIN_ENGINE
#define RUST_PINYIN_ENGINE

#include <stdint.h>

// create pinyin->sinograms database
void *db_new (const char* fname);
void db_free(void* db);

// create a vector of suggestions based on a search string
void* pinyin2suggestions_c(
    void* db,
    const char* pinyinRawString
);
void suggestions_free(void* suggestions);

// get the number of suggestions in suggestions vector
uint32_t suggestions_size(void* suggestions);
// get the suggestions number #index
const char* suggestions_value_get(void *suggestions, uint32_t index);
void suggestions_value_free(const char* value);

int run(int argc, char** argv, void (*kont)(void));

#endif
