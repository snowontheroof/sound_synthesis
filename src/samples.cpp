#include "minisynth.hpp"

extern double	seconds_per_beat;

std::vector<float>
generate_sample(Notes note, double master_volume, enum Type type)
{
	switch (type)
	{
		case SINE:
			return generate_sine_sample(note.frequency, master_volume, note.duration);
		case SAW:
			return generate_saw_sample(note.frequency, master_volume, note.duration);
		case SQUARE:
			return generate_square_sample(note.frequency, master_volume, note.duration);
		case TRIANGLE:
			return generate_triangle_sample(note.frequency, master_volume, note.duration);
		default:
			return generate_sine_sample(note.frequency, master_volume, note.duration);
	}
}

std::vector<float>
generate_sine_sample( uint16_t freq, double amplitude, double beats, double seconds_per_beat)
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
generate_saw_sample( uint16_t freq, double amplitude, double beats, double seconds_per_beat )
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
generate_square_sample( uint16_t freq, double amplitude, double beats, double seconds_per_beat )
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
generate_triangle_sample( uint16_t freq, double amplitude, double beats, double seconds_per_beat )
{
	std::vector<float>	buffer{};
	double const			sample_duration		= beats * seconds_per_beat;
	size_t const			total_samples		= sample_duration * SAMPLE_RATE;
	double const			step				= freq * 2.0 / SAMPLE_RATE;
	double					minus_one_to_one	= 0;

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
