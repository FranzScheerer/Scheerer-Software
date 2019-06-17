#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define N 256
#define NN 512
#define N2 128

unsigned char a, i, j, k, w, z, s[N];

#define LOW(B)  ((B) & 0xf)
#define HIGH(B) ((B) >> 4)

static void
initialize_state()
{
    for (int v = 0; v < N; v++) {
        s[v] = (unsigned char) v;
    }
    a = i = j = k = 0;
    w = 1;
    z = 0;
}

static void
update()
{
    unsigned char t,y;

    i += w;
    y = j + s[i];
    j = k + s[y];
    k = i + k 
             + s[j];
    t = s[i];
    s[i] = s[j];
    s[j] = t;
}

static unsigned char
output()
{
    const unsigned char y1 = z + k;
    const unsigned char x1 = i + s[y1];
    const unsigned char y2 = j + s[x1];

    z = s[y2];

    return z;
}

static void
whip()
{
    for (int v = 0; v < NN; v++) {
        update();
    }
    w += 2;
}

static void
shuffle()
{
    whip();
    a = 0;
}

static void
absorb_nibble(const unsigned char x)
{
    unsigned char t, y;

    if (a == N2) {
        shuffle();
    }
    y = N2 + x;
    t = s[a];
    s[a] = s[y];
    s[y] = t;
    a++;
}

static void
absorb_byte(const unsigned char b)
{
    absorb_nibble(LOW(b));
    absorb_nibble(HIGH(b));
}

static unsigned char
drip()
{
    if (a > 0) {
        shuffle();
    }
    update();

    return output();
}

static void
squeeze(unsigned char *out, size_t outlen)
{
    if (a > 0) {
        shuffle();
    }
    for (size_t v = 0; v < outlen; v++) {
        out[v] = drip();
    }
}

int main(){
  int ii,c;

  unsigned char out[32];
  
  printf("compile source: gcc h.c :\n");
  printf("cat file | ./a.out:\n\n");
  printf("hash (SPRITZ-256):\n");
  initialize_state();
  while ( (c = fgetc(stdin)) != -1 ){
    absorb_byte((unsigned char) c);
  }
  squeeze(out, 32);
  for (ii=0; ii<32; ii++)  
     printf("%02x", out[ii]);
  printf("\n");
}