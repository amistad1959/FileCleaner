// FileCleaner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>      // std::istringstream
#include <vector>

#include <chrono>


using namespace std;

/*********************Make verse************************************/
class Verse
{
	uint16_t	verseNbr;
	uint16_t	chapterNbr;
	string		book;
	string		verse;

public:
	Verse();
	~Verse();
	Verse(const Verse&);
	Verse& operator=(const Verse& verse);
	Verse(Verse&&);
	Verse& operator=(const Verse&& verse);
	friend ostream &operator<<(ostream &output, const Verse &v);
	friend istream &operator>>(istream &input, Verse &v);
	string toString(size_t width=0);
	static Verse makeVerse(string& line);
	static bool makeVerses(string& inFile, string& outFile);
	static bool loadObjects(string& inFile, vector<unique_ptr<Verse>>& verses);
	static bool SearchString(string searchStr, vector<unique_ptr<Verse>>& verses, vector<Verse>& versesOut);
};

Verse::~Verse()
{
	this->verseNbr = 0;
	this->chapterNbr = 0;
	(this->book).empty();
	(this->verse).empty();
}

Verse::Verse()
{
	this->verseNbr=0;
	this->chapterNbr=0;
	this->book="";
	this->verse="";
}

Verse::Verse(const Verse& verse)
{
	this->verseNbr=verse.verseNbr;
	this->chapterNbr=verse.chapterNbr;
	this->book=verse.book;
	this->verse=verse.verse;
}

Verse& Verse::operator=(const Verse& verse) 
{
	this->verseNbr=verse.verseNbr;
	this->chapterNbr=verse.chapterNbr;
	this->book=verse.book;
	this->verse=verse.verse;

	return *this;
}

Verse::Verse(Verse&& verse)
{
	this->verseNbr=verse.verseNbr;
	this->chapterNbr=verse.chapterNbr;
	this->book=move(verse.book);
	this->verse=move(verse.verse);
}

Verse& Verse::operator=(const Verse&& verse)
{
	this->verseNbr = verse.verseNbr;
	this->chapterNbr = verse.chapterNbr;
	this->book = move(verse.book);
	this->verse = move(verse.verse);

	return *this;
}

ostream &operator<<(ostream &output, const Verse &v)
{
	char sep = '@';
	output << v.book << sep << v.chapterNbr << sep << v.verseNbr  << sep << v.verse << endl;
	return output;
}

istream &operator>>(istream &input, Verse &v)
{
	string str;
	getline(input, v.book, '@');
	getline(input, str, '@');
	v.chapterNbr=  std::stoi(str);
	getline(input, str, '@');
	v.verseNbr =  std::stoi(str);
	getline(input, v.verse, '@');

	return input;
}

string Verse::toString(size_t width)
{
	string strReturn;
	string strRemain=verse;
	size_t strIdx=0;

	strReturn=book+" "+to_string(chapterNbr)+":"+to_string(verseNbr)+" ";
	
	if (width == 0)
	{
		strReturn += verse;
	}
	else
	{
		strReturn+="\n";
		do
		{
			size_t nBegin = strIdx;
			string strTemp=strRemain;

			strIdx = strTemp.find(' ', width);
			if (strIdx != string::npos)
			{
				strTemp = strRemain.substr(0, strIdx+1);
				strRemain=strRemain.substr(strIdx);
			}
			if(strTemp[0]==' ')
				strTemp.erase(0,1);
			strReturn += strTemp;
			strReturn += '\n';
		} while (strIdx != string::npos);
	}

	return strReturn;
}

Verse Verse::makeVerse(string& line)
{
	Verse verseRet;
	string str;

	istringstream iss(line);
	getline(iss, verseRet.book, ' ');
	if (isdigit(verseRet.book[0]))
	{
		getline(iss, str, ' ');
		verseRet.book=verseRet.book + ' ' + str;
	}
	else if (verseRet.book == "Song")
	{
		verseRet.book = "Song of Solomon";
		getline(iss, str, ' ');
		getline(iss, str, ' ');

	}
	getline(iss,str,':');
	verseRet.chapterNbr= std::stoi(str);
	getline(iss,str,' ');
	verseRet.verseNbr= std::stoi(str);
	while (getline(iss, str, ' '))
	{
		verseRet.verse = verseRet.verse + str + " ";
	}
	verseRet.verse.pop_back();  // remove the last space added

	return verseRet;
}

