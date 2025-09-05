#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <cinttypes>
#include "portaudio.h"

constexpr uint16_t	SAMPLE_RATE	= 44100;

// Helper struct to hold float buffers for the left and right ear
// (don't know why they use phase for the naming)
typedef struct
{
	std::vector<float>	left_phase;
	std::vector<float>	right_phase;
}
paTestData;

std::vector<float>
generate_sine_sample( uint16_t freq, double amplitude, double beats );

std::vector<float>
generate_saw_sample( uint16_t freq, double amplitude, double beats );

std::vector<float>
generate_square_sample( uint16_t freq, double amplitude, double beats );

std::vector<float>
generate_triangle_sample( uint16_t freq, double amplitude, double beats );
