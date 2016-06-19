#ifndef CACHE_H
#define CACHE_H

#include<stdio.h>
#include<iostream>
#include<stdlib.h>
//#include <vector>


/**********TYPE DEFS AND HELPER FUNCTIONS**********/

typedef enum { HIT, MISS } Result;
typedef enum {V, I} Valid;
#define CMD_SIZE 32

void DecToBinary(int n, int* arr);
int BinaryToDec(int* parray, int size);

class Mem_block
{
public:
	//Mem_block(int _tag, int _adress);
	Mem_block();
	//void remove() { empty = true; }

	bool tagcmp(const int cmp) const  //compare tags
	{
		if (tag == cmp) return true;
		return false;
	}
	void change_tag(int _tag) { tag = _tag; }
	void change_dirty(bool newstatus) { dirty = newstatus; }
	void change_valid(Valid newstatus) { valid = newstatus; }
	void change_adress(int newadr) { adress = newadr; }
	//void chanage_empty(bool newstatus) { empty = newstatus; }
	//bool isEmpty() const { return empty; }
	Valid isValid() const { return valid; }
	bool isDirty() const { return dirty; }
	int getTag() const { return tag; }
	int getAdress() const { return adress; }

	//	Mem_block& operator=(const Mem_block& rhs);

protected:
	int tag;
	bool dirty;
	Valid valid;
	bool empty;
	int adress;
};

/*Mem_block::Mem_block(int _tag, int _adress) :
	tag(_tag) {
	dirty = false;
	valid = V;
	empty = false;
	adress = _adress;
}*/





class Cache{

public:
	Cache(int _numOfCycles, int _block_size, int _cache_size, int _numOfWays);
	//Cache(const Cache& rhs); //C-ctor
	~Cache();
	int tag_found(int tag, int set);//return way number by tag. if tag does't found returns -1
	Result read(int tag, int set);
	void LRU_upd(int way, int set);
	int get_lru_way(int set);
	Mem_block** get_way_arr() const { return way_arr; }
	int get_num_of_ways() const { return num_of_ways; }
	int get_block_size() const { return block_size; }
	int get_cache_size() const { return cache_size; }
	void write(int l1_tag, int l1_set);


protected:
	/*parameters from user*/
	const int num_of_cycles;
	const int block_size;
	const int cache_size;
	const int num_of_ways;
	/*********************/

	int num_of_blocks;
	int way_size;
	Mem_block** way_arr; //array of ponters to way arrays
	int** LRU; //when replacemen is needed evict LRU[x] = 0


	/*internal functions*/

	/*int** cpyLRU(const int** LRU) {
		int** LRU_cpy;
		if (LRU == NULL) return NULL;


		for (int j = 0; j < way_size; j++) {
			LRU_cpy[j] = new int[num_of_ways];
			for (int i = 0; i < num_of_ways; i++) {//init LRU
				LRU_cpy[j][i] = LRU[j][i];
			}
		}
		return LRU_cpy;
	}*/

	/*Mem_block** cpyWayArr(Mem_block** way_arr) {
		Mem_block** cpy;
		if (way_arr == NULL) return  NULL;
		for (int i = 0; i < num_of_ways; i++) {
			cpy[i] = new Mem_block[way_size];
		}
		for (int i = 0; i < num_of_ways; i++) {
			for (int j = 0; j < way_size; j++)
			{
				cpy[i][j] = way_arr[i][j];
			}
		}
		return cpy;
	}*/

};
class  L2Cache;

class  L1Cache : public Cache
{
public:
	L1Cache(int _numOfCycles, int _block_size, int _cache_size, int _numOfWays);
	void snoop(L2Cache *l2, int l1_set, int l1_tag);
private:

};

class  L2Cache : public Cache
{
public:
	L2Cache(int _numOfCycles, int _block_size, int _cache_size, int _numOfWays);
	void mem_to_l2(L1Cache& l1, int l2_tag, int l2_set, int l1_tag, int l1_set, int adress); //write from mem to l2 
	void l2_to_l1(L1Cache& l1, int l2_tag, int l2_set, int l1_tag, int l1_set, int adress); //write from l2 to l1
private:

};





/*Mem_block& Mem_block::operator=(const Mem_block& rhs) {
	if (this != &rhs) {
		tag = rhs.tag;
		dirty = rhs.dirty;
		valid = rhs.valid;
		empty = rhs.empty;
		adress = rhs.adress;
	}
	return *this;
}*/

#endif