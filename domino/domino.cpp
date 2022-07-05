
/*
1.6. Домино (7)
Имеется N костей игры домино. На каждой кости имеется 2 числа (каждое от 0 до 6). Требуется написать программу, которая  будет определять максимальное число, составленное из цифр на костях цепочки, составленной по правилам домино из имеющихся костей.
Ввод. Первая строка входного файла содержит целое число  N – количество костей (2 ≤ N ≤ 20).
Следующие N строк содержат два целых числа X и Y (0 ≤ X, Y ≤ 6), разделенные пробелом.
Вывод. В выходной файл необходимо вывести максимальное целое число, цифры которого соответствуют значению костей в цепочке.
Примеры
Ввод 1         Ввод 2         Ввод 3
3              2              5
1 6            6 6            1 5
0 0            3 6            3 4
2 5                           4 1
                              1 6
                              1 0
Вывод 1        Вывод 2        Вывод 3
61             6663           611443

*/
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <optional>
#include <algorithm>
#include < functional>
#include <list>

using Domino = std::array<int, 2>;
using DominoList = std::vector<Domino>;
struct Chain
{
	int index;
	bool isReversed = false;
};
using ChainList = std::vector<Chain>;

struct FileNames {
    std::string inputFileName;
    std::string outputFileName;
};

std::optional<FileNames> ParseArguments(int argc, char* argv[]) {
    if (argc != 3) {
        return std::nullopt;
    }
    FileNames output = {argv[1],argv[2]};
    return output;
};

struct Int {
    int intVal;
};

std::optional<Int> ReadInt(std::ifstream& inputFile) 
{
    Int output;
    inputFile >> output.intVal;
    if (!inputFile) { return std::nullopt; }
    return output;
};

struct DL { DominoList dominoList; };

std::optional<DL> ReadDominoList(const std::string& inputFileName) 
{
    std::ifstream inputFile;
    inputFile.open(inputFileName);
    if(!inputFile.is_open())
    {
        return std::nullopt;
    }
    auto sizeOfDominoList = ReadInt(inputFile);
    if (!sizeOfDominoList) { return std::nullopt; };
    DominoList dominoList;
    for (int i = 0; i < sizeOfDominoList->intVal;i++) 
    {
        auto firstNum = ReadInt(inputFile);
        if (!firstNum) { return std::nullopt; }

        auto secondNum = ReadInt(inputFile);
        if (!secondNum) { return std::nullopt; }
        Domino domino = {firstNum->intVal,secondNum->intVal};
        dominoList.push_back(domino);
    }
    DL output = {dominoList};
    return output;
};

struct Fit
{
	bool fit = false;
	bool end = false;
};

Fit IsFit(Domino domino, DominoList DominoList)
{
	for (auto elem :domino)
	{
		if ((elem == DominoList[0][0]))
		{
			return { true ,false};
		}
		else if ((elem == DominoList[DominoList.size() - 1][1]))
		{
			return {true , true};
		}
		else
		{
			return { false, false };
		};
	}
}

Domino FlipDomino(const Domino& domino) { return {domino[1],domino[0]}; }

DominoList PushInDominoList(Fit fit,const DominoList& seq,const Domino& domino ) 
{
	if (fit.end)
	{
		DominoList output = seq;
		if (domino[0] != seq[seq.size() - 1][1])
		{
			output.push_back(FlipDomino(domino));
		}
		else
		{
			output.push_back(domino);
		}
		return output;
	}
	else
	{
		DominoList output = {};
		if (domino[1] != seq[0][0])
		{
			output.push_back(FlipDomino(domino));
		}
		else
		{
			output.push_back(domino);
		}
		for (auto elem : seq)
		{
			output.push_back(elem);
		};
		return output;
	}
}

int DominoListSum(const DominoList& dominoList){
	int output = 0;
	for (auto elem : dominoList)
	{
		output += (elem[0]+elem[1]);
	}
	return output;
};

struct Edge
{
	int end=0;
	int begin=0;
};


using IndexList = std::list<int>;
using BlackList = std::vector<bool>;


struct ChainBlock
{
	ChainList chainList;
	int size;
};

struct ChainFit
{
	bool fit = false;
	bool end = false;
};

ChainFit IsChainFit(const Edge& edge,const Domino& domino) 
{
	for (auto elem : domino)
	{
		if ((elem == edge.begin))
		{
			return { true, false };
		}
		else if ((elem == edge.end))
		{
			return { true, true };
		}
		else
		{
			return { false, false };
		};
	}
}

