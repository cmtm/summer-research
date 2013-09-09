#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <chrono>
#include <iostream>

#include "aam_driver.h"

#define ROWS 12
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

	std::vector<std::pair<cell_t, iterator>> generateTrials(uint column, ulong howMany, ulong min, ulong max) {
		std::default_random_engine generator;
		std::uniform_int_distribution<ulong> dist(min, max);

		std::vector<std::pair<cell_t, iterator>> trials(howMany);

		for(int i=0; i < howMany; i++) {
			ulong position = dist(generator);
			trials[i] = std::pair<cell_t, iterator> (getCellVal(position, column), &getCellRef(position, column));
		}
		return trials;
	}


	void printTable() {
		std::cout << "\n\n" << "---------------------------------\n";
		for (int j = 0; j < rowCount; j++) {
			for (int i=0; i < columnCount; i++)
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
		cell_t message[ASSOC_LENGTH];
		for (ulong i=0; i < rowCount; i++) {
			for (ulong j=0; j< columnCount; j++)
				message[j]= getCellVal(i,j);
			storeAssociation(message);
		}


	}

	iterator query_AAM(uint column, cell_t value) {
		cell_t message[ASSOC_LENGTH];
		memset(message, 0xFF, ASSOC_LENGTH * WORD_SIZE);
		message[column] = value;
		// return queryAssociation(message);
		// this returns pointer to AAM data
		// lets return iterator to RAM table instead 
		return &getCellRef(queryAssociation(message)[ASSOC_LENGTH - 1], column);
	}



private:
	const ulong rowCount;
	const ulong columnCount;

	cell_t* cells;
};

Table<uint32_t> tableGen() {
	Table<uint32_t> tble(ROWS, 4);
	// first column contains enumeration
	// second will contain uniformly distributed
	// and ordered from 4 to 8100. no duplicates
	std::uniform_int_distribution<uint32_t> dist_u(4, ROWS * 4);
	tble.fillColumn(1, true, false, dist_u);

	//third will be a binomial distribution WITH duplicates. also ordered
	std::binomial_distribution<uint32_t> dist_b( ROWS/2, 0.5 );
	tble.fillColumn(2, true, true, dist_b);

	// the fourth and final will be a uniform distribution
	// There will be no duplicates, and it'll be unordered.
	// because the range is equal to the rowCount, the rows 
	// should contain the values from 0 to rowCount-1.
	std::uniform_int_distribution<uint32_t> dist_u2(0, ROWS-1);
	tble.fillColumn(3, false, false, dist_u2);

	return tble;

}

// template <typename func>
// int test_runs(Table<uint32_t>& tble, std::vector<std::pair<cell_t, Table<uint32_t>::iterator>> trials, ulong column) {
// 	for (auto it = trials.begin(); it < trials.end(); it++)
// 		if (func(column, (*it).first) != (*it).second)
// 			return -1;
// 	// success
// 	return 0; 
// }

typedef uint32_t cell_t;

int test_Ordered_RAM(Table<uint32_t>& tble, std::vector<std::pair<cell_t, Table<uint32_t>::iterator>> trials, ulong column) {
	for (auto it = trials.begin(); it < trials.end(); it++)
		if (tble.queryOrdered_RAM(column, (*it).first) != (*it).second)
			return -1;
	// success
	return 0; 
}

int test_Unordered_RAM(Table<uint32_t>& tble, std::vector<std::pair<cell_t, Table<uint32_t>::iterator>> trials, ulong column) {
	for (auto it = trials.begin(); it < trials.end(); it++)
		if (tble.queryUnordered_RAM(column, (*it).first) != (*it).second)
			return -1;
	// success
	return 0; 
}


int test_AAM(Table<uint32_t>& tble, std::vector<std::pair<cell_t, Table<uint32_t>::iterator>> trials, ulong column) {
	// IO data pointed to by returned iterator is structured using the cluster
	// count array followed by the neuron list.
	// if there are a total of 4 clusters and 3 of them are erased, the result
	// will look like this if there is one response per cluster 
	// [1, 1, 1, x, y, z]   where x y z are the associated neurons
	// | counts | results |
	
	for (auto it = trials.begin(); it < trials.end(); it++)
		if (tble.query_AAM(column, (*it).first) != (*it).second)
			return -1;
	// success
	return 0; 
}

