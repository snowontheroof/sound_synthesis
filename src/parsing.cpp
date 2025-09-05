#include "minisynth.hpp"
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>

Type	find_type(std::string instrument)
{
	if (instrument == "sine")
		return SINE;
	if (instrument == "saw")
		return SAW;
	if (instrument == "square")
		return SQUARE;
	if (instrument == "triangle")
		return TRIANGLE;
	return INVALID;
}

std::map<std::string, double>	make_freq()
{
	std::ifstream	file("../hertz.txt");
	std::map<std::string, double>	freq;
	std::string		buffer;

	while (std::getline(file, buffer))
	{
		size_t	pos = buffer.find('=');
		std::string	pitch = buffer.substr(0, pos);
		double	value = std::stod(buffer.substr(pos + 1));
		freq.insert({pitch, value});
	}
	return freq;
}

double	find_freq(std::string note, std::map<std::string, double> freq)
{
	std::map<std::string, double>::const_iterator	it = freq.find(note);
	if (it != freq.end())
	{
		return it->second;
	}
	return -1.0;
}

std::string	fill_note(std::string notestr, std::vector<Notes> note, size_t idx)
{
	int	notelength = notestr.length();
	if (notelength == 3)
		return notestr;
	if (notelength == 1 || (notelength == 2 && (notestr[1] == '#' || notestr[1] == 'b')))
	{
			if (idx > 0)
			{
				int	length = note[idx - 1].pitch.length();
				notestr += note[idx - 1].pitch[length - 1];
			}
			else
				notestr += "4";
	}
	return notestr;
}

Data	parseFile(std::string content)
{
	Data	test;
	size_t	j = 0;
	size_t	k = 0;
	size_t	l = 0;

	size_t	i = content.find('t');
	while (content[i])
	{
		if (content.substr(i, 6) == "tempo " && (i == 0 || content[i - 1] == '\n'))
		{
			j = i + 6;
			k = j;
			while (std::isdigit(content[j]))
				j++;
			std::string	tmp = content.substr(k, (j - k));
			test.tempo = std::stoi(tmp);
			// std::cout << "tempo is " << test.tempo << std::endl;
		}
		i++;
	}
	i = j + 1;
	while (content[i])
	{
		if (content.substr(i, 7) == "tracks " && content[i - 1] == '\n')
		{
			j = i + 7;
			k = j;
			test.track_amt = 1;
			while (content[j] != '\n')
			{
				if (content[j] == ',')
					test.track_amt++;
				j++;
			}
			break;
		}
		i++;
	}
	// std::cout << "track amt is " << test.track_amt << std::endl;
	test.tracks = new Track[test.track_amt];
	j = i + 7;
	for (size_t l = 0; l < test.track_amt && content[j]; l++)
	{
		k = j;
		while (content[j] != ',' && content[j] != '\n')
			j++;
		std::string	tmp = content.substr(k, (j - k));
		test.tracks[l].instrument = find_type(tmp);
		test.tracks[l].track_dur = 0;
		// std::cout << test.tracks[l].instrument << std::endl;
		j++;
	}
	while (content[j])
	{
		while (content[j] && !(std::isdigit(content[j]) && content[j - 1] == '\n'))
			j++;
		if (!content[j])
			break;
		size_t	o = j;
		while (content[j] != ':')
			j++;
		size_t	track_idx = std::stoi(content.substr(o, (j - o))) - 1;
		j += 2;
		k = j;
		while (content[j] != '\n')
			j++;
		std::string	string = content.substr(k, (j - k));
		std::istringstream	iss(string);
		std::string	token;
		std::map<std::string, double>	freq = make_freq();

		int	idx = 0;
		while (iss >> token)
		{
			if (token[0] == '|')
				continue;
			size_t	pos = token.find('/');
			double	frequency;
			double	duration;
			if (pos != std::string::npos)
			{
				std::string	note = fill_note(token.substr(0, pos), test.tracks[track_idx].notes, idx);
				frequency = find_freq(note, freq);
				duration = std::stod(token.substr(pos + 1));
				test.tracks[track_idx].notes.push_back({note, duration, frequency});
			}
			else
			{
				std::string	note = fill_note(token, test.tracks[track_idx].notes, idx);
				frequency = find_freq(note, freq);
				if (idx > 0)
					duration = test.tracks[track_idx].notes[idx - 1].duration;
				else
					duration = 1.0;
				test.tracks[track_idx].notes.push_back({note, duration, frequency});
			}
			test.tracks[track_idx].track_dur += duration;
			idx++;
		}
		while (content[j] != '\n')
			j++;
		j++;
	}
	// for (size_t m = 0; m < test.track_amt; m++)
	// {
	// 	std::cout << "Track no " << m << ": instrument: " << test.tracks[m].instrument
	// 		<< ", dur: " << test.tracks[m].track_dur << ", notes: \n";
	// 	for (const auto& t : test.tracks[m].notes)
	// 		std::cout << "duration: " << t.duration << ", freq: " << t.frequency
	// 			<< ", pitch: " << t.pitch << std::endl;
	// }
	return test;
}

Data	parser(std::string input)
{
	std::string		oneLine;
	std::string		fileContent;
	Data			parsedData;

	std::ifstream	ReadFile(input);
	while (std::getline(ReadFile, oneLine))
		fileContent = fileContent + oneLine + '\n';
	ReadFile.close();
	return parseFile(fileContent);
	//rest of the program
	// delete[] parsedData.tracks;
}
//
// int	main(int argc, char **argv)
// {
// 	if (argc != 2)
// 		std::cout << "Usage: ./minisynth <file>\n";
// 	else
// 		minisynth(static_cast<std::string>(argv[1]));

// 	return 0;
// }
