// RealEstateWin32.cpp
// Pure Win32 API Real Estate Management System
// Requires linking with comctl32.lib
// Compile with Visual Studio Developer Command Prompt:
// cl /EHsc RealEstateWin32.cpp /link comctl32.lib

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

#pragma comment(lib, "comctl32.lib")

// Control IDs - Section Buttons
#define IDC_BTN_SECTION_LOGIN 50
#define IDC_BTN_SECTION_REGISTER 51
#define IDC_BTN_SECTION_SEARCH 52
#define IDC_BTN_SECTION_ADD 53

// Login Section
#define IDC_LOGIN_USERNAME 60
#define IDC_LOGIN_PASSWORD 61
#define IDC_BTN_LOGIN 62

// Register Section
#define IDC_REG_USERNAME 70
#define IDC_REG_PASSWORD 71
#define IDC_REG_CONFIRM_PASS 72
#define IDC_BTN_REGISTER 73

// Add Property Section
#define IDC_TYPE 101
#define IDC_LOCATION 102
#define IDC_PRICE 103
#define IDC_AREA 104
#define IDC_OWNER 105
#define IDC_BTN_ADD 201

// Search Section
#define IDC_SEARCH_TYPE 110
#define IDC_SEARCH_LOCATION 111
#define IDC_SEARCH_PRICE 112
#define IDC_SEARCH_MIN_PRICE 113
#define IDC_SEARCH_MAX_PRICE 114

#define IDC_BTN_SHOW 202
#define IDC_BTN_SORT 203
#define IDC_BTN_SEARCH_TYPE 204
#define IDC_BTN_SEARCH_LOC 205
#define IDC_BTN_SEARCH_RANGE 206
#define IDC_BTN_SEARCH_EXACT 207
#define IDC_LISTVIEW 301

// Globals
HINSTANCE ghInst;
HWND hListView;
HWND hType, hLocation, hPrice, hArea, hOwner;

// Login/Register controls
HWND hLoginUsername, hLoginPassword;
HWND hRegUsername, hRegPassword, hRegConfirmPass;

// Search input controls
HWND hSearchType, hSearchLocation, hSearchPrice, hSearchMinPrice, hSearchMaxPrice;

// Section panels (groups of controls)
std::vector<HWND> loginControls;
std::vector<HWND> registerControls;
std::vector<HWND> searchControls;
std::vector<HWND> addPropertyControls;

std::vector<std::tuple<std::string, std::string, int, int, std::string>> properties; // type, location, price, area, owner
const char *PROPERTY_FILE = "properties.csv";
const char *USER_FILE = "users.csv";

bool isLoggedIn = false;
std::string currentUser = "";

// Utils
std::string ToUpper(const std::string &s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::toupper);
    return r;
}

void LoadProperties() {
    properties.clear();
    std::ifstream fin(PROPERTY_FILE);
    if (!fin.is_open())
        return;
    std::string line;
    while (std::getline(fin, line))
    {
        if (line.empty())
            continue;
        std::stringstream ss(line);
        std::string t, loc, priceS, areaS, owner;
        std::getline(ss, t, ',');
        std::getline(ss, loc, ',');
        std::getline(ss, priceS, ',');
        std::getline(ss, areaS, ',');
        std::getline(ss, owner, ',');
        try
        {
            int price = std::stoi(priceS);
            int area = std::stoi(areaS);
            properties.emplace_back(t, loc, price, area, owner);
        }
        catch (...)
        { /* skip invalid Lines*/
        }
    }
    fin.close();
}

// User management functions
void SaveUser(const std::string &username, const std::string &password)
{
    std::ofstream fout(USER_FILE, std::ios::app);
    fout << username << "," << password << "\n";
    fout.close();
}

bool ValidateUser(const std::string &username, const std::string &password)
{
    std::ifstream fin(USER_FILE);
    if (!fin.is_open())
        return false;

    std::string line;
    while (std::getline(fin, line))
    {
        if (line.empty())
            continue;
        std::stringstream ss(line);
        std::string user, pass;
        std::getline(ss, user, ',');
        std::getline(ss, pass, ',');
        if (user == username && pass == password)
        {
            fin.close();
            return true;
        }
    }
    fin.close();
    return false;
}

