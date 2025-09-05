#include "samples.hpp"

uint16_t	bpm					= 60;			// bpm is global out of convenience
double		seconds_per_beat	= 60.0 / bpm;

static int patestCallback(	const void *inputBuffer, void *outputBuffer,
							unsigned long framesPerBuffer,
							const PaStreamCallbackTimeInfo *timeInfo,
							PaStreamCallbackFlags statusFlags,
							void *userData );
static int	report_error_and_return_one( PaError err );

// Simple test main for hearing the different wave types
int	main( void )
{
	paTestData	data;

	// Set globals
	bpm									= 120;
	seconds_per_beat					= 60.0 / bpm;

	double			beats				= 1;	// How many beats to calculate for the buffers
	double			playback_duration	= 3;	// How many seconds to keep playing the sampe buffer
	double			master_volume		= 0.05;
	double const	sample_duration		= beats * seconds_per_beat;
	size_t const	total_samples		= sample_duration * SAMPLE_RATE;

	// Choose wave types and frequencies for samples
	double	freq_right	= 440;
	double	freq_left	= 880;
	data.right_phase = generate_sine_sample( freq_right, master_volume, beats );
	data.left_phase = generate_sine_sample( freq_left, master_volume, beats );

	// Alternate buffers, can I make the note change?
	std::vector<float>	alt_buf = generate_triangle_sample( 0.33 * freq_right, master_volume, beats);

	// Start PortAudio
	PaError	err = Pa_Initialize();
	if ( err != paNoError )
		return report_error_and_return_one( err );

	PaStream	*stream;
	err = Pa_OpenDefaultStream(	&stream,
								0,				// No input
								2,				// Stereo
								paFloat32,		// Output buffer type
								SAMPLE_RATE,
								total_samples,	// How much information is in the buffer that keeps looping
								patestCallback,	// Callback function pointer
								&data );		// Data passed to callback function
	if ( err != paNoError )
		return report_error_and_return_one( err );

	err = Pa_StartStream( stream );
	if ( err != paNoError )
		return report_error_and_return_one( err );

	Pa_Sleep( 2 * 1000 );
	data.right_phase = alt_buf;
	data.left_phase = alt_buf;
	// Play at least for playback_duration time
	Pa_Sleep( playback_duration * 1000 );

	err = Pa_StopStream( stream );
	if ( err != paNoError )
		return report_error_and_return_one( err );
	
	err = Pa_CloseStream( stream );
	if ( err != paNoError )
		return report_error_and_return_one( err );

	err = Pa_Terminate();
	if ( err != paNoError )
		return report_error_and_return_one( err );

	return 0;
}

static int patestCallback(	const void *inputBuffer, void *outputBuffer,
							unsigned long framesPerBuffer,
							const PaStreamCallbackTimeInfo *timeInfo,
							PaStreamCallbackFlags statusFlags,
							void *userData )
{
	paTestData	*data	= static_cast<paTestData *>( userData );
	float		*out	= static_cast<float *>( outputBuffer );

	(void)inputBuffer;
	(void)timeInfo;
	(void)statusFlags;

	unsigned int	i;
	for ( i = 0; i < framesPerBuffer; ++i )
	{
		*out++ = data->left_phase[i];
		*out++ = data->right_phase[i];
	}
	return 0;
}

static int	report_error_and_return_one( PaError err )
{
	std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
	err = Pa_Terminate();
	if ( err != paNoError )
		std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
	return 1;
}
