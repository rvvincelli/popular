//Implementation of the fingerprint matching algorithm.
//Given a sample, for each fingerprint in the db we compute their BER and we
//save the result if it is less than a threshold constant; once we're done we
//reorder the array and return the results.

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //req by dir listing
#include <dirent.h> //req by dir listing
#include <string.h>
#include <float.h>

//#define TH .165 //ad occhio; è la metà dell'originale ma pare buon compromesso

double BER(FILE *sample, FILE *whole) {
	int bufwhole = 315000;
	int bufsample = 5000;
	int bufcount = 10;
	char whole_bitstream[bufwhole];
	char sample_bitstream[bufsample];
	fgets(whole_bitstream, bufwhole, whole);
	fgets(sample_bitstream, bufsample, sample);
	char whole_count[bufcount];
	char sample_count[bufcount];
	fgets(whole_count, bufcount, whole);
	fgets(sample_count, bufcount, sample);
	int whole_num = atoi(whole_count);
	int sample_num = atoi(sample_count);
	if (whole_num == 0) {
		fprintf(stderr, "Empty db bitstream, skipping.\n");
		return(DBL_MAX);
	}
	if (sample_num == 0) {
		fprintf(stderr, "Empty sample, exiting.\n");
		exit(1);
	}
	_Bool whole_bits[whole_num];
	_Bool sample_bits[sample_num];
	int i;
	for (i=0; i<sample_num; i++)
		//subtract the ascii code and improperly cast to bit, neat!
		sample_bits[i]=(int)sample_bitstream[i]-'0';
	for (i=0; i<whole_num; i++)
		whole_bits[i]=(int)whole_bitstream[i]-'0';
	int base=0; //sliding base
	int ber_curr; //curr win ber
	int ber_temp;
	//slide the sample all over the whole and compute the hamming distance each
	//time; return the minimum distance found
	while (base<=whole_num-sample_num) {
		ber_temp=0;
		for (i=0; i<sample_num; i++)
			if (sample_bits[i] != whole_bits[base+i])
				ber_temp++;
		base++;
		if (base==0) //1st distance
			ber_curr=ber_temp;
		else if (ber_temp<ber_curr) //we want the min
			ber_curr=ber_temp;
	}
	return (double) ber_curr/sample_num;
}

int cmp(const char a[], const char b[]) {
	double aval, bval;
	sscanf(a, "%lf %*s", &aval);
	sscanf(b, "%lf %*s", &bval);
	if (aval<bval)
		return -1;
	else if (aval>bval)
		return 1;
	return 0;
}

int main(int argc, char** argv) {
	const int buf = 200; //this represents the maximum filename length
	struct dirent *dp;
	DIR *dfd = opendir(argv[2]); //path to db dir
	int count;
	if(dfd != NULL) {
		count = -2; //skip ., ..
		while((dp = readdir(dfd)) != NULL) //:P
			count++;
	}
	else {
		fprintf(stderr, "Directory path %s not found", argv[2]);
		exit(1);
	}
	char results[count][buf+10];
	dfd = opendir(argv[2]);
	dp = readdir(dfd); //skip .
	dp = readdir(dfd); //skip ..
	int c = 0;
	double score_curr, score_temp;
	char song_curr[buf];
	while((dp = readdir(dfd)) != NULL) {
		char temp[buf];
		strcpy(temp, argv[2]);
		FILE *sample = fopen(argv[1], "r"); //keep on opening...
		FILE *whole = fopen(strcat(temp, dp->d_name), "r");
		if (sample==NULL) {
			fprintf(stderr, "Sample file %s does not exist", argv[1]);
			exit(1);
		}
		//fprintf(stderr, "%s\n", dp->d_name);
		score_temp = BER(sample, whole);
		//if (score_temp < TH)
		//	fp++;
		//printf("%f, ", score_temp);
		//printf("%s--->%f\n", temp, score_temp);
		if (c == 0) { //this part is just what we do in BER()
			score_curr = score_temp;
			*song_curr = 0;
			strcpy(song_curr, dp->d_name);
		}
		else if (score_temp<score_curr) {
			score_curr = score_temp;
			*song_curr = 0;
			strcpy(song_curr, dp->d_name);
		}
		sprintf(results[c], "%f %s", score_temp, dp->d_name);
		c++;
		fclose(sample); //and closing; it works
		fclose(whole);
	}
	closedir(dfd);
	//test sct assumes this exact printf layout - don't change!
	qsort(results, count, buf+10, cmp);
	printf("\n-------\n");
	printf("Query: %s", argv[1]);
	printf("\n-------\n");
	printf("Results:\n");
	for (c=0; c<count; c++) {
		if (c==9)
			break;
		printf("%s\n", results[c]);
	}
	//ORA DEVO ORDINARE IL VETTORE DI RISULTATI (scrivere una compare())
	//printf("\n\n%f\n", score_curr);
	//printf("%s\n", song_curr);
	return 0;
}
