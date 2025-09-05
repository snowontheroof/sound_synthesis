#include "../inc/minisynth.hpp"
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
	std::ifstream	file("hertz.txt");
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

size_t	find_tempo(std::string content, Data& data)
{
	size_t	i = content.find('t');
	size_t	j = 0;
	size_t	k = 0;

	while (content[i])
	{
		if (content.substr(i, 6) == "tempo " && (i == 0 || content[i - 1] == '\n'))
		{
			j = i + 6;
			k = j;
			while (std::isdigit(content[j]))
				j++;
			std::string	tmp = content.substr(k, (j - k));
			data.tempo = std::stoi(tmp);
		}
		i++;
	}
	i = j + 1;
	return i;
}

size_t	find_track_amt(std::string content, Data& data, size_t i)
{
	size_t	j = 0;
	size_t	k = 0;

	while (content[i])
	{
		if (content.substr(i, 7) == "tracks " && content[i - 1] == '\n')
		{
			j = i + 7;
			k = j;
			data.track_amt = 1;
			while (content[j] != '\n')
			{
				if (content[j] == ',')
					data.track_amt++;
				j++;
			}
			break;
		}
		i++;
	}
	i += 7;
	return i;
}

size_t	find_instrument(std::string content, Data &data, size_t i)
{
	size_t	k = 0;

	for (size_t l = 0; l < data.track_amt && content[i]; l++)
	{
		k = i;
		while (content[i] != ',' && content[i] != '\n')
			i++;
		std::string	tmp = content.substr(k, (i - k));
		data.tracks[l].instrument = find_type(tmp);
		data.tracks[l].track_dur = 0;
		data.tracks[l].volume = 1;
		i++;
	}
	return i;
}

size_t	find_volume(std::string content, Data& data, size_t i)
{
	size_t	k = 0;

	if (content.substr(i, 7) == "volume " && content[i - 1] == '\n')
	{
		i += 7;
		for (size_t l = 0; l < data.track_amt && content[i]; l++)
		{
			k = i;
			while (content[i] != ',' && content[i] != '\n')
				i++;
			std::string	tmp = content.substr(k, (i - k));
			data.tracks[l].volume = std::stoi(tmp);
			i++;
		}
	}
	return i;
}

void	find_notes(std::string string, Data& data, size_t track_idx)
{
	std::map<std::string, double>	freq = make_freq();
	std::istringstream				iss(string);
	std::string						token;

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
			std::string	note = fill_note(token.substr(0, pos), data.tracks[track_idx].notes, idx);
			frequency = find_freq(note, freq);
			duration = std::stod(token.substr(pos + 1));
			data.tracks[track_idx].notes.push_back({note, duration, frequency});
		}
		else
		{
			std::string	note = fill_note(token, data.tracks[track_idx].notes, idx);
			frequency = find_freq(note, freq);
			if (idx > 0)
				duration = data.tracks[track_idx].notes[idx - 1].duration;
			else
				duration = 1.0;
			data.tracks[track_idx].notes.push_back({note, duration, frequency});
		}
		data.tracks[track_idx].track_dur += duration;
		idx++;
	}
}

Data	parseFile(std::string content)
{
	Data							data;
	size_t							k = 0;

	size_t	i = find_tempo(content, data);
	i = find_track_amt(content, data, i);
	data.tracks = new Track[data.track_amt];
	i = find_instrument(content, data, i);
	while (content[i] == '\n' && content[i] == '#')
	{
		if (content[i] == '#')
		{
			while (content[i] != '\n')
				i++;
		}
		i++;
	}
	i = find_volume(content, data, i);
	while (content[i])
	{
		while (content[i] && !(std::isdigit(content[i]) && content[i - 1] == '\n'))
			i++;
		if (!content[i])
			break;
		size_t	j = i;
		while (content[i] != ':')
			i++;
		size_t	track_idx = std::stoi(content.substr(j, (i - j))) - 1;
		i += 2;
		k = i;
		while (content[i] != '\n')
			i++;
		std::string	string = content.substr(k, (i - k));
		find_notes(string, data, track_idx);
		while (content[i] != '\n')
			i++;
		i++;
	}
	// for (size_t m = 0; m < data.track_amt; m++)
	// {
	// 	std::cout << "Track no " << m << ": instrument: " << data.tracks[m].instrument
	// 		<< ", dur: " << data.tracks[m].track_dur<< ", volume: " << data.tracks[m].volume
	// 		<< ", notes: \n";
	// 	for (const auto& t : data.tracks[m].notes)
	// 		std::cout << "duration: " << t.duration << ", freq: " << t.frequency
	// 			<< ", pitch: " << t.pitch << std::endl;
	// }
	return data;
}

Data	parser(std::string input)
{
	std::string		oneLine;
	std::string		fileContent;

	std::ifstream	ReadFile(input);
	while (std::getline(ReadFile, oneLine))
		fileContent = fileContent + oneLine + '\n';
	ReadFile.close();
	return parseFile(fileContent);
}
