//  Match Fixture List Generator
//
//  WEBSITE: http://matchgen.sourceforge.net
//
//  NOTE: This software is licensed under the GPL. See: COPYING-GPL.txt
//
//  This program  is distributed in the  hope that it will  be useful, but
//  WITHOUT   ANY  WARRANTY;   without  even   the  implied   warranty  of
//  MERCHANTABILITY  or FITNESS  FOR A  PARTICULAR PURPOSE.   See  the GNU
//  General Public License for more details.
//
//  Jonathan Rennison (or anybody else) is in no way responsible, or liable
//  for this program or its use in relation to users, 3rd parties or to any
//  persons in any way whatsoever.
//
//  You  should have  received a  copy of  the GNU  General Public
//  License along  with this program; if  not, write to  the Free Software
//  Foundation, Inc.,  59 Temple Place,  Suite 330, Boston,  MA 02111-1307
//  USA
//
//  2011 - Jonathan G Rennison - j.g.rennison@gmail.com
//==========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <list>
#include <algorithm>
#include <limits.h>
#include "SimpleOpt.h"
using namespace std;

enum { OPT_MAXTEAMS, OPT_MINTEAMS, OPT_SIMU, OPT_RANDOM, OPT_HELP, OPT_DUPRS, OPT_CREDITS, OPT_DEBUG, OPT_LIMIT,
OPT_BRUTEON, OPT_BRUTEOFF };

int maxteams=20;
int minteams=4;
int simu=2;
bool random=false;
int brute=0;
int repthreshold=0;
int debug=0;
int limit=INT_MAX;


CSimpleOptA::SOption g_rgOptions[] =
{
	{ OPT_MAXTEAMS,		"-m",			SO_REQ_SHRT  },
	{ OPT_MAXTEAMS,		"--max-teams",		SO_REQ_SHRT  },
	{ OPT_MINTEAMS,		"-n",			SO_REQ_SHRT  },
	{ OPT_MINTEAMS,		"--min-teams",		SO_REQ_SHRT  },
	{ OPT_SIMU,		"-s",			SO_REQ_SHRT  },
	{ OPT_SIMU,		"--simultaneous-matches",	SO_REQ_SHRT  },
	{ OPT_RANDOM,		"-r",			SO_NONE  },
	{ OPT_RANDOM,		"--random",		SO_NONE  },
	{ OPT_HELP,		"-h",			SO_NONE  },
	{ OPT_HELP,		"--help",		SO_NONE  },
	{ OPT_HELP,		"-?",			SO_NONE  },
	{ OPT_DUPRS,		"-t",			SO_REQ_SHRT  },
	{ OPT_DUPRS,		"--repeat-lower-simu-threshold",		SO_REQ_SHRT  },
	{ OPT_DEBUG,		"-d",			SO_REQ_SHRT  },
	{ OPT_DEBUG,		"--debug",		SO_REQ_SHRT  },
	{ OPT_LIMIT,		"-l",			SO_REQ_SHRT  },
	{ OPT_LIMIT,		"--limit",		SO_REQ_SHRT  },
	{ OPT_BRUTEON,		"--brute-force-game-set",SO_NONE  },
	{ OPT_BRUTEOFF,		"--dont-brute-force-game-set",SO_NONE  },
	{ OPT_BRUTEON,		"-b",			SO_NONE  },
	{ OPT_BRUTEOFF,		"-f",			SO_NONE  },
	SO_END_OF_OPTIONS
};

