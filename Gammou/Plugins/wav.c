
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "wav.h"

struct wav_t {
  unsigned short int channel_count;
  unsigned int sample_count;
  unsigned int sample_rate;
  double** data;
};

// Trick from gist.github.com/PhilCK/1534763
#ifdef __GNUC__
#define PACKED(struct_decl) struct_decl __attribute__((__packed__))
#else
#define PACKED(struct_decl) __pragma( pack(push, 1) ) struct_decl __pragma( pack(pop) )
#endif

PACKED(
struct wav_file_header {                // En tete wav
  char RIFF[4];
  unsigned int taille_m8;
  char WAVEfmt[8];
  unsigned int nb_octet_block_format;
  unsigned short int format;
  unsigned short int nb_ch;
  unsigned int sample_rate;
  unsigned int octet_par_sec;
  unsigned short int byte_per_block;
  unsigned short int bit_depth;
  char data_magic[4];
  unsigned int data_size;
});

static void wav_alloc(wav_t *wav)
{
	const unsigned int channel_count = wav->channel_count;
	const unsigned int sample_count = wav->sample_count;

	wav->data = 
		(double**)malloc(
			channel_count * 
			sizeof(double*));

	double *data = 
		(double*)malloc(
			sample_count *
			channel_count * 
			sizeof(double));

	//	TODO error check

	for (unsigned int i = 0; i < channel_count; i++)
		wav->data[i] = data + i * sample_count;
}

wav_t *wav_copy(const wav_t *src)
{
	wav_t *ret = 
		(wav_t*)malloc(sizeof(wav_t));

	if (ret == NULL)
		return NULL;

	ret->channel_count = src->channel_count;
	ret->sample_count = src->sample_count;
	ret->sample_rate = src->sample_rate;

	wav_alloc(ret);
	return ret;
}

int wav_free(wav_t *wav)
{
	free(wav->data[0]);
	free(wav->data);
	free(wav);
	return 0;
}

int wav_save(
	wav_t *wav, 
	const char *path,
	const unsigned bit_depth)
{
	struct wav_file_header hdr;

	hdr.RIFF[0] = 'R';
	hdr.RIFF[1] = 'I';
	hdr.RIFF[2] = 'F';
	hdr.RIFF[3] = 'F';

	hdr.WAVEfmt[0] = 'W';
	hdr.WAVEfmt[1] = 'A';
	hdr.WAVEfmt[2] = 'V';
	hdr.WAVEfmt[3] = 'E';
	hdr.WAVEfmt[4] = 'f';
	hdr.WAVEfmt[5] = 'm';
	hdr.WAVEfmt[6] = 't';
	hdr.WAVEfmt[7] = ' ';

	hdr.data_magic[0] = 'd';
	hdr.data_magic[1] = 'a';
	hdr.data_magic[2] = 't';
	hdr.data_magic[3] = 'a';

	hdr.nb_octet_block_format = 16;
	hdr.format = 1;
	hdr.sample_rate = wav->sample_rate;
	hdr.bit_depth = bit_depth;
	hdr.nb_ch = wav->channel_count;
	hdr.byte_per_block = (bit_depth/8) * wav->channel_count;
	hdr.data_size = hdr.byte_per_block * wav->channel_count;
	hdr.octet_par_sec = hdr.byte_per_block * wav->sample_rate;
	hdr.taille_m8 = hdr.data_size + 36;


	unsigned char *data = (unsigned char *)malloc(hdr.data_size);
	FILE *fd = fopen(path,"w+");

	// double -> data
	const unsigned int block_max = 
		(1u << (hdr.bit_depth - 1)) - 1;
	const unsigned int nb_octet = hdr.bit_depth / 8;
	

	for(unsigned int i = 0 ; i < wav->channel_count; i++){
		for(unsigned int j=0 ; j < wav->channel_count ; j++){

			int block;

			if(wav->data[j][i] < -1)
				block = -block_max;
			else if(wav->data[j][i] > 1)
				block = block_max;
			else
				block = wav->data[j][i] * (double)block_max;

			for(unsigned int k=0 ; k < nb_octet ; k++)
				data[nb_octet*(i*wav->channel_count + j) + k] = 0xFF & (block >> (k*8));
		}
	}


	fwrite(&hdr, sizeof(struct wav_file_header), 1, fd);
	fwrite(data, hdr.data_size, 1, fd);
	fclose(fd);
	free(data);

	return 0;
}

