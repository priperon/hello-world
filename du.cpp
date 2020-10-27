/*!	\file		du.cpp
	\author		Danielle Miike
	\date		2020-04-20
				C++17  application (Windows platform) that parses a given directory on your hard drive

*/
#include<locale>
#include<iostream>
#include<fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <Windows.h>
#include<filesystem>


using namespace std::filesystem;

class Fileusage {
public:

	std::string filename;
	double filespace = 0;
	char symbol = ' ';
};

std::vector<Fileusage> vectorH;

std::string myVersion = "1.0.0";
uintmax_t blockSizeValue = 4096;

//function to sort data by file_size
bool bySizeP(const std::pair<std::string, uintmax_t>& a, const std::pair<std::string, uintmax_t>& b) {
	return a.second < b.second;
}

bool bySizeF(Fileusage const& lhs, Fileusage const& rhs) {
	return lhs.filespace < rhs.filespace;
}

//function to sort data by filename
bool byNameP(const std::pair<std::string, uintmax_t>& a, const std::pair<std::string, uintmax_t>& b) {
	//verificar set 
	return a.first.size() < b.first.size();
}

bool byNameF(Fileusage const& lhs, Fileusage const& rhs) {
	return lhs.filename < rhs.filename;
}

/****************************************************************
function: recursive_scan()
Purpose: recursively scan the current directory and all subfolders
******************************************************************/

std::vector<std::pair<std::string, uintmax_t>>folders;

uintmax_t sizeToCluster(uintmax_t size) {
	return ceil((double)size / blockSizeValue);
}

uintmax_t recursive_scan(std::vector<std::string>& path) {
	uintmax_t folderSize = 0;
	uintmax_t currSize = 0;
	uintmax_t totalSize = 0;

	for(int i =0; i < path.size();i++){
		totalSize = 0;
		directory_iterator d(path[i]);	// first entry of folder 'f'
		directory_iterator e;

		while (d != e) {
			folderSize = 0;

			if (!is_directory(d->path())) {
				currSize = sizeToCluster(file_size(d->path()));
				totalSize += currSize;
			}

			//Iterate path and add all files/folders size. 
			else {
				for (auto f : recursive_directory_iterator(d->path().string(), directory_options::skip_permission_denied)) {
					folderSize += sizeToCluster(file_size(f.path().string()));

				}
				
				totalSize += folderSize;
				folders.push_back(make_pair(d->path().string(), folderSize));
			}
			++d;
		}
		folders.push_back(make_pair(path[i], totalSize));
	}
	return totalSize;
}
	

/*******************************************************************************
Function: help();
Purpose: display all help intructions
*********************************************************************************/
void help_switch() {
	std::cout	<<	"du(c) 2020, Danielle Miike\n"
			<<	"===========================================================\n"
			<<	"Version 1.0.0\n\n"
			<<	"A disk usage utility inspired by the UNIX du command.\n\n"
			<<	"Usage: du [-skhb] [--help] [--version] [--block-size=dddd] [folder]*\n\n"
				"Examples:\n"
				" du\n"
				"  > display the sum of the cluster sizes of each directory\n"
				" starting the cwd\n\n"
				" du folder\n"
				"  > display the sum of the cluster sizes of each directory\n"
				"   starting with 'folder'\n\n"
				" du -h\n"
				"  > display the results in a human readable form\n\n"
				" du -s\n"
				"  > display only the final summary\n\n"
				" du -b\n"
				"  > display in bytes\n\n"
				" du -k\n"
				"  > cluster size is 1024\n\n"
				" du -z\n"
				"  > display the list sorted by size\n\n"
				" du -n\n"
				"  > display the list sorted by name\n\n"
				" du -r\n"
				"  > display the list in reverse order\n\n"
				" du --block-size=dddd\n"
				"  > set the cluster size to the specified integer > 0\n\n"
				" du --help\n"
				"  > displays the help\n\n" 
				" du --version \n"
				"  > displays version number in the format d.d.d"
				<< std::endl;

	exit(EXIT_SUCCESS);
}

