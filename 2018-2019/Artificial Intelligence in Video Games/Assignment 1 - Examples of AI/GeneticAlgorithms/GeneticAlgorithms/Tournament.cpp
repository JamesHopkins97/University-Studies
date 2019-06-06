#include <iostream>
#include <Windows.h>
#include <vector>
#include <time.h>
#include <algorithm>

using namespace std;

struct Member
{
	int fitness;
};

struct Population
{

};



//int main()
//{
//	//The DNA string is the string that we intend to find. The bool states if we've found the string or not.
//	string DNA = "I like cheese. But I'm lack toes in toddler ants.";
//	bool SequenceFound = false;
//	
//	//1000 = 100% = 0.01% mutation rate
//	int MutationRate = 25;
//	srand(time(NULL));
//
//
//	//Create the population
//	Population population;
//	//Add to the current population
//	for (int i = 0; i < population.Members.size(); i++)
//	{
//		population.Members.at(i).DNA.resize(DNA.size());
//		for (int j = 0; j < DNA.size(); j++)
//		{
//			population.Members.at(i).DNA.at(j) = (unsigned char)rand() % 96 + 32;
//		}
//		population.Members.at(i).Fitness = 0;
//	}
//
//	//The generation variable is being used to keep track of the amount of generations passing
//	int Generation = 0;
//
//	//While loop will stop when it finds the DNA sequence
//	while (!SequenceFound)
//	{
//		Generation++;
//		for (int i = 0; i < population.Members.size(); i++)
//		{
//			population.Members.at(i).Fitness = 0;
//			for (int j = 0; j < population.Members.at(i).DNA.size(); j++)
//			{
//				if (population.Members.at(i).DNA.at(j) == DNA.at(j))
//				{
//					population.Members.at(i).Fitness += 10;
//				}
//			}
//			if (population.Members.at(i).Fitness == DNA.size() * 10)
//			{
//				SequenceFound = true;
//			}
//		}
//		sort(population.Members.begin(), population.Members.end(), [](Member const &a, Member &b) {return a.Fitness > b.Fitness; });;
//
//		vector<Member> Parents{ population.Members.at(0), population.Members.at(1) };
//
//		for (int i = 0; i < population.Members.size(); i++)
//		{
//			for (int j = 0; j < population.Members.at(i).DNA.size(); j++)
//			{
//				int TempSelection = rand() % Parents.size();
//				population.Members.at(i).DNA.at(j) = Parents.at(TempSelection).DNA.at(j);
//
//				if (rand() % 1000 < MutationRate)
//				{
//					population.Members.at(i).DNA.at(j) = (unsigned char)rand() % 96 + 32;
//				}
//			}
//
//		}
//
//		cout << "Generation: " << Generation << "\n\nHighest Fitness: " << Parents.at(0).Fitness << "\n\nWith Sequence: " << Parents.at(0).DNA.c_str() << endl;
//
//
//
//	}
//	cout << "Generation " << Generation << " evolved to full sequence" << endl;
//	system("pause");
//	return 0;
//}