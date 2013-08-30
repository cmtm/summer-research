#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <cstdint>
#include <cassert>

#include <iostream>

#include "aam_driver.h"

template <class cell_t>
class Table {
public:

	Table(uint rowCount, uint columnCount)
	  : rowCount(rowCount), columnCount(columnCount)
	{
		assert(columnCount > 1);
		assert(rowCount > 0);
		cells = (cell_t*) malloc(columnCount * rowCount * (sizeof(cell_t)));
		enumerateColumn(0);
	}

	~Table() {
		free(cells);
	}
// ---- Iterator -----------------------------------------
	typedef cell_t* iterator;
	typedef const cell_t* const_iterator;
	iterator begin(ulong column) {return &getCellRef(0, column); }
	iterator end(ulong column) {return &getCellRef(rowCount, column); }
	const_iterator cbegin(ulong column) {return &getCellRef(0, column); }
	const_iterator cend(ulong column) {return &getCellRef(rowCount, column); }
// ------------------------------------------------------

	cell_t& getCellRef(uint row, uint col) {
		return cells[col * (rowCount) + row];
	}
	cell_t getCellVal(uint row, uint col) {
		return cells[col * (rowCount) + row];
	}

	void enumerateColumn(uint column) {
		for (long i = 0; i < rowCount; i++)
			getCellRef(i, column) = i;
	}

	// functions to fill columns with random data
	// a random generator is passed as a type
	template <class Distribution>
	void fillColumn(uint column, bool isOrdered, bool allowDuplicates, Distribution distGen) 
	{

		// make sure that the range of values comming from the distribution
		// is large enough so that each row can have a unique value in the
		// case where allowDuplicates is false
		if(!allowDuplicates)
			assert(distGen.max() - distGen.min() + 1 >= rowCount);

		std::default_random_engine generator;

		if(isOrdered) {
			// it is ordered, we'll use a multiset

			std::multiset<cell_t> randomCells;

			while(randomCells.size() < rowCount) {
				cell_t newCell = distGen(generator);
				if(allowDuplicates or randomCells.find(newCell) == randomCells.end())
					randomCells.insert(newCell);
			}

			long i = 0;
			for (auto c = randomCells.begin(); c != randomCells.end(); c++) {
				getCellRef(i, column) = (*c);
				i++;
			}


		} else {
			// it's unordered; we'll use a multimap instead of a multiset
			// multimap of value -> index
			// this work for both with and without duplicates
			std::multimap<cell_t, cell_t> randomCells;
			// although the cell values are unordered. It's important
			// to ensure that the order is random. This is why a map is
			// used: with it we can conserve the order in which elements
			// are generated
			ulong index = 0; 
			while(index < rowCount) {
				cell_t newCell = distGen(generator);
				if(allowDuplicates or randomCells.find(newCell) == randomCells.end()) {
					randomCells.insert(std::pair<cell_t, cell_t>(newCell, index));
					index++;
				}
			}

			// add values in map to actual column
			for(auto i = randomCells.begin(); i != randomCells.end(); i++)
				(getCellRef((*i).second, column)) = (*i).first;
		}
	}

	vector<pair<cell_t, ulong>> generateTrials(uint column, ulong howMany, ulong min, ulong, max) {
		std::default_random_engine generator;
		std::uniform_int_distribution<ulong> dist(min, max);

		vector<pair<cell_t, ulong>> trials(howMany);

		for(int i=0; i < howMany; i++) {
			ulong position = dist(generator);
			trials[i] = pair<cell_t, ulong> (getCellVal(position, column), position);
		}
		return trials;
	}


	void printTable() {
		std::cout << "\n\n" << "---------------------------------\n";
		for (int j = 0; j < 32; j++) {
			for (int i=0; i < 4; i++)
				std::cout << getCellVal(j, i) << "\t|";
			std::cout << "\n";
		}
	}


	iterator queryOrdered_RAM(uint column, cell_t value) {
		return std::lower_bound(begin(column), end(column), value);

	}

	iterator queryUnordered_RAM(uint column, cell_t value) {
		return std::find(begin(column), end(column), value);
	}

