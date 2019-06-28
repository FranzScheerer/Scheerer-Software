#include <stdio.h>

#define SWAP(A,B) tmp = A; A = B; B = tmp;

int a = 0, i = 0, j = 0, w = 1, s[256];
int tmp;

void update()
{
    i = (i + w) % 256;
    j = s[(j + s[i]) % 256];
    SWAP(s[i], s[j])
}

void absorb_nibble(int x)
{
    if (a == 240){ 
       for (int v = 0; v < 256; v++) 
           update();
       w = (w + 2) % 256;
       a = 0;
    }
    SWAP(s[a], s[x+240])
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
   if (a != 0){ 
       for (int v = 0; v < 256; v++) 
           update();
       w = (w + 2) % 256;
       a = 0;
   }    
   for (int v = 0; v < outlen; v++) 
       out[v] = output();
}

int main(){
  int c,o;
  unsigned char key[] = "64babb0d78945ea197adb45496010636bd635a3c59dfcb8b2ff66a301f82fa084ff7a14295f3275f9526f38f72bd28cd0315afd71d91a516a68a726d78e8b82b";
  unsigned char out[32];
  
  fprintf(stderr,"hash ");

  for (int v = 0; v < 256; v++) 
      s[v] = v;
  c = 0;
  while ( key[c++] ){
    absorb_byte(key[c]);
  }
  while ( (c = fgetc(stdin)) != -1 ){
    update();
    o = c ^ s[j];
    absorb_byte(o);
    fputc((unsigned char)o, stdout);
  }
  squeeze(out, 32);
  for (int ii=0; ii<32; ii++)  
     fprintf(stderr,"%x", out[ii]);
  fprintf(stderr, "\n");
}