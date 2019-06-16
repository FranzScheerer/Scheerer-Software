#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define N 256
#define NN 512
#define N2 128

typedef struct {
    unsigned char a, i, j, k, w, z, s[N];
} State;

#define LOW(B)  ((B) & 0xf)
#define HIGH(B) ((B) >> 4)

static void
initialize_state(State *state)
{
    unsigned int v;

    for (v = 0; v < N; v++) {
        state->s[v] = (unsigned char) v;
    }
    state->a = state->i = state->j = state->k = 0;
    state->w = 1;
    state->z = 0;
}

static void
update(State *state)
{
    unsigned char t,y;

    state->i += state->w;
    y = state->j + state->s[state->i];
    state->j = state->k + state->s[y];
    state->k = state->i + state->k 
             + state->s[state->j];
    t = state->s[state->i];
    state->s[state->i] = state->s[state->j];
    state->s[state->j] = t;
}

static unsigned char
output(State *state)
{
    const unsigned char y1 = state->z + state->k;
    const unsigned char x1 = state->i + state->s[y1];
    const unsigned char y2 = state->j + state->s[x1];

    state->z = state->s[y2];

    return state->z;
}

static void
whip(State *state)
{
    for (int v = 0; v < NN; v++) {
        update(state);
    }
    state->w += 2;
}

static void
shuffle(State *state)
{
    whip(state);
    state->a = 0;
}


static void
absorb_nibble(State *state, const unsigned char x)
{
    unsigned char t, y;

    if (state->a == N2) {
        shuffle(state);
    }
    y = N2 + x;
    t = state->s[state->a];
    state->s[state->a] = state->s[y];
    state->s[y] = t;
    state->a++;
}

static void
absorb_byte(State *state, const unsigned char b)
{
    absorb_nibble(state, LOW(b));
    absorb_nibble(state, HIGH(b));
}

static unsigned char
drip(State *state)
{
    if (state->a > 0) {
        shuffle(state);
    }
    update(state);

    return output(state);
}

static void
squeeze(State *state, unsigned char *out, size_t outlen)
{
    if (state->a > 0) {
        shuffle(state);
    }
    for (size_t v = 0; v < outlen; v++) {
        out[v] = drip(state);
    }
}




int main(){
  int ii,c;
  State state;

  unsigned char out[32];
  
  printf("compile source: gcc h.c :\n");
  printf("cat file | ./a.out:\n\n");
  printf("hash (SPRITZ-256):\n");
  initialize_state(&state);
  while ( (c = fgetc(stdin)) != -1 ){
    absorb_byte(&state, (unsigned char) c);
  }
  squeeze(&state, out, 32);
  for (ii=0; ii<32; ii++)  
     printf("%02x", out[ii]);
  printf("\n");
}