char argtext[]={
"Match Fixture List Generator: version: " __TIMESTAMP__ "\n"
"Author: Jonathan G. Rennison\n"
"License: GPLv2\n"
"Credits: SimpleOpt command-line processing library v3.4 by Brodie Thiesfield,\n"
"	(modified slightly)\n\n"
"Command line switches:\n"
"-?, -h, --help\n"
"	Display this help\n"
"-m num, --max-teams num\n"
"	Sets the maximum number of teams (n) to num. Default 20.\n"
"-n num, --min-teams num\n"
"	Sets the minimum number of teams (n) to num. Default 4.\n"
"-s num, --simultaneous-matches num\n"
"	Sets the number of simultaneous matches (s) to num. Default 2.\n"
"-r, --random\n"
"	Produce a randomised fixture list.\n"
"-t num, --repeat-lower-simu-threshold num\n"
"	Repeat the fixture list generation from the minimum number of teams to\n"
"	this threshold, with the number of simultaneous matches reduced by one.\n"
"-l num, --limit num\n"
"	Only display the first num match sets, for any given number of teams.\n"
"-d num, --debug num\n"
"	Sets the debug level to num.\n"
"-b, --brute-force-game-set\n"
"	Brute-forces generating a set of simultaneous matches from game costs.\n"
"-f, --dont-brute-force-game-set\n"
"	Never use the above brute-forcing method.\n"
"\n"
"Note that the algorithm used is of order O(n^6) (!!!) when generating a\n"
"full fixture list, as this program is intended to be used with a small (<40)\n"
"number of teams (n). Consider using -l/--limit for larger numbers of teams.\n"
"\n"
"The brute-forcing method is intended to solve issues of poor results when the\n"
"number of teams (n) approaches twice the number of simultaneous matches (s),\n"
"for large values of s. It is normally applied when 2s<=n<2s+2. It is *very*\n"
"slow for large n and/or large s.\n"
"The computation order for this step is approximately: O(n^2s).\n\n"
};

void cmdline(char *argv[], int argc) {
	CSimpleOptA args(argc, argv, g_rgOptions, SO_O_NOSLASH|SO_O_CLUMP|SO_O_EXACT|SO_O_SHORTARG|SO_O_FILEARG|SO_O_CLUMP_ARGD);
	while (args.Next()) {
		if (args.LastError() != SO_SUCCESS) {
			printf("Argument Error: %s, Option Text: %s\nTry --help\n", args.LastError(), args.OptionText());
			exit(1);
		}
		switch(args.OptionId()) {
			case OPT_HELP: {
				printf("%s", argtext);
				exit(0);
			}
			case OPT_MAXTEAMS:
				maxteams=atoi(args.OptionArg());
				if(maxteams<2 || maxteams>99) {
					printf("Maximum teams must be between 2 and 99. You supplied: %s\n", args.OptionArg());
				}
				break;
			case OPT_MINTEAMS:
				minteams=atoi(args.OptionArg());
				if(minteams<2 || minteams>99) {
					printf("Minimum teams must be between 2 and 99. You supplied: %s\n", args.OptionArg());
				}
				break;
			case OPT_SIMU:
				simu=atoi(args.OptionArg());
				if(simu<1 || simu>10) {
					printf("Simultaneous games must be between 1 and 10. You supplied: %s\n", args.OptionArg());
				}
				break;
			case OPT_DEBUG:
				debug=atoi(args.OptionArg());
				break;
			case OPT_RANDOM:
				random=true;
				break;
			case OPT_BRUTEON:
				brute=1;
				break;
			case OPT_BRUTEOFF:
				brute=-1;
				break;
			case OPT_DUPRS:
				repthreshold=atoi(args.OptionArg());
				if(repthreshold<2 || repthreshold>98) {
					printf("Team threshold for repetition with simultaneous game reduction must be between 2 and 98. You supplied: %s\n", args.OptionArg());
				}
				break;
			case OPT_LIMIT:
				limit=atoi(args.OptionArg());
				if(limit<1) {
					printf("Match set limit be 1 or greater. You supplied: %s\n", args.OptionArg());
				}
				break;
			case -1:
				printf("Not an argument: %s, try --help\n", args.OptionArg());
				exit(1);
				break;
		}
	}

}

struct costst {
	unsigned int team1;
	unsigned int team2;
	unsigned int cost;
	int randval;
};

struct fixture {
	unsigned int team1;
	unsigned int team2;
};


