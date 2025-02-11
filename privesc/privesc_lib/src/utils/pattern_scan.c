#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "pattern_scan.h"

char* pattern_scan(struct bin_section* section, char* pattern, char* mask) {
  const size_t mask_len = strlen(mask);
  const size_t size = section->size;
  const char* base = section->base;

  for (int i = 0; i < size; i++) {
    bool found = 1;
    for (int j = 0; j < mask_len; j++) {
      const char* start_of_match = &base[i];

      if (mask[j] != '?' && pattern[j] != start_of_match[j]) {
        found = 0;
        break;
      }
    }

    if (found) {
      return &base[i];
    }
  }

  return NULL;
}