//  Match Fixture List Generator
//
//  WEBSITE: https://github.com/JGRennison/matchgen
//  WEBSITE: https://bitbucket.org/JGRennison/matchgen
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
#include <string>
#include <limits.h>
#include <math.h>
#include "SimpleOpt.h"
using namespace std;

#define DEBUG_MATRIX 1
#define DEBUG_GAP 1
#define DEBUG_NEST 2
#define DEBUG_PLAYCOUNT 2
#define DEBUG_TCOSTCOND 5
#define DEBUG_TCOSTALL 6
#define DEBUG_COSTDUMP 4
#define DEBUG_MATCHTOTAL 3
#define DEBUG_POSDIST 1

enum { OPT_MAXTEAMS, OPT_MINTEAMS, OPT_SIMU, OPT_RANDOM, OPT_HELP, OPT_DUPRS, OPT_CREDITS, OPT_DEBUG, OPT_LIMIT,
OPT_BRUTEON, OPT_BRUTEOFF, OPT_SETTEAMS, OPT_HEADER, OPT_OUTPUT, OPT_POSBALANCE, OPT_CIRCULAR };

int maxteams=20;
int minteams=4;
int simu=2;
bool randomflag=false;
int brute=0;
int repthreshold=0;
int debug=0;
unsigned int limit=UINT_MAX;
bool header=false;
char *outfilename=0;
FILE *outhandle=0;
bool posbalance=false;
bool circular=false;


CSimpleOptA::SOption g_rgOptions[] =
{
	{ OPT_MAXTEAMS,		"-m",			SO_REQ_SHRT  },
	{ OPT_MAXTEAMS,		"--max-teams",		SO_REQ_SHRT  },
	{ OPT_SETTEAMS,		"-g",			SO_REQ_SHRT  },
	{ OPT_SETTEAMS,		"--set-teams",		SO_REQ_SHRT  },
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
	{ OPT_HEADER,		"-i",			SO_NONE  },
	{ OPT_HEADER,		"--header",		SO_NONE  },
	{ OPT_OUTPUT,		"-o",			SO_REQ_SHRT  },
	{ OPT_OUTPUT,		"--output-file",	SO_REQ_SHRT  },
	{ OPT_POSBALANCE,	"-p",			SO_NONE  },
	{ OPT_POSBALANCE,	"--position-balance",	SO_NONE  },
	{ OPT_CIRCULAR,		"-c",			SO_NONE  },
	{ OPT_CIRCULAR,		"--circular",		SO_NONE  },
	SO_END_OF_OPTIONS
};

char version[]="Match Fixture List Generator: version: " __TIMESTAMP__;

char argtext[]={
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
"-g num, --teams num\n"
"	Sets both the minimum and maximum number of teams (n) to num.\n"
"-s num, --simultaneous-matches num\n"
"	Sets the number of simultaneous matches (s) to num. Default 2.\n"
"-r, --random\n"
"	Produce a randomised fixture list.\n"
"-p, --position-balance\n"
"	For s>1, balance the distribution of each team across each of the\n"
"	simultaneous match positions. Otherwise sort by team number.\n"
"-t num, --repeat-lower-simu-threshold num\n"
"	Repeat the fixture list generation from the minimum number of teams to\n"
"	this threshold, with the number of simultaneous matches reduced by one.\n"
"-l num, --limit num\n"
"	Only display the first num match sets, for any given number of teams.\n"
"-d num, --debug num\n"
"	Sets the debug level to num.\n"
"	Debug level one displays some result statistics.\n"
"	Level two also displays nested team and play count warnings.\n"
"	Higher than that is likely only of interest to developers.\n"
"-b, --brute-force-game-set\n"
"	Brute-forces generating a set of simultaneous matches from game costs.\n"
"-f, --dont-brute-force-game-set\n"
"	Never use the above brute-forcing method.\n"
"-i, --header\n"
"	Display an informational header at the start of the output.\n"
"-o file, --output-file file\n"
"	Also write output (excluding debug information) to 'file'.\n"
"	'file' must be representable in the locale/ANSI character set.\n"
"\n"
"Note that generating a full fixture list is an operation of approximate order\n"
"O(n^6) in time/computation (!!!).\n"
"This program is intended to be used with a small (<60) number of teams (n).\n"
"Consider using -l/--limit for larger numbers of teams.\n"
"\n"
"The brute-forcing method is intended to solve issues of poor results when the\n"
"number of teams (n) approaches twice the number of simultaneous matches (s),\n"
"and/or when s is large. By default it is used when 2s<=n<=2s+2.\n"
"It will however produce better or equal results for all n, s.\n"
"It can be rather slow for large n and large s, as the computation order for\n"
"calculating a single match set is, in the worst case: O(n^s).\n"
"The default method is a simple greedy search.\n"
};

