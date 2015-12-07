/** This is an implementation of a radix counting sort, a linear time stable hash-based sort.
 * Copyright (C) 2015 Lorne Schell <orange.toaster@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * gcc -O3 -std=c99 -Wall radix_count.c -o radix_count
 */
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  size_t count;
  uint64_t *data;
} array;

static void radix(array scratch, array a, array count, uint64_t mask, uint64_t offset) {
  register size_t i = 0;

  for (i = a.count - 1; i > 0; --i) {
    scratch.data[ -- count.data[(a.data[i] & mask) >> offset ] ] = a.data[i];
  }
  assert(i == 0);
  scratch.data[ count.data[(a.data[i] & mask) >> offset ] - 1 ] = a.data[i];
}

static void count(array a, array count, uint64_t mask, uint64_t offset) {
  memset(count.data, 0, count.count * sizeof(uint64_t));
  register size_t i = 0;

  for (i = 0; i < a.count; ++i) {
    count.data[(a.data[i] & mask) >> offset ] ++ ;
  }

  for (i = 1; i < count.count; ++i) {
    count.data[i] += count.data[i - 1];

    if (count.data[i] >= a.count) {
      return;
    }
  }
}
// \theta_m ( 2n + c ) \theta_t ( 8n )
int radix_counting_sort(array a) {
  array scratch = { a.count, NULL };
  scratch.data = malloc(a.count * sizeof(uint64_t));

  if (!scratch.data) {
    return -1;
  }

  uint64_t stackmem[256];
  array counting_array = { 256, stackmem };
  uint64_t mask = 0xFF, offset = 0;

  do {
    count(a, counting_array, mask, offset);
    radix(scratch, a, counting_array, mask, offset);
    array tmp = scratch;
    scratch = a;
    a = tmp;
    mask <<= CHAR_BIT;
    offset += CHAR_BIT;

  } while (mask);

  free(scratch.data);
  return 0;
}

#include <stdio.h>

static int print_array(array a) {
  size_t i = 0;

  for (i = 0; i < a.count; ++i) {
    printf("%lu ", a.data[i]);
  }

  printf("\n");
  return 0;
}

static int verify_sorted(array a) {
  size_t i = 0;

  for (i = 1; i < a.count; ++i) {
    if (a.data[i] < a.data[i - 1]) {
      return -1;
    }
  }

  return 0;
}

int main(int argc, char *args[]) {
  uint64_t data[] = { 17, 9, 32, 9, 534, 5413, 243, 23, 29, 4839, 93, 90 };
  array a = { sizeof(data) / sizeof(uint64_t), data };
  print_array(a);
  radix_counting_sort(a);

  if (verify_sorted(a) != 0) {
    printf("NOT SORTED!\n");
  }

  print_array(a);
  printf("\n");

  return 0;
}