/*******************************************************************************
Function: getargv();
Purpose: storage all command line instructions into an array
*********************************************************************************/
void getargv(std::string arg, std::vector<std::string>& f) {
	size_t len = arg.length() - 1;
	
	for (size_t i = 0; i < len; i++)
	{
		char ch;
		std::string word;

		if (arg[i] == '-' && arg[i + 1] == '-') {
			f.push_back(arg.substr(2, arg.length()));
			break;
		}
		else if (arg[i] == '-' && arg[i + 1] != '-') {
			ch = arg[i + 1];
			word.push_back(ch);
			f.push_back(word);
		}
		else if (arg[i] != '-' && arg[i + 1] != '-') {
			if (i == 0) {
				f.push_back(arg);
				break;
			}
			else {
				ch = arg[i + 1];
				word.push_back(ch);
				f.push_back(word);
			}
		}
	}
}

/*******************************************************************************
Function: main();
Purpose: run the code
*********************************************************************************/
int main(int argc, char* argv[]) {


	if (argc == 1) {
		std::vector<std::string> current;
		current.push_back(".");
		

		try {
			recursive_scan(current);
		}
		catch (filesystem_error e) {
			std::cerr << "Error: " << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}

		for (auto x : folders) {
			std::cout << x.second << " \t " << x.first << std::endl;
		}
		
		exit(EXIT_SUCCESS);
	}
	

	//vector of command-line-args
	std::vector<std::string> argsV;
	
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		getargv(arg, argsV);	
	}

	//checking for repeated arguments
	for (size_t i = 0; i < argsV.size(); ++i) {
		for (size_t j = i + 1; j < argsV.size(); ++j) {
			if (argsV[i] == argsV[j]) {
				std::cerr << "\nError: Repeated arguments" << std::endl;
				exit(EXIT_SUCCESS);
			}
		}
	}


	//switches
	bool s = false, b = false, h = false, z = false, n = false, r = false, 
		 k = false, help = false, blockSize = false, folder = false, version = false;

	
	//vector<char>switchVec;
	path folderNameCheck;
	std::string clusterSize;
	std::vector<std::string> folderName;


	// set the swtich bool values to true if found it
	for (size_t i = 0; i < argsV.size(); ++i) {

		std::string str = argsV[i].substr(0, 11);

		if (str == "block-size=") {
			clusterSize = argsV[i].substr(11, argsV[i].length());
			blockSize = true;
		}

		else if (argsV[i] == "help") {
			help = true;
		}

		else if (argsV[i] == "version") {
			version = true;
		}

		else if (argsV[i] == "s")
			s = true;
		else if (argsV[i] == "b")
			b = true;
		else if (argsV[i] == "h")
			h = true;
		else if (argsV[i] == "z")
			z = true;
		else if (argsV[i] == "k")
			k = true;
		else if (argsV[i] == "r")
			r = true;
		else if (argsV[i] == "n")
			n = true;
		
		else if(argsV[i].length() > 1){
			folderNameCheck = argsV[i];
			if (!(exists(folderNameCheck))) {
				if (!(is_directory(folderNameCheck))) {
					std::cerr << "\nError: folder <" << argsV[i] << "> does not exist." << std::endl;
					exit(EXIT_SUCCESS);
				}
			}
			else {
				folderName.push_back(folderNameCheck.string());
			}
		}
		else {
			std::cerr << "\nError: Unknown switch <" << argsV[i] << "> entered." << std::endl;
			exit(EXIT_SUCCESS);
		}
	}

	//switch validation 

	if (b == true && h == true) {
		std::cerr << "\nError: Cannot use -b and -h switches together." << std::endl;
		exit(EXIT_SUCCESS);
	}

	if (k == true && blockSize == true) {
		std::cerr << "\nError: Cannot use -k and --block-size switches together." << std::endl;
		exit(EXIT_SUCCESS);
	}

	if (z == true && n == true) {
		std::cerr << "\nError: Cannot use -z and -n switches together."<< std::endl;
		exit(EXIT_SUCCESS);
	}


	//default cluster size
	unsigned int size = 4096;
	unsigned int convertPrint = 1024;


	//Display only the help listing
	if (help == true) {
		help_switch();
		exit(EXIT_SUCCESS);
	}

	//Display the version number only in the form d.d.d
	if (version == true) {
		std::cout << myVersion << std::endl;
		exit(EXIT_SUCCESS);
	}

	//setting the switches 
	if (k == true)
		blockSizeValue = 1024;

	if (blockSize == true) {
		blockSizeValue = stoi(clusterSize, nullptr, 0);
		if (!isdigit(blockSizeValue)) {
			std::cerr << "Error: bad block-size switch <" << blockSizeValue << ">" << std::endl;
			exit(EXIT_SUCCESS);
		}
		else if (blockSizeValue <= 0) {
			std::cerr << "Error: block-size value too small <" << blockSizeValue << ">" << std::endl;
			exit(EXIT_SUCCESS);
		}
	}

	//verify if the user enter a folder to scan or not
	std::string currentFolder = ".";
	std::string scanfolder;

	if (folderName.empty()) {
		folderName.push_back(currentFolder);
	}
	
	//scan the directory
	uintmax_t totalSize = recursive_scan(folderName);
	
	//Display in bytes, not clusters
	if (b == true) {
		for (size_t i = 0; i < folders.size(); ++i){
			folders[i].second = folders[i].second * blockSizeValue;
		}
	}

	double tempVal = 0;
	Fileusage fileInfo;

	//Make output in human readable format, converting to KiB, MiB, GiB, TiB
	if (h == true) {
		
		for (size_t i = 0; i < folders.size(); ++i) {
			if ((folders[i].second * blockSizeValue) > pow(convertPrint, 4)) {
				tempVal = (folders[i].second * blockSizeValue) / pow(convertPrint, 4);
				fileInfo.symbol = 'T';				
			}
			else if ((folders[i].second * blockSizeValue) > pow(convertPrint, 3)) {
				tempVal = (folders[i].second * blockSizeValue) / pow(convertPrint, 3);
				fileInfo.symbol = 'G';
			}
			else if ((folders[i].second * blockSizeValue) > pow(convertPrint, 2)) {
				tempVal = (folders[i].second * blockSizeValue) / pow(convertPrint, 2);
				fileInfo.symbol = 'M';
			}
			else if ((folders[i].second * blockSizeValue) > pow(convertPrint, 1)) {
				tempVal = (folders[i].second * blockSizeValue) / pow(convertPrint, 1);
				fileInfo.symbol = 'K';
			}
			else{
				tempVal = (folders[i].second * blockSizeValue) / convertPrint;
				fileInfo.symbol = 'B';
			}
			
			if (tempVal < 10) {
				if (fmod(tempVal, (int)tempVal) >= 0.01) {
					tempVal = trunc(tempVal) + 0.5;
				}
				else if (fmod(tempVal, (int)tempVal) >= 0.5) {
					tempVal = ceil(tempVal);
				}	
			}
			else {
					tempVal = round(tempVal);
				}
			
			fileInfo.filespace = tempVal;
			fileInfo.filename = folders[i].first;
			vectorH.push_back(fileInfo);
		}
	}
	
	//Display only the root folder, showing the sum of all clusters
	if (s == true) {
		uintmax_t sum = 0;
		//display summary
		std::cout << totalSize << "\t" << scanfolder << std::endl;
		exit(EXIT_SUCCESS);
	}

	//Sort by cluster or byte size
	if (z == true && h!= true) {
		sort(folders.begin(), folders.end(), bySizeP);
	}
	else if (z == true && h == true) {
		sort(vectorH.begin(), vectorH.end(), bySizeF);
	}

	//Sort by path name
	if (n == true && h != true) {
		sort(folders.begin(), folders.end(), byNameP);
	}
	else if(n == true && h == true) {
		sort(vectorH.begin(), vectorH.end(), byNameF);
	}

	//Reverse the output order, whether sorted or not
	if (r == true && h != true) {
		reverse(folders.begin(), folders.end());
	}
	else if (r == true && h == true) {
		reverse(vectorH.begin(), vectorH.end());
	}
	
	//print the array after all users requests
	if (h==true) {
		for (auto x : vectorH) {	
			std::cout << x.filespace << x.symbol <<" \t " << x.filename << std::endl;
		}
	}
	else {
		for (auto x : folders) {
			std::cout << x.second << " \t " << x.first << std::endl;
		}
	}
}//end main

