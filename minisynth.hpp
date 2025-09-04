#pragma once
#include <iostream>
#include <vector>

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

std::vector<Notes> parsedNotes;

// struct Notes
// {
// 	char			pitch;
// 	bool			alteration;
// 	unsigned int	octave;
// 	double			duration;
// };

struct Track
{
	Type				instrument;
	std::vector<Notes>	notes;
	double				track_dur;
};

struct Data
{
	int				tempo;
	unsigned int	track_amt;
	Track*			tracks;
};
