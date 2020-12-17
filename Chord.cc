#include "Chord.h"
#include <cassert>
#include <map>
#include <cstring>
#include <cstdlib>

static const char *FLAT="♭", *SHARP="♯";

static std::string notes[12] = {
	"C", "C♯", "D", "D♯", "E",
	"F", "F♯", "G", "G♯", "A", "A♯", "B"
};

static std::map<std::string, std::string> synonyms = {
	{"D♭", "C♯"},
	{"E♭", "D♯"},
	{"G♭", "F♯"},
	{"A♭", "G♯"},
	{"B♭", "A♯"},
	{"F♭", "E"},
	{"E♯", "F"},
	{"C♭", "B"},
	{"B♯", "C"},
};

int Chord::parse_note(const char *s)
{
	if (!s || !*s) return -1;
	
	bool neg = false;
	int b = FREE_CHORD;
	if (*s == '-' && isdigit(s[1]))
	{
		neg = true; ++s;
	}
	if (isdigit(*s))
	{
		b = *s - '0'; ++s;
		while (isdigit(*s)) { b *= 10; b += *s-'0'; ++s; }
		if (neg) b = -b;
	}

	int n = -1;
	switch (*s)
	{
		case 'C': n =  0; break;
		case 'D': n =  2; break;
		case 'E': n =  4; break;
		case 'F': n =  5; break;
		case 'G': n =  7; break;
		case 'A': n =  9; break;
		case 'B': n = 11; break;
		default: return -1;
	}
	++s;
	while (*s)
	{
		if      (*s == '#') { ++n; ++s; }
		else if (*s == 'b') { --n; ++s; }
		else if (isspace((unsigned char)*s)) {  }
		else if (!strcmp(s, FLAT )) { --n; s += strlen(FLAT ); }
		else if (!strcmp(s, SHARP)) { --n; s += strlen(SHARP); }
		else return -1;
	}

	if (b == FREE_CHORD)
	{
		while (n < 0) n += 12;
		n %= 12;
		return PITCH_CLASS + n;
	}
	return b*12 + n;
}

std::string Chord::name() const
{
	if (C.empty()) return std::string();

	// only works for normalized chords!
	
	static std::map<std::string, std::string> names = {
		{"33",    "augmented dominant"},
		{"11102", "augmented 11th"},
		{"332",   "augmented major 7th"},
		{"331",   "augmented 7th"},
		{"53",    "augmented 6th"},
		{"22",    "diminished"},
		{"224",   "diminished major 7th"},
		{"222",   "diminished 7th"},
		{"32",    "major"},
		{"11012", "dominant 11th"},
		{"0222",  "dominant minor 9th"},
		{"1122",  "dominant 9th"},
		{"322",   "dominant 7th"},
		{"313",   "dominant 7th flat 5"},
		{"2022",  "dominant 7th # 9 (Hendrix)"},
		{"110110", "dominant 13th"},
		{"400",    "Dream"},
		{"0221",   "Elektra"},
		{"3301", "Farben"},
		{"223", "half-diminished 7th"},
		{"322", "harmonic 7th"},
		{"3103", "Lydian"},
		{"11013", "major 11th"},
		{"323", "major 7th"},
		{"3112", "major 7th # 11th"},
		{"321", "major 6th"},
		{"1121", "major 6 add 9"},
		{"1123", "major 9th"},
		{"111011", "major 13th"},
		{"23", "minor"},
		{"10112", "minor 11th"},
		{"233", "minor major 7th"},
		{"1032", "minor 9th"},
		{"232", "minor 7th"},
		{"231", "minor 6th"},
		{"1031", "minor 6/9"},
		{"101110", "minor 13th"},
		{"112", "Mu"},
		{"11120", "Mystic"},
		{"1131", "9th augmented 5th"},
		{"1113", "9th flat 5th"},
		{"02020", "Ode to Napoleon"},
		{"02102", "Petrushka"},
		{"6", "powerchord"},
		{"3210", "seven six"},
		{"412", "7th suspended 4"},
		{"2112", "So What"},
		{"41", "suspended 4th"},
		{"223", "Tristan"},
		{"14", "suspended 2nd"},
	};

	const int n = (int)C.size();
	std::string key(n, 'x');
	int k = 11;
	for (int i = 0; i < n; ++i)
	{
		key[i] = '0'+C[i]; // garbage is ok - will not match
		k -= C[i]+1;
	}
	char c = '0'+k;

	// try chord and all inversions
	std::set<std::string> found;
	std::string ret;
	for (int j = 0; j <= n; ++j)
	{
		auto it = names.find(key);
		if (it != names.end() && !found.count(it->second))
		{
			if (!ret.empty()) ret += " / ";
			ret += it->second;
			found.insert(it->second);
		}
		char tmp = key[0];
		for (int i = 1; i < n; ++i) key[i-1] = key[i];
		key[n-1] = c; c = tmp;
	}
	return ret;
}
	
