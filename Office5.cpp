#include <iostream>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <set>
#include <fstream>
#include <filesystem>
#include <map>
#include <vector>
#include <utility>
#include <sstream>


using namespace std;

class student{
//object to represent each student for office hours
public:
	//constructors
	student(int Ta);

	//getters
	int getTarrive() const {return Tarrive;}
	int getSrate() const {return Srate;}
	int getUrate() const {return Urate;}
	char getTopic() const {return Topic;}
	string getName() const {return Name;}

	//operator overloads
	//these two functions allow student to be used in a priority queue
	bool operator < (const student& rhs)const{return Urate < rhs.Urate;}
	bool operator >(const student& rhs)const{return Urate > rhs.Urate;}

private:
	string Name; // their name
	char Topic; //their topic
	int Tarrive; //their arrival time
	int Srate; // their service time with the professor
	int Urate; //their urgency rate
};

student::student(int Ta): Tarrive(Ta){
//implimentation of 1 argument constructor

	//set service rate to random number between 2 and 10 minutes
	Srate = rand() % 9 + 2;
	//set urgency rate to random number between 1 and 10
	Urate = rand() % 10 + 1;
	//set their topic to a random lower case letter
	int letterascii1 = rand() %26 + 97;
	Topic = static_cast<char>(letterascii1);
	//set their name to StudentX or any capital letter
	//this means that there are 26 actual students in the class
	Name = "Student";
	int letterascii2 = rand() %26 + 65;
	Name.push_back(static_cast<char>(letterascii2));
}

bool CheckNewStudent(double arrivalprob){
//Returns true if random number is less than arrival probability
	int rnum = rand() % 100;
	if(rnum < (arrivalprob*100)) {return true;} 
	return false;
}

void InsertionSort(string file, char order){//insertion sort for a file with lines split by a colon
	ifstream inputfile(file);
	vector<pair<string,string>> pairs;
	string line;

	while(getline(inputfile, line)) { //read each line into a pair and add pair to vector
        istringstream iss(line);
        string key, value;
        if (getline(iss, key, ':')) {
            value = line.substr(line.find(':'));// Get the substring starting at the colon
            pairs.push_back(make_pair(key, value)); //add the pair to the vector
        }
    }
	inputfile.close();

	if(order == '1'){//if user wants sorted alphabetical by topic
		for (int i = 1; i< pairs.size(); i++){
			for(int j = i; j > 0 and pairs[j].second < pairs[j-1].second;j--){
				swap(pairs[j],pairs[j-1]);
			}
		}
	}

	if(order == '2'){//if user wants sorted reverse alphabetical by topic
		for (int i = 1; i< pairs.size(); i++){
			for(int j = i; j > 0 and pairs[j].second > pairs[j-1].second;j--){
				swap(pairs[j],pairs[j-1]);
			}
		}
	}

	if(order == '3'){//if user wants sorted reverse alphabetical by name
		for (int i = 1; i< pairs.size(); i++){
			for(int j = i; j > 0 and pairs[j].first > pairs[j-1].first;j--){
				swap(pairs[j],pairs[j-1]);
			}
		}
	}
	//overwrite file with contents in newly sorted order
	ofstream outputfile(file);
	for(auto pair: pairs){
		outputfile<<pair.first<<pair.second<<"\n";
	}
	outputfile.close();
}

