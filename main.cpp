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
        int findStockPrice(char stockCode[10]);
        bool isValidStockCode (char stockCode[10]);
        bool stockAlreadyBought(details team, char stockCodeToBuy[10], int & index);
        int calcProfit(details team);
        void run(dataFile obj1, stocksFile obj2);
};

class dataFile: private functions{
    fstream fileStream;
    void createDataFile();
    public:
        dataFile();
        dataFile(const dataFile & obj);
        ~dataFile();
        void addDetails();
        void printTeamDetails();
        void printDetails(int n);
        void commandBuy();
        void commandSell();
        int nRecords();
        void commandShow();
        friend void commandAdd(dataFile obj1, stocksFile obj2);
        friend void commandAll(dataFile obj1, stocksFile obj2);
};

class stocksFile: private functions{
    fstream fileStream;
    public:
        stocksFile();
        ~stocksFile();
        stocksFile(const stocksFile & obj);
        void addStocks();
        void createStocksDataFile();
        void printStockDetails(int n);
        void printIndStockDetails();
        void commandModify();
        friend void commandAdd(dataFile obj1, stocksFile obj2);
        friend void commandAll(dataFile obj1, stocksFile obj2);
};

class configFile: private functions{
    fstream fileStream;
    void createConfig();
    public:
        configFile();
        ~configFile();
        void fetchInitBalance();
};

int main(){
    functions obj4;
    configFile obj3;
    stocksFile obj2;
    dataFile obj1;
    obj4.run(obj1,obj2);
    return 0;
}

dataFile::dataFile(const dataFile &obj){
    ;
}

int dataFile::nRecords(){
    int beg, cur, numOfRecords;
    cur = fileStream.tellg();
    fileStream.seekg(0, ios::beg);
    beg = fileStream.tellg();
    fileStream.seekg(0, ios::end);
    numOfRecords = ((int) fileStream.tellg() - beg)/sizeof(details);
    fileStream.seekg(cur, ios::beg);
    return numOfRecords;
}

void dataFile::commandShow(){
    fileStream.close();
    fileStream.open("data.dat", ios::in | ios::binary);
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
}

dataFile::dataFile(){
    if (!compareDir("data.dat"))
        createDataFile();
}

dataFile::~dataFile(){
    fileStream.close();
}

stocksFile::stocksFile(){
        if (!compareDir("stocks.dat"))
            createStocksDataFile();
}

stocksFile::stocksFile(const stocksFile & obj){
    ;
}

stocksFile::~stocksFile(){
    fileStream.close();
}