wav_t *wav_load(const char *path)
{
	FILE* fd;
	struct wav_file_header hdr;

	if((fd = fopen(path, "rb")) == NULL) {
		printf("Unable to open %s\n", path);
		return NULL;
	}

	if(fread(&hdr , 1 , sizeof(struct wav_file_header) , fd) 
		!= sizeof(struct wav_file_header)) {
		printf("Unable to read header\n");
		return NULL;
	}

	if(	strncmp("RIFF",hdr.RIFF, 4) != 0  		|| 
		strncmp("WAVEfmt ",hdr.WAVEfmt, 8) != 0 	||
		strncmp("data",hdr.data_magic, 4) != 0  	|| 
		hdr.format != 1 						|| 
		hdr.nb_octet_block_format != 16) {
		printf("Unrecognized header format\n");
		return NULL;
	}

	wav_t *wav = (wav_t*)malloc(sizeof(wav_t));

	wav->sample_rate = hdr.sample_rate;
	wav->sample_count = hdr.data_size / hdr.byte_per_block;
	wav->channel_count = hdr.nb_ch;

	wav_alloc(wav);

	unsigned char *data = (unsigned char*)malloc(hdr.data_size);
	if(fread(data, 1 , hdr.data_size , fd) != hdr.data_size) {
		printf("Unable to read data\n");
		return NULL;
	}

	fclose(fd);

	//---

	const unsigned nb_octet = hdr.bit_depth / 8;
	const unsigned int block_max = 
		(1u << (hdr.bit_depth - 1)) - 1;
	int block;

	for(unsigned i=0 ; i < wav->sample_count ; i++){
		for(unsigned j = 0 ; j < wav->channel_count ; j++){
			block = 0;

			for(unsigned int k=0 ; k < nb_octet ; k++)
				block |= data[nb_octet*(i*wav->channel_count + j) + k] << (k * 8);

			if( (data[nb_octet*(i*wav->channel_count + j) + nb_octet - 1] & 0x80) != 0){// Le block est negatif
				for(unsigned int k=nb_octet ; k < 4 ; k++)
					block |= 0xFF << (k * 8);
			}

			wav->data[j][i] = (double)block / (double)block_max;
		}
	}

	free(data);
	
	return wav;
}


unsigned int wav_get_sample_count(const wav_t *wav)
{
	return wav->sample_count;
}

unsigned int wav_get_channel_count(const wav_t *wav)
{
	return wav->channel_count;
}

unsigned int wav_get_samplerate(const wav_t *wav)
{
	return wav->sample_rate;
}

double *wav_get_channel(
	wav_t *wav, 
	const unsigned int channel)
{
	if (channel < wav->channel_count)
		return wav->data[channel];
	else
		return NULL;
}

static double wav_get_sample(
	const wav_t *wav,
	const unsigned int index,
	const unsigned int channel)
{
	//	No channel check because used inside the libe

	if (index < wav->sample_count)
		return wav->data[channel][index];
	else
		return 0.0;
}

double wav_get_value(
	const wav_t *wav,
	const double t, 
	const unsigned int channel)
{
	if (channel < wav->channel_count) {
		double int_index;
		double dec_index =
			modf(t *(double)wav->sample_rate, &int_index);

		//	linear interpolation

		const double a = wav_get_sample(wav, (unsigned int)int_index, 	0);
		const double b = wav_get_sample(wav, (unsigned int)int_index + 1, 0);

		return (a * (1.0 - dec_index) + b * dec_index);
	}	
	else {
		return 0.0;
	}
}



/////
