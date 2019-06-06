#include <iostream>
#include <Windows.h>
#include <vector>
#include <time.h>
#include <algorithm>

using namespace std;

int chance(int factor);
//Creates a member per population.
//That member will have a DNA string and a fitness level as identifiers.
struct Member 
{
	int Fitness;
	int Intelligence;
	int Desirability;
	int Total;
	int Average;
	int HungerLevel;
};

//The population will be a total number of members
struct Population
{
	vector<Member> Members = vector<Member>(100);
};

struct Tournament
{
	vector<Member> Parents = vector<Member>(0);
	vector<Member> NewPopulation = vector<Member>(0);
};

int main()
{
	Population p;
	Tournament t;

	//We'll add a random fitness value to each member of the population
	for (int i = 0; i < p.Members.size(); i++)
	{
		p.Members.at(i).Fitness = (int)rand() % 100 + 1;
		p.Members.at(i).Intelligence = (int)rand() % 100 + 1;
		p.Members.at(i).Desirability = (int)rand() % 100 + 1;
		p.Members.at(i).HungerLevel = (int)rand() % 100 + 1;
		p.Members.at(i).Total = p.Members.at(i).Fitness + p.Members.at(i).Intelligence + p.Members.at(i).Desirability;
		p.Members.at(i).Average = p.Members.at(i).Total / 3;
	}

	for (int i = 0; i < p.Members.size(); i++)
		cout << "Population Member: " << (i + 1) << " Fitness: " << p.Members.at(i).Fitness << " Intelligence: " << p.Members.at(i).Intelligence << " Desirability: " << p.Members.at(i).Desirability << "" << endl;

	//We sort the struct so that the people with the highest average go first.
	sort(p.Members.begin(), p.Members.end(), [](Member const &a, Member &b) 
	{return a.Average > b.Average; });;

	int StarvationCounter = 0;
	int InabilityToSurviveCounter = 0;
	for (int i = 0; i < p.Members.size(); i++)
	{
		if (p.Members.at(i).HungerLevel > 80)
		{
			p.Members.erase(p.Members.begin() + i);
			StarvationCounter++;
		}
	}

	cout << "Death from starvation: " << StarvationCounter << endl;
	
	for (int i = 0; i < p.Members.size(); i++)
	{
		if (p.Members.at(i).Intelligence < 50)
		{
			p.Members.erase(p.Members.begin() + i);
			InabilityToSurviveCounter++;
		}
	}
	cout << "Death from inability to survive: " << InabilityToSurviveCounter << endl;

	cout << "Size: " << p.Members.size() << endl;

	int rando = 0;
	for (int i = 0; i < p.Members.size(); i++)
	{
		rando = (int)rand() % 100 + 1;
		if (rando < p.Members.at(i).Desirability)
		{
			t.Parents.push_back(p.Members.at(i));
		}
	}


	for (int i = 0; i < t.Parents.size(); i++)
	{

		cout << "Parent Member: " << (i + 1) << " Fitness: " << t.Parents.at(i).Fitness << " Intelligence: " << t.Parents.at(i).Intelligence << " Desirability: " << t.Parents.at(i).Desirability << endl;
	}

	int AddTo = 0;
	for (int i = 0; i < t.Parents.size(); i++)
	{
		int FitnessAverage = 0;
		int IntelligenceAverage = 0;
		int DesirabilityAverage = 0;
		int NumOfChildren = (int)rand() % 3 + 1;
		if (i % 2 != 0)
		{
			FitnessAverage = (t.Parents.at(i - 1).Fitness + t.Parents.at(i).Fitness) / 2;
			IntelligenceAverage = (t.Parents.at(i - 1).Intelligence + t.Parents.at(i).Intelligence) / 2;
			DesirabilityAverage = (t.Parents.at(i - 1).Desirability + t.Parents.at(i).Desirability) / 2;
			for (int j = 0; j < NumOfChildren; j++) {
				t.NewPopulation.push_back(t.Parents.at(i));
				t.NewPopulation.at(AddTo).Fitness = FitnessAverage;
				t.NewPopulation.at(AddTo).Intelligence = IntelligenceAverage;
				t.NewPopulation.at(AddTo).Desirability = DesirabilityAverage;
				AddTo++;
			}
		}
	}

	for (int i = 0; i < t.NewPopulation.size(); i++)
	{

		cout << "Children Member: " << (i + 1) << " Fitness: " << t.Parents.at(i).Fitness << " Intelligence: " << t.Parents.at(i).Intelligence << " Desirability: " << t.Parents.at(i).Desirability << endl;
	}



	system("pause");
	return 0;
}