bool UserExists(const std::string &username)
{
    std::ifstream fin(USER_FILE);
    if (!fin.is_open())
        return false;

    std::string line;
    while (std::getline(fin, line))
    {
        if (line.empty())
            continue;
        std::stringstream ss(line);
        std::string user;
        std::getline(ss, user, ',');
        if (user == username)
        {
            fin.close();
            return true;
        }
    }
    fin.close();
    return false;
}

// Section visibility management
void HideAllControls(const std::vector<HWND> &controls)
{
    for (HWND hwnd : controls)
    {
        ShowWindow(hwnd, SW_HIDE);
    }
}

void ShowControls(const std::vector<HWND> &controls)
{
    for (HWND hwnd : controls)
    {
        ShowWindow(hwnd, SW_SHOW);
    }
}

void SwitchToSection(int section)
{
    // Hide all sections
    HideAllControls(loginControls);
    HideAllControls(registerControls);
    HideAllControls(searchControls);
    HideAllControls(addPropertyControls);

    // Show selected section
    switch (section)
    {
    case IDC_BTN_SECTION_LOGIN:
        ShowControls(loginControls);
        break;
    case IDC_BTN_SECTION_REGISTER:
        ShowControls(registerControls);
        break;
    case IDC_BTN_SECTION_SEARCH:
        if (isLoggedIn)
        {
            ShowControls(searchControls);
        }
        else
        {
            MessageBoxA(NULL, "Please login first!", "Access Denied", MB_ICONWARNING);
            ShowControls(loginControls);
        }
        break;
    case IDC_BTN_SECTION_ADD:
        if (isLoggedIn)
        {
            ShowControls(addPropertyControls);
        }
        else
        {
            MessageBoxA(NULL, "Please login first!", "Access Denied", MB_ICONWARNING);
            ShowControls(loginControls);
        }
        break;
    }
}

void SaveProperties()
{
    std::ofstream fout(PROPERTY_FILE);
    for (auto &p : properties)
    {
        fout << std::get<0>(p) << "," << std::get<1>(p) << "," << std::get<2>(p) << ","
            << std::get<3>(p) << "," << std::get<4>(p) << "\n";
    }
    fout.close();
}


void BubbleSortByPrice()
{
    int n = (int)properties.size();
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = 0; j < n - i - 1; ++j)
        {
            if (std::get<2>(properties[j]) > std::get<2>(properties[j + 1]))
            {
                std::swap(properties[j], properties[j + 1]);
            }
        }
    }
}


int BinarySearchByPrice(int price)
{
    int L = 0, R = (int)properties.size() - 1;
    while (L <= R)
    {
        int mid = L + (R - L) / 2;
        int val = std::get<2>(properties[mid]);
        if (val == price)
            return mid;
        else if (val < price)
            L = mid + 1;
        else
            R = mid - 1;
    }
    return -1;
}

// ListView helpers
void InitListViewColumns(HWND lv)
{
    LVCOLUMN col = {0};
    col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    col.cx = 60;
    col.pszText = const_cast<LPSTR>("No.");
    ListView_InsertColumn(lv, 0, &col);
    col.cx = 110;
    col.pszText = const_cast<LPSTR>("Type");
    ListView_InsertColumn(lv, 1, &col);
    col.cx = 120;
    col.pszText = const_cast<LPSTR>("Location");
    ListView_InsertColumn(lv, 2, &col);
    col.cx = 90;
    col.pszText = const_cast<LPSTR>("Price");
    ListView_InsertColumn(lv, 3, &col);
    col.cx = 80;
    col.pszText = const_cast<LPSTR>("Area");
    ListView_InsertColumn(lv, 4, &col);
    col.cx = 120;
    col.pszText = const_cast<LPSTR>("Owner");
    ListView_InsertColumn(lv, 5, &col);
}