void OfficeHours(){
//function to simulate n office hour sessions.
	srand(time(0)); // create new random seed

	//set of the 26 different class topics
	set<char> Topics;
	Topics.insert({'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'});

	//multimap for students and their topics
	multimap<string,pair<char,int>> studentmap;

	//create subfolder for topic output files
	string subfolder1 = "TopicOutputFolder";
	std::__fs::filesystem::create_directory(subfolder1);

	//create subfolder for each student report
	string subfolder2 = "StudentOutputFolder";
	std::__fs::filesystem::create_directory(subfolder2);


	//stats we want to take
	double total_students = 0;
	double total_extra_office_time = 0;
	double total_time_with_students = 0;
	double total_time_inline = 0;

	//indepedent variables the user will enter
	double arrivalprob;
	int sessions;


	cout<<"Welcome to the Office Hours Simulator!"<<endl<<endl;
	//get independent variables from the user
	cout<<"Enter probability that student shows up every minute ex: 0.05: ";
	cin>> arrivalprob;
	cout<<"Enter number of office hour sessions: ";
	cin>>sessions;
	cout<<endl;
	cout<<"Running the simulation..."<<endl;

	//change the independent variables to manageable values
	if(arrivalprob > 1 or arrivalprob < 0) {arrivalprob = 0.1;}
	if(sessions> 500 or sessions < 0) {sessions = 500;}

	for(int i=0; i<sessions; i++){ //run the office hours sessions amount of times
		priority_queue<student> q1;
		set<char> StudentsTopics;

		int t = 0;
		int tWithStudent = 0;
		while(t<=60 or !q1.empty()){ 
		//loop that ends when time is > 60 or no students
			if(t<=60 and CheckNewStudent(arrivalprob)){
			//If student arrives before 60 minutes
				++total_students;
				student temp(t); //create a student

				total_time_with_students+=temp.getSrate(); //add their service rate to the total

				StudentsTopics.insert(temp.getTopic());//add the students topic to the set

				pair<char,int> studentvalues(temp.getTopic(),i+1);//create pair with the students topic and session number
				studentmap.insert({temp.getName(),studentvalues});//add the student to the map

				
				q1.push(temp);//add the student to the priority queue
			}
			if(!q1.empty()){ //process student in line
				++tWithStudent;
				if(tWithStudent>=q1.top().getSrate()){
				//determines if student is finished with the teacher
					total_time_inline += (t - q1.top().getTarrive() - q1.top().getSrate() + 1);
					//adds the time they waited to the total
					q1.pop();
					tWithStudent = 0;
				}
			}
			++t;
		}
		if(t>60){
			//adds to the total extra office hours time
			total_extra_office_time += (t-61);
		}

		//Summarize the topic results in new file in the TopicOutputFolder
		string filename = subfolder1 + "/output"+to_string(i+1)+".txt";
		ofstream file(filename);
		file<<"session# "<<i+1<<"\n";
		file<<"The students asked questions about these topics: \n";
		for(auto element : StudentsTopics){
			file<<element<<"\n";
		}
		file.close();
		filename.clear(); // reset the file name
	}

	//create file with students reports in subfolder
	string filename2 = subfolder2 + "/StudentsReport.txt";
	ofstream file2(filename2); // open file
	for(auto it = studentmap.begin(); it != studentmap.end(); ++it){ //iterate across map
		file2<<it->first<<" asked about topic: "<<it->second.first<<" at session# "<<it->second.second<<"\n"; //add student and topic to report
	}
	file2.close(); // close file

	//print statistics
	cout<<endl<<"Here are some basic statistics:"<<endl<<endl;
	cout<<"average time student waits in line: "<< total_time_inline/total_students<< " minutes"<<endl;
	cout<< "The average time with professor: "<<total_time_with_students/total_students<< " minutes"<< endl;
	cout<< "The average extra office hours time: "<<total_extra_office_time/sessions<<" minutes"<<endl;
	cout<< "total students: "<<total_students<<endl<<endl;

	//ask user how they want the file sorted
	cout<<"How would you like your students reports sorted? "<<endl<<endl;
	cout<<"type 1 for alphabetical by topic"<<endl;
	cout<<"type 2 for reverse alphabetical by topic"<<endl; 
	cout<<"type 3 for reverse alphabetical by name"<<endl; 
	cout<<"type 4 or anything else for alphabetical by name: "<<endl;
	char sortstyle;
	cin>>sortstyle;

	//call sort function
	InsertionSort("StudentOutputFolder/StudentsReport.txt",sortstyle);
	cout<<endl<<"A folder was created with the session topics"<<endl;
	cout<< "A folder was created with the sorted students report"<<endl;

	//search method below	
	char c;
	int count;
	while (c!='0'){ //allows user to search as long as they want
		cout<<endl<<"Would you like to search for a Topic or Student?"<<endl;
		cout<<"If yes, type an uppercase letter A-Z for studentA - studentZ, or a lower case letter a-z for topic a-z.";
		cout<<"If you want be done, type 0: "<<endl;
		cin>>c;
		cout<<endl;
		count = 0; //reset count

		if(c!='0'){//This is a basic linear search algorithm
			if(isupper(c)){//upper case letter means search for student
				for(auto it = studentmap.begin(); it != studentmap.end(); ++it){ //iterate across map
					string tempname = "Student"; 
					tempname+=c;
					if(it->first == tempname){//check if right student
						cout<<tempname<<" asked about topic: "<<it->second.first<<" at session# "<<it->second.second<<endl;
						count++;
					}
				}			
			}
			else{//lower case letter means search for a subject
				for(auto it = studentmap.begin(); it != studentmap.end(); ++it){ //iterate across map
					if(it->second.first == c){ //check if right topic
						cout<<it->first<<" asked about topic: "<<it->second.first<<" at session# "<<it->second.second<<endl;
						count++;
					}
				}		
			}
			if(count != 0){ //means there was a match
				cout<<endl<<"Your results are above ^";
			}
			else{//means no matches were found 
				cout<<endl<<"There were no matches";
			}
			cout<<endl;
		}
	}
	cout<<endl;
}

int main(){

	OfficeHours(); //call the Office Hours Function

	return 0;
}