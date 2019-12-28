# include <iostream>
# include <fstream>
# include <stdlib.h>
# include <cstdio>
# include <memory>
# include <stdexcept>
# include <string>
# include <array>
# include <ctype.h>
# include <conio.h>
# include <string.h>
# include <limits>

using namespace std;

struct details{
    char teamName[100];
    int numOfStocks;
    char stockCodes[10][10];
    int stockQuant[10];
    long int balance;
};

struct stocks{
    char stockName[50];
    char stockCode[10];
    int stockPrice;
};

int initBalance = 10000;
//char string[100];

void fileIn();
void printDetails(int n);
int nRecords ();
void commandAll();
void commandShow();
void commandAdd();
void printIndStockDetails();
int findStockPrice(char stockCode[10]);
void commandModify();
void getDecision (int &);
void commandBuy();
void commandSell();
bool isValidStockCode (char stockCode[10]);
int calcProfit(details team);
void fetchInitBalance();
bool stockAlreadyBought(details team, char stockCodeToBuy[10], int & index);

class functions;
class dataFile;
class stocksFile;
class configFile;

class functions{
    public:
        string exec(const char* cmd);
        bool checkSubString (string s1, string s2);
        void prettyPrint();
        //char * lowerString(char st[]);
        void clrscr();
        bool compareDir(string s);
        int nStocksRecords();
};

class dataFile: private functions{
    fstream fileStream;
    void createDataFile();
    public:
        dataFile();
        ~dataFile();
        void addDetails();
        void printTeamDetails();
        void printDetails(int n);
        void commandBuy();
        void commandSell();
};

class stocksFile: private functions{
    fstream fileStream;
    public:
        stocksFile();
        ~stocksFile();
        void addStocks();
        void createStocksDataFile();
        void printStockDetails();
        bool stockAlreadyBought(details team, char stockCodeToBuy[10], int & index);
        bool isValidStockCode (char stockCode[10]);
};

class configFile: private functions{
    fstream fileStream;
    void createConfig();
    public:
        configFile();
        void configFile::fetchInitBalance();
};

int main(){
    dataFile obj1;
    stocksFile obj2;
    configFile obj3;
    fileIn();
    return 0;
}

void dataFile::commandBuy(){
    fileStream.close();
    fileStream.open("data.dat", ios::in | ios::out | ios::binary);
    details team;
    int tNum, quantToBuy, numOfRecords = nRecords(), index; //index for stock index
    char stockCodeToBuy[10];
    bool bought = false;
    for (int i = 0; i < numOfRecords; ++i){
        fileStream.read ((char *) & team, sizeof(details));
        cout<<i + 1<<" --> "<<team.teamName<<endl;
    }
    cout<<"Enter team number: ";
    cin>>tNum;
    cin.ignore();
    if (tNum > 0 && tNum <= numOfRecords){
        fileStream.seekg((tNum - 1)*sizeof(details), ios::beg);
        fileStream.read ((char *) & team, sizeof(details));
        printDetails(tNum);
        cout<<"Enter stock code to buy: ";
        cin>>stockCodeToBuy;
        if (!isValidStockCode(stockCodeToBuy)){
            cout<<"Warning: "<<stockCodeToBuy<<" does not exist! Exiting..."<<endl;
             cout<<"Press any button"<<endl;
            cin.ignore();
            getchar();
            return;
        }
        cout<<"Enter quantity to buy: ";
        cin>>quantToBuy;
        if (quantToBuy < 0){
            cout<<"Warning: Quantity to buy can't be negative! Reversing last action and exiting"<<endl;
            cout<<"Press any button"<<endl;
            cin.ignore();
            getchar();
            return;
        }
        team.balance -= quantToBuy*findStockPrice(stockCodeToBuy);
        if (team.balance < 0){
            cout<<"Warning: Team balance negative! Reversing last action and exiting"<<endl;
            cout<<"Press any button"<<endl;
            cin.ignore();
            getchar();
            return;
        }
        bought = stockAlreadyBought(team, stockCodeToBuy, index);
        if (bought){
            team.stockQuant[index] += quantToBuy;
        }
        else{
            ++team.numOfStocks;
            strncpy(team.stockCodes[team.numOfStocks - 1], stockCodeToBuy, 10);
            team.stockQuant[team.numOfStocks - 1] = quantToBuy;
            bought = true;
        }
    }
    else{
        cout<<"Warning: Enter a valid team number!"<<endl;
        cout<<"Exiting"<<endl;
        cout<<"Press enter";
        getchar();
        return;
    }
    fileStream.seekp((tNum - 1)*sizeof(details), ios::beg);
    fileStream.write((char *) & team, sizeof(details));
    fileIn();
}