void PopulateListView(HWND lv, const std::vector<int> &indices)
{
    ListView_DeleteAllItems(lv);
    LVITEM item = {0};
    item.mask = LVIF_TEXT;
    char buf[256];
    int pos = 0;
    for (int idx : indices)
    {
        item.iItem = pos;
        snprintf(buf, sizeof(buf), "%d", pos + 1);
        item.iSubItem = 0;
        item.pszText = buf;
        ListView_InsertItem(lv, &item);
        // columns
        snprintf(buf, sizeof(buf), "%s", std::get<0>(properties[idx]).c_str());
        ListView_SetItemText(lv, pos, 1, buf);
        snprintf(buf, sizeof(buf), "%s", std::get<1>(properties[idx]).c_str());
        ListView_SetItemText(lv, pos, 2, buf);
        snprintf(buf, sizeof(buf), "%d", std::get<2>(properties[idx]));
        ListView_SetItemText(lv, pos, 3, buf);
        snprintf(buf, sizeof(buf), "%d", std::get<3>(properties[idx]));
        ListView_SetItemText(lv, pos, 4, buf);
        snprintf(buf, sizeof(buf), "%s", std::get<4>(properties[idx]).c_str());
        ListView_SetItemText(lv, pos, 5, buf);
        ++pos;
    }
}

// Fill whole list with current properties order
void RefreshListViewAll(HWND lv)
{
    std::vector<int> idx(properties.size());
    for (int i = 0; i < (int)properties.size(); ++i)
        idx[i] = i;
    PopulateListView(hListView, idx);
}

// Login handler
void OnLogin(HWND hWnd)
{
    char user[100], pass[100];
    GetWindowTextA(hLoginUsername, user, sizeof(user));
    GetWindowTextA(hLoginPassword, pass, sizeof(pass));

    if (strlen(user) == 0 || strlen(pass) == 0)
    {
        MessageBoxA(hWnd, "Please enter username and password.", "Error", MB_ICONERROR);
        return;
    }

    if (ValidateUser(user, pass))
    {
        isLoggedIn = true;
        currentUser = user;
        MessageBoxA(hWnd, "Login successful! You can now access Search and Add Property sections.", "Success", MB_ICONINFORMATION);
        SwitchToSection(IDC_BTN_SECTION_SEARCH);
    }
    else
    {
        MessageBoxA(hWnd, "Invalid username or password!", "Login Failed", MB_ICONERROR);
    }
}

// Register handler
void OnRegister(HWND hWnd)
{
    char user[100], pass[100], confirmPass[100];
    GetWindowTextA(hRegUsername, user, sizeof(user));
    GetWindowTextA(hRegPassword, pass, sizeof(pass));
    GetWindowTextA(hRegConfirmPass, confirmPass, sizeof(confirmPass));

    if (strlen(user) == 0 || strlen(pass) == 0 || strlen(confirmPass) == 0)
    {
        MessageBoxA(hWnd, "Please fill all fields.", "Error", MB_ICONERROR);
        return;
    }

    if (strcmp(pass, confirmPass) != 0)
    {
        MessageBoxA(hWnd, "Passwords do not match!", "Error", MB_ICONERROR);
        return;
    }

    if (UserExists(user))
    {
        MessageBoxA(hWnd, "Username already exists!", "Error", MB_ICONERROR);
        return;
    }

    SaveUser(user, pass);
    MessageBoxA(hWnd, "Registration successful! You can now login.", "Success", MB_ICONINFORMATION);
    SwitchToSection(IDC_BTN_SECTION_LOGIN);
}

