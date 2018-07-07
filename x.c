/*
	   knoppix@Microknoppix:~$ gcc spritzenigma.c
	   knoppix@Microknoppix:~$ cat spritzenigma.c | ./a.out HASH > x
	   calculating hash function: 
	   D2BB4A8D96D4C5A81ABB47FAEDF9C6CB2705676DDFF2C396D52C9EAFA236
	   Finshed
	   knoppix@Microknoppix:~$ cat x | ./a.out HASH D > y
	   calculating hash function: 
	   D2BB4A8D96D4C5A81ABB47FAEDF9C6CB2705676DDFF2C396D52C9EAFA236
	   Finshed
	   knoppix@Microknoppix:~$ diff spritzenigma.c y
	   knoppix@Microknoppix:~$ gzip y
	   knoppix@Microknoppix:~$ gzip x
	   knoppix@Microknoppix:~$ ls -l
	   insgesamt 52
	   -rwxr-xr-x 1 knoppix knoppix  8835 Jul  3 15:48 a.out
	   drwxr-xr-x 2 knoppix knoppix    60 Jul  3 08:34 Desktop
	   -rw-r--r-- 1 knoppix knoppix   492 Jul  3 14:19 ftp.py
	   -rw-r--r-- 1 knoppix knoppix  6165 Jul  3 15:47 spritzenigma.c
	   -rw-r--r-- 1 knoppix knoppix 13895 Jul  3 15:02 spritz.html
	   -rw-r--r-- 1 knoppix knoppix  6190 Jul  3 15:48 x.gz
	   -rw-r--r-- 1 knoppix knoppix  1856 Jul  3 15:48 y.gz
	   knoppix@Microknoppix:~$ 
	   *************************************************
	   */
/**********************************************************************
 *  ** spritzenigma.c Source file for implementation of a PRNG driven   **
 *   ** Secure Rotor Cipher                                              **
 *    ** Franz Scherer Software Spritz Rotor Cipher                       **
 *     ** Created: 07/11/2015                                              **
 *      **********************************************************************
 *       */

/*
 *  ***********************************************************************
 *   ** Copyright (C) 2015 - 2016, Scheerer Software, All rights reserved.**
 *    **                                                                   **
 *     ** License to copy and use this software is granted provided that    **
 *      ** it is identified as the "Franz Scheerer Software, Rotor           **
 *       ** Cipher Algorithm" in all material mentioning or referencing this  **
 *        ** software or this function.                                        **
 *         **                                                                   **
 *          ** License is also granted to make and use derivative works          **
 *           ** provided that such works are identified as "derived from          **
 *            ** Franz Scheerer Software Rotor Cipher Algorithm" in all            **
 *             ** material mentioning or referencing the derived work.              **
 *              **                                                                   **
 *               ** Franz Scheerer Software makes no representations concerning       **
 *                ** either the merchantability of this software or the suitability    **
 *                 ** of this software for any particular purpose.  It is provided "as  **
 *                  ** is" without express or implied warranty of any kind.              **
 *                   **                                                                   **
 *                    ** These notices must be retained in any copies of any part of this  **
 *                     ** documentation and/or software.                                    **
 *                      ** June 2016 - Now with Sponge hash                                  **
 *                       ***********************************************************************
 *                        */


/*
 *  * compile: gcc spritzenigma.c
 *   * encrypt: cat data.dat | ./a.out secretX > data.crypt
 *    * decrypt: cat data.crypt | ./a.out secretX D > data.copy
 *     */

#include <stdio.h>
#include <string.h>
 
/* Spritz Cipher, slightly modified 
 *
 * Spritz:
 * (from https://www.schneier.com/blog/archives/2014/10/spritz_a_new_rc.html)
 *
 *     1: i = i + w
 *         2: j = k + S[j + S[i]]
 *             2a: k = i + k + S[j]
 *                 3: SWAP(S[i];S[j])
 *                     4: z = S[j + S[i + S[z + k]]]
 *                         5: Return z
 *                         */