void cmdline(char *argv[], int argc) {
	CSimpleOptA args(argc, argv, g_rgOptions, SO_O_NOSLASH|SO_O_CLUMP|SO_O_EXACT|SO_O_SHORTARG|SO_O_FILEARG|SO_O_CLUMP_ARGD);
	while (args.Next()) {
		if (args.LastError() != SO_SUCCESS) {
			printf("Argument Error: %d, Option Text: %s\nTry --help\n", args.LastError(), args.OptionText());
			exit(1);
		}
		switch(args.OptionId()) {
			case OPT_HELP: {
				printf("%s\n%s", version, argtext);
				exit(0);
			}
			case OPT_MAXTEAMS:
				maxteams=atoi(args.OptionArg());
				if(maxteams<2) {
					printf("Maximum teams should be 2 or greater. You supplied: %s\n", args.OptionArg());
				}
				break;
			case OPT_MINTEAMS:
				minteams=atoi(args.OptionArg());
				if(minteams<2) {
					printf("Minimum teams should be 2 or greater. You supplied: %s\n", args.OptionArg());
				}
				break;
			case OPT_SETTEAMS:
				minteams=atoi(args.OptionArg());
				maxteams=minteams;
				if(minteams<2) {
					printf("Teams should be 2 or greater. You supplied: %s\n", args.OptionArg());
				}
				break;
			case OPT_SIMU:
				simu=atoi(args.OptionArg());
				if(simu<1) {
					printf("Simultaneous games should be 1 or greater. You supplied: %s\n", args.OptionArg());
				}
				break;
			case OPT_DEBUG:
				debug=atoi(args.OptionArg());
				break;
			case OPT_RANDOM:
				randomflag=true;
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
					printf("Team threshold for repetition with simultaneous game reduction should be between 2 and 98. You supplied: %s\n", args.OptionArg());
				}
				break;
			case OPT_LIMIT:
				limit=max(atoi(args.OptionArg()),0);
				if(limit<1) {
					printf("Match set limit should be 1 or greater. You supplied: %s\n", args.OptionArg());
				}
				break;
			case OPT_HEADER:
				header=true;
				break;
			case OPT_OUTPUT:
				outfilename=args.OptionArg();
				break;
			case OPT_POSBALANCE:
				posbalance=true;
				break;
			case OPT_CIRCULAR:
				circular=true;
				break;
			case -1:
				printf("Not an argument: %s, try --help\n", args.OptionArg());
				exit(1);
				break;
		}
	}
	if(outfilename) {
		outhandle=fopen(outfilename,"w");
		if(!outhandle) {
			printf("Cannot open file for writing: %s\n", outfilename);
			exit(2);
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

struct costfuncparams {
	double targetgap;
	bool circular;
	unsigned int circle_length;
};


inline bool costsortfunc(const costst s1, const costst s2) {
	if(s1.cost<s2.cost) return true;
	else if(s1.cost>s2.cost) return false;
	else if(randomflag) return (s1.randval<s2.randval);
	else if(s1.team1<s2.team1) return true;
	else if(s1.team1>s2.team1) return false;
	else if(s1.team2<s2.team2) return true;
	else return false;
}

inline bool fixturesortfunc(const fixture s1, const fixture s2) {
	if(s1.team1<s2.team1) return true;
	else if(s1.team1>s2.team1) return false;
	else if(s1.team2<s2.team2) return true;
	else return false;
}

inline unsigned int getcost(unsigned int i, unsigned int j, list< vector<fixture> > &prevgames, unsigned int n, const costfuncparams &cfp) {
	if(i==j) return INT_MAX;
	unsigned int tcost=0;
	unsigned int tcost2=0;
	bool gotprevi=false;
	bool gotprevj=false;
	unsigned int lastcircdisti=0;
	unsigned int lastcircdistj=0;
	list< vector<fixture> >::reverse_iterator rit;
	unsigned int iternum=1;
	for( rit=prevgames.rbegin() ; rit != prevgames.rend(); rit++ ) {
		unsigned int cost=0;
		unsigned int dist=(cfp.circular)?min(iternum, cfp.circle_length-iternum):iternum;
		vector<fixture>::iterator fx;
		for(fx=rit->begin() ; fx != rit->end(); fx++ ) {
			if(fx->team1==i && fx->team2==j) tcost2+=100000;
			if(fx->team1==i || fx->team2==i) {
				cost+=1;
				if(!gotprevi) tcost2+=max(5000.0-50.0*pow((dist-cfp.targetgap),3),0.0);
				else lastcircdisti=dist;
				gotprevi=true;
			}
			if(fx->team1==j || fx->team2==j) {
				cost+=1;
				if(!gotprevj) tcost2+=max(5000.0-50.0*pow((dist-cfp.targetgap),3),0.0);
				else lastcircdistj=dist;
				gotprevj=true;
			}
		}
		if(cost) tcost+=(((double) cost)*(100.0+(250.0*exp(-sqrt(dist)))));
		if((debug>=DEBUG_TCOSTCOND && cost) || debug>=DEBUG_TCOSTALL ) printf("%d v %d, tcost: %d, tcost2: %d, iternum: %d, dist: %d, cost: %d\n", i+1, j+1, tcost, tcost2, iternum, dist, cost);
		iternum++;
	}
	if(cfp.circular && iternum+cfp.targetgap>=cfp.circle_length) {
		if(lastcircdisti) tcost2+=max(5000.0-50.0*pow((lastcircdisti-cfp.targetgap),3),0.0);
		if(lastcircdistj) tcost2+=max(5000.0-50.0*pow((lastcircdistj-cfp.targetgap),3),0.0);
	}
	return tcost+tcost2;
}

unsigned int gensimmatches(int simt, unsigned int firstcostcandidate, vector<costst> &costs, vector<fixture> &currentgames, unsigned int &missedcost) {
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
	unsigned int lpfc=runningcost;	//lowest possible future cost
	for(unsigned int i=0; i<gamesleft; i++) {
		if(costs[costtotry+i].cost==UINT_MAX) {
			lpfc=UINT_MAX;
			break;
		}
		lpfc+=costs[costtotry+i].cost;
	}
	if(lpfc < bestcost) {
		if(!haveteams[costs[costtotry].team1] && !haveteams[costs[costtotry].team2]) {
			vector<fixture> newfixture = currentfixture;
			vector<bool> newhaveteams = haveteams;
			fixture f1={costs[costtotry].team1, costs[costtotry].team2};
			newfixture.push_back(f1);
			newhaveteams[costs[costtotry].team1]=true;
			newhaveteams[costs[costtotry].team2]=true;
			if(gamesleft>1 && costs[costtotry].cost!=UINT_MAX) {
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

void checkdumpnesting(string &s, unsigned int team, list< vector<fixture> > &prevgames, unsigned int n) {
	char buffer[100];
	list< vector<fixture> >::reverse_iterator rit;
	bool stop=false;
	vector<unsigned int> teamssincelast(n,0);
	for( rit=prevgames.rbegin() ; rit != prevgames.rend(); rit++ ) {
		vector<fixture>::iterator fx;
		for(fx=rit->begin() ; fx != rit->end(); fx++ ) {
			if(fx->team1==team || fx->team2==team) {
				stop=true;
				break;
			}
		}
		if(stop) break;
		for(fx=rit->begin() ; fx != rit->end(); fx++ ) {
			teamssincelast[fx->team1]++;
			teamssincelast[fx->team2]++;
		}
	}
	string cat;
	for(unsigned int i=0; i<n; i++) {
		if(teamssincelast[i]>=2) {
			snprintf(buffer, sizeof(buffer), " %d,", i+1);
			cat+=buffer;
		}
	}
	if(cat.size()) {
		snprintf(buffer, sizeof(buffer), "\t%d:", team+1);
		s+=buffer;
		s+=cat;
	}
}

void genfixtureset(unsigned int mint, unsigned int maxt, unsigned int simt) {	//this is order O(n^6) for each number of games
	mint=max(mint,simt*2);				//check that we've got sensible inputs
	if(maxt<mint) return;
	if(mint<2) return;
	if(simt<=0) return;

	for(unsigned int n=mint; n<=(unsigned int) maxt; n++) {
		unsigned int games=n*(n-1)/2;			//number of games is O(n^2)
		unsigned int maxgames=games;
		int matchrepeatfactor=1;

		while(maxgames%simt) {
			maxgames+=games;	//make sure that we end on a integral boundary of game rounds
			matchrepeatfactor++;
		}
		maxgames/=simt;

		costfuncparams cfp;
		cfp.targetgap=n/(simt*2);

		if(limit<maxgames) {
			maxgames=limit;
			cfp.circular=false;
		}
		else {
			cfp.circular=circular;
		}
		cfp.circle_length=maxgames;

		list< vector<fixture> > prevgames;
		vector<int> matchesleft(n*(n-1)/2,matchrepeatfactor);
		vector< vector<int> > posarray;
		if(posbalance) {
			vector<int> temp;
			temp.assign(simt,0);
			posarray.assign(n,temp);
		}

		printf("%d teams\n", n);
		if(outhandle) fprintf(outhandle, "%d teams\n", n);

		for(unsigned int c=0; c<maxgames; c++) {
			vector<costst> costs(n*(n-1)/2);

			unsigned int costnum=0;

			//i,j->costnum
			//sequence = 0,1 ... 0,n-1 1,2 ... 1,n-1
			//i->costnum offset
			//sum of i terms of n-1, n-2, etc
			//=(i/2) * (2(n-1) + (i-1)*(-1))
			//=0.5*i*(2n-1-i)
			//j->costnum offset
			//=j-(i+1)

			for(unsigned int i=0; i<n; i++) {
				for(unsigned int j=i+1; j<n; j++) {			//number of costs to calculate for each game is order O(n^2) per game
					costs[costnum].team1=i;
					costs[costnum].team2=j;
					if(matchesleft[costnum]>0) {
						costs[costnum].cost=getcost(i,j, prevgames, n, cfp);	//each cost is of same order to calculate as number of (previous) games, which is O(n^2)
						if(randomflag) costs[costnum].randval=rand();
						if(debug>=DEBUG_COSTDUMP) printf("Cost of fixture: %d v %d is %d\n", i+1, j+1, costs[costnum].cost);
					}
					else costs[costnum].cost=UINT_MAX;
					costnum++;
				}
			}

			vector<fixture> currentgames;

			sort(costs.begin(), costs.end(), costsortfunc);

			if((n>((2*simt)+2) && brute==0) || brute<0) {	//normal mode, pick first games that fit, greedy

				//now calculate actual games
				unsigned int missedcost;
				unsigned int tcost=gensimmatches(simt, 0, costs, currentgames, missedcost);
				if(debug>=DEBUG_MATCHTOTAL) printf("Sim Match Total: %d, missed: %d\n", tcost, missedcost);

			}
			else {			//not many games to choose from, do it more thoroughly/brute force it
				vector<bool> haveteams(n,0);
				unsigned int bestcost=UINT_MAX;
				vector<fixture> currentfixture;
				unsigned int gotcost=gensimmatchesbrute(currentfixture, costs, 0, simt, haveteams, 0, bestcost, currentgames);
				if(debug>=DEBUG_MATCHTOTAL) printf("Sim Match Total: %d, (brute forced)\n", gotcost);
			}

			sort(currentgames.begin(), currentgames.end(), fixturesortfunc);

			if(posbalance) {
				vector<fixture> permcurrentgames=currentgames;
				unsigned int mincost=UINT_MAX;
				do {
					unsigned int curcost=0;
					for(unsigned int i=0;i<permcurrentgames.size();i++) {
						curcost+=posarray[permcurrentgames[i].team1][i];
						curcost+=posarray[permcurrentgames[i].team2][i];
					}
					if(curcost<mincost) {
						currentgames=permcurrentgames;
						mincost=curcost;
					}
				} while(next_permutation(permcurrentgames.begin(), permcurrentgames.end(), fixturesortfunc));
			}


			string nestdump;
			vector<fixture>::iterator fx=currentgames.begin();
			unsigned int pos=0;
			do {
				printf("%2d v %2d", fx->team1+1, fx->team2+1);
				if(outhandle) fprintf(outhandle, "%2d v %2d", fx->team1+1, fx->team2+1);

				matchesleft[fx->team2-1-fx->team1+(fx->team1*((2*n)-1-fx->team1)/2)]--;
				if(posbalance) {
					posarray[fx->team1][pos]++;
					posarray[fx->team2][pos]++;
					pos++;
				}

				if(debug>=DEBUG_NEST) {
					checkdumpnesting(nestdump, fx->team1, prevgames, n);
					checkdumpnesting(nestdump, fx->team2, prevgames, n);
				}
				fx++;
				if(fx == currentgames.end()) break;
				printf("\t");				//don't add a tab to the last entry
				if(outhandle) fprintf(outhandle, "\t");

			} while(true);
			if(debug>=DEBUG_NEST && nestdump.size()) {
				printf("\tNesting alert: %s", nestdump.c_str());
			}

			prevgames.push_back(currentgames);

			if(debug>=DEBUG_PLAYCOUNT) {
				list< vector<fixture> >::reverse_iterator rit;
				vector<unsigned int> teamplayed(n,0);
				for( rit=prevgames.rbegin() ; rit != prevgames.rend(); rit++ ) {
					vector<fixture>::iterator fx;
					for(fx=rit->begin() ; fx != rit->end(); fx++ ) {
						teamplayed[fx->team1]++;
						teamplayed[fx->team2]++;
					}
				}
				unsigned int maxplayed=*max_element(teamplayed.begin(), teamplayed.end());
				unsigned int minplayed=*min_element(teamplayed.begin(), teamplayed.end());
				if(2+minplayed<=maxplayed) {
					printf("\tGames played alert:");
					for(unsigned int i=0; i<n; i++) {
						int delta=maxplayed-teamplayed[i];
						int delta2=teamplayed[i]-minplayed;
						if(delta>=2) {
							printf(" %d(%d,-%d),", i+1, teamplayed[i], delta);
						}
						else if(delta2>=2) {
							printf(" %d(%d,+%d),", i+1, teamplayed[i], delta2);
						}
						//else printf(" %d(%d,-%d,+%d),", i+1, teamplayed[i], delta, delta2);
					}
				}
			}

			printf("\n");
			if(outhandle) fprintf(outhandle, "\n");
		}

		printf("\n");
		if(outhandle) fprintf(outhandle, "\n");

		if(debug>=DEBUG_MATRIX) {		//print matrix, it's symmetric, and should have 0 on the diagonals
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
			printf("Match Matrix:\n    ");
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
		if(debug>=DEBUG_GAP) {		//print gap analysis, this ignores the start gap
			vector<unsigned int> gapmatrix(n*prevgames.size(),0);
			vector<unsigned int> lastplayed(n,0);
			vector<unsigned int> totalgap(n,0);
			vector<unsigned int> gapcount(n,0);
			unsigned int currentgame=1;
			unsigned int highestcount=0;
			unsigned int lowestcount=UINT_MAX;
			list< vector<fixture> >::iterator it;
			for( it=prevgames.begin() ; it != prevgames.end(); it++ ) {
				vector<fixture>::iterator fx;
				for(fx=it->begin() ; fx != it->end(); fx++ ) {
					if(lastplayed[fx->team1]) {
						unsigned int gap=currentgame-lastplayed[fx->team1];
						highestcount=max(highestcount,gap);
						lowestcount=min(lowestcount,gap);
						gapmatrix[(n*(gap-1))+fx->team1]++;
						totalgap[fx->team1]+=gap;
						gapcount[fx->team1]++;
					}
					if(lastplayed[fx->team2]) {
						unsigned int gap=currentgame-lastplayed[fx->team2];
						highestcount=max(highestcount,gap);
						lowestcount=min(lowestcount,gap);
						gapmatrix[(n*(gap-1))+fx->team2]++;
						totalgap[fx->team2]+=gap;
						gapcount[fx->team2]++;
					}
					lastplayed[fx->team1]=currentgame;
					lastplayed[fx->team2]=currentgame;
				}
				currentgame++;
			}
			if(highestcount) {
				printf("Gap Analysis (excluding start):\n    ");
				for(unsigned int i=lowestcount-1; i<highestcount; i++) printf("%2d ", i+1);
				printf("   T  C        A       SD\n");
				for(unsigned int i=0; i<n; i++) {
					printf("%2d: ", i+1);
					double avg=((double) totalgap[i])/((double) gapcount[i]);
					double sdaccum=0;
					for(unsigned int j=lowestcount-1; j<highestcount; j++) {
						printf("%2d ", gapmatrix[i+(n*j)]);
						sdaccum+=pow((((double) j+1)-avg),2)*gapmatrix[i+(n*j)];
					}
					printf("%4d ", totalgap[i]);
					printf("%2d ", gapcount[i]);
					printf("% -8.5g ", avg);
					printf("% -8.5g ", sqrt(sdaccum/gapcount[i]));
					printf("\n");
				}
				printf("\n");
			}
		}
		if(debug>=DEBUG_POSDIST && posbalance) {
			printf("Position analysis:\n");
			for(unsigned int i=0; i<n; i++) {
				printf("%2d:	",i+1);
				for(unsigned int j=0; j<simt; j++) {
					printf("%2d ", posarray[i][j]);
				}
				printf("\n");
			}
			printf("\n");
		}
	}
}

int main( int argc, char *argv[]) {
	cmdline(argv, argc);
	if(randomflag) srand (time(NULL));
	if(header) {
		printf("%s\nCommand line: ", version);
		if(outhandle) fprintf(outhandle, "%s\nCommand line: ", version);
		for(int i=0; i<argc; i++) {
			printf("%s ", argv[i]);
			fprintf(outhandle, "%s ", argv[i]);
		}
		char buff[256];
		time_t curtime=time(0);
		strftime(buff, 255, "%Y-%m-%d %H:%M:%SZ", gmtime(&curtime));
		printf("\nTime: %s\n\n\n", buff);
		if(outhandle) fprintf(outhandle, "\nTime: %s\n\n\n", buff);
	}

	genfixtureset(minteams, maxteams, simu);

	if(repthreshold) genfixtureset(minteams, repthreshold, simu-1);

	return 0;
}

