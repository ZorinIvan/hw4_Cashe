#ifndef CACHE_H
#define CACHE_H

#include<stdio.h>
#include<iostream>
#include<stdlib.h>

/**********TYPE DEFS AND HELPER FUNCTIONS**********/

typedef enum { FAILURE, SUCCESS } Result;

#define CMD_SIZE 32

void DecToBinary(int n, int* arr);
int BinaryToDec(int* parray, int size);






class Cache{

public:
	Cache(int _numOfCycles, int _block_size, int _cache_size, int _numOfWays);
	~Cache();
	Result read(int tag, int set);
	Result write(int tag, int set);

private: 
	void LRU_upd(int way);


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
	int* LRU; //when replacemen is needed evict LRU[x] = 0

};

#endif

class Mem_block
{
public:
	Mem_block(int _tag);
	Mem_block();
	void remove() { empty = true; }

	bool tagcmp(const int cmp) const  //compare tags
	{	if (tag == cmp) return true; 
		return false;
	}
	void change_tag(int _tag) { tag = _tag; }
	void change_dirty(bool newstatus) { dirty = newstatus; }
	void change_valid(bool newstatus) { valid = newstatus; }
	void chanage_empty(bool newstatus) { empty = newstatus; }
	bool isEmpty() const { return empty; }
	bool isValid() const { return valid; }
	bool isDirty() const { return dirty; }

protected:
	int tag;
	bool dirty;
	bool valid;
	bool empty;
};

Mem_block::Mem_block(int _tag) :
	tag(_tag) {
	dirty = false;
	valid = true;
	empty = false;
}

Mem_block::Mem_block() :
	empty(true) {}

