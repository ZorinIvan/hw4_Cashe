#include "cache.H"

Mem_block::Mem_block() :
	tag(-1) {
	dirty = false;
	valid = I;
	empty = false;
	adress = -1;
}

//===================CACHE FUNCTIONS===================
Cache::Cache(int _numOfCycles, int _block_size, int _cache_size, int _numOfWays) :
	num_of_cycles(_numOfCycles), block_size(_block_size), cache_size(_cache_size), num_of_ways(_numOfWays)
{
	num_of_blocks = cache_size / block_size;
	way_size = num_of_blocks / num_of_ways;

	way_arr = new Mem_block*[num_of_ways];
	for (int i = 0; i < num_of_ways; i++) {
		way_arr[i] = new Mem_block[way_size];
	}

	LRU = new int*[way_size];
	for (int j = 0; j < way_size; j++) {
		LRU[j] = new int[num_of_ways];
		for (int i = 0; i < num_of_ways; i++) {//init LRU
			LRU[j][i] = i;
		}
	}

}

/*Cache::Cache(const Cache& rhs)
: num_of_blocks(rhs.num_of_blocks), block_size(rhs.block_size), cache_size(rhs.cache_size),
num_of_ways(rhs.num_of_ways), num_of_cycles(rhs.num_of_cycles), way_size(rhs.way_size),
LRU(cpyLRU(rhs.LRU)), way_arr(cpyWayArr(rhs.way_arr)) {}*/

Cache::~Cache() {
	delete LRU;
	for (int i = 0; i < num_of_ways; i++) {
		delete way_arr[i];
	}
	delete way_arr;
}

void Cache::LRU_upd(int way, int set) {
	int x = LRU[set][way];
	LRU[set][way] = num_of_ways - 1;
	for (int j = 0; j < num_of_ways ; j++) {///
		if (j != way && LRU[set][j] > x) LRU[set][j]--;
	}
}

int Cache::tag_found(int tag, int set) {
	for (int i = 0; i < num_of_ways; i++) {
		if (way_arr[i][set].tagcmp(tag)) {
			return i;
		}

	}
	return -1;
}

int Cache::get_lru_way(int set) {
	for (int i = 0; i < num_of_ways; i++) {
		if (LRU[set][i] == 0)
			return i;
	}
	return 0;
}

Result Cache::read(int tag, int set) {
	for (int i = 0; i < num_of_ways; i++) {
		if (way_arr[i][set].tagcmp(tag) && way_arr[i][set].isValid() == V) {
			LRU_upd(i, set);
			return HIT;
		}

	}
	return MISS;
}

void Cache::write(int tag, int set) {
	for (int i = 0; i < num_of_ways; i++) {
		if (way_arr[i][set].tagcmp(tag)) {
			way_arr[i][set].change_dirty(true);
			return;
		}
	}
	return;
}




//===================L1 FUNCTIONS===================
L1Cache::L1Cache(int _numOfCycles, int _block_size, int _cache_size, int _numOfWays)
	: Cache(_numOfCycles, _block_size, _cache_size, _numOfWays) {}

void L1Cache::snoop(L2Cache* l2, int l1_set, int l1_tag) {
	int way_to_delete = tag_found(l1_tag, l1_set);

	if (way_to_delete == -1) return;

	if (way_arr[way_to_delete][l1_set].isDirty()) {//dirty need to move to l2


												   /*Calculate l2_set and l2_tag according to adress in l1*/

		Mem_block** l2_way = l2->get_way_arr();
		int adr = way_arr[way_to_delete][l1_set].getAdress();
		int* bin_adress = new int[CMD_SIZE];
		DecToBinary(adr, bin_adress);
		int offset_bits = log2(l2->get_block_size() / 4);
		int l2_set_bits = log2(l2->get_cache_size() / (l2->get_block_size()*l2->get_num_of_ways()));

		int del_l2_set = BinaryToDec(bin_adress + 2 + offset_bits, l2_set_bits);
		int del_l2_tag = BinaryToDec(bin_adress + 2 + offset_bits + l2_set_bits, CMD_SIZE - l2_set_bits - offset_bits - 2);


		for (int i = 0; i < l2->get_num_of_ways(); i++) {
			if (l2_way[i][del_l2_set].tagcmp(del_l2_tag)) {
				l2_way[i][del_l2_set].change_dirty(true);
				l2->LRU_upd(i, del_l2_set);
				break;
			}
		}

		way_arr[way_to_delete][l1_set].change_dirty(false);
		delete bin_adress;
	}

	//LRU_upd(way_to_delete, l1_set);
	way_arr[way_to_delete][l1_set].change_valid(I);
	return;
}






