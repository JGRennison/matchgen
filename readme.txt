Match Fixture List Generator: version: Fri Feb 14 21:26:50 2014
Author: Jonathan G. Rennison
License: GPLv2
Credits: SimpleOpt command-line processing library v3.4 by Brodie Thiesfield,
	(modified slightly)

Command line switches:
-?, -h, --help
	Display this help
-m num, --max-teams num
	Sets the maximum number of teams (n) to num. Default 20.
-n num, --min-teams num
	Sets the minimum number of teams (n) to num. Default 4.
-g num, --teams num
	Sets both the minimum and maximum number of teams (n) to num.
-s num, --simultaneous-matches num
	Sets the number of simultaneous matches (s) to num. Default 2.
-r, --random
	Produce a randomised fixture list.
-p, --position-balance
	For s>1, balance the distribution of each team across each of the
	simultaneous match positions. Otherwise sort by team number.
-t num, --repeat-lower-simu-threshold num
	Repeat the fixture list generation from the minimum number of teams to
	this threshold, with the number of simultaneous matches reduced by one.
-l num, --limit num
	Only display the first num match sets, for any given number of teams.
-d num, --debug num
	Sets the debug level to num.
	Debug level one displays some result statistics.
	Level two also displays nested team and play count warnings.
	Higher than that is likely only of interest to developers.
-b, --brute-force-game-set
	Brute-forces generating a set of simultaneous matches from game costs.
-f, --dont-brute-force-game-set
	Never use the above brute-forcing method.
-i, --header
	Display an informational header at the start of the output.
-o file, --output-file file
	Also write output (excluding debug information) to 'file'.
	'file' must be representable in the locale/ANSI character set.

Note that generating a full fixture list is an operation of approximate order
O(n^6) in time/computation (!!!).
This program is intended to be used with a small (<60) number of teams (n).
Consider using -l/--limit for larger numbers of teams.

The brute-forcing method is intended to solve issues of poor results when the
number of teams (n) approaches twice the number of simultaneous matches (s),
and/or when s is large. By default it is used when 2s<=n<=2s+2.
It will however produce better or equal results for all n, s.
It can be rather slow for large n and large s, as the computation order for
calculating a single match set is, in the worst case: O(n^s).
The default method is a simple greedy search.