// int test_RAM(Table<uint32_t>& tble) {
// 
// 	long position;
// 
// 	// try a binary search
// 	std::cout << "\n\n";
// 	position = tble.queryOrdered_RAM(2, 5) - tble.begin(2);
// 	std::cout << "The position of the first value of 5 in col 2 is: " << position;
// 
// 	// try a linear search
// 	std::cout << "\n\n";
// 	position = tble.queryUnordered_RAM(3, 7) - tble.begin(3);
// 	std::cout << "The position of the value 7 in col 3 is: " << position;
// 
// 	return 0;
// }
// 
// int test_AAM(Table<uint32_t>& tble) {
// 
// 	long position;
// 
// 	// try a binary search
// 	std::cout << "\n\n";
// 	position = tble.query_AAM(2, 5) - tble.begin(2);
// 	std::cout << "The position of the first value of 5 in col 2 is: " << position;
// 
// 	// try a linear search
// 	std::cout << "\n\n";
// 	position = tble.query_AAM(3, 7) - tble.begin(3);
// 	std::cout << "The position of the value 7 in col 3 is: " << position;
// 
// 	return 0;
// }
// 
// //return 0 for success
// int test_table() {
// 	Table<uint32_t> tble(32, 4);
// 	// first column contains enumeration
// 	// second will contain uniformly distributed
// 	// and ordered from 4 to 81. no duplicates
// 	std::uniform_int_distribution<uint32_t> dist_u(4, 81);
// 	tble.fillColumn(1, true, false, dist_u);
// 
// 	//third will be a binomial distribution WITH duplicates. also ordered
// 	std::binomial_distribution<uint32_t> dist_b(10, 0.5);
// 	tble.fillColumn(2, true, true, dist_b);
// 
// 	// the fourth and final will be a uniform distribution
// 	// There will be no duplicates, and it'll be unordered.
// 	// because the range is equal to the rowCount, the rows 
// 	// should contain the values from 0 to 31.
// 	std::uniform_int_distribution<uint32_t> dist_u2(0, 31);
// 	tble.fillColumn(3, false, false, dist_u2);
// 
// 	//print out the table!
// 	std::cout << "\n\n" << "---------------------------------\n";
// 	for (int j = 0; j < 32; j++) {
// 		for (int i=0; i < 4; i++)
// 			std::cout << tble.getCellVal(j, i) << "\t|";
// 		std::cout << "\n";
// 	}
// 
// 	long position;
// 
// 	// try a binary search
// 	std::cout << "\n\n";
// 	position = tble.queryOrdered_RAM(2, 5) - tble.begin(2);
// 	std::cout << "The position of the first value of 5 in col 2 is: " << position;
// 
// 	// try a linear search
// 	std::cout << "\n\n";
// 	position = tble.queryUnordered_RAM(3, 7) - tble.begin(3);
// 	std::cout << "The position of the value 7 in col 3 is: " << position;
// 
// 	std::cout << "\n\n";
// 
// 	// dummy return. see printed table
// 	return 0;
// }

int main(void) {
	// take initial program time
	auto start_time = std::chrono::steady_clock::now();
	Table<uint32_t> tble = tableGen();
	// initialize AAM driver
	init_driver();

	tble.store_in_AAM();

	// generate trials
	// 0th column contains enumeration
	// 1st        contains uniformly ordered
	// 2nd        contains binomial (with dups) and ordered
	// 3rd        contains uniform unordered
	auto trials = tble.generateTrials(1, 100, 0, ROWS-1);
	
	auto init_time = std::chrono::steady_clock::now();

	std::cout << "return of test_Unordered: :" << test_Unordered_RAM(tble, trials, 1) << std::endl;
	auto unordered_time = std::chrono::steady_clock::now();

	std::cout << "return of test_ordered: :" << test_Ordered_RAM(tble, trials, 1) << std::endl;
	auto ordered_time = std::chrono::steady_clock::now();

	std::cout << "return of test_AAM :" << test_AAM(tble, trials, 1) << std::endl;
	auto aam_time = std::chrono::steady_clock::now();

	std::cout << "time taken to run program: \n";
	std::cout << "initialization:  " << std::chrono::duration_cast<std::chrono::microseconds>(init_time - start_time).count() << std::endl;
	std::cout << "unordered time:  " << std::chrono::duration_cast<std::chrono::microseconds>(unordered_time - init_time).count()<< std::endl;
	std::cout << "ordered time:    " << std::chrono::duration_cast<std::chrono::microseconds>(ordered_time - unordered_time).count() << std::endl;
	std::cout << "aam time:        " << std::chrono::duration_cast<std::chrono::microseconds>(aam_time - ordered_time).count() << std::endl;
	std::cout << "\nclock stats\n";
	std::cout << "clock frequency: " << std::chrono::steady_clock::period::den
	          << "is the clock steady: " << std::chrono::steady_clock::is_steady
		  << std::endl;

	return 0;
}