inline bool costsortfunc(costst s1, costst s2) {
	if(s1.cost<s2.cost) return true;
	else if(s1.cost>s2.cost) return false;
	else if(random) return (s1.randval<s2.randval);
	else if(s1.team1<s2.team1) return true;
	else if(s1.team1>s2.team1) return false;
	else if(s1.team2<s2.team2) return true;
	else return false;
}

inline bool fixturesortfunc(fixture s1, fixture s2) {
	if(s1.team1<s2.team1) return true;
	else if(s1.team1>s2.team1) return false;
	else if(s1.team2<s2.team2) return true;
	else return false;
}

unsigned int getcost(unsigned int i, unsigned int j, list< vector<fixture> > &prevgames) {
	if(i==j) return INT_MAX;
	unsigned int tcost=0;
	list< vector<fixture> >::reverse_iterator rit;
	unsigned int costmul=prevgames.size();
	for( rit=prevgames.rbegin() ; rit != prevgames.rend(); rit++ ) {
		unsigned int cost=0;
		vector<fixture>::iterator fx;
		for(fx=rit->begin() ; fx != rit->end(); fx++ ) {
			if(fx->team1==i && fx->team2==j) cost+=1000;
			if(fx->team1==i) cost+=1;
			if(fx->team2==j) cost+=1;
			if(fx->team1==j) cost+=1;
			if(fx->team2==i) cost+=1;
		}
		tcost+=cost*costmul;
		costmul--;
	}
	return tcost;
}

int gensimmatches(int simt, unsigned int firstcostcandidate, vector<costst> &costs, vector<fixture> &currentgames, unsigned int &missedcost) {
	int t=simt;
	unsigned int nextcandidatefx=firstcostcandidate;
	unsigned int tcost=0;
	missedcost=0;
	do {
		fixture f1={costs[nextcandidatefx].team1, costs[nextcandidatefx].team2};
		unsigned int curfixturecost=costs[nextcandidatefx].cost;
		nextcandidatefx++;

		//check it
		bool ok=true;
		vector<fixture>::iterator fx;
		for(fx=currentgames.begin() ; fx != currentgames.end(); fx++ ) {
			if(f1.team1==fx->team1) { ok=false;  break; }
			if(f1.team1==fx->team2) { ok=false;  break; }
			if(f1.team2==fx->team1) { ok=false;  break; }
			if(f1.team2==fx->team2) { ok=false;  break; }
		}
		if(!ok) {
			missedcost+=curfixturecost;
			continue;
		}

		//found one
		currentgames.push_back(f1);
		tcost+=curfixturecost;
		t--;
	} while(t);

	return tcost;
}

unsigned int gensimmatchesbrute(vector<fixture> currentfixture, vector<costst> &costs, unsigned int costtotry, unsigned int gamesleft, vector<bool> haveteams, unsigned int runningcost, unsigned int &bestcost, vector<fixture> &bestfixture) {
	unsigned int cost1=UINT_MAX;
	unsigned int cost2=UINT_MAX;
	if(costs[costtotry].cost+runningcost < bestcost) {
		if(!haveteams[costs[costtotry].team1] && !haveteams[costs[costtotry].team2]) {
			vector<fixture> newfixture = currentfixture;
			vector<bool> newhaveteams = haveteams;
			fixture f1={costs[costtotry].team1, costs[costtotry].team2};
			newfixture.push_back(f1);
			newhaveteams[costs[costtotry].team1]=true;
			newhaveteams[costs[costtotry].team2]=true;
			if(gamesleft>1) {
				cost1=gensimmatchesbrute(newfixture, costs, costtotry+1, gamesleft-1, newhaveteams, runningcost+costs[costtotry].cost, bestcost, bestfixture);
			}
			else {
				cost1=runningcost+costs[costtotry].cost;
				bestcost=cost1;
				bestfixture=newfixture;
				return cost1;
			}
		}
		if(costtotry+gamesleft<costs.size()) {
			cost2=gensimmatchesbrute(currentfixture, costs, costtotry+1, gamesleft, haveteams, runningcost, bestcost, bestfixture);
		}
	}
	return min(cost1,cost2);
}

