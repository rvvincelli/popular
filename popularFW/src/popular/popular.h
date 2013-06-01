//Algorithm parametrizations and structures

#define		BLOCKLENGTH		11
#define		NNUFREQS		66
#define		NNUBANDS		9
#define     SBLIMIT         32
#define     SSLIMIT         18

//lines per subband:
const int newbands[NNUBANDS] = {4, 4, 4, 4, 6, 8, 10, 12, 14};
//frequency lines indices defining single subbands:
const int newbandsbounds[NNUBANDS][2] = {
		{0, 3},
		{4, 7},
		{8, 11},
		{12, 15},
		{16, 21},
		{22, 29},
		{30, 39},
		{40, 51},
		{52, 65}
};

//A block is made up of 44 granules if the .mp3 is stereo, 22 otherwise; a
//single granule has 576 values; the first 2 is the stereo value, we assume it
//is always stereo as we explicitly pass the parameter anyway. As the authors
//didn't make a distinction between mono and stereo, in case of a stereo mp3 we
//let the block have a doubled number of granules, ie 22 granules per channel.

double block[BLOCKLENGTH][2][2][SBLIMIT][SSLIMIT];
unsigned blocktypes[BLOCKLENGTH][2][2]; //long, short etc
double ogr[SBLIMIT][SSLIMIT]; //overlapping granule scores
unsigned ogrtype[2][2]; //overlapping granule type
double newfreqs[BLOCKLENGTH][2][2][NNUFREQS]; //rearrangement
double sbes[2][NNUBANDS]; //sub band energy per block
double H[2]; //entropy container
double Hbuf[2][2]; //entropy buffer
int bits[2]; //bitstream buffer
int ready = 0; //flag

//Algorithm functions. RVV

void rearranger(
		double block[BLOCKLENGTH][2][2][SBLIMIT][SSLIMIT],
		int stereo,
		unsigned blocktypes[BLOCKLENGTH][2][2],
		double newfreqs[BLOCKLENGTH][2][2][NNUFREQS]
) {

	//TESTED
	//This function implements both the alignment and division as we keep going
	//until we reached the exact number of new frequency lines (if I got the
	//algorithm right - the authors are pretty ambiguous and they don't return
	//my mails).
	//It's a good idea to recall the order of the coefficients by block type:
	//long: 32 subbands, 18 freqs (we got [SBLIMIT, SSLIMIT])
	//short: 3 windows, 32 subbands, 6 freqs (ie we got [0][0,...,5] standing
	//for the freqs of the 1st subband, 1st win).
	//Anyway, we always output nnfreqs new frequency lines, regardless of the
	//block type!
	//Also here, we relax parametrization a little bit (ie how many granules we
	//group together is hardcoded in the for())
	//Cycling is neat if we iterate over the new frequency lines, but as the
	//input block[] is of form [subbands][lines] we have to fool around with % to
	//index block[] properly.

	int sfreqs = SSLIMIT/3;
	int i, j, k, l, c;
	for (i=0; i<BLOCKLENGTH; i++)
		for (j=0; j<stereo; j++)
			for (k=0; k<2; k++) {
				c=0;
				for (l=0; l<NNUFREQS; l++)
					if (blocktypes[i][j][k]==2) {
						if (l!=0 && l%sfreqs==0)
							c++;
						newfreqs[i][j][k][l]=(
								fabs(block[i][j][k][c][l%sfreqs])+
								fabs(block[i][j][k][c][(l%sfreqs)+sfreqs])+
								fabs(block[i][j][k][c][(l%sfreqs)+2*sfreqs])
						)/3;
					}
					else {
						if (l!=0 && l%sfreqs==0)
							c++;
						newfreqs[i][j][k][l]=(
								fabs(block[i][j][k][c][(3*l)%SSLIMIT])+
								fabs(block[i][j][k][c][(3*l+1)%SSLIMIT])+
								fabs(block[i][j][k][c][(3*l+2)%SSLIMIT])
						)/3;
					}
			}
}

