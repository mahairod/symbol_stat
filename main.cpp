#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <locale>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

using namespace std;

void fetchFiles(string dirName, set<string>& files){
	DIR* dir = opendir(dirName.c_str());
	if (!dir){
		cout << "Unable to list directory " << dirName << endl;
	}
	dirent* dirEnt = readdir(dir);

	while (0!=dirEnt) {
		if (dirEnt->d_name[0]!=0 && dirEnt->d_name[0]!='.'){
			string fName = dirName + "/" + dirEnt->d_name;
			if (DT_DIR == dirEnt->d_type){
				fetchFiles(fName, files);
			}else{
				files.insert(fName);
			}
		}
		dirEnt = readdir(dir);
	}
}

int main(int argc, char** argv){
	if (argc<2){
		cout << "Incorrect parameters\n";
		return 1;
	}
	// load file names
	set<string> files;
	{
		string dirname(argv[1]);
		fetchFiles(dirname, files);
	}

	//reading files
	locale old_locale;
	locale utf8_locale(old_locale, new boost::filesystem::detail::utf8_codecvt_facet);

	typedef map<wchar_t,long> StatMap;

	StatMap javaStat;
	StatMap xmlStat;
	StatMap xhtmlStat;
	StatMap otherStat;
	for (set<string>::iterator it = files.begin(); it != files.end(); ++it){
		string fname = *it;
		StatMap& stat =
			fname.rfind(".java") !=	string::npos ? javaStat :
			fname.rfind(".xml") !=	string::npos ? xmlStat :
			fname.rfind(".xhtml") !=string::npos ? xhtmlStat : otherStat;
		if (&stat==&otherStat){
			continue;
		}
		wifstream file(fname.c_str());
		file.imbue(utf8_locale);
		while(file.good()){
			wchar_t symbol;
			file >> symbol;
			stat[symbol]++;
		}
	}

	wcout.imbue(utf8_locale);
	for (StatMap::iterator it = javaStat.begin(); it != javaStat.end(); ++it){
		wcout << it->first << L"(" << (int)it->first << L")\t" << it->second << L'\n';
	}

}
