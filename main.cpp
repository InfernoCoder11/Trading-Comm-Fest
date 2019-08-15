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

std::fstream fileStream;
std::fstream stocksFileStream;
std::string fileMode;
std::string stocksFileMode;
const long int initBalance = 10000;
//char string[100];

std::string exec(const char* cmd);
bool checkSubString (std::string s1, std::string s2);
void fileOut();
void fileApp();
void fileIn();
void addDetails();
void printDetails(int n);
int nRecords ();
void addStocks();
void createStocksDataFile();
void createDataFile();
void commandAll();
void commandShow();
void commandAdd();
void prettyPrint();
void printTeamDetails();
int nStocksRecords();
void printStocksDetails();
void printIndStockDetails();
int findStockPrice(char stockCode[10]);
char * lowerString(char st[]);
void commandModify();
void commandDelete();
void getDecision (int &);
void commandBuy();
void commandSell();
bool isValidStockCode (char stockCode[10]);

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

int main(){
    std::string dir;
    dir = exec("dir");
    if (!checkSubString(dir, "stocks.dat"))
        createStocksDataFile();
    if (!checkSubString(dir, "data.dat"))
        createDataFile();
    fileIn();
    fileStream.close();
    stocksFileStream.close();
    //getch();
    return 0;
}

void addStocks(){
    stocks stock;
    int n;
    std::cout<<"How many stocks would you like to add: ";
    std::cin>>n;
    for (int i = 0; i < n; ++i){
        std::cin.ignore();
        std::cout<<"Enter stock name: ";
        std::cin.getline(stock.stockName, 50);
        std::cout<<"Enter stock code: ";
        std::cin>>stock.stockCode;
        std::cout<<"Enter stock price: ";
        std::cin>>stock.stockPrice;
        stocksFileStream.write ((char *) & stock, sizeof(stocks));
    }
}

void addDetails(){
    details team;
    int n;
    char tName[100], stockCode[10]; //char * required for cin.getline()
    std::cout<<"How many team's details do you want to add? ";
    std::cin>>n;
    for (int i = 0; i < n; ++i){
        team.balance = initBalance;
        std::getchar();
        std::cout<<"Enter team name: ";
        std::cin.getline(tName, 100);
        strncpy(team.teamName, tName, 100);
        std::cout<<"Enter number of stocks they have: ";
        std::cin>>team.numOfStocks;
        for (int j = 0; j < team.numOfStocks; ++j){
            std::getchar();
            std::cout<<"Enter code of stock "<<j + 1<<": ";
            std::cin.getline(stockCode, 10);
            strncpy(team.stockCodes[j], stockCode, 10);
            std::cout<<"Enter quantity of "<<stockCode<<": ";
            std::cin>>team.stockQuant[j];
            team.balance -= team.stockQuant[j]*findStockPrice(team.stockCodes[j]);
            if (team.balance < 0){
                std::cout<<"Warning: Team balance negative! Reversing last action"<<std::endl;
                team.balance += team.stockQuant[j]*findStockPrice(team.stockCodes[j]);
                --j;
            }
        }
        fileStream.write((char *) &team, sizeof(details));
    }
}