void SBE(
		double newfreqs[BLOCKLENGTH][2][2][NNUFREQS],
		int stereo,
		double sbes[2][NNUBANDS]
) {

	//TESTED
	//First we obtain the energy of every subband for each single granule, then
	//we compute the energy of the whole single blocks, recallin that a block is
	//composed of BLOCKLENGTH frames as a frame is split into two granules.

	double granulesum[BLOCKLENGTH][stereo][2][NNUBANDS];
	int i, j, k, l, m;
	for (i=0; i<BLOCKLENGTH; i++)
		for (j=0; j<stereo; j++)
			for (k=0; k<2; k++)
				for (l=0; l<NNUBANDS; l++) {
					granulesum[i][j][k][l]=0;
					if (l!=NNUBANDS-1)
						for (
								m=newbandsbounds[l][0];
								m<newbandsbounds[l+1][0];
								m++
						)
							granulesum[i][j][k][l]=
									granulesum[i][j][k][l]+newfreqs[i][j][k][m];
					else
						for (m=newbandsbounds[l][0]; m<NNUFREQS; m++)
							granulesum[i][j][k][l]=
									granulesum[i][j][k][l]+newfreqs[i][j][k][m];
				}
	for (i=0; i<stereo; i++)
		for (j=0; j<NNUBANDS; j++) {
			sbes[i][j]=0;
			for (l=0; l<BLOCKLENGTH; l++)
				for (k=0; k<2; k++)
					sbes[i][j]=sbes[i][j]+granulesum[l][i][k][j];
		}
}

void PMFlike(double sbes[2][9], int stereo) {

	//TESTED
	//The pseudo-distribution is computed in sbes[]. If the denominator is zero,
	//then every band is zero as they are absolute values (see rearranger()),
	//and, as the authors didn't consider this case, we build an uniform distro.


	double sums[stereo];
	int j, k;
	for (j=0; j<stereo; j++) {
		sums[j]=0;
		for (k=0; k<NNUBANDS; k++)
			sums[j]=sums[j]+sbes[j][k];
	}
	for (j=0; j<stereo; j++)
		for (k=0; k<NNUBANDS; k++)
			if (sums[j]!=0)
				sbes[j][k]=sbes[j][k]/sums[j];
			else
				sbes[j][k]=1/NNUBANDS;
}

void entropy(double sbes[2][NNUBANDS], int stereo, double H[2]) {

	//TESTED
	//Simply returns the shannon bit entropy of the distribution sbes[].

	int j, k;
	for (j=0; j<stereo; j++)
		for (k=0; k<NNUBANDS; k++)
			sbes[j][k]=sbes[j][k]*log2(sbes[j][k]);
	for (j=0; j<stereo; j++) {
		H[j]=0;
		for (k=0; k<NNUBANDS; k++)
			H[j]=H[j]+sbes[j][k];
		H[j]=-H[j];
	}
}

void bufbit(
		int blockNum,
		double H[2],
		int stereo,
		double Hbuf[2][2],
		int bits[2]
) {

	//TESTED
	//This function is responsible for updating the entropies buffer Hbuf and
	//computing the bitstream bits.
	//If the block number is even we insert the entropy in position 0, 1 if odd;
	//for this reason the rule for generating a bit has two cases. No bits are
	//emitted for the first block.

	int j;
	for (j=0; j<stereo; j++) {
		Hbuf[blockNum%2][j]=H[j];
		if (blockNum != 0) {//screw you gcc and your ambiguousities
			if (blockNum%2 == 0)
				if (Hbuf[1][j]<Hbuf[0][j])
					bits[j]=0;
				else
					bits[j]=1;
			else
				if (Hbuf[0][j]<Hbuf[1][j])
					bits[j]=0;
				else
					bits[j]=1;
		}
	}
}
