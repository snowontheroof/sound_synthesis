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

Data	parser(std::string input);
