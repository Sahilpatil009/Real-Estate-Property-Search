#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
using namespace std;

// ================= ANSI Color Codes =================
#define RESET       "\033[0m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define CYAN        "\033[36m"
#define MAGENTA     "\033[35m"
#define WHITE       "\033[37m"
#define BOLD        "\033[1m"

// ================= Helper Function =================
string toUpperCase(const string &s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// ================= Property Class =================
class Property {
public:
    string type;
    string location;
    int price;
    int area;
    string owner;

    void input(const string &username) {
        cout << WHITE << "Enter property type (House/Apartment/Plot): " << RESET;
        cin >> type;
        type = toUpperCase(type);

        cout << WHITE << "Enter location: " << RESET;
        cin >> location;
        location = toUpperCase(location);

        cout << WHITE << "Enter price: " << RESET;
        cin >> price;

        cout << WHITE << "Enter area (in sq.ft): " << RESET;
        cin >> area;

        owner = username;
    }

    void displayRow(int index) const {
        cout << BLUE << "| " << left << setw(3) << index << " | "
             << setw(13) << type << " | "
             << setw(13) << location << " | "
             << setw(11) << price << " | "
             << setw(9) << area << " | "
             << setw(13) << owner << " |" << RESET << "\n";
    }
};

// ================= User Class =================
class User {
public:
    string username;
    string password;
    User(string u, string p) : username(u), password(p) {}
    User() {}
};

// ================= RealEstate Class =================
class RealEstate {
private:
    vector<Property> properties;
    vector<User> users;
    const string userFile = "users.csv";
    const string propertyFile = "properties.csv";

public:
    RealEstate() {
        loadUsers();
        loadProperties();
    }

    // ================= CSV File Handling =================
    void loadUsers() {
        ifstream fin(userFile);
        if (!fin) return;
        string line;
        while (getline(fin, line)) {
            stringstream ss(line);
            string u, p;
            getline(ss, u, ',');
            getline(ss, p, ',');
            if (!u.empty() && !p.empty())
                users.push_back(User(u, p));
        }
        fin.close();
    }

    void saveUsers() {
        ofstream fout(userFile);
        for (auto &user : users)
            fout << user.username << "," << user.password << "\n";
        fout.close();
    }

    void loadProperties() {
        ifstream fin(propertyFile);
        if (!fin) return;
        string line;
        while (getline(fin, line)) {
            stringstream ss(line);
            Property p;
            string priceStr, areaStr;
            getline(ss, p.type, ',');
            getline(ss, p.location, ',');
            getline(ss, priceStr, ',');
            getline(ss, areaStr, ',');
            getline(ss, p.owner, ',');
            if (!p.type.empty() && !priceStr.empty() && !areaStr.empty()) {
                p.price = stoi(priceStr);
                p.area = stoi(areaStr);
                properties.push_back(p);
            }
        }
        fin.close();
    }

    void saveProperties() {
        ofstream fout(propertyFile);
        for (auto &p : properties)
            fout << p.type << "," << p.location << "," << p.price << ","
                 << p.area << "," << p.owner << "\n";
        fout.close();
    }

    // ================= Core Functions =================
    bool registerUser() {
        string u, p;
        cout << WHITE << "Enter username: " << RESET;
        cin >> u;

        for (auto &user : users)
            if (user.username == u) {
                cout << RED << "Username already exists!\n" << RESET;
                return false;
            }

        cout << WHITE << "Enter password: " << RESET;
        cin >> p;
        users.push_back(User(u, p));
        saveUsers();
        cout << GREEN << "User registered successfully!\n" << RESET;
        return true;
    }

    bool loginUser(string &loggedUser) {
        string u, p;
        cout << GREEN << "Enter username: " << RESET;
        cin >> u;
        cout << RED << "Enter password: " << RESET;
        cin >> p;

        for (auto &user : users)
            if (user.username == u && user.password == p) {
                loggedUser = u;
                cout << GREEN << "Login successful!\n" << RESET;
                return true;
            }

        cout << RED << "Invalid username or password!\n" << RESET;
        return false;
    }

    void addProperty(const string &username) {
        Property p;
        p.input(username);
        properties.push_back(p);
        saveProperties();
        cout << GREEN << "Property added successfully!\n" << RESET;
    }

    // ================= Display Utilities =================
    void printTableHeader() const {
        cout << CYAN << "+-----+---------------+---------------+-------------+-----------+---------------+\n";
        cout << "| No. | Type          | Location      | Price       | Area      | Owner         |\n";
        cout << "+-----+---------------+---------------+-------------+-----------+---------------+\n" << RESET;
    }

    void printTableFooter() const {
        cout << CYAN << "+-----+---------------+---------------+-------------+-----------+---------------+\n" << RESET;
    }

    // ================= Sorting & Searching =================
    void bubbleSortByPrice() {
        int n = properties.size();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (properties[j].price > properties[j + 1].price)
                    swap(properties[j], properties[j + 1]);
            }
        }
    }

    int binarySearchByPrice(int price) {
        int left = 0, right = properties.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (properties[mid].price == price)
                return mid;
            else if (properties[mid].price < price)
                left = mid + 1;
            else
                right = mid - 1;
        }
        return -1;
    }

    // ================= Property Display =================
    void showAllProperties() {
        if (properties.empty()) {
            cout << RED << "No properties available.\n" << RESET;
            return;
        }

        bubbleSortByPrice();
        cout << BOLD << YELLOW << "\n=== All Properties (Sorted by Price) ===\n" << RESET;
        printTableHeader();
        int index = 1;
        for (auto &p : properties)
            p.displayRow(index++);
        printTableFooter();
    }

    void searchProperty() {
        int choice;
        cout << CYAN << "Search by:\n1."<<GREEN<<"Type\n2."<<YELLOW<<" Location\n3."<<RED<<" Price Range\n4."<<BLUE<<" Exact Price (Binary Search)\n"<<"Enter choice: " << RESET;
        cin >> choice;

        bool found = false;
        if (choice == 1) {
            string t;
            cout << WHITE << "Enter property type: " << RESET;
            cin >> t;
            t = toUpperCase(t);

            printTableHeader();
            int index = 1;
            for (auto &p : properties)
                if (toUpperCase(p.type) == t) {
                    p.displayRow(index++);
                    found = true;
                }
            printTableFooter();
        } else if (choice == 2) {
            string loc;
            cout << WHITE << "Enter location: " << RESET;
            cin >> loc;
            loc = toUpperCase(loc);

            printTableHeader();
            int index = 1;
            for (auto &p : properties)
                if (toUpperCase(p.location) == loc) {
                    p.displayRow(index++);
                    found = true;
                }
            printTableFooter();
        } else if (choice == 3) {
            int minPrice, maxPrice;
            cout << WHITE << "Enter minimum price: " << RESET;
            cin >> minPrice;
            cout << WHITE << "Enter maximum price: " << RESET;
            cin >> maxPrice;

            printTableHeader();
            int index = 1;
            for (auto &p : properties)
                if (p.price >= minPrice && p.price <= maxPrice) {
                    p.displayRow(index++);
                    found = true;
                }
            printTableFooter();
        } else if (choice == 4) {
            if (properties.empty()) {
                cout << RED << "No properties available.\n" << RESET;
                return;
            }

            bubbleSortByPrice();

            int price;
            cout << WHITE << "Enter exact price to search: " << RESET;
            cin >> price;

            int result = binarySearchByPrice(price);
            if (result != -1) {
                printTableHeader();
                properties[result].displayRow(1);
                printTableFooter();
                found = true;
            }
        } else {
            cout << RED << "Invalid choice!\n" << RESET;
            return;
        }

        if (!found)
            cout << RED << "No matching properties found.\n" << RESET;
    }

    void showMyProperties(const string &username) const {
        cout << BOLD << MAGENTA << "\n=== My Properties ===\n" << RESET;
        bool found = false;
        printTableHeader();
        int index = 1;
        for (auto &p : properties)
            if (p.owner == username) {
                p.displayRow(index++);
                found = true;
            }
        printTableFooter();
        if (!found)
            cout << YELLOW << "You have not added any properties yet.\n" << RESET;
    }
};

