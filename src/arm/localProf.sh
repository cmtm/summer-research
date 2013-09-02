g++ -std=c++11 -pg -o profable.out db_Benchmark.cpp aam_driver.c
profable.out
gprof profable.out > analysis.txt
python << END
unordered = None
ordered = None
aam = None

tags = 0
for line in reversed(open('analysis.txt', 'r').readlines()):
	if 'queryUnordered_RAM' in line:
		unordered = line.split()[0]
	elif 'queryOrdered_RAM' in line:
		ordered = line.split()[0]

	if ordered and unordered:
		break

for line in open('analysis.txt', 'r').readlines():
	if ordered == line[0:len(ordered)] or unordered == line[0:len(unordered)]:
		print(line)
END