void dataFile::printDetails(int n){
    int numOfRecords;
    numOfRecords = nRecords();
    if (n < 0)
        return;
    if (n){
        fileStream.seekg((n - 1)*sizeof(details), ios::beg);
        printTeamDetails();
    }
    else{
        for (int i = 0; i < numOfRecords; ++i){
            //fileStream.seekg(i*sizeof(details), ios::beg);
            printTeamDetails();
        }
    }
}

void dataFile::printTeamDetails(){
    details team;
    fileStream.read ((char *) & team, sizeof(details));
    cout<<"Team name: "<<team.teamName<<endl;
    cout<<"Stocks: "<<endl;
    for (int i = 0; i < team.numOfStocks; ++i){
        cout<<'\t'<<team.stockCodes[i]<<" : "<<team.stockQuant[i];
        cout<<" : "<<team.stockQuant[i]*findStockPrice(team.stockCodes[i])<<endl;
    }
    cout<<"Balance: "<<team.balance<<endl;
    cout<<"Profit: "<<calcProfit(team)<<endl;
    prettyPrint();
}

void dataFile::addDetails(){
    details team;
    int n, numOfRecords = nStocksRecords(), index, tempnumOfStocks;
    char tName[100], stockCode[10]; //char * required for cin.getline()
    cout<<"How many team's details do you want to add? ";
    cin>>n;
    for (int i = 0; i < n; ++i){
        team.balance = initBalance;
        getchar();
        cout<<"Enter team name "<<i + 1<<": ";
        cin.getline(tName, 100);
        strncpy(team.teamName, tName, 100);
        cout<<"Enter number of stocks they have: ";
        cin>>tempnumOfStocks;
        team.numOfStocks = 0;
        if (tempnumOfStocks < 0){
            cout<<"Warning: Number of stocks can't be negative"<<endl;
            cout<<"Reversing last action..."<<endl;
            --i;
            continue;
        }
        if (tempnumOfStocks > numOfRecords){
            cout<<"Warning: You have added only "<<numOfRecords<<" stocks."<<endl;
            cout<<"Reversing last action..."<<endl;
            --i;
            continue;
        }
        for (int j = 0; j < tempnumOfStocks; ++j){
            getchar();
            cout<<"Enter code of stock "<<j + 1<<": ";
            cin.getline(stockCode, 10);
            if (!isValidStockCode(stockCode)){
                cout<<"Warning: "<<stockCode<<" does not exist! Reversing last action..."<<endl;
                cout<<"Press enter to continue"<<endl;
                --j;
                continue;
            }
            strncpy(team.stockCodes[j], stockCode, 10);
            cout<<"Enter quantity of "<<stockCode<<": ";
            cin>>team.stockQuant[j];
            if (team.stockQuant[j] < 0){
                cout<<"Warning: stock quantity can't be negative!"<<endl;
                cout<<"Reversing last action"<<endl;
                --j;
                continue;
            }
            team.balance -= team.stockQuant[j]*findStockPrice(team.stockCodes[j]);
            if (team.balance < 0){
                cout<<"Warning: Team balance negative! Reversing last action"<<endl;
                team.balance += team.stockQuant[j]*findStockPrice(team.stockCodes[j]);
                cout<<"Available team balance: "<<team.balance<<endl;
                --j;
                continue;
            }
            if (stockAlreadyBought(team, stockCode, index)){
                cout<<"This team has already bought "<<stockCode<<". Adding stock quantity to previous purchase."<<endl;
                team.stockQuant[index] += team.stockQuant[j];
                --j;
                //--tempnumOfStocks;
            }
            else
                ++team.numOfStocks;
        }
        fileStream.write((char *) &team, sizeof(details));
    }
}