// ================= MAIN =================
int main() {
    RealEstate app;
    string loggedUser = "";
    int option;

    do {
        if (loggedUser.empty()) {
            cout << BOLD << CYAN << "\n=== Real Estate Property Search ===\n" << RESET;
            cout << YELLOW << "1. Show All Properties (Sorted by Price)\n";
            cout << GREEN<<"2. Search Property\n";
            cout << RED<<"3. Register\n";
            cout << BLUE<<"4. Login\n";
            cout << MAGENTA<<"5. Exit\n" << RESET;
            cout << WHITE << "Enter your choice: " << RESET;
            cin >> option;

            switch (option) {
                case 1: app.showAllProperties(); break;
                case 2: app.searchProperty(); break;
                case 3: app.registerUser(); break;
                case 4: app.loginUser(loggedUser); break;
                case 5: cout << GREEN << "Exiting...\n" << RESET; break;
                default: cout << RED << "Invalid option!\n" << RESET;
            }
        } else {
            cout << BOLD << CYAN << "\n=== Welcome, " << loggedUser << " ===\n" << RESET;
            cout << YELLOW << "1. Add Property\n";
            cout << RED<<"2. Show All Properties (Sorted by Price)\n";
            cout << GREEN<<"3. Search Property\n";
            cout << CYAN<<"4. Show My Properties\n";
            cout << RED<<"5. Logout\n" << RESET;
            cout << WHITE << "Enter your choice: " << RESET;
            cin >> option;

            switch (option) {
                case 1: app.addProperty(loggedUser); break;
                case 2: app.showAllProperties(); break;
                case 3: app.searchProperty(); break;
                case 4: app.showMyProperties(loggedUser); break;
                case 5: loggedUser = ""; cout << GREEN << "Logged out successfully!\n" << RESET; break;
                default: cout << RED << "Invalid option!\n" << RESET;
            }
        }
    } while (option != 5 || !loggedUser.empty());

    return 0;
}