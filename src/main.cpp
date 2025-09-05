#include <iostream>
#include "../inc/samples.hpp"
#include "../inc/minisynth.hpp"

uint16_t	bpm					= 60;			// bpm is global out of convenience
double		seconds_per_beat	= 60.0 / bpm;

static int patestCallback(	const void *inputBuffer, void *outputBuffer,
							unsigned long framesPerBuffer,
							const PaStreamCallbackTimeInfo *timeInfo,
							PaStreamCallbackFlags statusFlags,
							void *userData );
static int	report_error_and_return_one( PaError err );

std::vector<float>	generate_sample(Notes note, double master_volume, enum Type type)
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

int	main( int argc, char **argv )
{
	if (argc != 2)
	{
		std::cout << "Usage: ./minisynth <file>\n";
		return 0;
	}

	Data		parsedData = parser(argv[1]);

	// Set globals
	bpm									= parsedData.tempo;

	double			beats				= 1;	// How many beats to calculate for the buffers
	double			playback_duration	= 3;	// How many seconds to keep playing the sample buffer
	double			master_volume		= 1;
	double const	sample_duration		= beats * seconds_per_beat;
	size_t			total_samples		= sample_duration * SAMPLE_RATE;

	paTestData	data;

	for (size_t i = 0; i < parsedData.tracks[0].notes.size(); i++)
	{
		Notes	note = parsedData.tracks[0].notes[i];
		enum Type	type = parsedData.tracks[0].instrument;
		if (note.frequency == -1)
			master_volume = 0;
		else
			master_volume = 1;
		std::vector<float>	note_sample = generate_sample(note, master_volume, type);
		for ( auto &s: note_sample )
		{
			data.right_phase.push_back(s);
			data.left_phase.push_back(s);
		}
	}
	total_samples = data.right_phase.size();
	playback_duration = total_samples / SAMPLE_RATE;

	// Choose wave types and frequencies for samples
	// double	freq_right	= 440;
	// double	freq_left	= 880;
	// data.right_phase = generate_sine_sample( freq_right, master_volume, beats );
	// data.left_phase = generate_sine_sample( freq_left, master_volume, beats );

	// Alternate buffers, can I make the note change?
	// std::vector<float>	alt_buf = generate_triangle_sample( 0.33 * freq_right, master_volume, beats);

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

	// Pa_Sleep( 2 * 1000 );
	// data.right_phase = alt_buf;
	// data.left_phase = alt_buf;
	// // Play at least for playback_duration time
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

	delete[] parsedData.tracks;
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
