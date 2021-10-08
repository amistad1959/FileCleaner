// FileCleaner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>      // std::istringstream

using namespace std;

void processLine(string& line)
{
	char current=0;
	char previous=0;
	bool bInLabel=false;

	istringstream iss(line);
	iss >> noskipws;

	while (iss >> current)
	{
		// let's iliminate extra blank spaces
	/*	if((current==' ')&&(previous==' '))
			continue;
		previous=current;*/

		if(current=='\t')
			current=' ';

		// do not incluede xml tags
		if(current=='<')
			bInLabel=true;
		if(!bInLabel)
			cout<<current;
		if(current=='>')
			bInLabel=false;

	}

	cout<<endl;

}

bool ReadFile(string& inFile)
{
	ifstream ifs;
	string line;
	ifs.open(inFile);
	if (ifs.fail())
	{
		cerr<<"error opening file"<<endl;
		return false;
	}

	while (getline(ifs, line,'\n'))
	{
		processLine(line);
	}

	ifs.close();

	return true;
}

int main(int argc, char* argv[])
{ 
	if (argc == 0)
	{
		cerr << "No argument has been entered. Enter file to process" << endl;
		return 0;
	}

	string str=argv[1];

	ReadFile(str);
}