unsigned char 
spritz(unsigned char *key, unsigned int key_length,
		       int add_j) {
	  static unsigned char S[256];
	    static unsigned int i,j,k,w,z;
	      int nrepeat,t;
	        
	        if (key_length > 0) /* initialize, if key_length greater than zero */
			  {
				      if (key_length == 1) 
					             for (i = 0; i < 256; i++) S[i] = i;
				          j = 0;
					      for (nrepeat=0; nrepeat<3; nrepeat++){
						            for (i = 0; i < 256; i++) {
								              j = (j + key[(i + nrepeat) % key_length] + S[i]) % 256;
									                t = S[i]; S[i] = S[j]; S[j]=t;
											      }
							        }
					          i = key[0] % 256;
						      j = (key_length + key[key_length-1]) % 256;
						          for (nrepeat=0; nrepeat<999; nrepeat++){
								          i = (i + 1) % 256;      
									          j = (j + S[i]) % 256;
										          t = S[i]; S[i] = S[j]; S[j]=t;      
											      }
							      i = S[42]; 
							          j = S[0]; k = S[8];
								      w = 2*S[15] + 1;
								          z = S[47] + S[11];
									     } /* end initialize */

		   i = (i + w) % 256;
		      j = (k + S[j + S[i]]) % 256;
		         k = (i + k + S[j]) % 256;
			    t = S[i]; S[i] = S[j]; S[j]=t;
			       z = S[(j + S[(i + S[(z + k) % 256]) % 256]) % 256];
			          j += add_j;
				     return z;        
}
 
 
int main(int narg, char **argv) {
	  unsigned char IS[256],  S[256];
	    unsigned char IR1[256], R1[256];
	      unsigned char IR2[256], R2[256];
	        unsigned char IR3[256], R3[256];
		  unsigned int i, j, k;
		    int t, c;
		      int addR1 = 17;

		        for (i = 0; i < 256; i++){
				      S[i] = i;
				            R1[i] = i;
					          R2[i] = i;
						        R3[i] = i;
							  }
			  if (narg == 1){
				        fprintf(stderr,"ERROR: no password\n");
					      return -1;     
					        } else {
							      spritz("1", strlen("1"),0);      
							            spritz(argv[1], strlen(argv[1]),0); 
								          spritz("Pass2", strlen("Pass2"),0);       
									    }
			    i = 256;  
			      while (i > 1) {
				           i = i - 1;
					        j = spritz("ENIGMA2", 0, 0);
						     while ( j > i ){
							            j = spritz("ENIGMA2", 0, 0);
								         }
						          t = S[i]; S[i] = S[j]; S[j] = t;
							    } 
			        i = 256;  
				  while (i > 1) {
					       i = i - 1;
					            j = spritz("ENIGMA2", 0, 0);
						         while ( j > i ){
								        j = spritz("ENIGMA2", 0, 0);
									     }
							      t = R1[i]; R1[i] = R1[j]; R1[j] = t;
							        } 
				    i = 256;  
				      while (i > 1) {
					           i = i - 1;
						        j = spritz("ENIGMA2", 0, 0);
							     while ( j > i ){
								            j = spritz("ENIGMA2", 0, 0);
									         }
							          t = R2[i]; R2[i] = R2[j]; R2[j] = t;
								    } 
				        i = 256;  
					  while (i > 1) {
						       i = i - 1;
						            j = spritz("ENIGMA2", 0, 0);
							         while ( j > i ){
									        j = spritz("ENIGMA2", 0, 0);
										     }
								      t = R3[i]; R3[i] = R3[j]; R3[j] = t;
								        }

					    for (i = 0; i < 256; i++){
						          IS[S[i]] = i;
							        IR1[R1[i]] = i;
								      IR2[R2[i]] = i;
								            IR3[R3[i]] = i;
									          /* printf("%d %d %d %d\n", S[i],R1[i],R2[i],R3[i]); */
									      }
					     
					         while ((c = fgetc(stdin)) != -1){
							         int t,i1,i2,i3;
								         i1 = (addR1 + spritz("ENIGMA2", 0, addR1)) % 256;
									         i2 = spritz("ENIGMA2", 0, addR1);
										         i3 = spritz("ENIGMA2", 0, addR1);
											         t = R1[i1]; R1[i1]=R1[i2];R1[i2]=t;
												         IR1[R1[i1]] = i1;
													         IR1[R1[i2]] = i2;
														         if (narg == 2){
																            int t1,t2,t3,t4;
																	               t1 = R1[(i1 + c) % 256];
																		                  t2 = R2[(t1 + i2) % 256];
																				             t3 = R3[(t2 + i3) % 256];
																					                t4 = S[t3];
																							           printf("%c", t4);  
																								              addR1 = t4;
																									              } else {
																											                  int t1,t2,t3,t4;
																													              t4 = c;
																														                  t3 = IS[t4];
																																              t2 = (256 + IR3[t3] - i3) % 256;
																																	                  t1 = (256 + IR2[t2] - i2) % 256;
																																			              printf("%c", (256 + IR1[t1] - i1) % 256);
																																				                  addR1 = t4;
																																						          }
															      }
						      fprintf(stderr,"calculating hash function: \n");
						           spritz("ENIGMA2", 0, addR1);
							        for (c=0; c<32; c++){
									        fprintf(stderr,"%X", spritz("ENIGMA2", 0, 0));
										     }
								     fprintf(stderr,"\nFinshed\n");

}


