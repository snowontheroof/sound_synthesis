#include "minisynth.hpp"

std::vector<float> apply_lowpass_filter(const std::vector<float>& input, float cutoff_freq, float sample_rate)
{
	std::vector<float> output(input.size(), 0.0f);
	float alpha = cutoff_freq / (cutoff_freq + sample_rate / (2.0f * M_PI));
	float prev_output = 0.0f;

	for (size_t i = 0; i < input.size(); ++i)
	{
		output[i] = alpha * input[i] + (1.0f - alpha) * prev_output;
		prev_output = output[i];
	}

	return output;
}

std::vector<float> applyReverb(const std::vector<float>& input, float decay, size_t delaySamples)
{
    std::vector<float> output(input.size(), 0.0f);

    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i];
        if (i >= delaySamples) {
            output[i] += output[i - delaySamples] * decay;
        }
    }

    return output;
}


void applyADSR(std::vector<float>& samples, 
               float attack, float decay, float sustain, float release) 
{
    size_t totalSamples = samples.size();
    size_t attackSamples  = attack  * SAMPLE_RATE;
    size_t decaySamples   = decay   * SAMPLE_RATE;
    size_t releaseSamples = release * SAMPLE_RATE;
    size_t sustainStart   = attackSamples + decaySamples;
    size_t sustainEnd     = totalSamples > releaseSamples ? totalSamples - releaseSamples : totalSamples;

    for (size_t i = 0; i < totalSamples; i++) {
        float env = 1.0f;

        if (i < attackSamples)
            env = (float)i / attackSamples; 
        else if (i < sustainStart) {
            float t = (float)(i - attackSamples) / decaySamples;
            env = 1.0f - t * (1.0f - sustain);
        } 
        else if (i < sustainEnd)
            env = sustain;
        else {
            float t = (float)(i - sustainEnd) / releaseSamples;
            env = sustain * (1.0f - t);
        }
        samples[i] *= env;
    }
}

std::vector<float> sampleTracks( Data data )
{
	double songDuration = data.tracks[0].track_dur;
	for (unsigned i = 1; i < data.track_amt; ++i)
		songDuration = std::max(songDuration, data.tracks[i].track_dur);
	size_t totalSamples = songDuration * SAMPLE_RATE;
	std::vector<float> mixBuffer(totalSamples, 0.0f);
	float cutoff_freq = 1000.0f; 

	for (unsigned t = 0; t < data.track_amt; ++t) {
		const Track& track = data.tracks[t];
		size_t sampleOffset = 0;

		for (const Notes& note : track.notes) {
			size_t noteSamplesCount = note.duration * secondsPerBeat * SAMPLE_RATE;

			if (!note.pitch.empty() && note.pitch[0] == 'r') {
				sampleOffset += noteSamplesCount;
				continue;
			}
			std::vector<float> noteSamples;
			switch (track.instrument) {
				case SINE:
					noteSamples = generate_sine_sample( note.frequency, 0.5, note.duration );
					break;
				case SAW:
					noteSamples = generate_saw_sample( note.frequency, 0.5, note.duration );
					break;
				case SQUARE:
					noteSamples = generate_square_sample( note.frequency, 0.5, note.duration );
					break;
				case TRIANGLE:
					noteSamples = generate_triangle_sample( note.frequency, 0.5, note.duration );
					break;
				case INVALID:
					break;
			}
			applyADSR(noteSamples, 0.01f, 0.2f, 0.6f, 0.2f);
			for (size_t i = 0; i < noteSamples.size(); ++i) {
				if (sampleOffset + i < mixBuffer.size()) {
					mixBuffer[sampleOffset + i] += noteSamples[i];
				}
				else
				break;
			}
			sampleOffset += noteSamplesCount;
		}
	}
	mixBuffer = apply_lowpass_filter(mixBuffer, cutoff_freq, SAMPLE_RATE);
	mixBuffer = applyReverb(mixBuffer, 0.3f, SAMPLE_RATE / 10);
	return mixBuffer;
}
