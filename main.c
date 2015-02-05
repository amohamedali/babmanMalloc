#include <sys/types.h>
#include <unistd.h>

#define align4(x) (((((x)-1)>>2)<<2)+4)
#define BLOCK_SIZE 12

typedef struct s_meta *t_meta;

struct s_meta {
  size_t size;
  int free;
  t_meta next;
  char data[1];
};

void *base = NULL;

void split_block(t_meta base, size_t size) {
  t_meta new;
  new = base->data + size;
  new->size = base->size - size - BLOCK_SIZE;
  new->next = base->next;
  new->free = 1;
  base->size = size;
  base->next = new;
}

t_meta find_block(t_meta *last, size_t size){
  t_meta b = base;
  while (b && !(b->free && b->size >= size)) {
    *last = b;
    b = b->next;
  }
  return (b);
}

t_meta extend_heap(t_meta last, size_t size) {
  t_meta b;
  b = sbrk(0);
  if (sbrk(BLOCK_SIZE + size) == (void*)-1)
    return (NULL);
  b->size = size;
  b->next = NULL;
  if (last)
    last->next = b;
  b->free = 0;
  return (b);
}

void *my_malloc(size_t size) {
  t_meta b, last;
  size_t s;

  printf("okok\n");
  s = align4(size);
  if (base) {
    last = base;
    b = find_block(&last, s);
    if (b) {
      if ((b->size - s) >= (BLOCK_SIZE + 4))
	split_block(b, s);
      b->free = 0;
    } else {
      b = extend_heap(last, s);
      if (!b)
	return (NULL);
    }
  } else {
    b = extend_heap(NULL, s);
    if (!b)
      return (NULL);
    base = b;
  }
  return (b->data);
}

int main(){
    printf("okok0\n");
  my_malloc(50);
  return (0);
}
