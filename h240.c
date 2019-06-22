#include <stdio.h>
#include <stddef.h>
#include <string.h>

int a = 0, i = 0, j = 0, w = 1, s[240];

void update()
{
    int tmp;
    i = (i + w) % 256;
    j = s[(j + s[i]) % 256];
    tmp = s[i];
    s[i] = s[j];
    s[j] = tmp;
}

void shuffle()
{
    for (int v = 0; v < 256; v++) {
        update();
    }
    w = (w + 2) % 256;
    a = 0;
}

void absorb_nibble(int x)
{
    int tmp;

    if (a == 240) 
        shuffle();
    tmp = s[a];
    s[a] = s[240 + x];
    s[240 + x] = tmp;
    a++;
}

void absorb_byte(int b)
{
    absorb_nibble(b % 16);
    absorb_nibble(b / 16);
}

int output()
{
    update();
    return s[j];
}

void squeeze(char *out, size_t outlen)
{
    if (a != 0) 
        shuffle();
    for (int v = 0; v < outlen; v++) 
        out[v] = output();
}

int main(){
  int c;
  unsigned char out[32];
  
  printf("compile source: gcc h240.c \n");
  printf("cat file | ./a.out\n\n");
  printf("hash-240\n");

  for (int v = 0; v < 256; v++) 
      s[v] = v;

  while ( (c = fgetc(stdin)) != -1 ){
    absorb_byte(c);
  }
  squeeze(out, 32);
  for (int ii=0; ii<32; ii++)  
     printf("%x", out[ii]);
  printf("\n");
}