// Add property from UI
void OnAddProperty(HWND hWnd)
{
    char buf[256];
    GetWindowTextA(hType, buf, sizeof(buf));
    std::string type = buf;
    GetWindowTextA(hLocation, buf, sizeof(buf));
    std::string loc = buf;
    GetWindowTextA(hPrice, buf, sizeof(buf));
    std::string priceS = buf;
    GetWindowTextA(hArea, buf, sizeof(buf));
    std::string areaS = buf;
    GetWindowTextA(hOwner, buf, sizeof(buf));
    std::string owner = buf;

    if (type.empty() || loc.empty() || priceS.empty() || areaS.empty() || owner.empty())
    {
        MessageBoxA(hWnd, "Please fill all fields.", "Error", MB_ICONERROR);
        return;
    }

    try
    {
        int price = std::stoi(priceS);
        int area = std::stoi(areaS);
        type = ToUpper(type);
        loc = ToUpper(loc);
        properties.emplace_back(type, loc, price, area, owner);
        SaveProperties();
        MessageBoxA(hWnd, "Property added successfully.", "Success", MB_ICONINFORMATION);

        // Clear input fields after successful addition
        SetWindowTextA(hType, "");
        SetWindowTextA(hLocation, "");
        SetWindowTextA(hPrice, "");
        SetWindowTextA(hArea, "");
        SetWindowTextA(hOwner, "");

        // refresh list
        RefreshListViewAll(hListView);
    }
    catch (...)
    {
        MessageBoxA(hWnd, "Price and Area must be numbers.", "Error", MB_ICONERROR);
    }
}

// Show all properties (sorted by price)
void OnShowAll(HWND)
{
    BubbleSortByPrice();
    SaveProperties();
    RefreshListViewAll(hListView);
}

void OnSort(HWND)
{
    BubbleSortByPrice();
    SaveProperties();
    RefreshListViewAll(hListView);
}

// Search by type (case-insensitive)
void OnSearchByType(HWND hWnd)
{
    char buf[256];
    GetWindowTextA(hSearchType, buf, sizeof(buf));
    std::string type = ToUpper(buf);
    if (type.empty())
    {
        MessageBoxA(hWnd, "Enter Type to search.", "Info", MB_OK);
        return;
    }
    std::vector<int> results;
    for (int i = 0; i < (int)properties.size(); ++i)
    {
        if (ToUpper(std::get<0>(properties[i])) == type)
            results.push_back(i);
    }
    if (results.empty())
        MessageBoxA(hWnd, "No matching properties found.", "Info", MB_OK);
    else
        MessageBoxA(hWnd, ("Found " + std::to_string(results.size()) + " properties!").c_str(), "Search Results", MB_ICONINFORMATION);
    PopulateListView(hListView, results);
}

// Search by location
void OnSearchByLocation(HWND hWnd)
{
    char buf[256];
    GetWindowTextA(hSearchLocation, buf, sizeof(buf));
    std::string loc = ToUpper(buf);
    if (loc.empty())
    {
        MessageBoxA(hWnd, "Enter Location to search.", "Info", MB_OK);
        return;
    }
    std::vector<int> results;
    for (int i = 0; i < (int)properties.size(); ++i)
    {
        if (ToUpper(std::get<1>(properties[i])) == loc)
            results.push_back(i);
    }
    if (results.empty())
        MessageBoxA(hWnd, "No matching properties found.", "Info", MB_OK);
    else
        MessageBoxA(hWnd, ("Found " + std::to_string(results.size()) + " properties!").c_str(), "Search Results", MB_ICONINFORMATION);
    PopulateListView(hListView, results);
}

// Search by price range (linear)
void OnSearchByRange(HWND hWnd)
{
    char inputMin[32] = "", inputMax[32] = "";

    // Use search-specific fields
    GetWindowTextA(hSearchMinPrice, inputMin, sizeof(inputMin));
    GetWindowTextA(hSearchMaxPrice, inputMax, sizeof(inputMax));

    if (strlen(inputMin) == 0 || strlen(inputMax) == 0)
    {
        MessageBoxA(hWnd, "Enter both minimum and maximum prices.", "Info", MB_OK);
        return;
    }

    try
    {
        int minP = std::stoi(inputMin);
        int maxP = std::stoi(inputMax);
        std::vector<int> results;
        for (int i = 0; i < (int)properties.size(); ++i)
        {
            int p = std::get<2>(properties[i]);
            if (p >= minP && p <= maxP)
                results.push_back(i);
        }
        if (results.empty())
            MessageBoxA(hWnd, "No matching properties found.", "Info", MB_OK);
        else
            MessageBoxA(hWnd, ("Found " + std::to_string(results.size()) + " properties in price range " + std::to_string(minP) + " - " + std::to_string(maxP) + "!").c_str(), "Search Results", MB_ICONINFORMATION);
        PopulateListView(hListView, results);
    }
    catch (...)
    {
        MessageBoxA(hWnd, "Invalid input for range.", "Error", MB_ICONERROR);
    }
}

