#include <iostream>
#include<fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>
using namespace std;
unordered_map<string, int> key;
unordered_map<int,string> inverseKey;
vector<int> nImpressions;
vector<double> pageRanks;
vector<int>result;
vector<int> clickThrough;
vector<double> score;
bool EXIT=false;
class Graph
{
public:
    int vertices;
    vector<vector<int>> list;
    Graph(int size){
        vertices = size;
        list.resize(vertices);
    }
    void addEdge(int source, int destination){
        if (source >= 0 && source < vertices && destination >= 0 && destination < vertices)
            list[source].push_back(destination);
    }
    void print(){
        for (int i = 0; i < vertices; i++)
        {   cout<<i<<" is connected with : ";
            for (int j = 0; j < list[i].size(); j++)
            {
                cout << list[i][j] << " ";
            }
            cout << endl;
        }
    }
};
Graph createGraph() {
    vector<string> websites;
    fstream webGraph;
    string line;
    string temp;
    string temp1;
    int counter = 0;
    webGraph.open("graph.csv");
    if (!webGraph.good())
        cout << "File could not open\n";
    while (!webGraph.eof()) {
        getline(webGraph, line);
        temp = line.substr(0, line.find(','));
        temp1 = line.substr(line.find(',') + 1);
        websites.push_back(temp);
        websites.push_back(temp1);

    }
    webGraph.close();
    for (int i = 0; i < websites.size(); i++) {
        if (!key.count(websites[i])) {
            key.emplace(websites[i], counter);
            counter++;
        }
    }
    Graph outputGraph(counter);
    for(int i=0; i<websites.size();i+=2)
    {
        outputGraph.addEdge(key[websites[i]],key[websites[i+1]]);
    }
    return outputGraph;
}
void createInverseMap(){
    int counter = 0;
    for(auto it=key.begin();it!=key.end();it++) {
        if(counter==key.size()-1) break;
        inverseKey[it->second]=(it->first);
        counter++;
    }
}
vector<vector<string>> siteKeywords(){
    vector<string> words;
    fstream keywords;
    vector<vector<string>> output(key.size(),vector<string>(0));
    string line;
    string temp;
    int counter = 0;
    keywords.open("keywords.csv");
    if (!keywords.good())
        cout << "File could not open\n";
    while (keywords.good()) {
        getline(keywords, line);
        words.push_back(line);
    }
    keywords.close();
    int index;
    for(int i=0; i<words.size();i++)
    {
        index=0;
        counter=0;
        stringstream s(words[i]);
        while(!s.eof()) {
            getline(s, temp, ',');
            if (counter==0) {
                if (!key.count(temp)) {
                    key.emplace(temp, key.size());}
                index = key[temp];
            }
            else
            {
                output[index].push_back(temp);
            }
            counter++;
        }
    }
    return output;
}
void computeRanks()
{
    Graph webGraph=createGraph();
    bool iterate=true;
    vector<double> prev(webGraph.vertices),current(webGraph.vertices);
    for(int i=0;i<webGraph.vertices;i++)
        prev[i]=1.0/webGraph.vertices;
    while(iterate)
    {
        for(int j=0;j<webGraph.vertices;j++)
        {
            double temp=0;
            for(int k=0; k<webGraph.vertices;k++)
            {   for(int m=0;m<webGraph.list[k].size();m++){
                    if(j==webGraph.list[k][m]){
                        temp+=(double)prev[k]/webGraph.list[k].size();
                    }
                }
            }
            current[j]=temp;
        }
        for(int k=0;k<current.size();k++){
            if(prev[k]==current[k])
                iterate=false;
            else prev[k]=current[k];
        }

    }
    for(int i=0;i<webGraph.vertices;i++)
        pageRanks.push_back(current[i]);
}