void dataFile::createDataFile(){
    fileStream.close();
    fileStream.open("data.dat", ios::out | ios::binary);
    cout<<"Teams data file created as 'data.dat'.\n"
            <<"Do not delete it unless you're absolutely sure of what you're doing!\n"
            <<"Always keep a backup just in case."<<endl;
    addDetails();
}

void dataFile::commandSell(){
    fileStream.close();
    fileStream.open("data.dat", ios::in | ios::out | ios::binary);
    details team;
    int stockIndex, quantToSell;
    int tNum, numOfRecords = nRecords();
    for (int i = 0; i < numOfRecords; ++i){
        fileStream.read ((char *) & team, sizeof(details));
        cout<<i + 1<<" --> "<<team.teamName<<endl;
    }
    cout<<"Enter team number: ";
    cin>>tNum;
    cin.ignore();
    if (tNum > 0 && tNum <= numOfRecords){
        fileStream.seekg((tNum - 1)*sizeof(details), ios::beg);
        fileStream.read ((char *) & team, sizeof(details));
        printDetails(tNum);
        for (int i = 0; i < team.numOfStocks; ++i)
            cout<<i + 1<<" --> "<<team.stockCodes[i]<<endl;
        cout<<"Enter stock to sell: ";
        cin>>stockIndex; --stockIndex;
        cin.ignore();
        if (stockIndex < 0 || stockIndex >team.numOfStocks - 1){
            cout<<"Warning: Invalid stock number entered! Reversing last action and exiting"<<endl;
            cout<<"Press any button"<<endl;
            getchar();
            return;
        }
        cout<<"Enter quantity to sell: ";
        cin>>quantToSell;
        if (quantToSell < 0){
            cout<<"Warning: Quantity to sell can't be negative! Reversing last action and exiting"<<endl;
            cout<<"Press any button"<<endl;
            cin.ignore();
            getchar();
            return;
        }
        if (team.stockQuant[stockIndex] >= quantToSell){
            team.balance += quantToSell*findStockPrice(team.stockCodes[stockIndex]);
            team.stockQuant[stockIndex] -= quantToSell;
            fileStream.seekp((tNum - 1)*sizeof(details), ios::beg);
            fileStream.write ((char *) & team, sizeof(team));
        }
        else{
            cout<<"Warning: Trying to sell more shares than they own!"<<endl;
            //getchar();
        }
    }
    else{
        cout<<"Enter a valid team number!"<<endl;
        cout<<"Reversing last action"<<endl;
    }
    fileIn();
}

dataFile::dataFile(){
    if (!compareDir("data.dat"))
        createDataFile();
}

dataFile::~dataFile(){
    fileStream.close();
}

stocksFile::stocksFile(){
        if (compareDir("stocks.dat"))
            createStocksDataFile();
}

stocksFile::~stocksFile(){
    fileStream.close();
}

bool stocksFile::isValidStockCode (char stockCode[10]){
    fileStream.close();
    fileStream.open("stocks.dat", ios::in | ios::binary);
    stocks stock;
    int numOfRecords = nStocksRecords();
    for (int i  = 0; i < numOfRecords; ++i){
        fileStream.read((char *) & stock, sizeof(stocks));
        if (!strcmp(stock.stockCode, stockCode))
            return true;
    }
    return false;
}

int functions::nStocksRecords(){
    ifstream tempstocksFileStream;
    tempstocksFileStream.open("stocks.dat", ios::in | ios::binary);
    int beg, cur, numOfRecords;
    cur = tempstocksFileStream.tellg();
    tempstocksFileStream.seekg(0, ios::beg);
    beg = tempstocksFileStream.tellg();
    tempstocksFileStream.seekg(0, ios::end);
    numOfRecords = ((int) tempstocksFileStream.tellg() - beg)/sizeof(stocks);
    tempstocksFileStream.seekg(cur, ios::beg);
    tempstocksFileStream.close();
    return numOfRecords;
}

