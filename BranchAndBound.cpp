// BranchAndBound.cpp : Ten plik zawiera funkcjê „main”. W nim rozpoczyna siê i koñczy wykonywanie programu.
//

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <queue>

using namespace std;

struct Node
{
    int lower_bound;
    int vertex;
};

int cnt;//iloœæ wierzcho³ków w grafie
string fileNames[7];    //tabela zawieraj¹ca nazwy plików z konkretnymi instancjami
int instancyCounter[7]; //tabela zawieraj¹ca informacje ile razy mamy przetestowaæ dan¹ instancjê
int optimumInstancyCost[7]; //tabela zawieraj¹ca informacje o optymalnej œcie¿ce dla danej instancji

ofstream plikWynikowy("daneWynikowe.txt");

vector < vector<int> > miasta; //wektor zawieraj¹cy listê wektorów, w której indeksem s¹ numery miast, a wartoœciami odleg³oœci
int minCost;
int rootLowerBound;
vector <int> outgoingEdges;
vector <bool> isUsedInPath;


vector <int> shortestPath;
vector <int> tempPath;





bool operator > (const Node& arg1, const Node& arg2)
{
    if (arg1.lower_bound > arg2.lower_bound) return true;
    else return false;
}

bool operator < (const Node& arg1, const Node& arg2)
{
    if (arg1.lower_bound < arg2.lower_bound) return true;
    else return false;
}

bool operator == (const Node& arg1, const Node& arg2)
{
    if (arg1.lower_bound == arg2.lower_bound) return true;
    else return false;
}


void loadConfig() //funkcja wczytuje konfiguracjê z pliku tekstowego
{

    ifstream plik;
    plik.open("./dataFiles/config.txt");
    if (!plik.good())
    {
        cout << "Nie udalo sie wczytac pliku, sprobuj ponownie";
        return;
    }
    cout << "Ladowanie konfiguracji..." << endl;

    string name;
    for (int i = 0; i < 7; i++)
    {
        plik >> name;
        fileNames[i] = "./dataFiles/" + name;
        plik >> instancyCounter[i];
        plik >> optimumInstancyCost[i];
    }


    plik.close();
}
void loadFromFile(int index) //funkcja wczytuj¹ca dane z pliku
{
    ifstream plik;
    plik.open(fileNames[index]);
    if (!plik.good())
    {
        cout << "Nie udalo sie wczytac pliku, sprobuj ponownie";
        return;
    }
    cout << "Ladowanie danych..." << endl;
    plik >> cnt;
    cout << "Ilosc miast: " << cnt << endl;
    int li;
    int tempMin = INT_MAX;
    minCost = INT_MAX;
    miasta.clear();
    isUsedInPath.clear();
    outgoingEdges.clear();
    tempPath.clear();
    shortestPath.clear();
    rootLowerBound = 0;
    for (int i = 0; i < cnt; i++)
    {
        vector <int> odleglosc;
        for (int k = 0; k < cnt; k++)
        {
            plik >> li;
            odleglosc.push_back(li);
            if (li < tempMin && li != 0 && li != -1) tempMin = li;
        }
        miasta.push_back(odleglosc);
        isUsedInPath.push_back(false);
        outgoingEdges.push_back(tempMin);
        rootLowerBound += tempMin;
        tempMin = INT_MAX;
    }
    plik.close();
    cout << "Wczytywanie zakonczone powodzeniem" << endl;

}

void testData() //funkcja sprawdzaj¹ca poprawnoœæ wczytania danych
{
    cout << endl;
    for (int i = 0; i < cnt; i++)
    {
        for (int k = 0; k < cnt; k++)
        {
            cout << miasta[i][k] << "  ";
        }
        cout << endl;
    }

    /*cout <<"\n Wszystkie polaczenia: "<< endl;
    for (int i = 0; i < cnt; i++)
    {
        for (int k = 0; k < cnt; k++)
        {
            cout << "Miasto: "<<i<<" do miasto "<<k<<"  -  odleglosc: "<<miasta[i][k] << endl;
        }
        cout << endl;
    }*/
}

