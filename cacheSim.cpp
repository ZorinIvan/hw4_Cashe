#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<math.h>

#include "cache.h"

using std::string;
using namespace std;

int main(int argc, char const *argv[]){

	/********** PARAMETERS TO CALCULATE OUTPUT *********/
	int total_commands = 0, l1_commands=0, l2_commands = 0;
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
	int block_size =pow(2, strtol(argv[5], NULL, 0));
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

	L1Cache l1(l1_cycle, block_size, l1_size, l1_assoc);
	L2Cache l2(l2_cycle, block_size, l2_size, l2_assoc);


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

		int offset_bits = log2(block_size / 4);
		int l1_set_bits = log2(l1_size / (block_size*l1_assoc));
		int l2_set_bits = log2(l2_size / (block_size*l2_assoc));
		int l1_offset = BinaryToDec(bin_adress + 2, offset_bits);
		int l2_offset = BinaryToDec(bin_adress + 2, offset_bits);
		l1_set = BinaryToDec(bin_adress + 2 + offset_bits, l1_set_bits);
		l2_set = BinaryToDec(bin_adress + 2 + offset_bits, l2_set_bits);
		l1_tag = BinaryToDec(bin_adress + 2 + offset_bits + l1_set_bits, CMD_SIZE - l1_set_bits - offset_bits - 2);
		l2_tag = BinaryToDec(bin_adress + 2 + offset_bits + l2_set_bits, CMD_SIZE - l2_set_bits - offset_bits - 2);


		/*cout << "l1_set " << l1_set << endl;
		cout << "l2_set " << l2_set << endl;
		cout << "l1_tag " << l2_set << endl;
		cout << "l1_tag " << l2_set << endl;*/
		if (total_commands == 12) {
			int i = 0;
		}

		if (!tokens[0].compare("r")) { //read
			l1_commands++;
			if (l1.read(l1_tag, l1_set) == HIT) {
				time += l1_cycle;
			//	continue;
			}
			else //miss l1
			{
				l1_miss++;
				l2_commands++;


				if (l2.read(l2_tag, l2_set) == HIT) {//miss l1 hit l2
					l2.l2_to_l1(l1, l2_tag, l2_set, l1_tag, l1_set, adress);
					time += l2_cycle;
				}
				else//miss l1 miss l2
				{
					l2_miss++;
					time += mem_cycle;
					l2.mem_to_l2(l1, l2_tag, l2_set, l1_tag, l1_set, adress);
				}
			}

		}

		if (!tokens[0].compare("w")) { //write
			l1_commands++;
			if (l1.read(l1_tag, l1_set) == HIT) {
				l1.write(l1_tag, l1_set);
				time += l1_cycle;
				//continue;
			}
			else //miss l1
			{
				l1_miss++;
				l2_commands++;


				if (l2.read(l2_tag, l2_set) == HIT) {//miss l1 hit l2
					l2.l2_to_l1(l1, l2_tag, l2_set, l1_tag, l1_set, adress);
					l1.write(l1_tag, l1_set);
					time += l2_cycle;
					//continue;
				}
				else//miss l1 miss l2
				{
					l2_miss++;
					time += mem_cycle;
					l2.mem_to_l2(l1, l2_tag, l2_set, l1_tag, l1_set, adress);
					l1.write(l1_tag, l1_set);
					//continue;
				}
			}

		}
		double l1_miss_tot = (round(1000 * (double)l1_miss / l1_commands)) / 1000;
		double l2_miss_tot = (round(1000 * (double)l2_miss / l2_commands)) / 1000;
		double time_avg = (round(1000 * (double)time / total_commands)) / 1000;
		printf("L1miss=%.3f L2miss=%.3f AccTimeAvg=%.3f\n", l1_miss_tot, l2_miss_tot, time_avg);
		cout << total_commands << endl;

	}//for 

	 // ======CALCULATE THE OUTPUT =======

}