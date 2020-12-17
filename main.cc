#include <iostream>
#include <vector>
using std::cerr;
using std::cout;
using std::endl;
#include <cstring>
#include <set>
#include <vector>
#include "Chord.h"

int usage(const char *p)
{
	cerr << "Usage: " << p << " [all] [Number of keys] [base tone]" << endl;
	cerr << "Number between 1 and 12" << endl;
	return 1;
}

int main(int argc, char **argv)
{
	int min = 2, max = 12;
	bool good = true;
	int base = FREE_CHORD;

	for (int i = 1; i < argc; ++i)
	{
		int tmp;
		if (strcmp(argv[i], "all") == 0)
		{
			good = false;
		}
		else if ((tmp = Chord::parse_note(argv[i])) != -1)
		{
			base = tmp;
		}
		else
		{
			char *ep;
			int n = strtol(argv[i], &ep, 10);
			if (!*argv[i] || *ep || n < 1 || n > 12)
				return usage(argv[0]);
			min = max = n;
		}
	}
	--min; --max;

	for (int N = min; N <= max; ++N)
	{
		bool any_found = false;
		std::set<std::vector<Chord::T>> done;

		Chord c(N, base);
		while (true)
		{
			if (!done.count(c.C) && (!good || c.good()))
			{
				if (!any_found)
				{
					cout << (good ? "Good chords" : "Chords") << " with " << (N+1) << " keys:\n";
					any_found = true;
				}

				for (int j = 0; j <= N; ++j)
				{
					if (!done.count(c.C))
					{
						done.insert(c.C);
						if (j > 0) cout << " = ";
						cout << c;
					}
					c.invert();
				}
				auto name = c.name();
				if (!name.empty()) cout << " (" << name << ")";
				cout << "\n";
			}
			bool ok = c.inc(11-N);
			if (!ok) break;
		}
		if (min != max && any_found) cout << std::endl;
	}
}
