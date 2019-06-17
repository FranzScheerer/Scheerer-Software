#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define N 256
#define NN 512
#define N2 128

int a, i, j, k, w, z, s[N];

int initialize_state()
{
    for (int v = 0; v < N; v++) {
        s[v] = v;
    }
    a = i = j = k = 0;
    w = 1;
    z = 0;
    return 0;
}

void update()
{
    int t,y;

    i = (i + w) % 256;
    y = (j + s[i]) % 256;
    j = (k + s[y]) % 256;
    k = (i + k + s[j]) % 256;
    t = s[i];
    s[i] = s[j];
    s[j] = t;
}

int output()
{
    int y1 = (z + k) % 256;
    int x1 = (i + s[y1]) % 256;
    int y2 = (j + s[x1]) % 256;

    z = s[y2];
    return z;
}

int whip()
{
    for (int v = 0; v < NN; v++) {
        update();
    }
    w = (w + 2) % 256;
    return 0;
}


int shuffle()
{
    whip();
    a = 0;
    return 0;
}


void absorb_nibble(int x)
{
    int t, y;

    if (a == N2) {
        shuffle();
    }
    y = (N2 + x) % 256;
    t = s[a];
    s[a] = s[y];
    s[y] = t;
    a++;
}

void absorb_byte(int b)
{
    absorb_nibble(b % 16);
    absorb_nibble(b / 16);
}

int drip()
{
    if (a > 0) {
        shuffle();
    }
    update();
    return output();
}

void squeeze(char *out, size_t outlen)
{
    if (a > 0) {
        shuffle();
    }
    for (int v = 0; v < outlen; v++) {
        out[v] = drip();
    }
}

int main(){
  int c, ii;
  unsigned char out[32];
  
  printf("compile source: gcc h.c \n");
  printf("cat file | ./a.out\n\n");
  printf("hash (SPRITZ-256)\n");
  initialize_state();
  while ( (c = fgetc(stdin)) != -1 ){
    absorb_byte(c);
  }
  squeeze(out, 32);
  for (ii=0; ii<32; ii++)  
     printf("%02x", out[ii]);
  printf("\n");
}