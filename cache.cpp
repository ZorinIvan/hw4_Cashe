#include "cache.H"


Cache::Cache(int _numOfCycles, int _block_size, int _cache_size, int _numOfWays) :
	num_of_cycles(_numOfCycles), block_size(_block_size), cache_size(_cache_size), num_of_ways(_numOfWays) 
{
	num_of_blocks = cache_size / block_size;
	way_size = num_of_blocks / num_of_ways;

	for (int i = 0; i < num_of_ways; i++) {
		way_arr[i] = new Mem_block[way_size];
	}

	LRU = new int[num_of_ways];
	for (int i = 0; i < num_of_ways; i++) {//init LRU
		LRU[i] = i;
	}

}

Cache::~Cache() {
	delete LRU;
	for (int i = 0; i < num_of_ways; i++) {
		delete way_arr[i];
	}
	delete way_arr;
}


void Cache::LRU_upd(int way) {
	int x = LRU[way];
	LRU[way] = num_of_ways - 1;
	for (int j = 0; j < num_of_ways - 1; j++) {
		if (j != way && LRU[j] > x) LRU[j]--;
	}
}

Result Cache::read(int tag, int set) {
	for (int i = 0; i < num_of_ways; i++) {
		if (!way_arr[i][set].isEmpty && way_arr[i][set].tagcmp(tag) && way_arr[i][set].isValid) { //find matching tag in way_arr 
			
			this->LRU_upd(i);
			return SUCCESS;
		}
	}
	return FAILURE; //matching tag is not found
}

Result Cache::write(int tag, int set) {
	for (int i = 0; i < num_of_ways; i++) {
		if (way_arr[i][set].isEmpty) {

			way_arr[i][set].change_tag(tag);
			way_arr[i][set].chanage_empty(false);
			this->LRU_upd(i);
			return SUCCESS;
		}

		if (way_arr[i][set].tagcmp(tag) && way_arr[i][set].isValid) { //find matching tag in way_arr 
			
			way_arr[i][set].change_dirty(true);
			this->LRU_upd(i);
			return SUCCESS;
		}
	}
	return FAILURE; //matching tag is not found
}







/*************Helper functions*************/
void DecToBinary(int n, int* arr) {

	for (int i = 0; i < CMD_SIZE; i++)
	{
		arr[i] = n % 2;
		n /= 2;
	}
	return;
}

int BinaryToDec(int* parray, int size) {
	int tmp = 0;
	for (int i = 0; i < size; i++) {
		tmp += parray[i] * pow(2, i);
	}
	return tmp;
}