void initialImpression(){
    vector<string> impressions;
    fstream numbers;
    nImpressions.resize(key.size());
    string line;
    string temp;
    int counter = 0;
    numbers.open("impressions.csv");
    if (!numbers.good())
        cout << "File could not open\n";
    while (numbers.good()) {
        getline(numbers, line);
        impressions.push_back(line);
    }
    numbers.close();
    int index;
    for(int i=0; i<impressions.size();i++) {
        counter = 0;
        stringstream s(impressions[i]);
        while (!s.eof()) {
            getline(s, temp, ',');
            if (counter == 0) {
                if (!key.count(temp)) {
                    key.emplace(temp, key.size());}
                index = key[temp];
            }
            else{ nImpressions[index]=stoi(temp);}
            counter++;
        }
    }
}
vector<string> splitQuery(string query, bool& AND, bool& OR)
{
    vector<string> words;
    string temp;
    //transform(query.begin(), query.end(), query.begin(), ::tolower);
    if(query[0]=='"')
        words.push_back(query.substr(1,query.substr(1).find('"')));
else {
        if (query.find("AND") != string::npos) {
            AND = true;
            query.erase(query.find("AND"), 4);
        } else if (query.find("OR") != string::npos) {
            OR = true;
            query.erase(query.find("OR"), 3 );
        } else OR = true;
        stringstream s(query);
        while (s >> temp)
            words.push_back(temp);
    }
    return words;
}
void searchQuery(string query) {
    bool AND = false;
    bool OR = false;
    int counter = 0;
    vector<vector<string>> keywords = siteKeywords();
    vector<string> searchWords = splitQuery(query, AND, OR);
    if (AND) {
        for (int i = 0; i < keywords.size(); i++) {
            counter=0;
            for (int j = 0; j < searchWords.size(); j++) {
                auto p = find(keywords[i].begin(), keywords[i].end(), searchWords[j]);
                if (keywords[i].end() != p) {
                    counter++;
                }
                if (counter == searchWords.size()) {
                    result.push_back(i);
                }
            }
        }
    } else if (OR) {
        for (int i = 0; i < keywords.size(); i++) {
            for (int j = 0; j < searchWords.size(); j++) {
                auto p = find(keywords[i].begin(), keywords[i].end(), searchWords[j]);
                if (keywords[i].end() != p) {
                    result.push_back(i);
                        }
                    }
                }
            }

     else {
        for (int i = 0; i < keywords.size(); i++) {
            auto p = find(keywords[i].begin(), keywords[i].end(), searchWords[0]);
            if (keywords[i].end() != p) {
                result.push_back(i);
                    }
        }}
}
void clickReader()
{
    clickThrough.resize(key.size(),0);
    vector<string> clicks;
    string line;
    string temp;
    int counter;
    ifstream readFile;
    readFile.open("clicks.csv");
    if(!readFile.good())
        return;
    while (readFile.good()) {
        getline(readFile, line);
        clicks.push_back(line);
    }
    readFile.close();
    int index;
    for(int i=0; i<clicks.size();i++) {
        counter = 0;
        stringstream s(clicks[i]);
        while (!s.eof()) {
            getline(s, temp, ',');
            if (counter == 0) {
                if (!key.count(temp)) {
                    key.emplace(temp, key.size());}
                index = key[temp];
            }
            else{ clickThrough[index]=stoi(temp);}
            counter++;
        }
    }
}
void calcScore()
{
    score.resize(result.size());
    for(int i=0; i<result.size();i++)
    {
        score[i]=0.4 * pageRanks[result[i]] + ((1 - ((0.1 * nImpressions[result[i]]) /(1 + 0.1 * nImpressions[result[i]])))
                * pageRanks[result[i]] + ((0.1 * nImpressions[result[i]]) / (1 + 0.1 * nImpressions[result[i]])) *
                (clickThrough[result[i]]/(double)nImpressions[result[i]]) * 0.6);
    }

}
int partition(int low, int high)
{
    int temp3;
    double temp4;
    double pivot = score[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (score[j] < pivot)
        {
            i++;
            temp3=result[i];
            result[i]=result[j];
            result[j]=temp3;
            temp4=score[i];
            score[i]=score[j];
            score[j]=temp4;

        }
    }
    int temp=result[i+1];
    result[i+1]=result[high];
    result[high]=temp;
    double temp5=score[i+1];
    score[i+1]=score[high];
    score[high]=temp5;
    return (i + 1);

}
void quickSortResult( int low, int high)
{

    if (low < high)
    {
        int pi = partition(low, high);
        quickSortResult( low, pi - 1);
        quickSortResult( pi + 1, high);
    }
}
void updateImpressions()
{
    for(int i=0; i<result.size();i++)
    {
        nImpressions[result[i]]++;
    }
    ofstream update("E:\\Uni\\Fall 2020\\Anlsys lab\\Project\\impressions.csv");
        int counter = 0;
        for(auto it=key.begin();it!=key.end();it++) {
            if(counter==key.size()-1) break;
            update<<(it->first)<<","<<nImpressions[it->second]<<endl;
            counter++;
        }
    update.close();
}
void updateClicks(int input)
{
        clickThrough[result[input]]++;
    ofstream update("E:\\Uni\\Fall 2020\\Anlsys lab\\Project\\clicks.csv");
    int counter = 0;
    for(auto it=key.begin();it!=key.end();it++) {
        if(counter==key.size()-1) break;
        update<<(it->first)<<","<<clickThrough[it->second]<<endl;
        counter++;
    }
    update.close();
}
void initializeProgram()
{
    computeRanks();
    initialImpression();
    clickReader();
    createInverseMap();
}
void displayResult() {
    calcScore();
    quickSortResult(0, result.size() - 1);
    cout<<endl;
    cout << "Search Results:  \n";
    int counter=1;
    for (int i = result.size() - 1; i >= 0; i--) {
            cout << counter << ". " << inverseKey[result[i]]<< " ";
        cout << endl;
        counter++;
    }
    cout<<endl;
    updateImpressions();
}
void search()
{
    string query="";
    cout<<endl;
    cout<<"For a specific term, enter your query between quotations,\n"
          "\nTo get a page with two specific terms, enter AND in between,\n"
          "\nAnd to get a page with either terms, enter OR in between\n";
    cout<<"\nEnter your search inquiry below:_\n";
    cin.ignore();
    getline(cin,query);
    searchQuery(query);
    displayResult();

}
void clickPage(){
    int input;
    cout<<endl;
    cout<<"Choose the WebPage number to open:_ \n";
    cin>>input;
    cout<<endl;
    cout<<"You are now viewing: "<<inverseKey[result[result.size()-input]]<<endl;
    updateClicks(result.size()-input);

}
void REsearch()
{
    int input;
    string query="";
    cout<<endl;
    cout<<"For a specific term, enter your query between quotations,\n"
          "To get a page with two specific terms, enter AND in between,\n"
          "And to get a page with either terms, enter OR in between\n";
    cout<<"Enter your search inquiry below:_\n";
    cin.ignore();
    getline(cin,query);
    result.clear();
    searchQuery(query);
    displayResult();

    }
