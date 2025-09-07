#include "minisynth.hpp"


void applyLowPass(std::vector<float>& samples, float cutoff, int sampleRate, int passes = 3) {
    float rc = 1.0f / (cutoff * 2.0f * M_PI);
    float dt = 1.0f / sampleRate;
    float alpha = dt / (rc + dt);
    std::vector<float> temp(samples.size());
    for (int p = 0; p < passes; ++p) {
        float prev = samples[0];
        temp[0] = prev;
        for (size_t i = 1; i < samples.size(); ++i) {
            prev = prev + alpha * (samples[i] - prev);
            temp[i] = prev;
        }
        samples = temp;
    }
}



void applyADSR(std::vector<float>& samples, 
               float attack, float decay, float sustain, float release, float noteDuration) 
{
    size_t totalSamples = samples.size();
    size_t attackSamples  = attack  * SAMPLE_RATE;
    size_t decaySamples   = decay   * SAMPLE_RATE;
    size_t releaseSamples = release * SAMPLE_RATE;
    size_t sustainStart   = attackSamples + decaySamples;
    size_t sustainEnd     = totalSamples > releaseSamples ? totalSamples - releaseSamples : totalSamples;

    for (size_t i = 0; i < totalSamples; i++) {
        float env = 1.0f;

        if (i < attackSamples) {
            env = (float)i / attackSamples;
        } 
        else if (i < sustainStart) {
            float t = (float)(i - attackSamples) / decaySamples;
            env = 1.0f - t * (1.0f - sustain);
        } 
        else if (i < sustainEnd) {
            env = sustain;
        } 
        else {
            float t = (float)(i - sustainEnd) / releaseSamples;
            env = sustain * (1.0f - t);
        }

        samples[i] *= env;
    }
}


std::vector<float> sampleTracks(Data data){
    double songDuration = data.tracks[0].track_dur;
    double seconds_per_beat = 60.0 / data.tempo;
    float trackAmplitude = 1.0f / data.track_amt;
    std::cout << seconds_per_beat;
    for (unsigned i = 1; i < data.track_amt; ++i)
        songDuration = std::max(songDuration, data.tracks[i].track_dur);
    size_t totalSamples = songDuration * SAMPLE_RATE;
    std::vector<float> mixBuffer(totalSamples, 0.0f);
    for (unsigned t = 0; t < data.track_amt; ++t) {
        const Track& track = data.tracks[t];
        size_t sampleOffset = 0;

        for (const Notes& note : track.notes) {
            size_t noteSamplesCount = note.duration * seconds_per_beat * SAMPLE_RATE;

            if (!note.pitch.empty() && note.pitch[0] == 'r') {
                sampleOffset += noteSamplesCount;
                continue;
            }
            std::vector<float> noteSamples;
            switch (track.instrument) {
                case SINE:
                    noteSamples = generate_sine_sample(note.frequency, trackAmplitude, note.duration, seconds_per_beat);
                    break;
                case SAW:
                    noteSamples = generate_saw_sample(note.frequency, trackAmplitude, note.duration, seconds_per_beat);
                    break;
                case SQUARE:
                    noteSamples = generate_square_sample(note.frequency, trackAmplitude, note.duration, seconds_per_beat);
                    break;
                case TRIANGLE:
                    noteSamples = generate_triangle_sample(note.frequency, trackAmplitude, note.duration, seconds_per_beat);
                    break;
                case INVALID:
                    break;
            }
           
            
            for (size_t i = 0; i < noteSamples.size(); ++i) {
                if (sampleOffset + i < mixBuffer.size()) {
                    mixBuffer[sampleOffset + i] += noteSamples[i];
                    if (mixBuffer[sampleOffset + i] < -1 || mixBuffer[sampleOffset + i] > 1)
                        std::cout << mixBuffer[sampleOffset + i] << std::endl;
                }
                else
                    break;
            }
            sampleOffset += noteSamplesCount;
        }
    }
    applyADSR(mixBuffer, 0.01f, 0.1f, 0.7f, 0.2f, songDuration);
    applyLowPass(mixBuffer, 2000.0f, SAMPLE_RATE);

        return mixBuffer;
}