bool Verse::makeVerses(string& inFile, string& outFile)
{
	ifstream ifs;
	ofstream ofs;
	string line;

	ifs.open(inFile);
	if (ifs.fail())
	{
		cerr << "error opening input file" << endl;
		return false;
	}

	ofs.open(outFile);
	if (ofs.fail())
	{
		cerr << "error opening output file" << endl;
		return false;
	}

	// skip the first line
	getline(ifs, line, '\n');
	while (getline(ifs, line, '\n'))
	{
		Verse currVerse=Verse::makeVerse(line);
		ofs << currVerse;
	}

	ifs.close();
	ofs.close();

	return true;
}

bool Verse::loadObjects(string& inFile, vector<unique_ptr<Verse>>& verses)
{
	ifstream ifs;
	string line;

	ifs.open(inFile);
	if (ifs.fail())
	{
		cerr << "error opening input file" << endl;
		return false;
	}

	while (getline(ifs, line, '\n'))
	{
		istringstream iss(line);
		unique_ptr<Verse> versePtr = std::make_unique<Verse>();
		iss >> (*versePtr);
		verses.push_back(move(versePtr));
	}

	ifs.close();
	return true;
}

bool Verse::SearchString(string searchStr, vector<unique_ptr<Verse>>& verses, vector<Verse>& versesOut)
{
	for (const unique_ptr<Verse>& verse : verses)
	{
		if(verse->verse.find(searchStr) != string::npos)
			versesOut.push_back(*verse);
	}

	return true;
}
/*****************Clean line ******************************************/
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

void run(vector<unique_ptr<Verse>>& verses)
{
	chrono::time_point<std::chrono::system_clock> start, end;
	while (true)
	{
		string str;
		cout << "Type words to search or | to end" << endl;
		getline(cin,str);
		if (str.find('|') != string::npos)
		{
			cout << "Ending";
			break;
		}

		start = std::chrono::system_clock::now();
		cout << "Searching for " << str << endl;
		vector<Verse> versesOut;
		Verse::SearchString(str,verses,versesOut);
		for ( Verse verse : versesOut)
		{
			cout << verse.toString(60);
		}

		end = std::chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end - start;
		cout << "Search time: " << elapsed_seconds.count() << "s\n" << endl;
	}

}

int main(int argc, char* argv[])
{ 
	chrono::time_point<std::chrono::system_clock> start, end;

	start = std::chrono::system_clock::now();
	if (argc == 1)
	{
		cerr << "No argument has been entered. Enter file to process" << endl;
		return 0;
	}
	else if (argc == 2)
	{
		string str = argv[1];
		ReadFile(str);
	}
	else if ((argc == 3) && (!strcmp(argv[2],"-l")))
	{
		constexpr int max_elements = 31104;
		vector<unique_ptr<Verse>> verses;
		verses.reserve(max_elements);
		string fileInput = argv[1];
		Verse::loadObjects(fileInput,verses);
		end = std::chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end - start;
		cout << "Load time: " << elapsed_seconds.count() << "s\n"<<endl;
		run(verses);
	}
	else if(argc==3)
	{
		string fileInput = argv[1];
		string fileOutput = argv[2];
		Verse::makeVerses(fileInput,fileOutput);
	}
	else
	{
		cerr << "too many arguments" << endl;
		return 0;
	}

	end = std::chrono::system_clock::now();

	chrono::duration<double> elapsed_seconds = end - start;
	time_t end_time = std::chrono::system_clock::to_time_t(end);

	cout << "Total elapsed time: " << elapsed_seconds.count() << "s\n";

}


