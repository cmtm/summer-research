#include <vector>

class Table {
public:
	Table(uint rowCount, vector<char>& columnSizes)
	  : rowCount(rowCount), columnSizes(columnSizes)
	{
		assert(columnSizes.size() > 1);
		for(uint i = 0; i < columnSizes.size(); i++)
			columnIndices[i] = std::accumulate(columnSizes.begin(), columnSizes.begin() + i, 0);
		rowSize = columnIndices.back() + columnSizes.back();
		cells = (char*) malloc(rowSize * rowCount);
	}

	~Table() {
		free(cells);
	}

	template <class returnType>
	returnType* getCellRef(uint row, uint col) {
		return (returnType*) &cells[row * rowSize + columnIndices[col]];
	}

	


private:
	const uint rowCount;
	const vector<char> columnSizes;
	vector<uint> columnIndices;
	uint rowSize;

	char* cells;
};