void genfixtureset(int mint, int maxt, int simt) {	//this is order O(n^6) for each number of games
	mint=max(mint,simt*2);				//check that we've got sensible inputs
	if(maxt<mint) return;

	for(int n=mint; n<=maxt; n++) {
		int games=n*(n-1)/2;			//number of games is O(n^2)
		int maxgames=games;

		while(maxgames%simt) maxgames+=games;	//make sure that we end on a integral boundary of game rounds
		maxgames/=simt;

		maxgames=min(maxgames, limit);

		list< vector<fixture> > prevgames;

		printf("%d teams\n",n);

		for(unsigned int c=0; c<maxgames; c++) {
			vector<costst> costs(n*(n-1)/2);

			unsigned int costnum=0;
			for(unsigned int i=0; i<n; i++) {
				for(unsigned int j=i+1; j<n; j++) {			//number of costs to calculate for each game is order O(n^2) per game
					costs[costnum].team1=i;
					costs[costnum].team2=j;
					costs[costnum].cost=getcost(i,j, prevgames);	//each cost is of same order to calculate as number of (previous) games, which is O(n^2)
					if(random) costs[costnum].randval=rand();
					if(debug>=3) printf("%d, %d, %d\n", i+1, j+1, costs[costnum].cost);
					costnum++;
				}
			}

			vector<fixture> currentgames;

			sort(costs.begin(), costs.end(), costsortfunc);

			if((n>=((2*simt)+2) && brute==0) || brute<0) {	//normal mode, pick first games that fit, greedy

				//now calculate actual games
				unsigned int missedcost;
				unsigned int tcost=gensimmatches(simt, 0, costs, currentgames, missedcost);
				if(debug>=2) printf("Sim Match Total: %d, missed: %d\n", tcost, missedcost);

			}
			else {			//not many games to choose from, do it more thoroughly/brute force it
				vector<bool> haveteams(n,0);
				unsigned int bestcost=UINT_MAX;
				vector<fixture> currentfixture;
				unsigned int gotcost=gensimmatchesbrute(currentfixture, costs, 0, simt, haveteams, 0, bestcost, currentgames);
			}

			sort(currentgames.begin(), currentgames.end(), fixturesortfunc);

			prevgames.push_back(currentgames);

			vector<fixture>::iterator fx=currentgames.begin();
			do {
				printf("%2d v %2d", fx->team1+1, fx->team2+1);
				fx++;
				if(fx == currentgames.end()) break;
				printf("\t");				//don't add a tab to the last entry

			} while(true);
			printf("\n");
		}

		printf("\n");

		if(debug>0) {		//print matrix, it's symmetric, and should have 0 on the diagonals
			vector<unsigned int> matchmatrix(n*n,0);
			list< vector<fixture> >::iterator it;
			unsigned int total=0;
			for( it=prevgames.begin() ; it != prevgames.end(); it++ ) {
				vector<fixture>::iterator fx;
				for(fx=it->begin() ; fx != it->end(); fx++ ) {
					(matchmatrix[fx->team2+(fx->team1*n)])++;
					(matchmatrix[fx->team1+(fx->team2*n)])++;
					total++;
				}
			}
			printf("    ");
			for(unsigned int i=0; i<n; i++) printf("%2d ", i+1);
			printf("= %3d\n", total);
			for(unsigned int i=0; i<n; i++) {
				printf("%2d: ", i+1);
				unsigned int rowtotal=0;
				for(unsigned int j=0; j<n; j++) {
					printf("%2d ", matchmatrix[j+(n*i)]);
					rowtotal+=matchmatrix[j+(n*i)];
				}
				printf("= %3d\n", rowtotal);
			}
			printf("\n");
		}
	}
}

int main( int argc, char *argv[]) {
	cmdline(argv, argc);
	if(random) srand (time(NULL));

	genfixtureset(minteams, maxteams, simu);

	if(repthreshold) genfixtureset(minteams, repthreshold, simu-1);

	return 0;
}