void fileIn(){
    char command[20];
    int decision;
    std::cout<<"1 --> to view all details"<<std::endl;
    std::cout<<"2 --> to view details of a specific team"<<std::endl;
    std::cout<<"3 --> to add details"<<std::endl;
    std::cout<<"4 --> to modify stock prices"<<std::endl;
    std::cout<<"5 --> to delete details"<<std::endl;
    std::cout<<"6 --> to buy shares"<<std::endl;
    std::cout<<"7 --> to sell shares"<<std::endl;
    std::cout<<"8 --> to close the program"<<std::endl;
    //std::getchar();
    //std::cin.getline(command, 20);
    //getDecision (decision);
    std::cin>>decision;
    std::cin.ignore();
    if (std::cin.fail()){
        std::cout<<"Enter a number! exiting....."<<std::endl;
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
    else if (decision == 5)
        commandDelete();
    else if (decision == 6)
        commandBuy();
    else if (decision == 7)
        commandSell();
    else if (decision == 8)
        ;
    else{
        std::cout<<"Enter a valid command!"<<std::endl;
        fileIn();
    }
}

void commandBuy(){
    fileStream.close();
    fileStream.open("data.dat", std::ios::in | std::ios::out | std::ios::binary);
    fileMode = "inout";
    details team;
    int tNum, quantToBuy, numOfRecords = nRecords();
    char stockCodeToBuy[10];
    bool bought = false;
    for (int i = 0; i < numOfRecords; ++i){
        fileStream.read ((char *) & team, sizeof(details));
        std::cout<<i + 1<<" --> "<<team.teamName<<std::endl;
    }
    std::cout<<"Enter team number: ";
    std::cin>>tNum;
    std::cin.ignore();
    if (tNum > 0){
        fileStream.seekg((tNum - 1)*sizeof(details), std::ios::beg);
        fileStream.read ((char *) & team, sizeof(details));
        printDetails(tNum);
        std::cout<<"Enter stock code to buy: ";
        std::cin>>stockCodeToBuy;
        if (!isValidStockCode(stockCodeToBuy)){
            std::cout<<"Invalid stock code!"<<std::endl;
            return;
        }
        std::cout<<"Enter quantity to buy: ";
        std::cin>>quantToBuy;
        team.balance -= quantToBuy*findStockPrice(stockCodeToBuy);
        if (team.balance < 0){
            std::cout<<"Warning: Team balance negative! Reversing last action and exiting"<<std::endl;
            return;
        }
        for (int i = 0; i < team.numOfStocks; ++i)
            if (!strcmp(team.stockCodes[i], stockCodeToBuy)){
                team.stockQuant[i] += quantToBuy;
                bought = true;
            }
        if (!bought){
            ++team.numOfStocks;
            strncpy(team.stockCodes[team.numOfStocks - 1], stockCodeToBuy, 10);
            team.stockQuant[team.numOfStocks - 1] = quantToBuy;
            bought = true;
        }
    }
    else
        std::cout<<"Enter a valid team number!"<<std::endl;
    fileStream.seekp((tNum - 1)*sizeof(details), std::ios::beg);
    fileStream.write((char *) & team, sizeof(details));
    fileIn();
}

bool isValidStockCode (char stockCode[10]){
    stocksFileStream.close();
    stocksFileStream.open("stocks.dat", std::ios::in | std::ios::binary);
    stocksFileMode = "in";
    stocks stock;
    int numOfRecords = nStocksRecords();
    for (int i  = 0; i < numOfRecords; ++i){
        stocksFileStream.read((char *) & stock, sizeof(stocks));
        if (!strcmp(stock.stockCode, stockCode))
            return true;
    }
    return false;
}

void commandSell(){
    fileStream.close();
    fileStream.open("data.dat", std::ios::in | std::ios::out | std::ios::binary);
    fileMode = "inout";
    details team;
    int stockIndex, quantToSell;
    int tNum, numOfRecords = nRecords();
    for (int i = 0; i < numOfRecords; ++i){
        fileStream.read ((char *) & team, sizeof(details));
        std::cout<<i + 1<<" --> "<<team.teamName<<std::endl;
    }
    std::cout<<"Enter team number: ";
    std::cin>>tNum;
    std::cin.ignore();
    if (tNum > 0){
        fileStream.seekg((tNum - 1)*sizeof(details), std::ios::beg);
        fileStream.read ((char *) & team, sizeof(details));
        printDetails(tNum);
        for (int i = 0; i < team.numOfStocks; ++i)
            std::cout<<i + 1<<" --> "<<team.stockCodes[i]<<std::endl;
        std::cout<<"Enter stock to sell: ";
        std::cin>>stockIndex; --stockIndex;
        std::cin.ignore();
        std::cout<<"Enter quantity to sell: ";
        std::cin>>quantToSell;
        if (team.stockQuant[stockIndex] >= quantToSell){
            team.balance += quantToSell*findStockPrice(team.stockCodes[stockIndex]);
            team.stockQuant[stockIndex] -= quantToSell;
            fileStream.seekp((tNum - 1)*sizeof(details), std::ios::beg);
            fileStream.write ((char *) & team, sizeof(team));
        }
        else
            std::cout<<"Warning: Trying to sell more shares than they own!"<<std::endl;
    }
    else
        std::cout<<"Enter a valid team number!"<<std::endl;
    fileIn();
}

void printDetails(int n){
    int numOfRecords;
    numOfRecords = nRecords();
    if (n < 0)
        return;
    if (n){
        fileStream.seekg((n - 1)*sizeof(details), std::ios::beg);
        printTeamDetails();
    }
    else{
        for (int i = 0; i < numOfRecords; ++i){
            //fileStream.seekg(i*sizeof(details), std::ios::beg);
            printTeamDetails();
        }
    }
}

void printStockDetails(int n){
    int numOfRecords;
    numOfRecords = nStocksRecords();
    if (n < 0)
        return;
    if (n){
        stocksFileStream.seekg((n - 1)*sizeof(stocks), std::ios::beg);
        printIndStockDetails();
    }
    else{
        for (int i = 0; i < numOfRecords; ++i){
            //stocksFileStream.seekg(i*sizeof(stocks), std::ios::beg);
            printIndStockDetails();
        }
    }
}

void printIndStockDetails(){
    stocks stock;
    stocksFileStream.read ((char *) & stock, sizeof(stocks));
    std::cout<<"Stock name: "<<stock.stockName<<std::endl;
    std::cout<<"Stock code: "<<stock.stockCode<<std::endl;
    std::cout<<"Stock price: "<<stock.stockPrice<<std::endl;
    prettyPrint();
}

void printTeamDetails(){
    details team;
    fileStream.read ((char *) & team, sizeof(details));
    std::cout<<"Team name: "<<team.teamName<<std::endl;
    std::cout<<"Stocks: "<<std::endl;
    for (int i = 0; i < team.numOfStocks; ++i){
        std::cout<<'\t'<<team.stockCodes[i]<<" : "<<team.stockQuant[i];
        std::cout<<" : "<<team.stockQuant[i]*findStockPrice(team.stockCodes[i])<<std::endl;
    }
    std::cout<<"Balance: "<<team.balance<<std::endl;
    prettyPrint();
}

int findStockPrice(char stockCode[10]){
    stocks stock;
    stocksFileStream.close();
    stocksFileStream.open("stocks.dat", std::ios::in | std::ios::binary);
    stocksFileMode = "in";
    bool found = false;
    while (stocksFileStream.read((char *) & stock, sizeof(stocks))){
        if (!strcmp(stock.stockCode, stockCode)){
            found = true;
            break;
        }
    }
    if (!found)
        std::cout<<"Invalid stock code: "<<stockCode<<std::endl;
    return stock.stockPrice;
}

/*char * lowerString(char st[]){
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
    fileStream.seekg(0, std::ios::beg);
    beg = fileStream.tellg();
    fileStream.seekg(0, std::ios::end);
    numOfRecords = ((int) fileStream.tellg() - beg)/sizeof(details);
    fileStream.seekg(cur, std::ios::beg);
    return numOfRecords;
}

int nStocksRecords(){
    int beg, cur, numOfRecords;
    cur = stocksFileStream.tellg();
    stocksFileStream.seekg(0, std::ios::beg);
    beg = stocksFileStream.tellg();
    stocksFileStream.seekg(0, std::ios::end);
    numOfRecords = ((int) stocksFileStream.tellg() - beg)/sizeof(stocks);
    stocksFileStream.seekg(cur, std::ios::beg);
    return numOfRecords;
}

void commandAll(){
    int decision;
    std::cout<<"1 --> All Teams"<<std::endl;
    std::cout<<"2 --> All Stocks"<<std::endl;
    std::cin>>decision;
    std::cin.ignore();
    if (decision == 1){
        fileStream.close();
        fileStream.open("data.dat", std::ios::in | std::ios::binary);
        fileMode = "in";
        std::cout<<"Number of records: "<<nRecords()<<std::endl;
        prettyPrint();
        printDetails(0);
    }
    else if (decision == 2){
        stocksFileStream.close();
        stocksFileStream.open("stocks.dat", std::ios::in | std::ios::binary);
        stocksFileMode = "in";
        std::cout<<"Number of records: "<<nStocksRecords()<<std::endl;
        prettyPrint();
        printStockDetails(0);
    }
    fileIn();
}

void commandShow(){
    fileStream.close();
    fileStream.open("data.dat", std::ios::in | std::ios::binary);
    fileMode = "in";
    details team;
    int tNum, numOfRecords = nRecords();
    for (int i = 0; i < numOfRecords; ++i){
        fileStream.read ((char *) & team, sizeof(details));
        std::cout<<i + 1<<" --> "<<team.teamName<<std::endl;
    }
    std::cout<<"Enter team number to show: ";
    std::cin>>tNum;
    std::cin.ignore();
    if (tNum > 0)
        printDetails(tNum);
    else
        std::cout<<"Enter a valid team number!"<<std::endl;
    fileIn();
}

void commandAdd(){
    int decision;
    std::cout<<"1 --> Add Teams"<<std::endl;
    std::cout<<"2 --> Add Stocks"<<std::endl;
    std::cin>>decision;
    std::cin.ignore();
    if (decision == 1){
        fileStream.close();
        fileStream.open("data.dat", std::ios::app | std::ios::binary);
        fileMode = "app";
        addDetails();
        }
    else if (decision == 2){
        stocksFileStream.close();
        stocksFileStream.open("stocks.dat", std::ios::app | std::ios::binary);
        stocksFileMode = "app";
        addStocks();
    }
    else
        std::cout<<"Enter valid input!"<<std::endl;
    fileIn();
}

void commandModify(){
    /*int decision;
    std::cout<<"1 --> Modify Teams"<<std::endl;
    std::cout<<"2 --> Modify stocks price"<<std::endl;
    std::cin>>decision;
    std::cin.ignore();
    if (decision == 1){
        fileStream.close();
        fileStream.open("data.dat", std::ios::in | std::ios::out| std::ios::binary);
        fileMode = "inout";
        details team;
        int tNum, numOfRecords = nRecords();
        for (int i = 0; i < numOfRecords; ++i){
            fileStream.read ((char *) & team, sizeof(details));
            std::cout<<i + 1<<" --> "<<team.teamName<<std::endl;
        }
        std::cout<<"Enter team number to modify: ";
        std::cin>>tNum;
        std::cin.ignore();
        stocksFileStream.seekg((tNum - 1)*sizeof(stocks), std::ios::beg);
        stocksFileStream.read ((char *) & team, sizeof(stocks));
        stocksFileStream.seekp((tNum - 1)*sizeof(stocks), std::ios::beg);
    }
    */
    stocksFileStream.close();
    stocksFileStream.open("stocks.dat", std::ios::in | std::ios::out | std::ios::binary);
    stocksFileMode = "inout";
    stocks stock;
    int sNum, numOfRecords = nStocksRecords();
    for (int i = 0; i < numOfRecords; ++i){
        stocksFileStream.read ((char *) & stock, sizeof(stocks));
        std::cout<<i + 1<<" --> "<<stock.stockName<<std::endl;
    }
    std::cout<<"Enter stock number to modify: ";
    std::cin>>sNum;
    std::cin.ignore();
    stocksFileStream.seekg((sNum - 1)*sizeof(stocks), std::ios::beg);
    stocksFileStream.read ((char *) & stock, sizeof(stocks));
    stocksFileStream.seekp((sNum - 1)*sizeof(stocks), std::ios::beg);
    std::cout<<"Old price for "<<stock.stockName<<" : "<<stock.stockPrice<<std::endl;
    std::cout<<"Enter new price for "<<stock.stockName<<" : ";
    std::cin>>stock.stockPrice;
    std::cin.ignore();
    stocksFileStream.write ((char *) & stock, sizeof(stocks));
    fileIn();
}

