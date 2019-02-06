/*
 * Copyright Notice

 *  Copyright (C) The Internet Society (2001).  All Rights Reserved.
 *
 * Modified by Franz Scheerer in 2016
 * Compare: https://tools.ietf.org/html/rfc3174
 * This is a modified Version of SHA80 with 80 magic constants as used in SHA-2.
 * Besides the 80 constants the source code is widely unchanged compared to RFC 3174. 
 * But all text is included in this single file. The only file that is required.
 *
 * Compile (example)
 * 
 * gcc SHA80.c
 * Usage: ./a.out file
 * Calculates checksum of file
1. Overview of Contents


   NOTE: The text below is mostly taken from [FIPS 180-1] and assertions
   therein of the security of SHA-1 are made by the US Government, the
   author of [FIPS 180-1], and not by the authors of this document.

   This variant SHA80 is very likely to be at least as secure as SHA-1 using
   only four different values for K[t = 0..79].

   This document specifies a variant of the Secure Hash Algorithm, SHA-1, 
   with 80 different constants K[t] the first 32 bits of the fractional parts
   of the cube roots of the first 80 primes 2, 3, 5, 7, ... ,409.

   A condensed representation of a message or a data file is calculated.  
   A message of any length smaller 2^64 bits is input, the SHA-1 varaint
   produces a 160-bit output called a message digest. It can then,
   for example, be input to a signature algorithm which generates or
   verifies the signature for the message.  Signing the message digest
   rather than the message often improves the efficiency of the process
   because the message digest is usually much smaller in size than the
   message.  The same hash algorithm must be used by the verifier of a
   digital signature as was used by the creator of the digital
   signature.  Any change to the message in transit will, with very high
   probability, result in a different message digest, and the signature
   will fail to verify.
*/


#include <stdint.h>
/*
 * If you do not have the ISO standard stdint.h header file, then you
 * must typdef the following:
 *    name              meaning
 *  uint32_t         unsigned 32 bit integer
 *  uint8_t          unsigned 8 bit integer (i.e., unsigned char)
 *  int_least16_t    integer of >= 16 bits
 *
 */

#define sha80HashSize 20

/*
 *  This structure will hold context information for the SHA80
 *  hashing operation
 */
typedef struct sha80Context
{
    uint32_t Intermediate_Hash[sha80HashSize/4]; /* Message Digest  */

    uint32_t Length_Low;            /* Message length in bits      */
    uint32_t Length_High;           /* Message length in bits      */

                               /* Index into message block array   */
    int_least16_t Message_Block_Index;
    uint8_t Message_Block[64];      /* 512-bit message blocks      */

    int Computed;               /* Is the digest computed?         */
    int Corrupted;             /* Is the message digest corrupted? */
} sha80Context;



int sha80Reset(  sha80Context *);
int sha80Input(  sha80Context *,
                const uint8_t *,
                unsigned int);
int sha80Result( sha80Context *,
                uint8_t Message_Digest[sha80HashSize]);

void sha80ProcessMessageBlock(sha80Context *context);
void sha80PadMessage(sha80Context *context);


#define sha80CircularShift(bits,word) \
                (((word) << (bits)) | ((word) >> (32-(bits))))

int sha80Reset(sha80Context *context)
{
    if (!context)
    {
        return 1;
    }

    context->Length_Low             = 0;
    context->Length_High            = 0;
    context->Message_Block_Index    = 0;

    context->Intermediate_Hash[0]   = 0x67452301;
    context->Intermediate_Hash[1]   = 0xEFCDAB89;
    context->Intermediate_Hash[2]   = 0x98BADCFE;
    context->Intermediate_Hash[3]   = 0x10325476;
    context->Intermediate_Hash[4]   = 0xC3D2E1F0;

    context->Computed   = 0;
    context->Corrupted  = 0;

    return 0;
}

int sha80Result( sha80Context *context,
                uint8_t Message_Digest[sha80HashSize])
{
    int i;

    if (!context || !Message_Digest)
    {
        return 1;
    }

    if (context->Corrupted)
    {
        return context->Corrupted;
    }

    if (!context->Computed)
    {
        sha80PadMessage(context);
        for(i=0; i<64; ++i)
        {
            /* message may be sensitive, clear it out */
            context->Message_Block[i] = 0;
        }
        context->Length_Low = 0;    /* and clear length */
        context->Length_High = 0;
        context->Computed = 1;
    }

    for(i = 0; i < sha80HashSize; ++i)
    {
        Message_Digest[i] = context->Intermediate_Hash[i>>2]
                            >> 8 * ( 3 - ( i & 0x03 ) );
    }

    return 0;
}

