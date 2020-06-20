#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <csignal>
#include <unistd.h>
using namespace std;
vector<char*> split(string, string);

int main() {
	string input;
	string history[1024];
	string *str;
	vector<char*> stt;
	int flag = 0, count = 0;
	int index = 0;
	pid_t pid;

	sigset_t blockset;
	sigemptyset(&blockset);
	sigaddset(&blockset, SIGINT);
	sigaddset(&blockset, SIGQUIT);
	sigprocmask(SIG_BLOCK, &blockset, NULL);
	string id;
	cout << "ID: ";
	cin >> id;
	cin.ignore();
	string pwd;
	cout << "PW : ";
	cin >> pwd;
	cin.ignore();
	vector<char*>vec1;
	vector<char*>vec2;
	while (1) {
		cout << "ID:@" << id << ">";
		if (flag == 0) {
			if (!vec1.empty())vec1.clear();
			if (!vec2.empty())vec2.clear();
			getline(cin, input, '\n');
			stt = split(input, ",");
			count = stt.size();
			int i = 0;
			while (input.find('!') == string::npos && i != count && input != "!!") {
				history[index] = stt.at(i);
				cout << index;
				index++;
				i++;
			}
		}
		if (flag == 1)
		{
			flag = 0;
			cout << endl;
		}

		if (input.find('|') != string::npos) {
			if (!vec1.empty())vec1.clear();
			if (!vec2.empty())vec2.clear();
			int loc = input.find('|');
			string string1 = input.substr(0, loc - 1);
			string string2 = input.substr(loc + 1);
			vec1 = split(string1, " ");
			vec1.push_back(NULL);
			vec2 = split(string2, " ");
			vec2.push_back(NULL);
			int filedes[2];
			if (pipe(filedes) == -1) {
				cout << "pipe call fail" << endl;
				exit(1);
			}
			switch (fork())
			{
			case -1: {perror("fork error"); break; }
			case 0: {
				if (close(1) == -1) perror("close");
				if (dup(filedes[1]) != 0)
					if (close(filedes[0]) == -1 || close(filedes[1]) == -1) {
						perror("close");
					}
				char **com1 = &vec1[0];
				execvp(com1[0], com1);
				cout << "no exist command" << endl;
				exit(0);
			}
			}
			switch (fork())
			{
			case -1: {perror("fork"); break; }
			case 0: {
				if (close(0) == -1) perror("close");
				if (dup(filedes[0]) != 0);
				if (close(filedes[0]) == -1 || close(filedes[1]) == -1) perror("close");
				char **com2 = &vec2[0];
				execvp(com2[0], com2);
				cout << "no exist command" << endl;
				exit(0);
			}
			}

			if (close(filedes[0]) == -1 || close(filedes[1]) == -1) perror("close");
			while (wait(NULL) != -1);

		}
		else if (input.find('>') != string::npos) {
			if (!vec1.empty())vec1.clear();
			if (!vec2.empty())vec2.clear();
			int loc = input.find('>');
			string string1 = input.substr(0, loc - 1);
			string string2 = input.substr(loc + 1);
			vec1 = split(string1, " ");
			vec1.push_back(NULL);
			vec2 = split(string2, " ");
			vec2.push_back(NULL);
			cout << vec2.at(0);
			switch (fork())
			{
			case -1: {perror("fork"); break; }
			case 0: {
				int fdr;
				char ** com2 = &vec2[0];
				fdr = open(com2[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (fdr == -1) {
					perror("file open error"); exit(1);
				}
				if (dup2(fdr, 1) == -1) {
					perror("file duplicate error");
				}
				close(fdr);
				char **com1 = &vec1[0];
				execvp(com1[0], com1);
				cout << "no exist command" << endl;
				exit(0);
				break;
			}
			default: {wait(NULL); }
			}
		}
		else if (input.find('<') != string::npos) {
			if (!vec1.empty())vec1.clear();
			if (!vec2.empty())vec2.clear();
			int loc = input.find('<');
			string string1 = input.substr(0, loc - 1);
			string string2 = input.substr(loc + 1);
			vec1 = split(string1, " ");
			vec1.push_back(NULL);
			vec2 = split(string2, " ");
			vec2.push_back(NULL);
			cout << vec2.at(0);
			switch (fork())
			{
			case -1: {perror("fork"); break; }
			case 0: {
				int fdr;
				char ** com2 = &vec2[0];
				fdr = open(com2[0], O_RDONLY, 0644);
				if (fdr == -1) {
					perror("file open error"); exit(1);
				}
				if (dup2(fdr, 0) == -1) {
					perror("file duplicate error");
				}
				close(fdr);
				char **com1 = &vec1[0];
				execvp(com1[0], com1);
				cout << "no exist command" << endl;
				exit(0);
				break;
			}
			default: {wait(NULL); break; }
			}
		}
		else {
			if (!vec1.empty())vec1.clear();
			vec1 = split(input, " ");
			vec1.push_back(NULL);
			char **com1 = &vec1[0];
			if (com1[0] != NULL) {
				if (strcmp(com1[0], "cd") == 0) {
					chdir(com1[1]);

				}
				else if (strcmp(com1[0], "!!") == 0) {
					flag = 1;
					continue;
				}
				else if (input.find('!') != string::npos) {
					flag = 1;
					continue;
				}
				else if (strcmp(com1[0], "logout") == 0) {
					exit(0);
				}
				else if (strcmp(com1[0], "history") == 0)
				{
					cout << "HISTORY>";
					for (int line = 0; line < index; line++)
					{
						cout << endl << "[" << line << "] :   ";
						cout << history[line];
					}
					cout << endl;
				}
				else if (fork() == 0) {
					execvp(com1[0], com1);
					cout << "no exist command" << endl;
					exit(0);
				}
				wait(NULL);
			}
		}
	}
}
vector<char*> split(string str, string del)
{
	vector<char*> split;
	char *vChar;
	int count = 0;
	int  end = 0;
	while ((end = str.find(del, count)) < str.size())
	{
		string val = str.substr(count, end - count);
		vChar = new char[str.length() + 1];
		for (int a = 0; a <= val.length(); a++) {
			vChar[a] = val[a];
		}
		split.push_back(vChar);
		count = end + del.size();
	}
	if (count < str.size())
	{
		string val = str.substr(count);
		vChar = new char[str.length() + 1];
		for (int a = 0; a <= val.length(); a++) {
			vChar[a] = val[a];
		}
		split.push_back(vChar);
	}
	return split;
}