bool functions::stockAlreadyBought(details team, char stockCodeToBuy[10], int & index){
    for (int i = 0; i < team.numOfStocks; ++i)
        if (!strcmp(team.stockCodes[i], stockCodeToBuy)){
            index = i;
            return true;
        }
    return false;
}

void stocksFile::addStocks(){
    stocks stock;
    int n;
    cout<<"How many stocks would you like to add: ";
    cin>>n;
    for (int i = 0; i < n; ++i){
        cin.ignore();
        cout<<"Enter stock name "<<i + 1<<": ";
        cin.getline(stock.stockName, 50);
        cout<<"Enter stock code: ";
        cin>>stock.stockCode;
        cout<<"Enter stock price: ";
        cin>>stock.stockPrice;
        if (stock.stockPrice < 0){
            cout<<"Warning: Stock price can't be negative!"<<endl;
            cout<<"Reversing last action"<<endl;
            --i;
            continue;
        }
        fileStream.write ((char *) & stock, sizeof(stocks));
    }
    fileStream.close(); //Complete all write operations
}

void stocksFile::printStockDetails(int n){
    int numOfRecords;
    numOfRecords = nStocksRecords();
    if (n < 0)
        return;
    if (n){
        fileStream.seekg((n - 1)*sizeof(stocks), ios::beg);
        printIndStockDetails();
    }
    else{
        for (int i = 0; i < numOfRecords; ++i){
            //stocksFileStream.seekg(i*sizeof(stocks), ios::beg);
            printIndStockDetails();
        }
    }
}

void configFile::createConfig(){
    int bal;
    fileStream.open ("config.ini", ios::out);
    cout<<"Enter 0 to use default initial team balance of: "<<initBalance<<endl;
    cout<<"Enter initial team balance: ";
    cin>>bal;
    if (!bal){
        cout<<"Using default team balance of: "<<initBalance<<endl;
    }
    else{
        fileStream<<bal;
        cout<<"Team balance set in config.ini"<<endl;
        cout<<"Do not delete it unless you're absolutely sure of what you're doing!"<<endl;
        cout<<"To change it edit the config.ini file before running this program"<<endl;
        cout<<"Leave it blank to use default initial team balance"<<endl;
        initBalance = bal;
    }
}

configFile::configFile(){
    if (compareDir("config.ini"))
        fetchInitBalance();
    else{
        createConfig();
    }
}

void configFile::fetchInitBalance(){
    int bal;
    fileStream.open("config.ini", ios::in);
    fileStream>>bal;
    if (bal){
        cout<<"Found initial team balance in config.ini: "<<bal<<endl;
        initBalance = bal;
    }
    else
        cout<<"Using default initial team balance of: "<<initBalance<<endl;
}

bool functions::compareDir(string s){
    string dir = exec("dir");
    return checkSubString(dir, s);
}

void fileIn(){
    char command[20];
    int decision;
    cout<<"1 --> to view all details"<<endl;
    cout<<"2 --> to view details of a specific team"<<endl;
    cout<<"3 --> to add details"<<endl;
    cout<<"4 --> to modify stock prices"<<endl;
    //cout<<"5 --> to delete details"<<endl;
    cout<<"5 --> to buy shares"<<endl;
    cout<<"6 --> to sell shares"<<endl;
    cout<<"7 --> to clear the screen"<<endl;
    cout<<"8 --> to close the program"<<endl;
    //getchar();
    //cin.getline(command, 20);
    //getDecision (decision);
    cin>>decision;
    cin.ignore();
    if (cin.fail()){
        cout<<"Enter a number! exiting....."<<endl;
        getch();
    }
    else if (decision == 1)
        commandAll();
    else if (decision == 2)
        commandShow();
    else if (decision == 3)
        commandAdd();
    else if (decision == 4)
        commandModify();
    /*else if (decision == 5)
        commandDelete();*/
    else if (decision == 5)
        commandBuy();
    else if (decision == 6)
        commandSell();
    else if (decision == 7)
        clrscr();
    else if (decision == 8)
        ;
    else{
        cout<<"Enter a valid command!"<<endl;
        fileIn();
    }
}