	void store_in_AAM() {
		for (ulong i=0; i < rowCount; i++)
			storeAssociation(&getCellRef(i, 0));
	}

	iterator query_AAM(uint column, cell_t value) {
		cell_t message[ASSOC_LENGTH];
		memset(message, 0xFF, ASSOC_LENGTH * WORD_SIZE);
		message[column] = value;
		return queryAssociation(message);
	}



private:
	const ulong rowCount;
	const ulong columnCount;

	cell_t* cells;
};

Table<uint32_t> tableGen() {
	Table<uint32_t> tble(32, 4);
	// first column contains enumeration
	// second will contain uniformly distributed
	// and ordered from 4 to 81. no duplicates
	std::uniform_int_distribution<uint32_t> dist_u(4, 81);
	tble.fillColumn(1, true, false, dist_u);

	//third will be a binomial distribution WITH duplicates. also ordered
	std::binomial_distribution<uint32_t> dist_b(10, 0.5);
	tble.fillColumn(2, true, true, dist_b);

	// the fourth and final will be a uniform distribution
	// There will be no duplicates, and it'll be unordered.
	// because the range is equal to the rowCount, the rows 
	// should contain the values from 0 to 31.
	std::uniform_int_distribution<uint32_t> dist_u2(0, 31);
	tble.fillColumn(3, false, false, dist_u2);

	return tble;

}

int test_RAM(Table<uint32_t>& tble) {

	long position;

	// try a binary search
	std::cout << "\n\n";
	position = tble.queryOrdered_RAM(2, 5) - tble.begin(2);
	std::cout << "The position of the first value of 5 in col 2 is: " << position;

	// try a linear search
	std::cout << "\n\n";
	position = tble.queryUnordered_RAM(3, 7) - tble.begin(3);
	std::cout << "The position of the value 7 in col 3 is: " << position;

	return 0;
}

int test_AAM(Table<uint32_t>& tble) {

	long position;

	// try a binary search
	std::cout << "\n\n";
	position = tble.query_AAM(2, 5) - tble.begin(2);
	std::cout << "The position of the first value of 5 in col 2 is: " << position;

	// try a linear search
	std::cout << "\n\n";
	position = tble.query_AAM(3, 7) - tble.begin(3);
	std::cout << "The position of the value 7 in col 3 is: " << position;

	return 0;
}

//return 0 for success
int test_table() {
	Table<uint32_t> tble(32, 4);
	// first column contains enumeration
	// second will contain uniformly distributed
	// and ordered from 4 to 81. no duplicates
	std::uniform_int_distribution<uint32_t> dist_u(4, 81);
	tble.fillColumn(1, true, false, dist_u);

	//third will be a binomial distribution WITH duplicates. also ordered
	std::binomial_distribution<uint32_t> dist_b(10, 0.5);
	tble.fillColumn(2, true, true, dist_b);

	// the fourth and final will be a uniform distribution
	// There will be no duplicates, and it'll be unordered.
	// because the range is equal to the rowCount, the rows 
	// should contain the values from 0 to 31.
	std::uniform_int_distribution<uint32_t> dist_u2(0, 31);
	tble.fillColumn(3, false, false, dist_u2);

	//print out the table!
	std::cout << "\n\n" << "---------------------------------\n";
	for (int j = 0; j < 32; j++) {
		for (int i=0; i < 4; i++)
			std::cout << tble.getCellVal(j, i) << "\t|";
		std::cout << "\n";
	}

	long position;

	// try a binary search
	std::cout << "\n\n";
	position = tble.queryOrdered_RAM(2, 5) - tble.begin(2);
	std::cout << "The position of the first value of 5 in col 2 is: " << position;

	// try a linear search
	std::cout << "\n\n";
	position = tble.queryUnordered_RAM(3, 7) - tble.begin(3);
	std::cout << "The position of the value 7 in col 3 is: " << position;

	std::cout << "\n\n";

	// dummy return. see printed table
	return 0;
}

int main(void) {
	// let's test this table out
	// test_table();
	// initialize AAM driver
	Table<uint32_t> tble = tableGen();
	init_driver();
	tble.store_in_AAM();

	tble.printTable();

	test_AAM(tble);
	test_RAM(tble);

	return 0;
}
