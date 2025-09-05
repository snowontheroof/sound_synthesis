#include <iostream>
#include "minisynth.hpp"

static int patestCallback(	const void *inputBuffer, void *outputBuffer,
							unsigned long framesPerBuffer,
							const PaStreamCallbackTimeInfo *timeInfo,
							PaStreamCallbackFlags statusFlags,
							void *userData );
static int	report_error_and_return_one( PaError err );

int		bpm;
double	secondsPerBeat;

int	main( int argc, char **argv )
{
	if (argc != 2)
	{
		std::cout << "Usage: ./minisynth <file>\n";
		return 0;
	}

	Data	parsedData = parser(argv[1]);

	// Set globals
	bpm				= parsedData.tempo;
	secondsPerBeat	= 60.0 / bpm;

	std::vector<float> mix = sampleTracks( parsedData );
	double	masterVolume = 0.3;

	double	maxVal = 0.0;
	for ( double sample : mix )
		maxVal = std::max( maxVal, std::abs( sample ));
	double	normFactor = ( maxVal > 1.0 ) ? ( 1.0 / maxVal) : 1.0;

	paTestData	data;
	for ( auto sample : mix )
	{
		sample *= normFactor;
		sample *= masterVolume;
		data.right_phase.push_back(sample);
		data.left_phase.push_back(sample);
	}

	size_t	total_samples	= mix.size();

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
