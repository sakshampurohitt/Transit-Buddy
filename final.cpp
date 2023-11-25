
#include <iostream>
#include <limits.h>
#include <string>
#include <math.h>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <time.h>
#include <queue>

using namespace std;

// FORWARD DECLARATION
// Global queue to store user complaints
const int V = 248;         // Number of stations
const int HASH_SIZE = 200; // Size of HASH TABLE
ifstream userFileIn;
ofstream userFileOut;

struct station_code;
struct node1;
class MetroCard;
class MetroCardHashTable;

struct AdminCredentials
{
    string username;
    string password;
};

struct station_code
{
    string name;
    int code;
    string color;
};

struct station_code station[V];
float graph[V][V];
struct AdminCredentials adminCredentials = {"admin", "admin123"};

struct node1
{
    vector<float> p;
} P[V];

class MetroCard
{
public:
    static int cardCounter; // Static variable to keep track of the number of cards issued
    int cardNo;
    int phoneNo;
    string name;
    float balance;
    MetroCard(const string &n, float b, int phone) : name(n), balance(b), phoneNo(phone)
    {
        cardNo = ++cardCounter;
        // Assign a unique card number
    }

    void WriteToFile() const {
        userFileOut << cardNo << " " << name << " " << phoneNo << " " << balance << endl;
    }

    static MetroCard* ReadFromFile() {
        int cardNo;
        string name;
        int phoneNo;
        float balance;

        if (userFileIn >> cardNo >> name >> phoneNo >> balance) {
            MetroCard* newUser = new MetroCard(name, balance, phoneNo);
            newUser->cardNo = cardNo;
            return newUser;
        }

        return nullptr;
    }


    void DisplayCard() const
    {
        cout << "Card Number  : " << cardNo << endl;
        cout << "Name         : " << name << endl;
        cout << "Phone Number : " << phoneNo << endl;
        cout << "Balance      : " << balance << endl;
    }

    int GetCardNo() const
    {
        return cardNo;
    }

    void AddBalance(float amount)
    {
        if (amount > 50)
        {
            balance += amount;
            cout << "Balance added successfully. New balance: " << balance << "\n";
        }
        else
        {
            cout << "Please add amount more than 50.\n";
        }
    }
};

int MetroCard::cardCounter = 246800;

class MetroCardHashTable
{
public:
    MetroCard *table[HASH_SIZE];
    MetroCardHashTable()
    {
        for (int i = 0; i < HASH_SIZE; i++)
        {
            table[i] = nullptr;
        }
    }

    int getKey(MetroCard *card) const
    {
        int key = card->GetCardNo();
        return key;
    }

    int firstHash(int key) const
    {
        return key % HASH_SIZE;
    }

    int secondaryHash(int key) const
    {
        return (25 * key + 17) % HASH_SIZE;
    }

    int doubleHash(int key, int attempt) const
    {
        return (firstHash(key) + attempt * secondaryHash(key)) % HASH_SIZE;
    }

    int insert(MetroCard *newCard)
    {
        int pos = doubleHash(newCard->GetCardNo(), 0);
        for (int attempt = 1; attempt < HASH_SIZE; attempt++)
        {
            if (table[pos] == nullptr)
            {
                table[pos] = newCard;
                return 1;
            }
            else if (table[pos]->GetCardNo() == newCard->GetCardNo())
            {
                return 0;
            }
            else
            {
                pos = doubleHash(newCard->GetCardNo(), attempt);
            }
        }
        return -1;
    }

    MetroCard *search(int key) const
    {
        bool found = false;
        int attempt = 0;
        int pos;
        while (!found && attempt <= HASH_SIZE)
        {
            pos = doubleHash(key, attempt);
            attempt++;
            if (table[pos] != nullptr && table[pos]->GetCardNo() == key)
            {
                return table[pos];
            }
            else if (attempt >= HASH_SIZE)
            {
                cout << "Card details not found. Table is full." << endl;
                break;
            }
            else
            {
                cout << "Card details not found." << endl;
                break;
            }
        }
        return nullptr;
    }

    // void display() const
    // {
    //     for (int i = 0; i < HASH_SIZE; i++)
    //     {
    //         if (table[i] != nullptr)
    //         {
    //             table[i]->DisplayCard();
    //         }
    //     }
    // }
};

// Global object declaration
MetroCardHashTable *hashtable = new MetroCardHashTable();