void actions(int number)
{
    if(number==1) REsearch();
    if(number==2) clickPage();
    if(number==3) displayResult();
    if(number==4) EXIT=true;
}
void searchMenu(int menuSet)
{
    int input;
    if (menuSet==1) {
        cout<<endl;
        cout << "Would you like to: \n";
        cout << "1. Choose a webPage to open: \n"
                "2. New search. \n"
                "3. Exit. \n";
        cin >> input;
        if (input == 1) {actions(2);
            searchMenu(2);}
        if (input == 2) {actions(1);
        searchMenu(1);}
        if (input == 3) actions(4);
    }
    if(menuSet==2)
    {
        cout<<endl;
        cout<<"Would you like to: \n";
        cout<<"1. Go back to search results. \n"
              "2. New search. \n"
              "3. Exit. \n";
        cin>>input;
        if(input==1) {actions(3);
        searchMenu(1);}
        if(input==2){ actions(1); searchMenu(1);}
        if(input==3) actions(4);
    }
}


int main() {
    int input;

    cout<<"Welcome!"<<endl;
    cout<<"What would you like to do?\n";
    cout<<"1. New Search\n";
    cout<<"2. Exit\n";
    cout<<"Type in your choice number:_\n";
    cin >> input;
    if (input == 1) {
        initializeProgram();
        search();}
        if (input == 2)
            exit(1);
    while(!EXIT) {
  searchMenu(1);
    }
    return 0;
}


