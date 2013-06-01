/**********************************************************************
Copyright (c) 1991 MPEG/audio software simulation group, All Rights Reserved
huffman.h
**********************************************************************/
/**********************************************************************
 * MPEG/audio coding/decoding software, work in progress              *
 *   NOT for public distribution until verified and approved by the   *
 *   MPEG/audio committee.  For further information, please contact   *
 *   Chad Fogg email: <cfogg@xenon.com>                               *
 *                                                                    *
 * VERSION 4.1                                                        *
 *   changes made since last update:                                  *
 *   date   programmers                comment                        *
 *  27.2.92 F.O.Witte (ITT Intermetall)				      *
 *  8/24/93 M. Iwadare          Changed for 1 pass decoding.          *
 *  7/14/94 J. Koller		useless 'typedef' before huffcodetab  *
 *				removed				      *
 *********************************************************************/	
 
#define HUFFBITS unsigned long int
#define HTN	34
#define MXOFF	250
 
struct huffcodetab {
  char tablename[3];	/*string, containing table_description	*/
  unsigned int xlen; 	/*max. x-index+			      	*/ 
  unsigned int ylen;	/*max. y-index+				*/
  unsigned int linbits; /*number of linbits			*/
  unsigned int linmax;	/*max number to be stored in linbits	*/
  int ref;		/*a positive value indicates a reference*/
  HUFFBITS *table;	/*pointer to array[xlen][ylen]		*/
  unsigned char *hlen;	/*pointer to array[xlen][ylen]		*/
  unsigned char(*val)[2];/*decoder tree				*/ 
  unsigned int treelen;	/*length of decoder tree		*/
};

extern struct huffcodetab ht[HTN];/* global memory block		*/
				/* array of all huffcodtable headers	*/
				/* 0..31 Huffman code table 0..31	*/
				/* 32,33 count1-tables			*/
#ifdef PROTO_ARGS

extern int read_huffcodetab(FILE *); 
extern int read_decoder_table(FILE *);
 
extern void huffman_coder(unsigned int, unsigned int,
			  struct huffcodetab *, Bit_stream_struc *);
			  
extern int huffman_decoder(struct huffcodetab *,
			   /* unsigned */ int *, /* unsigned */ int*, int*, int*);

#else

extern int read_huffcodetab(); 
extern int read_decoder_table(); 
extern void huffman_coder();
extern int huffman_decoder();

#endif