// FORWARD DECLARATION OF FUNCTIONS
bool AdminLogin();
string convertToCapital(string str);
void DisplayUserInfo(MetroCard *user);
void UpdateUserInfo(MetroCard *user);
void CheckCardBalance(MetroCard *user);
void AddCardBalance(MetroCard *user);
void Logout();
void printPath(int parent[], int j);
void printSolution(float dist[], int n, int src, int targ);
void dijkstra(float graph[V][V], int src, int targ);
int minDistance(float dist[], bool sptSet[]);
void getRoute();
void RegisterComplaint(MetroCard *user);
void SaveComplaintsToFile(string str);
void ViewComplaints();
MetroCard *NewUserRegister();
void AdminDisplayAllUsers();
void ResolveComplaint();

// Function to input admin login credentials
bool AdminLogin()
{
    string enteredUsername, enteredPassword;
    cout << "Enter Admin Username: ";
    cin >> enteredUsername;

    cout << "Enter Admin Password: ";
    cin >> enteredPassword;

    return (enteredUsername == adminCredentials.username && enteredPassword == adminCredentials.password);
}

// Function to convert user input to capital because of file data
string convertToCapital(string str)
{
    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] > 96 && str[i] < 123)
            str[i] = str[i] - 32;
    }
    return str;
}

// Function to display user information
void DisplayUserInfo(MetroCard *user)
{
    cout << "----------------------------------------------------------------------" << endl;
    cout << "||                           USER INFORMATION                        ||" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    cout << "Card Number  : " << user->GetCardNo() << endl;
    cout << "Name         : " << user->name << endl;
    cout << "Phone Number : " << user->phoneNo << endl;
    cout << "Balance      : " << user->balance << endl;
    cout << "----------------------------------------------------------------------" << endl;
}

// Function to display information for all registered users to admin
void AdminDisplayAllUsers()
{
    cout << "----------------------------------------------------------------------" << endl;
    cout << "||                          ALL REGISTERED USERS                     ||" << endl;
    cout << "----------------------------------------------------------------------" << endl;

    for (int i = 0; i < HASH_SIZE; i++)
    {
        if (hashtable->table[i] != nullptr)
        {
            MetroCard *user = hashtable->table[i];
            DisplayUserInfo(user);
        }
    }

    cout << "----------------------------------------------------------------------" << endl;
}

// Function to update user information
void UpdateUserInfo(MetroCard *user)
{
    cout << "----------------------------------------------------------------------" << endl;
    cout << "||                         UPDATE USER INFORMATION                    ||" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    cout << "Enter new name: ";
    cin >> user->name;
    cout << "Enter new phone number: ";
    cin >> user->phoneNo;
    cout << "User information updated successfully!" << endl;
    cout << "----------------------------------------------------------------------" << endl;
}

// Function to check card balance
void CheckCardBalance(MetroCard *user)
{
    cout << "----------------------------------------------------------------------" << endl;
    cout << "||                          CARD BALANCE                             ||" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    cout << "Card Number: " << user->GetCardNo() << endl;
    cout << "Balance     : " << user->balance << endl;
    cout << "----------------------------------------------------------------------" << endl;
}

// Function to add balance to the metro card
void AddCardBalance(MetroCard *user)
{
    float amount;
    cout << "----------------------------------------------------------------------" << endl;
    cout << "||                        ADD CARD BALANCE                           ||" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    cout << "Enter the amount to add: ";
    cin >> amount;
    user->AddBalance(amount);
    cout << "----------------------------------------------------------------------" << endl;
}

// Function to register a complaint
void RegisterComplaint(MetroCard *user)
{
    string complaint;
    cout << "Enter your complaint: ";
    cin.ignore(); // Clear the input buffer
    getline(cin, complaint);

    string complaintDetails = "Card Number: " + to_string(user->GetCardNo()) + "\nName: " + user->name + "\nComplaint: " + complaint;

    SaveComplaintsToFile(complaintDetails);

    cout << "Complaint registered successfully!" << endl << endl;
}

// Function to save complaints to a text file
void SaveComplaintsToFile(string str)
{
    ofstream outFile("complaints.txt", ios::app); // Open file in append mode
    if (outFile.is_open())
    {
        outFile << str << "\n\n";
        outFile.close(); // Close the file
    }
    else
    {
        cout << "Unable to open file to save complaints." << endl;
    }
}

// Function to view complaints of users
void ViewComplaints()
{
    ifstream inFile("complaints.txt");
    string complaint;

    if (!inFile.is_open())
    {
        cout << "No complaints to view." << endl;
        return;
    }

    queue<string> complaintsQueue;

    // Read complaints from the file and enqueue them
    while (getline(inFile, complaint))
    {
        complaintsQueue.push(complaint);
    }

    inFile.close();

    if (complaintsQueue.empty())
    {
        cout << "No complaints to view." << endl;
        return;
    }

    // Display complaints to the admin
    cout << "List of User Complaints:\n\n";
    while (!complaintsQueue.empty())
    {
        cout << complaintsQueue.front() << endl;
        complaintsQueue.pop();
    }
}

