#include "minisynth.hpp"

std::vector<float> sampleTracks(Data data){
    double songDuration = data.tracks[0].track_dur;
    double seconds_per_beat = 60.0 / data.tempo;
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
                    noteSamples = generate_sine_sample(note.frequency, 0.5, note.duration, seconds_per_beat);
                    break;
                case SAW:
                    noteSamples = generate_saw_sample(note.frequency, 0.5, note.duration, seconds_per_beat);
                    break;
                case SQUARE:
                    noteSamples = generate_square_sample(note.frequency, 0.5, note.duration, seconds_per_beat);
                    break;
                case TRIANGLE:
                    noteSamples = generate_triangle_sample(note.frequency, 0.5, note.duration, seconds_per_beat);
                    break;
                case INVALID:
                    break;
            }
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

        return mixBuffer;
}