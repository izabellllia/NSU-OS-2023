#define _FILE_OFFSET_BITS 64
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

string checker(string name, string pattern) {
	int nameId = 0;
	int patternId = 0;
	bool fits = false;
	while (patternId < pattern.size() && nameId < name.size()) {
		if (pattern[patternId] != '*' && pattern[patternId] != '?') { // not & or *
			if (pattern[patternId] != name[nameId]) {
				break;
			}
			else {
				nameId++;
				patternId++;
				continue;
			}
		}
		else if (pattern[patternId] == '?') { //?
			nameId++;
			patternId++;
			continue;
		}
		else { //'*'
			while (patternId < pattern.size() && pattern[patternId] == '*') {
				patternId++;
			}
			if (pattern.size() == patternId) {// pattern ended with '*'
				fits = true;
				break;
			}

			else if (patternId >= pattern.size()) {  //"*?"
				break;
			}
			int questionsCount = 0;
			while (patternId + questionsCount < pattern.size() && pattern[patternId + questionsCount] == '?') {
				questionsCount++;
			}
			if (pattern[patternId + questionsCount] == '*') {//*?*
				patternId += questionsCount;
				nameId += questionsCount;
				continue;
			}

			string NOLEFT = pattern.substr(patternId + questionsCount);
			string closestSubstring = NOLEFT.substr(0, NOLEFT.find_first_of("*?"));


			if (name.find(closestSubstring, nameId + questionsCount) == string::npos) {
				break;
			}
			else {
				nameId = name.find(closestSubstring, nameId + questionsCount);
				nameId += closestSubstring.size();
				patternId += questionsCount + closestSubstring.size();
			}
		}
	}

	if (name.size() == nameId) {
		while (pattern.size() > patternId) {
			if ('*' != pattern[patternId]){
				break;
			}
			patternId++;
		}
		if (pattern.size() == patternId) {
			fits = true;
		}
	}
	if (fits){
		return name;
	}
	else{
	      	return string();
	}
}

int main(int argc, char **argv) {

	string pattern;
	struct dirent *cur;
	DIR *dir;
	vector<string> fits;

	// open current directory
	if ((dir = opendir(".")) == NULL) {
	        perror("Couldn't open current directory");
		return 1;
	}
	cout<<"pattern is:"<<endl;
	cin>>pattern;
	if (!pattern.find('/')==pattern.size()){
		cout <<"Using / is restricted"<<endl;
		return 0;	
	}

	while ((cur = readdir(dir)) != NULL) {
		string curName(cur->d_name);
		string result = checker (curName,pattern);
		if (!result.empty()){
			fits.push_back(checker(curName,pattern));      
		}
	}
	if (!fits.size()) {
		cout<<pattern<<" did not matched any file in the current directory"<<endl;
	}
	else{
		for (int i = 0 ; i<fits.size(); i++){
			cout<< fits[i]<<endl;
		}
	
	}
	closedir(dir);
	return 0;
}