// Function to log out
void Logout()
{
    cout << "----------------------------------------------------------------------" << endl;
    cout << "||                             LOG OUT                               ||" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    cout << "Thank you for using Transit Pulse. Have a great day!" << endl;
    cout << "----------------------------------------------------------------------" << endl;
}
int minDistance(float dist[], bool sptSet[])
{
    float min = INT_MAX;
    int min_index = -1;

    for (int v = 0; v < V; v++)
    {
        if (!sptSet[v] && dist[v] <= min)
        {
            min = dist[v];
            min_index = v;
        }
    }

    return min_index;
}
void dijkstra(float graph[V][V], int src, int targ)
{
    float dist[V];
    bool sptSet[V];

    for (int i = 0; i < V; i++)
    {
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }

    dist[src] = 0;

    for (int count = 0; count < V - 1; count++)
    {
        int u = minDistance(dist, sptSet);
        sptSet[u] = true;

        for (int v = 0; v < V; v++)
        {
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
            {
                dist[v] = dist[u] + graph[u][v];
                P[v].p.push_back(u);
            }
        }
    }

    printSolution(dist, V, src, targ);
}

void printPath(int parent[], int j)
{
    // Base Case : If j is source
    if (j == -1)
        return;

    printPath(parent, parent[j]);

    cout << station[j + 1].name << "=> ";
}

void printSolution(float dist[], int n, int src, int targ)
{
    int parent[V];

    // Extract the path for the target station
    int i = targ;
    vector<int> pathTarg;
    pathTarg.push_back(i);
    while (i != src)
    {
        i = P[i].p[0];
        pathTarg.push_back(i);
    }

    // Print the paths
    cout << "Shortest path to Target Station  " << endl << endl;
    for (int i = pathTarg.size() - 1; i >= 0; i--)
    {
        cout << station[pathTarg[i]].name << "{" << station[pathTarg[i]].color << "}";
        if(i > 0) {
            cout << endl <<"       | " << endl;
        }
    }
    cout << "\n";

    cout << "\n";
}

// Function to resolve a complaint
void ResolveComplaint()
{
    ifstream inFile("complaints.txt");
    string complaint;

    if (!inFile.is_open())
    {
        cout << "No complaints to resolve." << endl;
        return;
    }

    // Read complaints from the file and enqueue them
    queue<string> complaintsQueue;
    while (getline(inFile, complaint))
    {
        complaintsQueue.push(complaint);
    }

    inFile.close();

    if (complaintsQueue.empty())
    {
        cout << "No complaints to resolve." << endl;
        return;
    }

    // Display and resolve the first complaint
    cout << "Resolved Complaint:\n";
    cout << complaintsQueue.front() << endl;
    complaintsQueue.pop();
    cout << complaintsQueue.front() << endl;
    complaintsQueue.pop();
    cout << complaintsQueue.front() << endl;
    complaintsQueue.pop();
    cout << complaintsQueue.front() << endl;
    complaintsQueue.pop();

    // Update the file with the remaining complaints
    ofstream outFile("complaints.txt");
    while (!complaintsQueue.empty())
    {
        outFile << complaintsQueue.front() << endl;
        complaintsQueue.pop();
    }

    outFile.close();
    cout << "Complaint resolved and updated in the file." << endl;
}

// Function to register new user to
MetroCard *NewUserRegister()
{
    string name;
    int phoneNo;
    float balance;

    // Get user input
    cout << "Enter your name: ";
    cin >> name;

    cout << "Enter your phone number: ";
    cin >> phoneNo;

    cout << "Enter initial balance: ";
    cin >> balance;

    // Create a new MetroCard object
    MetroCard *newCard = new MetroCard(name, balance, phoneNo);
    int check = hashtable->insert(newCard);
    if (check == 0)
    {
        cout << "Card number already exists. Cannot store another user." << endl;
        return 0;
    }
    else if (check == 1)
    {
        // Display the details of the newly created card
        cout << "New User Registered!\n";
        newCard->DisplayCard();
        // Return the generated card number
        newCard->WriteToFile();
        return newCard;
    }
    else
    {
        cout << "User information not stored." << endl;
        return 0;
    }
}