void stocksFile::commandModify(){
   int decision;
   cout<<"1 --> Modify all stock prices"<<endl;
   cout<<"2 --> Modify single stock prices"<<endl;
   cin>>decision;
   fileStream.close();
   fileStream.open("stocks.dat", ios::in | ios::out | ios::binary);
   stocks stock;
   int sNum, numOfRecords = nStocksRecords();
   if (decision == 1){
       for (int i = 0; i < numOfRecords; ++i){
            sNum = i +1;
            fileStream.seekg((sNum - 1)*sizeof(stocks), ios::beg);
            fileStream.read ((char *) & stock, sizeof(stocks));
            //cout<<i + 1<<" --> "<<stock.stockName<<endl;
            fileStream.seekp((sNum - 1)*sizeof(stocks), ios::beg);
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
            fileStream.write ((char *) & stock, sizeof(stocks));
        }
   }
   else if (decision == 2){
        for (int i = 0; i < numOfRecords; ++i){
            fileStream.read ((char *) & stock, sizeof(stocks));
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
        fileStream.seekg((sNum - 1)*sizeof(stocks), ios::beg);
        fileStream.read ((char *) & stock, sizeof(stocks));
        fileStream.seekp((sNum - 1)*sizeof(stocks), ios::beg);
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
        fileStream.write ((char *) & stock, sizeof(stocks));
   }
   fileStream.close();
}

void stocksFile::printIndStockDetails(){
    stocks stock;
    fileStream.read ((char *) & stock, sizeof(stocks));
    cout<<"Stock name: "<<stock.stockName<<endl;
    cout<<"Stock code: "<<stock.stockCode<<endl;
    cout<<"Stock price: "<<stock.stockPrice<<endl;
    prettyPrint();
}
int functions::findStockPrice(char stockCode[10]){
    stocks stock;
    ifstream tempStocksFileStream;
    tempStocksFileStream.open("stocks.dat", ios::in | ios::binary);
    bool found = false;
    while (tempStocksFileStream.read((char *) & stock, sizeof(stocks))){
        if (!strcmp(stock.stockCode, stockCode)){
            found = true;
            break;
        }
    }
    if (!found)
        cout<<"Invalid stock code: "<<stockCode<<endl;
    tempStocksFileStream.close();
    return stock.stockPrice;
}

int functions::nStocksRecords(){
    ifstream tempStocksFileStream;
    tempStocksFileStream.open("stocks.dat", ios::in | ios::binary);
    int beg, cur, numOfRecords;
    cur = tempStocksFileStream.tellg();
    tempStocksFileStream.seekg(0, ios::beg);
    beg = tempStocksFileStream.tellg();
    tempStocksFileStream.seekg(0, ios::end);
    numOfRecords = ((int) tempStocksFileStream.tellg() - beg)/sizeof(stocks);
    tempStocksFileStream.seekg(cur, ios::beg);
    tempStocksFileStream.close();
    return numOfRecords;
}

bool functions::isValidStockCode (char stockCode[10]){
    ifstream tempStocksFileStream;
    tempStocksFileStream.open("stocks.dat", ios::in | ios::binary);
    stocks stock;
    int numOfRecords = nStocksRecords();
    for (int i  = 0; i < numOfRecords; ++i){
        tempStocksFileStream.read((char *) & stock, sizeof(stocks));
        if (!strcmp(stock.stockCode, stockCode))
            return true;
    }
    return false;
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

configFile::~configFile(){
    fileStream.close();
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

int functions::calcProfit(details team){
    int stocksWorth = 0, netWorth, profit;
    for (int i = 0; i < team.numOfStocks; ++i)
        stocksWorth += team.stockQuant[i]*findStockPrice(team.stockCodes[i]);
    netWorth = stocksWorth + team.balance;
    profit = netWorth - initBalance;
    return profit;
}

/*char * functions::lowerString(char st[]){
    int i;
    for (i = 0; st[i] != '\0'; ++i)
        string[i] = tolower(st[i]);
    string[i] = '\0';
    return string;
}
*/

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
}

void functions::prettyPrint(){
    cout<<"............................................"<<endl;
}

void commandAll(dataFile obj1, stocksFile obj2){
    int decision;
    cout<<"1 --> All Teams"<<endl;
    cout<<"2 --> All Stocks"<<endl;
    cin>>decision;
    cin.ignore();
    if (decision == 1){
        obj1.fileStream.close();
        obj1.fileStream.open("data.dat", ios::in | ios::binary);
        cout<<"Number of records: "<<obj1.nRecords()<<endl;
        obj1.prettyPrint();
        obj1.printDetails(0);
    }
    else if (decision == 2){
        obj2.fileStream.close();
        obj2.fileStream.open("stocks.dat", ios::in | ios::binary);
        cout<<"Number of records: "<<obj2.nStocksRecords()<<endl;
        obj2.prettyPrint();
        obj2.printStockDetails(0);
    }
}

void commandAdd(dataFile obj1, stocksFile obj2){
    int decision;
    cout<<"1 --> Add Teams"<<endl;
    cout<<"2 --> Add Stocks"<<endl;
    cin>>decision;
    cin.ignore();
    if (decision == 1){
        obj1.fileStream.close();
        obj1.fileStream.open("data.dat", ios::app | ios::binary);
        obj1.addDetails();
        }
    else if (decision == 2){
        obj2.fileStream.close();
        obj2.fileStream.open("stocks.dat", ios::app | ios::binary);
        obj2.addStocks();
    }
    else
        cout<<"Enter valid input!"<<endl;
}

void functions::run(dataFile obj1, stocksFile obj2){
    char command[20];
    int decision;
    char ch = 'y';
    do{
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
        commandAll(obj1, obj2);
    else if (decision == 2)
        obj1.commandShow();
    else if (decision == 3)
        commandAdd(obj1, obj2);
    else if (decision == 4)
        obj2.commandModify();
    /*else if (decision == 5)
        commandDelete();*/
    else if (decision == 5)
        obj1.commandBuy();
    else if (decision == 6)
        obj1.commandSell();
    else if (decision == 7)
        clrscr();
    else if (decision == 8)
        break;
    else{
        cout<<"Enter a valid command!"<<endl;
    }
    }
    while (1);
}