//===================L2 FUNCTIONS===================
L2Cache::L2Cache(int _numOfCycles, int _block_size, int _cache_size, int _numOfWays)
	: Cache(_numOfCycles, _block_size, _cache_size, _numOfWays) {}


void L2Cache::l2_to_l1(L1Cache& l1, int l2_tag, int l2_set, int l1_tag, int l1_set, int _adress) {
	Mem_block** l1way_arr = l1.get_way_arr();
	int way_LRU = l1.get_lru_way(l1_set);
	int l1_num_of_ways = l1.get_num_of_ways();

	for (int i = 0; i < l1_num_of_ways; i++) {
		if (l1way_arr[i][l1_set].tagcmp(-1) || l1way_arr[i][l1_set].isValid() == I) { //Empty new place in L1 or something is invalid
			l1way_arr[i][l1_set].change_tag(l1_tag);
			l1way_arr[i][l1_set].change_valid(V);
			l1way_arr[i][l1_set].change_adress(_adress);
			l1way_arr[i][l1_set].change_dirty(false);
			l1.LRU_upd(i, l1_set);
			return;
		}
	}



	if (l1way_arr[way_LRU][l1_set].isDirty()) {//l1 dirty WB.


											   /*Calculate l2_set and l2_tag according to adress in l1*/
		int adr = l1way_arr[way_LRU][l1_set].getAdress();
		int* bin_adress = new int[CMD_SIZE];
		DecToBinary(adr, bin_adress);
		int offset_bits = log2(block_size / 4);
		int l2_set_bits = log2(cache_size / (block_size*num_of_ways));

		int del_l2_set = BinaryToDec(bin_adress + 2 + offset_bits, l2_set_bits);
		int del_l2_tag = BinaryToDec(bin_adress + 2 + offset_bits + l2_set_bits, CMD_SIZE - l2_set_bits - offset_bits - 2);


		for (int i = 0; i < num_of_ways; i++) {
			if (way_arr[i][del_l2_set].tagcmp(del_l2_tag)) {
				way_arr[i][del_l2_set].change_dirty(true);
				LRU_upd(i, del_l2_set);
			}
		}
		l1way_arr[way_LRU][l1_set].change_dirty(false);
		delete bin_adress;
	}

	l1way_arr[way_LRU][l1_set].change_tag(l1_tag);
	l1way_arr[way_LRU][l1_set].change_valid(V);
	l1way_arr[way_LRU][l1_set].change_adress(_adress);
	l1.LRU_upd(way_LRU, l1_set);

	return;
}


void L2Cache::mem_to_l2(L1Cache& l1, int l2_tag, int l2_set, int l1_tag, int l1_set, int adr) {
	int l2_way_to_del = get_lru_way(l2_set);
	Mem_block** l2_mem_arr = get_way_arr();
	//int l2_tag_to_del = l2_mem_arr[l2_way_to_del][l2_set].getTag();

	for (int i = 0; i < num_of_ways; i++) {
		if (way_arr[i][l2_set].isValid() == I || way_arr[i][l1_set].tagcmp(-1)) { //there is a free space
			way_arr[i][l2_set].change_tag(l2_tag);
			way_arr[i][l2_set].change_valid(V);
			way_arr[i][l2_set].change_adress(adr);
			way_arr[i][l2_set].change_dirty(false);
			LRU_upd(i, l2_set);
			l2_to_l1(l1, l2_tag, l2_set, l1_tag, l1_set, adr);
			return;
		}

	}

	//no free space in l2
	int* bin_adress = new int[CMD_SIZE];
	DecToBinary(l2_mem_arr[l2_way_to_del][l2_set].getAdress(), bin_adress);
	int offset_bits = log2(l1.get_block_size() / 4);
	int l1_set_bits = log2(l1.get_cache_size() / (l1.get_block_size()*l1.get_num_of_ways()));

	int del_l1_set = BinaryToDec(bin_adress + 2 + offset_bits, l1_set_bits);
	int del_l1_tag = BinaryToDec(bin_adress + 2 + offset_bits + l1_set_bits, CMD_SIZE - l1_set_bits - offset_bits - 2);

	l1.snoop(this, del_l1_set, del_l1_tag);

	if (way_arr[l2_way_to_del][l2_set].isDirty()) {//write to mem
												   //write l2 to mem
		way_arr[l2_way_to_del][l2_set].change_dirty(false);
	}

	LRU_upd(l2_way_to_del, l2_set);
	l2_to_l1(l1, l2_tag, l2_set, l1_tag, l1_set, adr);
	way_arr[l2_way_to_del][l2_set].change_tag(l2_tag);
	way_arr[l2_way_to_del][l2_set].change_adress(adr);
	way_arr[l2_way_to_del][l2_set].change_valid(V);


	delete bin_adress;

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