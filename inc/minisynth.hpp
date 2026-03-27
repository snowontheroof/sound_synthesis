#pragma once

#include <cmath>
#include <vector>
#include <map>
#include <iostream>
#include <cinttypes>
#include "portaudio.h"

constexpr uint16_t	SAMPLE_RATE	= 44100;

extern double	secondsPerBeat;

/* -------------------------------------------------------------------------- */

enum Type
{
	SINE,
	SAW,
	SQUARE,
	TRIANGLE,
	KICK,
	INVALID
};

/* -------------------------------------------------------------------------- */

// Helper struct to hold float buffers for the left and right ear
// (don't know why they use phase for the naming)
typedef struct
{
	std::vector<float>	left_phase;
	std::vector<float>	right_phase;
}
paTestData;

struct Notes
{
	std::string	pitch;
	double		duration;
	double		frequency;
};

struct Track
{
	Type				instrument;
	unsigned int		volume;
	std::vector<Notes>	notes;
	double				track_dur;
};

struct Data
{
	int					tempo;
	unsigned int		track_amt;
	Track*				tracks;
};

/* -------------------------------------------------------------------------- */

Data	parser( std::string input );

/* ---------------------------------------------- Sample generating functions */

std::vector<float>
generate_sample( Notes note, double master_volume, enum Type type );

std::vector<float>
generate_sine_sample( uint16_t freq, double amplitude, double beats );

std::vector<float>
generate_saw_sample( uint16_t freq, double amplitude, double beats );

std::vector<float>
generate_square_sample( uint16_t freq, double amplitude, double beats );

std::vector<float>
generate_triangle_sample( uint16_t freq, double amplitude, double beats );

std::vector<float>
generate_kick_sample( uint16_t freq, double amplitude, double duration);

/* -------------------------------------------------------------------------- */

Type	find_type( std::string instrument );

/* -------------------------------------------------------------------------- */

std::vector<float>				sampleTracks( Data data );
void							writeWAV( std::vector<float> buffer, Data data );
std::map<std::string, double>	make_freq( void );

/* -------------------------------------------------------------------------- */
