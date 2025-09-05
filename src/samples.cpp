#include <cinttypes>
#include <cmath>
#include <vector>
#include <iostream>
#include "portaudio.h"

constexpr uint16_t	SAMPLE_RATE	= 44100;

uint16_t		tempo				= 60;			// Tempo is global out of convenience
double const	seconds_per_beat	= 60.0 / tempo;

// Amplitude goes from 0 to 1
std::vector<float>
generate_sine_sample( uint16_t freq, double amplitude, double beats )
{
	std::vector<float>	buffer{};
	double const		sample_duration	= beats * seconds_per_beat;
	size_t const		total_samples	= sample_duration * SAMPLE_RATE;

	for ( size_t i = 0; i < total_samples; ++i )
	{
		double	phase = 2.0 * M_PI * freq * i / SAMPLE_RATE;
		double	sample = amplitude * sin( phase );
		buffer.push_back( static_cast<float>( sample ) );
	}
	return buffer;
}

std::vector<float>
generate_saw_sample( uint16_t freq, double amplitude, double beats )
{
	std::vector<float>	buffer{};
	double const		sample_duration		= beats * seconds_per_beat;
	size_t const		total_samples		= sample_duration * SAMPLE_RATE;
	double const		step				= freq * 2.0 / SAMPLE_RATE;
	double				minus_one_to_one	= -1;

	for ( size_t i = 0; i < total_samples; ++i )
	{
		double	sample = amplitude * minus_one_to_one;
		buffer.push_back( static_cast<float>( sample ) );
		minus_one_to_one += step;
		if ( minus_one_to_one > 1 )
			minus_one_to_one -= 2;
	}
	return buffer;
}

std::vector<float>
generate_square_sample( uint16_t freq, double amplitude, double beats )
{
	std::vector<float>	buffer{};
	double const		sample_duration	= beats * seconds_per_beat;
	size_t const		total_samples	= sample_duration * SAMPLE_RATE;
	double const		sample_width	= sample_duration / SAMPLE_RATE;
	double const		peak_width		= 1 / ( 2.0 * freq );
	
	size_t	nth_peak;
	for ( size_t i = 0; i < total_samples; ++i )
	{
		nth_peak = i * sample_width / peak_width;
		if ( nth_peak % 2 != 0 )
			buffer.push_back( static_cast<float>( -amplitude ) );
		else
			buffer.push_back( static_cast<float>( amplitude ) );
	}
	return buffer;
}

std::vector<float>
generate_triangle_sample( uint16_t freq, double amplitude, double beats )
{
	std::vector<float>	buffer{};
	double const			sample_duration		= beats * seconds_per_beat;
	size_t const			total_samples		= sample_duration * SAMPLE_RATE;
	double const			sample_width		= sample_duration / SAMPLE_RATE;
	double const			step				= freq * 2.0 / SAMPLE_RATE;
	double					minus_one_to_one	= 0;

	size_t	section;
	for ( size_t i = 0; i < total_samples; ++i )
	{
		double	tri = 2.0 * fabs( minus_one_to_one ) - 1.0;
		double	sample = amplitude * tri;
		buffer.push_back( static_cast<float>( sample ) );
		minus_one_to_one += step;
		if ( minus_one_to_one > 1 )
			minus_one_to_one -= 2;
	}
	return buffer;
}

typedef struct
{
	float	left_phase;
	float	right_phase;
}
paTestData;

static int patestCallback(	const void *inputBuffer, void *outputBuffer,
							unsigned long framesPerBuffer,
							const PaStreamCallbackTimeInfo *timeInfo,
							PaStreamCallbackFlags statusFlags,
							void *userData )
{
	return 0;
}

int	main( void )
{
	std::vector<float>	sample = generate_triangle_sample( 2, 1, 1 );

	for ( auto &s: sample )
		std::cout << s << "\n";

	return 0;
}
