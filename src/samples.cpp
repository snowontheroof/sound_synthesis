#include <cinttypes>
#include <cmath>
#include <vector>
#include <iostream>
// #include "portaudio.h"

constexpr uint16_t	SAMPLE_RATE			= 40;
constexpr double	RADIANS_PER_SAMPLE	= 2 * M_PI / SAMPLE_RATE; // Helper for sine sampling

uint16_t		tempo				= 60;				// Tempo is global out of convenience
double const	seconds_per_beat	= 60.0 / tempo;

// Amplitude goes from 0 to 1
std::vector<int16_t>
generate_sine_sample( uint16_t freq, double amplitude, double beats )
{
	std::vector<int16_t>	buffer{};
	double const			sample_duration		= beats * seconds_per_beat;
	size_t const			total_samples		= sample_duration * SAMPLE_RATE;

	for ( size_t i = 0; i < total_samples; ++i )
	{
		double	sample = amplitude * INT16_MAX * sin( i * RADIANS_PER_SAMPLE * freq );
		buffer.push_back( static_cast<uint16_t>( sample ) );
	}
	return buffer;
}

std::vector<int16_t>
generate_saw_sample( uint16_t freq, double amplitude, double beats )
{
	std::vector<int16_t>	buffer{};
	double const			sample_duration		= beats * seconds_per_beat;
	size_t const			total_samples		= sample_duration * SAMPLE_RATE;
	double const			step				= freq * 2.0 / SAMPLE_RATE;
	double					minus_one_to_one	= 0;

	for ( size_t i = 0; i < total_samples; ++i )
	{
		double	sample = amplitude * INT16_MAX * minus_one_to_one;
		buffer.push_back( static_cast<uint16_t>( sample ));
		minus_one_to_one += step;
		if ( minus_one_to_one > 1 )
			minus_one_to_one -= 2;
	}
	return buffer;
}

std::vector<int16_t>
generate_square_sample( uint16_t freq, double amplitude, double beats )
{
	std::vector<int16_t>	buffer{};
	double const			sample_duration	= beats * seconds_per_beat;
	size_t const			total_samples	= sample_duration * SAMPLE_RATE;
	double const			sample_width	= sample_duration / SAMPLE_RATE;
	double const			peak_width		= 1 / ( 2.0 * freq );
	
	size_t	nth_peak;
	for ( size_t i = 0; i < total_samples; ++i )
	{
		nth_peak = i * sample_width / peak_width;
		double	sample = amplitude * INT16_MAX;
		if ( nth_peak % 2 != 0 )
			sample = -sample;
		buffer.push_back( static_cast<uint16_t>( sample ) );
	}
	return buffer;
}

std::vector<int16_t>
generate_triangle_sample( uint16_t freq, double amplitude, double beats )
{
	std::vector<int16_t>	buffer{};
	double const			sample_duration		= beats * seconds_per_beat;
	size_t const			total_samples		= sample_duration * SAMPLE_RATE;
	double const			sample_width		= sample_duration / SAMPLE_RATE;
	double const			step				= freq * 2.0 / SAMPLE_RATE;
	double					minus_one_to_one	= 0.5;

	size_t	section;
	for ( size_t i = 0; i < total_samples; ++i )
	{
		double	tri = 2.0 * fabs( minus_one_to_one ) - 1.0;
		double	sample = amplitude * INT16_MAX * tri;
		buffer.push_back( static_cast<int16_t>( sample ));
		minus_one_to_one += step;
		if ( minus_one_to_one > 1 )
			minus_one_to_one -= 2;
	}
	return buffer;
}

int	main( void )
{
	std::vector<int16_t>	sample = generate_triangle_sample( 2, 1, 1 );

	for ( auto &s: sample )
		std::cout << s << "\n";

	return 0;
}