int sha80Input(    sha80Context    *context,
                  const uint8_t  *message_array,
                  unsigned       length)
{
    if (context->Computed)
    {
        context->Corrupted = 3;
        return 3;
    }

    if (context->Corrupted)
    {
         return context->Corrupted;
    }
    while(length-- && !context->Corrupted)
    {
    context->Message_Block[context->Message_Block_Index++] =
                    (*message_array & 0xFF);

    context->Length_Low += 8;
    if (context->Length_Low == 0)
    {
        context->Length_High++;
        if (context->Length_High == 0)
        {
            /* Message is too long */
            context->Corrupted = 1;
        }
    }

    if (context->Message_Block_Index == 64)
    {
        sha80ProcessMessageBlock(context);
    }

    message_array++;
    }

    return 0;
}

void sha80ProcessMessageBlock(sha80Context *context)
{
    const uint32_t K[80] =    {       
/* Constants as defined in SHA-2   */
0x428a2f98,
0x71374491,
0xb5c0fbcf,
0xe9b5dba5,
0x3956c25b,
0x59f111f1,
0x923f82a4,
0xab1c5ed5,
0xd807aa98,
0x12835b01,
0x243185be,
0x550c7dc3,
0x72be5d74,
0x80deb1fe,
0x9bdc06a7,
0xc19bf174,
0xe49b69c1,
0xefbe4786,
0xfc19dc6,
0x240ca1cc,
0x2de92c6f,
0x4a7484aa,
0x5cb0a9dc,
0x76f988da,
0x983e5152,
0xa831c66d,
0xb00327c8,
0xbf597fc7,
0xc6e00bf3,
0xd5a79147,
0x6ca6351,
0x14292967,
0x27b70a85,
0x2e1b2138,
0x4d2c6dfc,
0x53380d13,
0x650a7354,
0x766a0abb,
0x81c2c92e,
0x92722c85,
0xa2bfe8a1,
0xa81a664b,
0xc24b8b70,
0xc76c51a3,
0xd192e819,
0xd6990624,
0xf40e3585,
0x106aa070,
0x19a4c116,
0x1e376c08,
0x2748774c,
0x34b0bcb5,
0x391c0cb3,
0x4ed8aa4a,
0x5b9cca4f,
0x682e6ff3,
0x748f82ee,
0x78a5636f,
0x84c87814,
0x8cc70208,
0x90befffa,
0xa4506ceb,
0xbef9a3f7,
0xc67178f2,
0xca273ece,
0xd186b8c7,
0xeada7dd6,
0xf57d4f7f,
0x6f067aa,
0xa637dc5,
0x113f9804,
0x1b710b35,
0x28db77f5,
0x32caab7b,
0x3c9ebe0a,
0x431d67c4,
0x4cc5d4be,
0x597f299c,
0x5fcb6fab,
0x6c44198c
                            };
    int           t;                 /* Loop counter                */
    uint32_t      temp;              /* Temporary word value        */
    uint32_t      W[80];             /* Word sequence               */
    uint32_t      A, B, C, D, E;     /* Word buffers                */

    /*
     *  Initialize the first 16 words in the array W
     */
    for(t = 0; t < 16; t++)
    {
        W[t] = context->Message_Block[t * 4] << 24;
        W[t] |= context->Message_Block[t * 4 + 1] << 16;
        W[t] |= context->Message_Block[t * 4 + 2] << 8;
        W[t] |= context->Message_Block[t * 4 + 3];
    }

    for(t = 16; t < 80; t++)
    {
       W[t] = sha80CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = context->Intermediate_Hash[0];
    B = context->Intermediate_Hash[1];
    C = context->Intermediate_Hash[2];
    D = context->Intermediate_Hash[3];
    E = context->Intermediate_Hash[4];

    for(t = 0; t < 20; t++)
    {
        temp =  sha80CircularShift(5,A) +
                ((B & C) | ((~B) & D)) + E + W[t] + K[t];
        E = D;
        D = C;
        C = sha80CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 20; t < 40; t++)
    {
        temp = sha80CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[t];
        E = D;
        D = C;
        C = sha80CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 40; t < 60; t++)
    {
        temp = sha80CircularShift(5,A) +
               ((B & C) | (B & D) | (C & D)) + E + W[t] + K[t];
        E = D;
        D = C;
        C = sha80CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 60; t < 80; t++)
    {
        temp = sha80CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[t];
        E = D;
        D = C;
        C = sha80CircularShift(30,B);
        B = A;
        A = temp;
    }

    context->Intermediate_Hash[0] += A;
    context->Intermediate_Hash[1] += B;
    context->Intermediate_Hash[2] += C;
    context->Intermediate_Hash[3] += D;
    context->Intermediate_Hash[4] += E;

    context->Message_Block_Index = 0;
}

void sha80PadMessage(sha80Context *context)
{
    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second
     *  block.
     */
    if (context->Message_Block_Index > 55)
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while(context->Message_Block_Index < 64)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }

        sha80ProcessMessageBlock(context);

        while(context->Message_Block_Index < 56)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }
    else
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while(context->Message_Block_Index < 56)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }

    /*
     *  Store the message length as the last 8 octets
     */
    context->Message_Block[56] = context->Length_High >> 24;
    context->Message_Block[57] = context->Length_High >> 16;
    context->Message_Block[58] = context->Length_High >> 8;
    context->Message_Block[59] = context->Length_High;
    context->Message_Block[60] = context->Length_Low >> 24;
    context->Message_Block[61] = context->Length_Low >> 16;
    context->Message_Block[62] = context->Length_Low >> 8;
    context->Message_Block[63] = context->Length_Low;

    sha80ProcessMessageBlock(context);
}