void printShortestPath(int index)
{
    plikWynikowy << "Najkrotsza sciezka: ";
    cout << "Najkrotsza sciezka: ";
    for (auto itr : shortestPath)
    {
        cout << itr << " ";
        plikWynikowy << itr << " ";
    }
    plikWynikowy << endl << "Dlugosc tej sciezki to: " << minCost << endl;
    cout << endl << "Dlugosc tej sciezki to: " << minCost << endl;
    plikWynikowy << endl << "Jest gorsza (dluzsza) od optymalnej o: " << minCost-optimumInstancyCost[index] << endl;
    cout << endl << "Jest gorsza (dluzsza) od optymalnej o: " << minCost - optimumInstancyCost[index] << endl;
}

void printTempPath()
{
    cout << "\n\n Aktualna sciezka: ";
    for (auto itr : tempPath) cout << itr << " ";
    //cout << endl << "Dlugosc tej sciezki to: " << tempCost << endl;

}

int lower_bound(Node actual, int next)
{
    int lower_bound = actual.lower_bound;
    lower_bound -= outgoingEdges[actual.vertex];
    lower_bound += miasta[actual.vertex][next];

    return lower_bound;
}

void findShortestPath(Node node)
{
    tempPath.push_back(node.vertex);
    isUsedInPath[node.vertex] = true;
    Node temp;
    priority_queue <Node, std::vector<Node>, std::greater<Node>> bounds;

    for (int i = 0; i < cnt; i++)
    {
        if (!isUsedInPath[i])
        {
            temp.vertex = i;
            temp.lower_bound = lower_bound(node, i);
            bounds.push(temp);
        }
    }

    if (bounds.empty())
    {
        if (lower_bound(node, 0) < minCost)
        {
            minCost = lower_bound(node, 0);
            shortestPath = tempPath;
        }
    }
    else
    {
        while (!bounds.empty())
        {
            temp = bounds.top();
            bounds.pop();

            if (temp.lower_bound < minCost) findShortestPath(temp);
            else break;
        }
    }

    tempPath.pop_back();
    isUsedInPath[node.vertex] = false;
}

void branchAndBound()
{
    Node root;
    root.vertex = 0;
    root.lower_bound = rootLowerBound;
    findShortestPath(root);
    //printTempPath();
}



int main()
{
    LARGE_INTEGER StartingTime, EndingTime;
    LARGE_INTEGER DifferenceOfTime = { 0 };
    LARGE_INTEGER frequency; QueryPerformanceFrequency(&frequency);


    loadConfig();   //funkcja wczytuje konfiguracjê programu 

    //testData();     //funkcja wyœwietla na ekranie wczytane dane, aby mo¿na by³o sprawdziæ ich poprawnoœæ

    plikWynikowy << "PLIK WEJSCIOWY" << "\t" << "CZAS[ns]" << endl;
    cout << "PLIK WEJSCIOWY" << "\t" << "CZAS[ns]" << endl;

    for (int k = 0; k < 7; k++)
    {

        //plikWynikowy << fileNames[k]<<endl;
        loadFromFile(k); //funkcja wczytuje dane z pliku
        for (int i = 0; i < instancyCounter[k]; i++)
        {
            QueryPerformanceCounter(&StartingTime);
            branchAndBound();   //funkcja znajduj¹ca najkrótsz¹ œcie¿kê metod¹ b&b
            QueryPerformanceCounter(&EndingTime);

            DifferenceOfTime.QuadPart = (EndingTime.QuadPart - StartingTime.QuadPart) * 1000000000 / frequency.QuadPart; //w nanosekundach
            plikWynikowy << fileNames[k] << "\t" << DifferenceOfTime.QuadPart << endl;
            cout << fileNames[k] << "\t" << DifferenceOfTime.QuadPart << endl;
        }

        printShortestPath(k); //funkcja wypisuje znalezion¹ najkrótsz¹ œcie¿kê wraz z jej d³ugoœci¹

    }

    //printShortestPath(); //funkcja wypisuje znalezion¹ najkrótsz¹ œcie¿kê wraz z jej d³ugoœci¹
    plikWynikowy.close();

    getchar(); //blokada przez samoczynnym zamkniêciem siê okna
}