void printIndStockDetails(){
    stocks stock;
    stocksFileStream.read ((char *) & stock, sizeof(stocks));
    cout<<"Stock name: "<<stock.stockName<<endl;
    cout<<"Stock code: "<<stock.stockCode<<endl;
    cout<<"Stock price: "<<stock.stockPrice<<endl;
    prettyPrint();
}

int calcProfit(details team){
    int stocksWorth = 0, netWorth, profit;
    for (int i = 0; i < team.numOfStocks; ++i)
        stocksWorth += team.stockQuant[i]*findStockPrice(team.stockCodes[i]);
    netWorth = stocksWorth + team.balance;
    profit = netWorth - initBalance;
    return profit;
}

int findStockPrice(char stockCode[10]){
    stocks stock;
    stocksFileStream.close();
    stocksFileStream.open("stocks.dat", ios::in | ios::binary);
    stocksFileMode = "in";
    bool found = false;
    while (stocksFileStream.read((char *) & stock, sizeof(stocks))){
        if (!strcmp(stock.stockCode, stockCode)){
            found = true;
            break;
        }
    }
    if (!found)
        cout<<"Invalid stock code: "<<stockCode<<endl;
    return stock.stockPrice;
}

/*char * functions::lowerString(char st[]){
    int i;
    for (i = 0; st[i] != '\0'; ++i)
        string[i] = tolower(st[i]);
    string[i] = '\0';
    return string;
}
*/

int nRecords(){
    int beg, cur, numOfRecords;
    cur = fileStream.tellg();
    fileStream.seekg(0, ios::beg);
    beg = fileStream.tellg();
    fileStream.seekg(0, ios::end);
    numOfRecords = ((int) fileStream.tellg() - beg)/sizeof(details);
    fileStream.seekg(cur, ios::beg);
    return numOfRecords;
}

void commandAll(){
    int decision;
    cout<<"1 --> All Teams"<<endl;
    cout<<"2 --> All Stocks"<<endl;
    cin>>decision;
    cin.ignore();
    if (decision == 1){
        fileStream.close();
        fileStream.open("data.dat", ios::in | ios::binary);
        fileMode = "in";
        cout<<"Number of records: "<<nRecords()<<endl;
        prettyPrint();
        printDetails(0);
    }
    else if (decision == 2){
        stocksFileStream.close();
        stocksFileStream.open("stocks.dat", ios::in | ios::binary);
        stocksFileMode = "in";
        cout<<"Number of records: "<<nStocksRecords()<<endl;
        prettyPrint();
        printStockDetails(0);
    }
    fileIn();
}

void commandShow(){
    fileStream.close();
    fileStream.open("data.dat", ios::in | ios::binary);
    fileMode = "in";
    details team;
    int tNum, numOfRecords = nRecords();
    for (int i = 0; i < numOfRecords; ++i){
        fileStream.read ((char *) & team, sizeof(details));
        cout<<i + 1<<" --> "<<team.teamName<<endl;
    }
    cout<<"Enter team number to show: ";
    cin>>tNum;
    cin.ignore();
    if (tNum > 0 && tNum <= numOfRecords)
        printDetails(tNum);
    else
        cout<<"Enter a valid team number!"<<endl;
    fileIn();
}

void commandAdd(){
    int decision;
    cout<<"1 --> Add Teams"<<endl;
    cout<<"2 --> Add Stocks"<<endl;
    cin>>decision;
    cin.ignore();
    if (decision == 1){
        fileStream.close();
        fileStream.open("data.dat", ios::app | ios::binary);
        fileMode = "app";
        addDetails();
        }
    else if (decision == 2){
        stocksFileStream.close();
        stocksFileStream.open("stocks.dat", ios::app | ios::binary);
        stocksFileMode = "app";
        addStocks();
    }
    else
        cout<<"Enter valid input!"<<endl;
    fileIn();
}