int main()
{
    vector<MetroCard*> metroCards;
    int temp, n1, n2;
    float dis;
    ifstream fin;

    // Open file for reading user data
    userFileIn.open("user_data.txt");

    // Open file for writing user data
    userFileOut.open("user_data.txt", ios::app);
    
    // Inserting user file data to hashtable
    MetroCard* existingUser;
    while ((existingUser = MetroCard::ReadFromFile()) != nullptr) {
        hashtable->insert(existingUser);
    }

    fin.open("node_values_new.txt");
    // while (fin) {
    //     string line;
    //     // Read a Line from File
    //     getline(fin, line);
    //
    //     // Print line in Console
    //     cout << line << endl;
    // }
    for (int i = 0; i < 248; i++)
    {
        for (int j = 0; j < 248; j++)
            graph[i][j] = 0;
    }
    for (int i = 1; i <= V; i++)
    {
        fin >> temp;
        fin >> n1;
        // cout<<temp<<" "<<n1<<endl;
        for (int j = 0; j < temp; j++)
        {
            fin >> n2;
            fin >> dis;
            // cout<<" "<<n2<<" "<<dis<<endl;
            graph[n1 - 1][n2 - 1] = dis;
        }
        // cout<<endl;
    }
    string line, col;
    ifstream code;
    ifstream color;
    code.open("stationcodes.txt");
    color.open("stationcolorcodes.txt");
    for (int i = 0; i < V; i++)
    {
        getline(code, line);
        station[i].name = line;
        station[i].code = i;
        getline(color, col);
        station[i].color = col;
    }
    string start;
    string finish;
    while (true)
    {
        cout << "----------------------------------------------------------------------" << endl;
        cout << "**********************************************************************" << endl;
        cout << "||                                                                  ||" << endl;
        cout << "||                          TRANSIT PULSE                           ||" << endl;
        cout << "||                                                                  ||" << endl;
        cout << "||                                                                  ||" << endl;
        cout << "||     1. USER REGISTRATION                                         ||" << endl;
        cout << "||                                                                  ||" << endl;
        cout << "||     2. USER LOGIN                                                ||" << endl;
        cout << "||                                                                  ||" << endl;
        cout << "||     3. ADMIN LOGIN                                               ||" << endl;
        cout << "||                                                                  ||" << endl;
        cout << "||     4. CHECK ROUTE                                               ||" << endl;
        cout << "||                                                                  ||" << endl;
        cout << "||     5. EXIT                                                      ||" << endl;
        cout << "||                                                                  ||" << endl;
        cout << "||                                                                  ||" << endl;
        cout << "||                                        your travel buddy.....    ||" << endl;
        cout << "||                                                                  ||" << endl;
        cout << "**********************************************************************" << endl;
        cout << "----------------------------------------------------------------------" << endl;
        int choice;
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
            // Code for USER REGISTRATION
            // MetroCard* card1;
            if (!NewUserRegister())
                break;

            // Add user login

            break;
        case 2:
            // Code for USER LOGIN
            int userCard;
            cout << endl;
            cout << "Enter your Metro Card Number:";
            cin >> userCard;

            // Search for the card in the hash table
            MetroCard *user2;
            user2 = hashtable->search(userCard);

            if (user2 == nullptr)
            {
                cout << "Invalid Metro Card Number. Please check and try again.\n";
                break;
            }
            int userLogout;
            userLogout = 0;
            while (!userLogout)
            {
                cout << "----------------------------------------------------------------------" << endl;
                cout << "**********************************************************************" << endl;
                cout << "||                                                                  ||" << endl;
                cout << "||                          TRANSIT PULSE                           ||" << endl;
                cout << "||                                                                  ||" << endl;
                cout << "||                                                                  ||" << endl;
                cout << "||     1. YOUR INFORMATION                                          ||" << endl;
                cout << "||                                                                  ||" << endl;
                cout << "||     2. UPDATE INFORMATION                                        ||" << endl;
                cout << "||                                                                  ||" << endl;
                cout << "||     3. CHECK CARD BALANCE                                        ||" << endl;
                cout << "||                                                                  ||" << endl;
                cout << "||     4. ADD CARD BALANCE                                          ||" << endl;
                cout << "||                                                                  ||" << endl;
                cout << "||     5. COMPLAINT REGISTRATION                                    ||" << endl;
                cout << "||                                                                  ||" << endl;
                cout << "||     6. LOGOUT                                                    ||" << endl;
                cout << "||                                                                  ||" << endl;
                cout << "||                                        your travel buddy.....    ||" << endl;
                cout << "||                                                                  ||" << endl;
                cout << "**********************************************************************" << endl;
                cout << "----------------------------------------------------------------------" << endl;
                int choice2;
                cout << endl << "Enter your choice: ";
                cin >> choice2;
                switch (choice2)
                {
                case 1:
                    // Code for USER INFORMATION
                    // Display User Information
                    DisplayUserInfo(user2);
                    break;
                case 2:
                    // Update User Information
                    UpdateUserInfo(user2);
                    break;
                case 3:
                    // Check Card Balance
                    CheckCardBalance(user2);
                    break;
                case 4:
                    // Add Card Balance
                    AddCardBalance(user2);
                    break;
                case 5:
                    // add complaint
                    RegisterComplaint(user2);
                    break;
                case 6:
                    // Logout
                    Logout();
                    userLogout = 1;
                    break;
                default:
                    cout << "Invalid choice. Please enter a valid option." << endl;
                }
            }
            break;
        case 3:
            cout << "You selected ADMIN LOGIN." << endl;
            int adminLogout;
            adminLogout = 0;
            // Check admin credentials
            if (AdminLogin())
            {
                int adminLogout = 0;
                while (!adminLogout)
                {
                    cout << "----------------------------------------------------------------------" << endl;
                    cout << "**********************************************************************" << endl;
                    cout << "||                                                                  ||" << endl;
                    cout << "||                          TRANSIT PULSE                           ||" << endl;
                    cout << "||                                                                  ||" << endl;
                    cout << "||                                                                  ||" << endl;
                    cout << "||     1. ALL USERS INFORMATION                                     ||" << endl;
                    cout << "||                                                                  ||" << endl;
                    cout << "||     2. RECHARGE USER'S CARD                                      ||" << endl;
                    cout << "||                                                                  ||" << endl;
                    cout << "||     3. COMPLAINTS                                                ||" << endl;
                    cout << "||                                                                  ||" << endl;
                    cout << "||     4. RESOLVE COMPLAINT                                         ||" << endl;
                    cout << "||                                                                  ||" << endl;
                    cout << "||     5. LOGOUT                                                    ||" << endl;
                    cout << "||                                                                  ||" << endl;
                    cout << "||                                        your travel buddy.....    ||" << endl;
                    cout << "||                                                                  ||" << endl;
                    cout << "**********************************************************************" << endl;
                    cout << "----------------------------------------------------------------------" << endl;

                    int choice3;
                    cout << endl
                         << "Enter your choice: ";
                    cin >> choice3;
                    switch (choice3)
                    {
                    case 1:
                        // Code for USER INFORMATION
                        // Display User Information
                        AdminDisplayAllUsers();
                        break;
                    case 2:
                        // Recharge a users card
                        int userCard;
                        cout << endl;
                        cout << "Enter the customers Metro Card Number:";
                        cin >> userCard;

                        // Search for the card in the hash table
                        MetroCard *user;
                        user = hashtable->search(userCard);

                        if (user == nullptr)
                        {
                            cout << "Invalid Metro Card Number. Please check and try again.\n";
                            break;
                        }
                        AddCardBalance(user);
                        break;
                    case 3:
                        ViewComplaints();
                        break;
                    case 4:
                        // Add Card Balance
                        ResolveComplaint();
                        break;
                    case 5:
                        Logout();
                        adminLogout = 1;
                        break;

                    default:
                        cout << "Invalid choice. Please enter a valid option." << endl;
                    }
                }
            }
            else
            {
                cout << "Invalid admin credentials. Please try again.\n";
            }
            break;

        case 4:
            cout << "You selected ROUTE PLANNER." << endl;

            cout << "Enter source station name: ";
            cin.ignore(); // Ignore newline character in the input buffer
            getline(cin, start);

            cout << "Enter target station name: ";
            getline(cin, finish);

            start = convertToCapital(start);
            finish = convertToCapital(finish);

            int source;
            source = -1;
            int target;
            target = -1;

            // Find station codes for source and target
            for (int i = 0; i < V; i++)
            {
                if (start.compare(station[i].name) == 0)
                {
                    source = station[i].code;
                }
                if (finish.compare(station[i].name) == 0)
                {
                    target = station[i].code;
                }
            }

            // Check if the station names are valid
            if (source == -1 || target == -1)
            {
                cout << "Invalid station names. Please enter valid station names.\n";
                break;
            }

            // Call Dijkstra's algorithm with the provided stations
            dijkstra(graph, source, target);
            break;

        case 5:
            // Code to end the application
            cout << "Exiting the application. Goodbye!" << endl;
            cout << endl;
            cout << endl;
            return 0;
        default:
            cout << "Invalid choice. Please enter a valid option." << endl;
        }
    }
    userFileIn.close();
    userFileOut.close();

    return 0;
}