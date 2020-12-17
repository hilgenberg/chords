#pragma once
#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <limits.h>

constexpr int FREE_CHORD = INT_MIN;
constexpr int PITCH_CLASS = INT_MAX-12;

struct Chord
{
	static int parse_note(const char *s);

	typedef int8_t T;
	std::vector<T> C;
	int base;

	Chord(int n = 0, int base = FREE_CHORD) : C(n, 0), base(base) {}
	
	void add(T c) { C.push_back(c); }
	
	std::set<int> keys(bool normalize) const;

	void invert();
	bool inc(int max_sum);
	bool good() const; // no single semitone intervals?
	void normalize();
	std::string name() const; // returns empty string if unknown

	friend std::ostream& operator<<(std::ostream &o, const Chord &c);

};