// Search exact price using binary search (only after sorting)
void OnSearchExactPrice(HWND hWnd)
{
    char buf[256];
    GetWindowTextA(hSearchPrice, buf, sizeof(buf));
    if (strlen(buf) == 0)
    {
        MessageBoxA(hWnd, "Enter exact price to search.", "Info", MB_OK);
        return;
    }
    try
    {
        int price = std::stoi(buf);
        if (properties.empty())
        {
            MessageBoxA(hWnd, "No properties available.", "Info", MB_OK);
            return;
        }
        BubbleSortByPrice();
        SaveProperties();
        int idx = BinarySearchByPrice(price);
        if (idx == -1)
        {
            MessageBoxA(hWnd, "No matching property found.", "Info", MB_OK);
            return;
        }
        MessageBoxA(hWnd, "Found matching property!", "Search Results", MB_ICONINFORMATION);
        std::vector<int> one = {idx};
        PopulateListView(hListView, one);
    }
    catch (...)
    {
        MessageBoxA(hWnd, "Invalid price input.", "Error", MB_ICONERROR);
    }
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        // Title
        CreateWindowA("STATIC", "Real Estate Management System",
                    WS_VISIBLE | WS_CHILD | SS_CENTER,
                    180, 10, 500, 30, hWnd, NULL, ghInst, NULL);

        // Navigation Buttons (Always visible)
        CreateWindowA("BUTTON", "Login", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    20, 50, 100, 35, hWnd, (HMENU)IDC_BTN_SECTION_LOGIN, ghInst, NULL);
        CreateWindowA("BUTTON", "Register", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    130, 50, 100, 35, hWnd, (HMENU)IDC_BTN_SECTION_REGISTER, ghInst, NULL);
        CreateWindowA("BUTTON", "Search Property", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    240, 50, 130, 35, hWnd, (HMENU)IDC_BTN_SECTION_SEARCH, ghInst, NULL);
        CreateWindowA("BUTTON", "Add Property", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    380, 50, 130, 35, hWnd, (HMENU)IDC_BTN_SECTION_ADD, ghInst, NULL);

        // ============= LOGIN SECTION =============
        HWND h;
        h = CreateWindowA("STATIC", "=== LOGIN ===", WS_CHILD | SS_CENTER,
                        250, 110, 300, 25, hWnd, NULL, ghInst, NULL);
        loginControls.push_back(h);

        h = CreateWindowA("STATIC", "Username:", WS_CHILD,
                        150, 150, 100, 20, hWnd, NULL, ghInst, NULL);
        loginControls.push_back(h);
        hLoginUsername = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                        WS_CHILD | ES_AUTOHSCROLL,
                                        270, 150, 200, 24, hWnd, (HMENU)IDC_LOGIN_USERNAME, ghInst, NULL);
        loginControls.push_back(hLoginUsername);

        h = CreateWindowA("STATIC", "Password:", WS_CHILD,
                        150, 190, 100, 20, hWnd, NULL, ghInst, NULL);
        loginControls.push_back(h);
        hLoginPassword = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                        WS_CHILD | ES_PASSWORD | ES_AUTOHSCROLL,
                                        270, 190, 200, 24, hWnd, (HMENU)IDC_LOGIN_PASSWORD, ghInst, NULL);
        loginControls.push_back(hLoginPassword);

        h = CreateWindowA("BUTTON", "LOGIN", WS_CHILD | BS_PUSHBUTTON,
                        300, 240, 120, 35, hWnd, (HMENU)IDC_BTN_LOGIN, ghInst, NULL);
        loginControls.push_back(h);

        // ============= REGISTER SECTION =============
        h = CreateWindowA("STATIC", "=== REGISTER ===", WS_CHILD | SS_CENTER,
                        250, 110, 300, 25, hWnd, NULL, ghInst, NULL);
        registerControls.push_back(h);

        h = CreateWindowA("STATIC", "Username:", WS_CHILD,
                        150, 150, 100, 20, hWnd, NULL, ghInst, NULL);
        registerControls.push_back(h);
        hRegUsername = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                    WS_CHILD | ES_AUTOHSCROLL,
                                    270, 150, 200, 24, hWnd, (HMENU)IDC_REG_USERNAME, ghInst, NULL);
        registerControls.push_back(hRegUsername);

        h = CreateWindowA("STATIC", "Password:", WS_CHILD,
                        150, 190, 100, 20, hWnd, NULL, ghInst, NULL);
        registerControls.push_back(h);
        hRegPassword = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                    WS_CHILD | ES_PASSWORD | ES_AUTOHSCROLL,
                                    270, 190, 200, 24, hWnd, (HMENU)IDC_REG_PASSWORD, ghInst, NULL);
        registerControls.push_back(hRegPassword);

        h = CreateWindowA("STATIC", "Confirm Password:", WS_CHILD,
                        150, 230, 110, 20, hWnd, NULL, ghInst, NULL);
        registerControls.push_back(h);
        hRegConfirmPass = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                        WS_CHILD | ES_PASSWORD | ES_AUTOHSCROLL,
                                        270, 230, 200, 24, hWnd, (HMENU)IDC_REG_CONFIRM_PASS, ghInst, NULL);
        registerControls.push_back(hRegConfirmPass);

        h = CreateWindowA("BUTTON", "REGISTER", WS_CHILD | BS_PUSHBUTTON,
                        300, 280, 120, 35, hWnd, (HMENU)IDC_BTN_REGISTER, ghInst, NULL);
        registerControls.push_back(h);

        // ============= ADD PROPERTY SECTION =============
        h = CreateWindowA("STATIC", "=== ADD PROPERTY ===", WS_CHILD | SS_CENTER,
                        250, 110, 300, 25, hWnd, NULL, ghInst, NULL);
        addPropertyControls.push_back(h);

        h = CreateWindowA("STATIC", "Type:", WS_CHILD,
                        30, 150, 80, 20, hWnd, NULL, ghInst, NULL);
        addPropertyControls.push_back(h);
        hType = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                WS_CHILD | ES_AUTOHSCROLL,
                                120, 150, 160, 24, hWnd, (HMENU)IDC_TYPE, ghInst, NULL);
        addPropertyControls.push_back(hType);

        h = CreateWindowA("STATIC", "Location:", WS_CHILD,
                        300, 150, 80, 20, hWnd, NULL, ghInst, NULL);
        addPropertyControls.push_back(h);
        hLocation = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                    WS_CHILD | ES_AUTOHSCROLL,
                                    390, 150, 160, 24, hWnd, (HMENU)IDC_LOCATION, ghInst, NULL);
        addPropertyControls.push_back(hLocation);

        h = CreateWindowA("STATIC", "Price:", WS_CHILD,
                        30, 190, 80, 20, hWnd, NULL, ghInst, NULL);
        addPropertyControls.push_back(h);
        hPrice = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                WS_CHILD | ES_AUTOHSCROLL,
                                120, 190, 160, 24, hWnd, (HMENU)IDC_PRICE, ghInst, NULL);
        addPropertyControls.push_back(hPrice);

        h = CreateWindowA("STATIC", "Area (sq.ft):", WS_CHILD,
                        300, 190, 80, 20, hWnd, NULL, ghInst, NULL);
        addPropertyControls.push_back(h);
        hArea = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                WS_CHILD | ES_AUTOHSCROLL,
                                390, 190, 160, 24, hWnd, (HMENU)IDC_AREA, ghInst, NULL);
        addPropertyControls.push_back(hArea);

        h = CreateWindowA("STATIC", "Owner:", WS_CHILD,
                        30, 230, 80, 20, hWnd, NULL, ghInst, NULL);
        addPropertyControls.push_back(h);
        hOwner = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                WS_CHILD | ES_AUTOHSCROLL,
                                120, 230, 160, 24, hWnd, (HMENU)IDC_OWNER, ghInst, NULL);
        addPropertyControls.push_back(hOwner);

        h = CreateWindowA("BUTTON", "Add Property", WS_CHILD | BS_PUSHBUTTON,
                        220, 280, 140, 35, hWnd, (HMENU)IDC_BTN_ADD, ghInst, NULL);
        addPropertyControls.push_back(h);

        // ============= SEARCH SECTION =============
        h = CreateWindowA("STATIC", "=== SEARCH PROPERTY ===", WS_CHILD | SS_CENTER,
                        250, 110, 300, 25, hWnd, NULL, ghInst, NULL);
        searchControls.push_back(h);

        // Search Input Fields
        h = CreateWindowA("STATIC", "Type:", WS_CHILD,
                        30, 150, 80, 20, hWnd, NULL, ghInst, NULL);
        searchControls.push_back(h);
        hSearchType = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                    WS_CHILD | ES_AUTOHSCROLL,
                                    120, 150, 160, 24, hWnd, (HMENU)IDC_SEARCH_TYPE, ghInst, NULL);
        searchControls.push_back(hSearchType);

        h = CreateWindowA("STATIC", "Location:", WS_CHILD,
                        300, 150, 80, 20, hWnd, NULL, ghInst, NULL);
        searchControls.push_back(h);
        hSearchLocation = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                        WS_CHILD | ES_AUTOHSCROLL,
                                        390, 150, 160, 24, hWnd, (HMENU)IDC_SEARCH_LOCATION, ghInst, NULL);
        searchControls.push_back(hSearchLocation);

        h = CreateWindowA("STATIC", "Exact Price:", WS_CHILD,
                        30, 190, 80, 20, hWnd, NULL, ghInst, NULL);
        searchControls.push_back(h);
        hSearchPrice = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                    WS_CHILD | ES_AUTOHSCROLL,
                                    120, 190, 160, 24, hWnd, (HMENU)IDC_SEARCH_PRICE, ghInst, NULL);
        searchControls.push_back(hSearchPrice);

        h = CreateWindowA("STATIC", "Min Price:", WS_CHILD,
                        300, 190, 80, 20, hWnd, NULL, ghInst, NULL);
        searchControls.push_back(h);
        hSearchMinPrice = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                        WS_CHILD | ES_AUTOHSCROLL,
                                        390, 190, 80, 24, hWnd, (HMENU)IDC_SEARCH_MIN_PRICE, ghInst, NULL);
        searchControls.push_back(hSearchMinPrice);

        h = CreateWindowA("STATIC", "Max Price:", WS_CHILD,
                        480, 190, 70, 20, hWnd, NULL, ghInst, NULL);
        searchControls.push_back(h);
        hSearchMaxPrice = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                                        WS_CHILD | ES_AUTOHSCROLL,
                                        560, 190, 80, 24, hWnd, (HMENU)IDC_SEARCH_MAX_PRICE, ghInst, NULL);
        searchControls.push_back(hSearchMaxPrice);

        // Search Buttons
        h = CreateWindowA("BUTTON", "Show All (Sorted)", WS_CHILD | BS_PUSHBUTTON,
                        30, 230, 130, 32, hWnd, (HMENU)IDC_BTN_SHOW, ghInst, NULL);
        searchControls.push_back(h);

        h = CreateWindowA("BUTTON", "Sort by Price", WS_CHILD | BS_PUSHBUTTON,
                        170, 230, 120, 32, hWnd, (HMENU)IDC_BTN_SORT, ghInst, NULL);
        searchControls.push_back(h);

        h = CreateWindowA("BUTTON", "Search by Type", WS_CHILD | BS_PUSHBUTTON,
                        300, 230, 130, 32, hWnd, (HMENU)IDC_BTN_SEARCH_TYPE, ghInst, NULL);
        searchControls.push_back(h);

        h = CreateWindowA("BUTTON", "Search by Location", WS_CHILD | BS_PUSHBUTTON,
                        440, 230, 140, 32, hWnd, (HMENU)IDC_BTN_SEARCH_LOC, ghInst, NULL);
        searchControls.push_back(h);

        h = CreateWindowA("BUTTON", "Search Price Range", WS_CHILD | BS_PUSHBUTTON,
                        30, 270, 140, 32, hWnd, (HMENU)IDC_BTN_SEARCH_RANGE, ghInst, NULL);
        searchControls.push_back(h);

        h = CreateWindowA("BUTTON", "Search Exact Price", WS_CHILD | BS_PUSHBUTTON,
                        180, 270, 140, 32, hWnd, (HMENU)IDC_BTN_SEARCH_EXACT, ghInst, NULL);
        searchControls.push_back(h);

        // ListView (shared between search and add)
        InitCommonControls();
        hListView = CreateWindowExA(WS_EX_CLIENTEDGE, WC_LISTVIEWA, "",
                                    WS_CHILD | LVS_REPORT | LVS_SINGLESEL,
                                    30, 315, 700, 270, hWnd, (HMENU)IDC_LISTVIEW, ghInst, NULL);
        searchControls.push_back(hListView);
        addPropertyControls.push_back(hListView);

        SendMessage(hListView, 0x1036, 0, 0x00000001 | 0x00000020);
        InitListViewColumns(hListView);

        // Load existing properties
        LoadProperties();
        RefreshListViewAll(hListView);

        // Start with LOGIN section visible
        SwitchToSection(IDC_BTN_SECTION_LOGIN);
    }
    break;

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        if (HIWORD(wParam) == BN_CLICKED)
        {
            switch (id)
            {
            // Section Navigation
            case IDC_BTN_SECTION_LOGIN:
                SwitchToSection(IDC_BTN_SECTION_LOGIN);
                break;
            case IDC_BTN_SECTION_REGISTER:
                SwitchToSection(IDC_BTN_SECTION_REGISTER);
                break;
            case IDC_BTN_SECTION_SEARCH:
                SwitchToSection(IDC_BTN_SECTION_SEARCH);
                break;
            case IDC_BTN_SECTION_ADD:
                SwitchToSection(IDC_BTN_SECTION_ADD);
                break;

            // Login/Register Actions
            case IDC_BTN_LOGIN:
                OnLogin(hWnd);
                break;
            case IDC_BTN_REGISTER:
                OnRegister(hWnd);
                break;

            // Property Actions
            case IDC_BTN_ADD:
                OnAddProperty(hWnd);
                break;
            case IDC_BTN_SHOW:
                OnShowAll(hWnd);
                break;
            case IDC_BTN_SORT:
                OnSort(hWnd);
                break;
            case IDC_BTN_SEARCH_TYPE:
                OnSearchByType(hWnd);
                break;
            case IDC_BTN_SEARCH_LOC:
                OnSearchByLocation(hWnd);
                break;
            case IDC_BTN_SEARCH_RANGE:
                OnSearchByRange(hWnd);
                break;
            case IDC_BTN_SEARCH_EXACT:
                OnSearchExactPrice(hWnd);
                break;
            }
        }
    }
    break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcA(hWnd, msg, wParam, lParam);
    }
    return 0;
}

// WinMain
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    ghInst = hInstance;
    const char CLASS_NAME[] = "RealEstateClass";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = ghInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(RGB(240, 248, 255)); // light background (alice blue)
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassA(&wc);

    HWND hWnd = CreateWindowExA(0, CLASS_NAME, "Real Estate Management System",
                                WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, // not resizable
                                CW_USEDEFAULT, CW_USEDEFAULT, 780, 640,
                                NULL, NULL, ghInst, NULL);

    if (!hWnd)
        return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // message loop
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int)msg.wParam;
}