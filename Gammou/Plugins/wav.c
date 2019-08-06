
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "wav.h"

#define WAVE_FORMAT_PCM 0x0001u
#define WAVE_FORMAT_IEEE_FLOAT 0x0003u

// Trick from gist.github.com/PhilCK/1534763
#ifdef __GNUC__
#define PACKED(struct_decl) struct_decl __attribute__((__packed__))
#else
#define PACKED(struct_decl) __pragma(pack(push, 1)) struct_decl __pragma(pack(pop))
#endif

//	Implementation of abstract datatype 
struct wav_t {
  unsigned short int channel_count;
  unsigned int sample_count;
  unsigned int sample_rate;
  double** data;
};

// WAVE file format
PACKED(
struct WAVE_file_header
{
	uint8_t RIFF[4];
	uint32_t file_size_m8;
	uint8_t WAVE[4];
});

PACKED(
struct WAVE_generic_block_header
{
	uint8_t block_id[4];
	uint32_t block_size;
});

PACKED(
struct WAVE_format_block
{
	struct WAVE_generic_block_header header;
	uint16_t format_id;
	uint16_t channel_count;
	uint32_t sample_rate;
	uint32_t byte_per_sec;
	uint16_t byte_per_block;
	uint16_t bit_per_sample;
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
	struct WAVE_file_header file_header;
	struct WAVE_format_block format_block;
	struct WAVE_generic_block_header data_block_header;

	file_header.RIFF[0] = 'R';
	file_header.RIFF[1] = 'I';
	file_header.RIFF[2] = 'F';
	file_header.RIFF[3] = 'F';

	file_header.WAVE[0] = 'W';
	file_header.WAVE[1] = 'A';
	file_header.WAVE[2] = 'V';
	file_header.WAVE[3] = 'E';

	// --

	format_block.header.block_id[0] = 'f';
	format_block.header.block_id[1] = 'm';
	format_block.header.block_id[2] = 't';
	format_block.header.block_id[3] = ' ';

	format_block.header.block_size = 16;
	format_block.format_id = WAVE_FORMAT_PCM;
	format_block.channel_count = wav->channel_count;
	format_block.sample_rate = wav->sample_rate;
	format_block.byte_per_sec = (bit_depth / 8) * wav->channel_count * wav->sample_rate;
	format_block.byte_per_block = (bit_depth / 8) * wav->channel_count;
	format_block.bit_per_sample = bit_depth;

	//
	data_block_header.block_id[0] = 'd';
	data_block_header.block_id[1] = 'a';
	data_block_header.block_id[2] = 't';
	data_block_header.block_id[3] = 'a';

	data_block_header.block_size = 
		format_block.byte_per_block * wav->sample_count;

	file_header.file_size_m8 =
		data_block_header.block_size + 
		sizeof(data_block_header) + 
		sizeof(format_block) + 
		sizeof(file_header) - 8;

	//
	uint8_t *data = (uint8_t *)malloc(data_block_header.block_size);
	FILE *fd = fopen(path,"w+");

	// double -> data
	const int block_max = (1u << (bit_depth - 1)) - 1;
	const unsigned int byte_per_sample = bit_depth / 8;
	

	for(unsigned int i = 0 ; i < wav->channel_count; i++){
		for(unsigned int j=0 ; j < wav->channel_count ; j++){

			int block;

			if(wav->data[j][i] < -1.)
				block = -block_max;
			else if(wav->data[j][i] > 1.)
				block = block_max;
			else
				block = wav->data[j][i] * (double)block_max;

			for(unsigned int k=0 ; k < byte_per_sample ; k++)
				data[byte_per_sample*(i*wav->channel_count + j) + k] = 0xFF & (block >> (k*8));
		}
	}

	fwrite(&file_header, sizeof(file_header), 1, fd);
	fwrite(&format_block, sizeof(format_block), 1, fd);
	fwrite(&data_block_header, sizeof(data_block_header), 1, fd);
	fwrite(data, data_block_header.block_size, 1, fd);
	fclose(fd);
	free(data);

	return 0;
}

static wav_t *wav_from_raw_data(
	const unsigned int sample_count,
	const unsigned int sample_rate,
	const unsigned int channel_count,
	const unsigned int bit_depth,
	const uint16_t wav_format,
	const void *raw_data)
{
	wav_t *wav = (wav_t *)malloc(sizeof(wav_t));

	wav->channel_count = channel_count;
	wav->sample_count = sample_count;
	wav->sample_rate = sample_rate;

	wav_alloc(wav);

	if (wav_format == WAVE_FORMAT_PCM) {
		const uint8_t *data = (uint8_t*)raw_data;
		const unsigned nb_octet = bit_depth / 8;
		const unsigned int block_max = (1u << (bit_depth - 1)) - 1;
		int block;

		for (unsigned int i = 0; i < wav->sample_count; i++) {
			for (unsigned int j = 0; j < wav->channel_count; j++) {
				block = 0;

				for (unsigned int k = 0; k < nb_octet; k++)
					block |= data[nb_octet * (i * wav->channel_count + j) + k] << (k * 8);

				if ((data[nb_octet * (i * wav->channel_count + j) + nb_octet - 1] & 0x80) != 0) { // Le block est negatif
					for (unsigned int k = nb_octet; k < 4; k++)
						block |= 0xFF << (k * 8);
				}

				wav->data[j][i] = (double)block / (double)block_max;
			}
		}
	}
	else if (wav_format == WAVE_FORMAT_IEEE_FLOAT) {
		const float *data = (float*)raw_data;

		if (bit_depth == 32) { // 32 bit floating point number : float
			const float *data = (float*)raw_data;
			for (unsigned int i = 0; i < wav->sample_count; i++) {
				for (unsigned int j = 0; j < wav->channel_count; j++) {
					wav->data[j][i] = (double)(data[j + i * wav->channel_count]);
				}
			}
		}
		else if (bit_depth == 64) { // 64 bit floating point number : double
			const double *data = (double*)raw_data;
			for (unsigned int i = 0; i < wav->sample_count; i++) {
				for (unsigned int j = 0; j < wav->channel_count; j++) {
					wav->data[j][i] = data[j + i * wav->channel_count];
				}
			}
		}
	}

	return wav;
}

wav_t *wav_load(const char *path)
{
	FILE* fd;
	uint8_t *file_data;

	//	Open file
	if((fd = fopen(path, "rb")) == NULL) {
		printf("Unable to open %s\n", path);
		return NULL;
	}

	//	Get data size
	fseek(fd, 0u, SEEK_END);
	const size_t file_data_size = ftell(fd);
	rewind(fd);

	// Read file data
	file_data = (uint8_t*)malloc(file_data_size);
	fread(file_data, file_data_size, 1u, fd);
	fclose(fd);

	//////////////

	uint8_t *cursor = file_data;
	struct WAVE_file_header *file_header = 
		(struct WAVE_file_header*)cursor;

	// Check file header

	if (strncmp(file_header->RIFF, "RIFF", 4) != 0 || strncmp(file_header->WAVE, "WAVE", 4) != 0) {
		free(file_data);
		return NULL;
	}

	cursor += sizeof(*file_header);

	//
	int got_format_block = 0;
	int bit_depth, channel_count, sample_rate, byte_per_block, format_id;

	while (cursor < file_data + file_data_size) {
		
		// format block
		if (strncmp(cursor, "fmt ", 4) == 0) {
			
			if (got_format_block) {
				//	There shouldnt be 2 format block
				break;
			}
			else {
				struct WAVE_format_block *fmt =
					(struct WAVE_format_block *)cursor;
				got_format_block = 1;

				//	check and get format info
				
				if (fmt->format_id != WAVE_FORMAT_PCM && 
					fmt->format_id != WAVE_FORMAT_IEEE_FLOAT)
					break;	//	Format not handled
				
				format_id = fmt->format_id;
				channel_count = fmt->channel_count;
				sample_rate = fmt->sample_rate;
				bit_depth = fmt->bit_per_sample;
				byte_per_block = fmt->byte_per_block;

				cursor += (sizeof(fmt->header) + fmt->header.block_size);
			}
		}
		// data block : if format block have been read, then read data
		else if (strncmp(cursor, "data", 4) == 0) {
			
			if (got_format_block) {
				struct WAVE_generic_block_header *data_header =
					(struct WAVE_generic_block_header *)cursor;

				wav_t *wav = wav_from_raw_data(
					data_header->block_size / byte_per_block,
					sample_rate,
					channel_count,
					bit_depth,
					format_id,
					cursor + sizeof(*data_header));

				free(file_data);
				return wav;
			}
			
		}
		// If block if is not known, just skip it
		else {
			struct WAVE_generic_block_header *block_header =
				(struct WAVE_generic_block_header *)cursor;
			//skip bext block
			cursor +=
				(sizeof(*block_header) + block_header->block_size);
		}
	}

	// end of file have been reached without reading audio data !
	free(file_data);
	return NULL;
}

wav_t * wav_zero(
	const unsigned int channel_count, 
	const unsigned int sample_count, 
	const unsigned int sample_rate)
{
	wav_t *wav = (wav_t*)malloc(sizeof(wav_t));
	wav->channel_count = channel_count;
	wav->sample_count = sample_count;
	wav->sample_rate = sample_rate;
	wav_alloc(wav);
	memset(
		wav->data[0],
		0,
		sample_count *
		channel_count *
		sizeof(double));
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
