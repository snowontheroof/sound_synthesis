#pragma once
#include <iostream>
#include <vector>

constexpr uint16_t	SAMPLE_RATE	= 44100;


enum Type
{
	SINE,
	SAW,
	SQUARE,
	TRIANGLE,
	INVALID
};

struct Notes
{
	std::string	pitch;
	double		duration;
	double		frequency;
};

struct Track
{
	Type				instrument;
	std::vector<Notes>	notes;
	double				track_dur;
};

struct Data
{
	int					tempo;
	unsigned int		track_amt;
	Track*				tracks;
};

std::vector<float> sampleTracks(Data data);

void writeWAV(std::vector<float> buffer, Data data);

std::vector<float>
generate_sine_sample( uint16_t freq, double amplitude, double beats, double seconds_per_beat );
std::vector<float>
generate_saw_sample( uint16_t freq, double amplitude, double beats, double seconds_per_beat );
std::vector<float>
generate_square_sample( uint16_t freq, double amplitude, double beats, double seconds_per_beat );
std::vector<float>
generate_triangle_sample( uint16_t freq, double amplitude, double beats, double seconds_per_beat );
