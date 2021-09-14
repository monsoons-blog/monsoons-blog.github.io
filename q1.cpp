
#include<stdlib.h>
#include<stdio.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>
#include<fstream>
#include<string>
#include<cstring>
#include<iostream>
#include <typeinfo>

using namespace std;

struct store
{
	double threshold;
	char** transactions;	//stores all the transactions by making a dynamic array of transactions of size (trans_count)
};

int main()
{

	double sup_threshold;
	int trans_count;
	store obj;

	//create unnamed pipe
	int p1[2];	//[0] for reading, [1] for writing
	pipe(p1);

	int p2[2];
	pipe(p2);

	//Step 1:
	//call fork()
	//parent process reads file by taking filename
	//read file, store into struct object
	//In unnamed pipe b/w parent and child
	//parent writes to child
	//child reads from parent and stores data

	string fName;
	int pid1 = fork();
	if(pid1 > 0) // parent process
	{	

		//read file
		cout<<"Enter data filename:-"<<endl;
		cin>>fName;
		ifstream inFile(fName, ios::in);
		inFile>>sup_threshold;
		inFile>>trans_count;

		obj.transactions = new char*[trans_count];
		for(int i=0; i<=trans_count; i++)
		{
			obj.transactions[i] = new char[500];	// max length of one transaction
		}
		obj.threshold = sup_threshold*trans_count;

		int line = 0;
		string buf;
		while(line<trans_count+3)		//+3 because of first two lines that are to be ignored
		{

			if(!(line==0 || line== 1))
			{
				getline(inFile,buf);
				for(int a =0; a<buf.length(); a++)
				{
					obj.transactions[line-3][a] = buf[a];
				}
				//obj.transactions[line-2] = &buf[0];
				//obj.transactions[line-2] = buf.c_str();
			}
			line++;
		}
		inFile.close();
		cout<<"Threshold = "<<obj.threshold<<endl;
		cout<<"Transactions:-"<<endl;
		cout<<"Trans count= "<<trans_count<<endl;

		for(int j=0; j<trans_count;j++)
		{
			cout<<"T"<<j<<" = "<<obj.transactions[j]<<endl;
		}

		//file successfully read and data stored
		//now send this object to child process by writing and reading through p1
		//Check if we can send char**, or obj, or write and read in a loop, or we'll have to create separate pipes for each data value.

		close(p1[0]);	//close reading end
		write(p1[1], to_string(obj.threshold).c_str(), sizeof(to_string(obj.threshold).c_str()));		//double casting to convert int into char*
		cout<<"Written threshold to pipe."<<endl;
		write(p1[1], to_string(trans_count).c_str(), sizeof(to_string(trans_count).c_str()));		//double casting to convert int into char*

		int maxlen = 32;
		write(p1[1], to_string(maxlen).c_str(), sizeof(to_string(maxlen).c_str()));		


		for(int i=0; i<trans_count; i++)
		{
			int length =0;
			int j =0;
			while(obj.transactions[i][j]!='\0')
			{
				length++;
				j++;
			}

			cout<<"length = "<<length<<endl;
			cout<<"sizeof(obj.transactions[i]) = "<<sizeof( obj.transactions)<<'\t';
			cout<<"obj.transactions["<<i<<"] = "<<obj.transactions[i]<<endl;

			write(p1[1], obj.transactions[i], maxlen);
			//wait(NULL);
		}


		close(p1[1]);
	}
	else if(pid1 == 0)	//child process
	{
		store obj1;
		char* readBuf = new char[50];
		close(p1[1]);	//close writing end
		wait(NULL);
		cout<<"Child reading from pipe"<<endl;
		read(p1[0], readBuf, sizeof(readBuf));
		cout<<"Received threshold = "<<readBuf<<endl;
		obj1.threshold = atoi(readBuf);					//convert from char* to int

		wait(NULL);
		read(p1[0], readBuf, sizeof(readBuf));			//this time read to get number of transactions
		cout<<"Number of transactions = "<<readBuf<<endl;
		trans_count = atoi(readBuf);
		obj1.transactions = new char*[trans_count];		//allocate space to object's transactions array
		for(int i=0; i<=trans_count; i++)
		{
			obj1.transactions[i] = new char[500];		// max length of one transaction
		}

		wait(NULL);
		read(p1[0], readBuf, sizeof(readBuf));			//this time read to get maxlen
		int maxlen = atoi(readBuf);

		cout<<"trans_count in child = "<<trans_count<<endl;
		for(int i=0; i<trans_count; i++)
		{
			wait(NULL);
			//read(p1[0], readBuf, sizeof(readBuf));			//this time read to get number of transactions
			//cout<<"Read Buffer for transactions = "<<readBuf<<endl;
			read(p1[0], obj1.transactions[i], maxlen);
			//INCOMPLETE READING!!!!!

		}

		for(int i=0; i<trans_count; i++)
		{
		    //cout<<"Size of read transaction elements:"<<endl;
			cout<<"T["<<i<<"] = "<<obj1.transactions[i]<<endl;
		}

		close(p1[0]);
		exit(0);
	}
	else
	{
		cout<<"Error in fork()"<<endl;
		exit(1);
	}

	return 0;
}