void commandModify(){
    /*int decision;
    cout<<"1 --> Modify Teams"<<endl;
    cout<<"2 --> Modify stocks price"<<endl;
    cin>>decision;
    cin.ignore();
    if (decision == 1){
        fileStream.close();
        fileStream.open("data.dat", ios::in | ios::out| ios::binary);
        fileMode = "inout";
        details team;
        int tNum, numOfRecords = nRecords();
        for (int i = 0; i < numOfRecords; ++i){
            fileStream.read ((char *) & team, sizeof(details));
            cout<<i + 1<<" --> "<<team.teamName<<endl;
        }
        cout<<"Enter team number to modify: ";
        cin>>tNum;
        cin.ignore();
        stocksFileStream.seekg((tNum - 1)*sizeof(stocks), ios::beg);
        stocksFileStream.read ((char *) & team, sizeof(stocks));
        stocksFileStream.seekp((tNum - 1)*sizeof(stocks), ios::beg);
    }
    */
   int decision;
   cout<<"1 --> Modify all stock prices"<<endl;
   cout<<"2 --> Modify single stock prices"<<endl;
   cin>>decision;
   stocksFileStream.close();
   stocksFileStream.open("stocks.dat", ios::in | ios::out | ios::binary);
   stocksFileMode = "inout";
   stocks stock;
   int sNum, numOfRecords = nStocksRecords();
   if (decision == 1){
       for (int i = 0; i < numOfRecords; ++i){
           sNum = i +1;
           stocksFileStream.seekg((sNum - 1)*sizeof(stocks), ios::beg);
            stocksFileStream.read ((char *) & stock, sizeof(stocks));
            //cout<<i + 1<<" --> "<<stock.stockName<<endl;
            stocksFileStream.seekp((sNum - 1)*sizeof(stocks), ios::beg);
            cout<<"Old price for "<<stock.stockName<<" : "<<stock.stockPrice<<endl;
            cout<<"Enter new price for "<<stock.stockName<<" : ";
            cin>>stock.stockPrice;
            cin.ignore();
             if (stock.stockPrice < 0){
                cout<<"Warning: Stock price can't be negative!"<<endl;
                cout<<"Reversing last action"<<endl;
                --i;
                continue;
            }
            stocksFileStream.write ((char *) & stock, sizeof(stocks));
        }
   }
   else if (decision == 2){
        for (int i = 0; i < numOfRecords; ++i){
            stocksFileStream.read ((char *) & stock, sizeof(stocks));
            cout<<i + 1<<" --> "<<stock.stockName<<endl;
        }
        cout<<"Enter stock number to modify: ";
        cin>>sNum;
        cin.ignore();
        if (sNum < 0 || sNum > numOfRecords){
            cout<<"Warning: Wrong stock number!"<<endl;
            cout<<"Exiting"<<endl;
            cout<<"Press enter";
            getchar();
            return;
        }
        stocksFileStream.seekg((sNum - 1)*sizeof(stocks), ios::beg);
        stocksFileStream.read ((char *) & stock, sizeof(stocks));
        stocksFileStream.seekp((sNum - 1)*sizeof(stocks), ios::beg);
        cout<<"Old price for "<<stock.stockName<<" : "<<stock.stockPrice<<endl;
        cout<<"Enter new price for "<<stock.stockName<<" : ";
        cin>>stock.stockPrice;
        cin.ignore();
        if (stock.stockPrice < 0){
            cout<<"Warning: Stock price can't be negative!"<<endl;
            cout<<"Reversing last action and exiting"<<endl;
            cout<<"Press enter";
            getchar();
            return;
        }
        stocksFileStream.write ((char *) & stock, sizeof(stocks));
   }
    fileIn();
}

void getDecision (int & decision){
    cin>>decision;
    if (cin.fail())
        decision = 99; //invalid input
    //cin.ignore(numeric_limits<streamsize>::max());
    //cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string functions::exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool functions::checkSubString (string s1, string s2){
    if (s1.find(s2) != string::npos)
        return true;
    else
        return false;    
}

void stocksFile::createStocksDataFile(){
    stocks stock;
    fileStream.open ("stocks.dat", ios::out);
    cout<<"Stocks data file created as 'stocks.dat'.\n"
            <<"Do not delete it unless you're absolutely sure of what you're doing!\n"
            <<"Always keep a backup just in case."<<endl;
    addStocks();
}

void functions::clrscr(){
    system("cls");
    fileIn();
}

void functions::prettyPrint(){
    cout<<"............................................"<<endl;
}