void commandDelete(){
    fileIn();
}

void getDecision (int & decision){
    std::cin>>decision;
    if (std::cin.fail())
        decision = 99; //invalid input
    //std::cin.ignore(std::numeric_limits<std::streamsize>::max());
    //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool checkSubString (std::string s1, std::string s2){
    if (s1.find(s2) != std::string::npos)
        return true;
    else
        return false;    
}

void createStocksDataFile(){
    stocks stock;
    stocksFileStream.open ("stocks.dat", std::ios::out);
    std::cout<<"Stocks data file created as 'stocks.dat'.\n"
            <<"Do not delete it or attempt to modify its content without this program!\n"
            <<"Always keep a backup just in case."<<std::endl;
    stocksFileMode = "out";
    addStocks();
}

void createDataFile(){
    fileStream.close();
    fileStream.open("data.dat", std::ios::out | std::ios::binary);
    fileMode = "out";
    std::cout<<"Teams data file created as 'data.dat'.\n"
            <<"Do not delete it or attempt to modify its content without this program!\n"
            <<"Always keep a backup just in case."<<std::endl;
    fileMode = "out";
    addDetails();
}

void prettyPrint(){
    std::cout<<"............................................"<<std::endl;
}

void fileOut(){
    addDetails();
}

void fileApp(){
    addDetails();
}