void Chord::invert()
{
	if (C.empty()) return;
	int k = 10 - C[0];
	for (int i = 1, n = (int)C.size(); i < n; ++i)
	{
		C[i-1] = C[i];
		k -= C[i]+1;
	}
	C.back() = k;
}

std::set<int> Chord::keys(bool normalize) const
{
	std::set<int> K;
	int k = base;
	if (k == FREE_CHORD) k = 0;
	else if (k >= PITCH_CLASS) k -= PITCH_CLASS;
	K.insert(k);
	for (T c : C)
	{
		k += c+1;
		if (normalize)
		{
			while (k <  base)    k += 12;
			while (k >= base+12) k -= 12;
		}
		K.insert(k);
	}
	return K;
}

void Chord::normalize()
{
	auto K = keys(true);
	C.clear();
	auto it = K.begin();
	int k = *it++;
	for (auto end = K.end(); it != end; ++it)
	{
		int kk = *it;
		assert(kk > k);
		C.push_back(kk-k-1);
		k = kk;
	}
}

bool Chord::good() const
{
	if (C.empty()) return true;
	int k = 11;
	for (T c : C)
	{
		if (c <= 0) return false;
		k -= c+1;
	}
	while (k < 0) k += 12;
	k %= 12;
	return (k > 0);
}

bool Chord::inc(int M)
{
	if (C.empty()) return false;
	int k = 0;
	for (T c : C) k += c;
	if (k > M) return false;
	++C.back(); ++k; if (k <= M) return true;
	for (int i = (int)C.size()-2; i >= 0; --i)
	{
		k -= C[i+1]; C[i+1] = 0;
		++C[i]; ++k;
		if (k <= M) return true;
	}
	return false;
}

std::ostream& operator<<(std::ostream &o, const Chord &chord)
{
	int k = 11; for (Chord::T c : chord.C) k -= c+1;

	if (chord.base == FREE_CHORD)
	{
		bool longform = (k < 0 || k >= 10);
		if (!longform) for (Chord::T c : chord.C)
		{
			if (c >= 0 && c < 10) continue;
			longform = true;
			break;
		}
		if (longform)
		{
			o << '(';
			for (Chord::T c : chord.C) o << (int)c << ',';
			o << k << ')';
		}
		else
		{
			for (Chord::T c : chord.C) o << (char)('0'+c);
			o << (char)('0'+k);
		}
	}
	else if (chord.base >= PITCH_CLASS)
	{
		int k = chord.base - PITCH_CLASS;
		assert(k >= 0 && k < 12);
		o << notes[k];
		for (Chord::T c : chord.C)
		{
			k += c+1;
			while (k < 0) k += 12;
			k %= 12;
			o << '/' << notes[k];
		}
	}
	else
	{
		int k = chord.base;
		int n = k/12;
		k -= n*12;
		while (k < 0) { k += 12; --n; }
		while (k >= 12) { assert(false); k -= 12; ++n; }
		o << n << notes[k];
		for (Chord::T c : chord.C)
		{
			k += c+1;
			o << '/';
			if (k < 0 || k > 11)
			{
				while (k <  0) { k += 12; --n; }
				while (k > 11) { k -= 12; ++n; }
				o << n;
			}
			o << notes[k];
		}
	}
	return o;
}

