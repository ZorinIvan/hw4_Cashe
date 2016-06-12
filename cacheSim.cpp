#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<math.h>

#include "cache.H"

using std::string;
using namespace std;

int main(int argc, char const *argv[]){

	/********** PARAMETERS TO CALCULATE OUTPUT *********/
	int total_commands = 0, l2_commands = 0;
	int l1_miss = 0, l2_miss = 0, time = 0;
	int* bin_adress = new int[CMD_SIZE]; //binary access adress 
	int adress; //dec access adress 
	int l1_tag, l2_tag, l1_set, l2_set; 

	if (argc !=18 ) //if not enough arguments
	{
		cerr << "Usage: " << argv[0] << "cacheSim <input file> --mem - cyc <# of cycles> --bsize <block log2(size)>";
		cerr << "--l1 - size <log2(size)> --l1 - assoc <log2(# of ways)> --l1 - cyc <# of cycles> --l2 - size <log2(size)>" <<
				"--l2 - assoc <log2(# of ways)> --l2 - cyc <# of cycles>\n" << endl;
		exit(-1);
	}

	/********** INIT INPUT PARAMETERS **********/

	int mem_cycle = strtol(argv[3],NULL,0);
	int block_size = strtol(argv[5], NULL, 0);
	int l1_size = pow(2, strtol(argv[7], NULL, 0));
	int l1_assoc = pow(2,strtol(argv[9], NULL, 0));
	int l1_cycle = strtol(argv[11], NULL, 0);
	int l2_size = pow(2, strtol(argv[13], NULL, 0));
	int l2_assoc = pow(2, strtol(argv[15], NULL, 0));
	int l2_cycle = strtol(argv[17], NULL, 0);
	
	/*cout << " mem_cycle " << mem_cycle << endl;
	cout << " block_size " << block_size << endl;
	cout << " l1_size " << l1_size << endl;
	cout << " l1_assoc " << l1_assoc << endl;
	cout << " l1_cycle " << l1_cycle << endl;
	cout << " l2_size " << l2_size << endl;
	cout << " l2_assoc" << l2_assoc << endl;
	cout << " l2_cycle " << l2_cycle << endl;
	*/


	/********** INIT NEW CACHE **********/

	Cache l1(l1_cycle, block_size, l1_size, l1_assoc);
	Cache l2(l2_cycle, block_size, l2_size, l2_assoc);


	/********** READ FILE WITH COMMANDS **********/
	std::ifstream input(argv[1]);

	string delim = " "; //Delimeter

	for (string line; std::getline(input, line); )
	{
		string tokens[10];
		int cnt = 0;
		size_t pos = 0;

		/********** TOKENIZE COMMAND LINE **********/
		while ((pos = line.find(delim)) != string::npos) {

			tokens[cnt] = line.substr(0, pos);
			line.erase(0, pos + delim.length());
			cnt++;
		} //while

		tokens[cnt] = line.substr(0, pos);
		/*cout << tokens[0] << endl;
		cout << tokens[1] << endl;*/

		if (cnt == 0) continue; //empty line
		if (cnt > 0 && cnt != 1) {
			cerr << "Invalid number of arguments in command line: " << line << endl;
			continue;
		}

		total_commands++;

		adress = stoi(tokens[1], nullptr, 16); //convert adress from string to dec
		DecToBinary(adress, bin_adress); //convert adress from dec to bin
		
		//**********************************************TODO finish 
		l1_tag = BinaryToDec(bin_adress, int size)
		l2_tag = 
		l1_set = BinaryToDec(bin_adress)
		l2_set = 

		if (!tokens[0].compare("r")) { //read
			// TODO
		}

		if (!tokens[0].compare("w")) { //write
		  // TODO
		}

		
	
	}//for 

}