struct ChainBig
{
	Edge edge = {0,0};
	ChainList chainList = {};
};

ChainBig ConstructChain(ChainBig chainBig,ChainFit fit,Domino domino,int dominoIndex)
{
	ChainBig output;
	if (fit.end)
	{
		output.chainList = chainBig.chainList;
		if (chainBig.edge.end == domino[0])
		{
			output.chainList.push_back({ dominoIndex, false });
			output.edge = { domino[0], chainBig.edge.begin };
		}
		else
		{
			output.chainList.push_back({ dominoIndex, true });
			output.edge = { chainBig.edge.end, domino[1] };
		}
	}
	else
	{
		if (chainBig.edge.begin == domino[1])
		{
			output.chainList.push_back({ dominoIndex, false });
			output.edge = { chainBig.edge.end, domino[1] };
		}
		else
		{
			output.chainList.push_back({ dominoIndex, true });
			output.edge = {domino[0], chainBig.edge.begin };
		}
		for (auto elm:chainBig.chainList)
		{
			output.chainList.push_back(elm);
		}
	}
	return output;
}

ChainBlock FindSeqvence(const DominoList& dominoList, const DominoList& dominoSeqvence, BlackList& blackList, const ChainList& chainList, const Edge& edge, int sum) //
{
	ChainBlock domOut = { chainList ,sum};
	int razm = 0;
	for (int index =0 ; index < dominoList.size()-1;index++)
	{
		if (!blackList[index])
		{
			if (chainList.size() != 0)
			{
				ChainFit chainFit = IsChainFit(edge,dominoList[index]);
				if(chainFit.fit){

					//ChainList newChain = chainList;
					//newChain.push_back({ index, false });
					ChainBig cf = ConstructChain({edge,chainList},chainFit,dominoList[index],index);
					BlackList newBlackList = blackList;
					newBlackList[index] = true; 
					//Edge newEdge = { dominoList[index][0], dominoList[index][1] };
					ChainBlock newChainBlock = FindSeqvence(dominoList, dominoSeqvence, newBlackList, cf.chainList, cf.edge, sum + dominoList[index][0] + dominoList[index][1]);
					if (newChainBlock.size > domOut.size)
					{
						domOut = newChainBlock;
					};
				}
			}
			else
			{
				ChainList newChain = chainList;
				newChain.push_back({index,false});
				BlackList newBlackList = blackList;
				newBlackList[index] = true;
				Edge newEdge = { dominoList[index][0], dominoList[index][1] };
				ChainBlock newChainBlock = FindSeqvence(dominoList, dominoSeqvence, newBlackList, newChain, newEdge, sum + dominoList[index][0]+dominoList[index][1]);
				if (newChainBlock.size>domOut.size)
				{
					domOut = newChainBlock;
				};
			}
		}
		else
		{
		}
	}
	return domOut;
}

struct MemElem
{
	int index;
	int mean;
};

using MemVector = std::vector<MemElem>;

std::vector<MemVector> Memoization(const DominoList& dominoList) 
{
	std::vector<MemVector> output = { {}, {}, {}, {}, {}, {}, {} };
	for (int iter =0 ;iter < dominoList.size();iter++)
	{
		for (auto i : dominoList[iter])
		{
			output[i].push_back({ iter, dominoList[iter][0] + dominoList[iter][1] });
		}
	}
	return output;
}



int main(int argc,char* argv[])
{
    auto fileNames = ParseArguments(argc,argv);
    auto dominoList = ReadDominoList(fileNames->inputFileName);
	DominoList dominoSeq;
	BlackList blackList;
	for (int i = 0; i < dominoList->dominoList.size() - 1; i++)
	{
		blackList.push_back(false);
	};
	ChainList cha;
	auto p = Memoization(dominoList->dominoList);
	//ChainBlock sd = FindSeqvence(dominoList->dominoList, dominoSeq, blackList, cha, {0,0},0);
    //проверка на длинну: берём домино, составляем самый большой по сумме путь возможно рекурсивно(возможно буду добавлять в начало:  берём домино, ищем подходящее для присоединения в начало или конец присоединяем, дальше то же самое только для присоединённой домино)
    return 0;
    //D:\cpluplus\domino\input.txt
    //D:\cpluplus\domino\output.txt
}