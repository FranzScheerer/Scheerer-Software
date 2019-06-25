#include <stdio.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    unsigned char s[256];
    unsigned char a;
    unsigned char i;
    unsigned char j;
    unsigned char w;
} Spritz_t;

#define LOW(B)  ((B) & 0xf)
#define HIGH(B) ((B) >> 4)

static void
initialize_state(Spritz_t *state)
{
    unsigned int v;

    for (v = 0; v < 256; v++) {
        state->s[v] = (unsigned char) v;
    }
    state->a = 0;
    state->i = 0;
    state->j = 0;
    state->w = 1;
}

static void
update(Spritz_t *state)
{
    unsigned char t;
    unsigned char y;

    state->i += state->w;
    y = state->j + state->s[state->i];
    state->j = state->s[y];
    t = state->s[state->i];
    state->s[state->i] = state->s[state->j];
    state->s[state->j] = t;
}

static unsigned char
output(Spritz_t *state)
{
    return state->s[state->j];
}

static void
whip(Spritz_t *state)
{
    unsigned int       v;

    for (v = 0; v < 256; v++) {
        update(state);
    }
    state->w += 2;
}

static void
shuffle(Spritz_t *state)
{
    whip(state);
    state->a = 0;
}

static void
absorb_nibble(Spritz_t *state, const unsigned char x)
{
    unsigned char t;
    unsigned char y;

    if (state->a == 240) {
        shuffle(state);
    }
    y = 240 + x;
    t = state->s[state->a];
    state->s[state->a] = state->s[y];
    state->s[y] = t;
    state->a++;
}

static void
absorb_byte(Spritz_t *state, const unsigned char b)
{
    absorb_nibble(state, LOW(b));
    absorb_nibble(state, HIGH(b));
}

static void
absorb(Spritz_t *state, const unsigned char *msg, size_t length)
{
    size_t v;

    for (v = 0; v < length; v++) {
        absorb_byte(state, msg[v]);
    }
}

static unsigned char
drip(Spritz_t *state)
{
    if (state->a > 0) {
        shuffle(state);
    }
    update(state);

    return output(state);
}

static void
squeeze(Spritz_t *state, unsigned char *out, size_t outlen)
{
    size_t v;

    if (state->a > 0) {
        shuffle(state);
    }
    for (v = 0; v < outlen; v++) {
        out[v] = drip(state);
    }
}

int main(){
  int ii,c;
  Spritz_t state;

  unsigned char out[32];
  
  printf("gcc std_spritz.c :\n");
  printf("cat file | ./a.out:\n\n");
  printf("hash (SPRITZ-256):\n");
  initialize_state(&state);
  while ( (c = fgetc(stdin)) != -1 ){
    absorb_byte(&state, (unsigned char) c);
  }
  squeeze(&state, out, 32);
  for (ii=0; ii<32; ii++)  
     printf("%x", out[ii]);
  printf("\n");
}