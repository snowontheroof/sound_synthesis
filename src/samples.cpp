#include <cinttypes>
#include <cmath>
#include <vector>
#include <iostream>
#include "minisynth.hpp"
// #include "portaudio.h"

std::vector<float>
generate_sine_sample( uint16_t freq, double amplitude, double beats );
std::vector<float>
generate_saw_sample( uint16_t freq, double amplitude, double beats );
std::vector<float>
generate_square_sample( uint16_t freq, double amplitude, double beats );
std::vector<float>
generate_triangle_sample( uint16_t freq, double amplitude, double beats );
// static int patestCallback(	const void *inputBuffer, void *outputBuffer,
// 							unsigned long framesPerBuffer,
// 							const PaStreamCallbackTimeInfo *timeInfo,
// 							PaStreamCallbackFlags statusFlags,
// 							void *userData );
// static int	report_error_and_return_one( PaError err );

// Helper struct to hold float buffers for the left and right ear
// (don't know why they use phase for the naming)
typedef struct
{
	std::vector<float>	left_phase;
	std::vector<float>	right_phase;
}
paTestData;

// // Simple test main for hearing the different wave types
// int	main( void )
// {
// 	paTestData	data;

// 	// Set globals
// 	bpm									= 120;
// 	seconds_per_beat					= 60.0 / bpm;

// 	double			beats				= 1;	// How many beats to calculate for the buffers
// 	double			playback_duration	= 3;	// How many seconds to keep playing the sampe buffer
// 	double			master_volume		= 0.05;
// 	double const	sample_duration		= beats * seconds_per_beat;
// 	size_t const	total_samples		= sample_duration * SAMPLE_RATE;

// 	// Choose wave types and frequencies for samples
// 	double	freq_right	= 440;
// 	double	freq_left	= 880;
// 	data.right_phase = generate_sine_sample( freq_right, master_volume, beats );
// 	data.left_phase = generate_sine_sample( freq_left, master_volume, beats );

// 	// Alternate buffers, can I make the note change?
// 	std::vector<float>	alt_buf = generate_triangle_sample( 0.33 * freq_right, master_volume, beats);

// 	// Start PortAudio
// 	PaError	err = Pa_Initialize();
// 	if ( err != paNoError )
// 		return report_error_and_return_one( err );

// 	PaStream	*stream;
// 	err = Pa_OpenDefaultStream(	&stream,
// 								0,				// No input
// 								2,				// Stereo
// 								paFloat32,		// Output buffer type
// 								SAMPLE_RATE,
// 								total_samples,	// How much information is in the buffer that keeps looping
// 								patestCallback,	// Callback function pointer
// 								&data );		// Data passed to callback function
// 	if ( err != paNoError )
// 		return report_error_and_return_one( err );

// 	err = Pa_StartStream( stream );
// 	if ( err != paNoError )
// 		return report_error_and_return_one( err );

// 	Pa_Sleep( 2 * 1000 );
// 	data.right_phase = alt_buf;
// 	data.left_phase = alt_buf;
// 	// Play at least for playback_duration time
// 	Pa_Sleep( playback_duration * 1000 );

// 	err = Pa_StopStream( stream );
// 	if ( err != paNoError )
// 		return report_error_and_return_one( err );
	
// 	err = Pa_CloseStream( stream );
// 	if ( err != paNoError )
// 		return report_error_and_return_one( err );

// 	err = Pa_Terminate();
// 	if ( err != paNoError )
// 		return report_error_and_return_one( err );

// 	return 0;
// }

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

// static int patestCallback(	const void *inputBuffer, void *outputBuffer,
// 							unsigned long framesPerBuffer,
// 							const PaStreamCallbackTimeInfo *timeInfo,
// 							PaStreamCallbackFlags statusFlags,
// 							void *userData )
// {
// 	paTestData	*data	= static_cast<paTestData *>( userData );
// 	float		*out	= static_cast<float *>( outputBuffer );

// 	(void)inputBuffer;

// 	unsigned int	i;
// 	for ( i = 0; i < framesPerBuffer; ++i )
// 	{
// 		*out++ = data->left_phase[i];
// 		*out++ = data->right_phase[i];
// 	}
// 	return 0;
// }

// static int	report_error_and_return_one( PaError err )
// {
// 	std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
// 	err = Pa_Terminate();
// 	if ( err != paNoError )
// 		std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
// 	return 1;
// }
