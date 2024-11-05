#include <stdio.h>
#define DATAFILE "d:\\research\\general\\combinatorial data\\graphs"

const int numGraphs[] = { 1,1,2,4,11,34,156,1044,12346 };  //number of graphs on n vertices for n=0,...,8.
const int V = 7;
int graph[2][12346][V+1][V+1];
int homomorph[V + 1];
int loopVec[V];

void loadGraph(int s, int n ) {  // load all graphs of order n to graphs[s]
	FILE* datafile = NULL;
	char dataFileName[200];
	char buffer[100];
	sprintf_s(dataFileName, "%s%d.txt", DATAFILE, n);
	fopen_s(&datafile, dataFileName, "r");
	for (int r = 0; r < numGraphs[n]; r++) {
		fgets(buffer, sizeof(buffer), datafile);
		fgets(buffer, sizeof(buffer), datafile);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++)
				graph[s][r][i][j] = fgetc(datafile)-'0';
			fgetc(datafile);
		}
	}
	fclose(datafile);
}

bool checkTwins(int s, int n, int r) {  // return true iff graph[s][r] on n vertices or its complement has a twin
	bool isTwinConn = false;
	bool isTwin = false;
	for (int i=0; i < n; i++)
		for (int j=i+1; j < n; j++) {
			bool found = true;
			for (int k = 0; k < n; k++)
				if (graph[s][r][i][k] != graph[s][r][j][k] && k != i && k != j) {
					found = false;
					break;
				}
			if (!found)
				continue;
			if (graph[s][r][i][j] == 1)
				isTwinConn = true;
			else
				isTwin = true;
		}
	return isTwin && isTwinConn;
}

void printGraph(int s, int n, int r) {  // print the adjacency matrix of graph[s][r] on n vertices
	printf("Printing graph #%d on %d vertices\n", r, n);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			printf("%d", graph[s][r][i][j]);
		printf("\n");
	}
}

bool checkLoopVecHom(int s1, int s2, int n1, int n2, int r1, int r2) {
	for (int i = 0; i < n2; i++)
		for (int j = i + 1; j < n2; j++) {
			if (homomorph[i] != homomorph[j] && graph[s2][r2][i][j] != graph[s1][r1][homomorph[i]][homomorph[j]])
				return false;
			if (homomorph[i] == homomorph[j] && (graph[s2][r2][i][j] != loopVec[homomorph[i]]))
				return false;
		}
	return true;
}

bool checkDeep(int s1, int s2, int n1, int n2, int r1, int r2) {
	// we want every loop assignment to have a corresponding homomorphism in order to return true
	for (int i = 0; i < n1; i++)
		loopVec[i] = 0;
	while (true) {
		int j = 0;
		while (loopVec[j] == 1 && j < n1) {
			loopVec[j] = 0;
			j++;
		}
		if (j == n1)
			return true;
		loopVec[j] = 1;
		for (int i = 0; i < n2; i++)
			homomorph[i] = 0;
		if (checkLoopVecHom(s1, s2, n1, n2, r1, r2)) // for the current loopVec, the trivial homomorphism is good, so switch to the next loopVec
			continue;
		while (true) {
			int j = 0;
			while (homomorph[j] == n1 - 1 && j < n2) {
				homomorph[j] = 0;
				j++;
			}
			if (j == n2)
				return false; // all homomorphisms for the present loopVec are invalid, so return false
			homomorph[j]++;
			if (checkLoopVecHom(s1, s2, n1, n2, r1, r2))
				break; // go to the next loopVec
		}
	}
}

bool checkHomomorph(int s1, int s2, int n1, int n2, int r1, int r2, bool star) {
	for (int i=0; i < n2; i++)
		for (int j = i + 1; j < n2; j++) {
			if (homomorph[i] != homomorph[j] && graph[s2][r2][i][j] != graph[s1][r1][homomorph[i]][homomorph[j]])
				return false;
			if (homomorph[i] == homomorph[j] && (graph[s2][r2][i][j] == (star ? 0 : 1)))
				return false;
		}
	return true;
}

bool checkHomomorph(int s1, int s2, int n1, int n2, int r1, int r2) {
	// check whether graph[s2][r2] on n2 vertices maps homomorphically to graph[s1][r1] on n1 vertices both for star and non-star
	bool foundStar = false;
	bool found = false;
	for (int i = 0; i < n2; i++)
		homomorph[i] = 0;
	found = found || checkHomomorph(s1, s2, n1, n2, r1, r2, false);
	foundStar = foundStar || checkHomomorph(s1, s2, n1, n2, r1, r2, true);
	while (true) {
		int j = 0;
		while (homomorph[j] == n1 - 1 && j < n2) {
			homomorph[j] = 0;
			j++;
		}
		if (j == n2)
			return found & foundStar;
		homomorph[j]++;
		found = found || checkHomomorph(s1, s2, n1, n2, r1, r2, false);
		foundStar = foundStar || checkHomomorph(s1, s2, n1, n2, r1, r2, true);
		if (found && foundStar)
			return true;
	}
}

int main()
{
	loadGraph(0, V);
	loadGraph(1, V+1);

	// The following piece of code checks if two graphs on V vertices are compatible
	for (int r1 = 0; r1 < numGraphs[V]; r1++) {
		if (!checkTwins(0, V, r1))
			continue;
		for (int r2 = r1 + 1; r2 < numGraphs[V]; r2++) {
			if (!checkTwins(0, V, r2))
				continue;
			if (checkHomomorph(0, 0, V, V, r1, r2) && checkHomomorph(0, 0, V, V, r2, r1)) {
				if (checkDeep(0, 0, V, V, r1, r2) && checkDeep(0, 0, V, V, r2, r1)) {
					printf("Graph %d on %d vertices and graph %d on %d vertices are compatible\n", r2, V, r1, V);
					printf("\n");
					printGraph(0, V, r1);
					printf("\n");
					printGraph(0, V, r2);
				}
			}
		}
	} 

	// The following piece of code checks if a graph on V vertices and a graph on V+1 vertices are compatible
	/* for (int r1 = 0; r1 < numGraphs[V]; r1++) {
		for (int r2 = 0; r2 < numGraphs[V+1]; r2++) {
			if (!checkTwins(1, V+1, r2))
				continue;
			if (checkHomomorph(0, 1, V, V+1, r1, r2)) {
				if (checkDeep(0, 1, V, V+1, r1, r2)) {
					printf("Graph %d on %d vertices and graph %d on %d vertices are compatible\n", r2, V+1, r1, V);
					printf("\n");
					printGraph(0, V, r1);
					printf("\n");
					printGraph(0, V+1, r2);
				}
			}
		}
	} */

	// The following piece of code checks if two graphs on V vertices are weakly compatible
	/* for (int r1 = 0; r1 < numGraphs[V]; r1++) {
		if (!checkTwins(0, V, r1))
			continue;
		for (int r2 = r1+1; r2 < numGraphs[V]; r2++) {
			if (!checkTwins(0, V , r2))
				continue;
			if (checkHomomorph(0, 0, V, V, r1, r2) && checkHomomorph(0, 0, V, V, r2, r1)) {
				printf("Graph %d on %d vertices and graph %d on %d vertices are weakly compatible\n", r2, V, r1, V);
				printf("\n");
				printGraph(0, V, r1);
				printf("\n");
				printGraph(0, V, r2);
			}
		}
	} */
}