/*
 *  sha80test
 *
 *  Description:
 *      This file will exercise the SHA80 code performing the three
 *      tests documented in FIPS PUB 180-1 plus one which calls
 *      sha80Input with an exact multiple of 512 bits, plus a few
 *      error test checks.
 *
 *  Portability Issues:
 *      None.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*
 *  Define patterns for testing
 */
#define TEST1   "abc"
#define TEST2a  "abcdbcdecdefdefgefghfghighijhi"
#define TEST2b  "jkijkljklmklmnlmnomnopnopq"
#define TEST2   TEST2a TEST2b
#define TEST3   "a"
#define TEST4a  "01234567012345670123456701234567"
#define TEST4b  "01234567012345670123456701234567"
    /* an exact multiple of 512 bits */
#define TEST4   TEST4a TEST4b
char *testarray[4] =
{
    TEST1,
    TEST2,
    TEST3,
    TEST4
};
long int repeatcount[4] = { 1, 1, 1000000, 10 };
char *resultarray[4] =
{
    "A9 99 3E 36 47 06 81 6A BA 3E 25 71 78 50 C2 6C 9C D0 D8 9D",
    "84 98 3E 44 1C 3B D2 6E BA AE 4A A1 F9 51 29 E5 E5 46 70 F1",
    "34 AA 97 3C D4 C4 DA A4 F6 1E EB 2B DB AD 27 31 65 34 01 6F",
    "DE A3 56 A2 CD DD 90 C7 A7 EC ED C5 EB B5 63 93 4F 46 04 52"
};

int main(int argn, char *argv[])
{
    sha80Context sha;
    int i, j, err;
    int c;
    uint8_t Message_Digest[20];
    sha80Reset(&sha);
    if (argn > 1){
      FILE *fp;
      fp = fopen(argv[1],"r");
      if (!fp) fprintf(stderr,"FILE NOT FOUND %s\n",argv[1]);
      while ((c = fgetc(fp)) != -1){
         sha80Input(&sha, (unsigned char*)&c, 1);
      }
      fclose(fp);
      sha80Result(&sha, Message_Digest);
      for(i = 0; i < 20 ; ++i) 
      {
          fprintf(stderr, "%02X ", Message_Digest[i]);
      }
      fprintf(stderr,"\n\n");
    } else {
    /*
     *  Perform SHA80 tests
     */
    for(j = 0; j < 4; ++j)
    {
        printf("ERROR Usage: ./a.out file");
        printf( "\nTest %d: %d, '%s'\n",
                j+1,
                repeatcount[j],
                testarray[j]);

        err = sha80Reset(&sha);
        if (err)
        {
            fprintf(stderr, "sha80Reset Error %d.\n", err );
            break;    /* out of for j loop */
        }

        for(i = 0; i < repeatcount[j]; ++i)
        {
            err = sha80Input(&sha,
                  (const unsigned char *) testarray[j],
                  strlen(testarray[j]));
            if (err)
            {
                fprintf(stderr, "sha80Input Error %d.\n", err );
                break;    /* out of for i loop */
            }
        }

        err = sha80Result(&sha, Message_Digest);
        if (err)
        {
            fprintf(stderr,
            "sha80Result Error %d, could not compute message digest.\n",
            err );
        }
        else
        {
            printf("\t");
            for(i = 0; i < 20 ; ++i)
            {
                printf("%02X ", Message_Digest[i]);
            }
            printf("\n");
        }
        printf("SHA-1:\n");
        printf("\t%s\n", resultarray[j]);
    }

    /* Test some error returns */
    err = sha80Input(&sha,(const unsigned char *) testarray[1], 1);
    printf ("\nError %d. Should be %d.\n", err, 3 );
    err = sha80Reset(0);
    printf ("\nError %d. Should be %d.\n", err, 1 );
    return 0;
   }
}
