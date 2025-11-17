# Real Estate Management System

A desktop application for managing real estate properties built using pure Win32 API in C++. This system provides user authentication, property management, and advanced search capabilities with a clean graphical interface.

## 📋 Features

### User Management
- **User Registration**: Create new user accounts with password confirmation
- **User Login**: Secure authentication system
- **Access Control**: Login required to access property management features

### Property Management
- **Add Properties**: Add new property listings with details:
  - Property Type (House, Apartment, Villa, etc.)
  - Location
  - Price
  - Area (in square feet)
  - Owner Name
- **View All Properties**: Display all properties in a sortable list view
- **Auto-clear Forms**: Input fields automatically clear after successful property addition

### Search & Filter
- **Search by Type**: Find properties by type (case-insensitive)
- **Search by Location**: Filter properties by location
- **Search by Exact Price**: Binary search for exact price match
- **Search by Price Range**: Find properties within a minimum and maximum price range
- **Sort by Price**: Bubble sort algorithm to organize properties by price

### Data Persistence
- Properties saved to `properties.csv`
- User credentials stored in `users.csv`
- Automatic loading on application startup

## 🛠️ Technical Details

### Technologies Used
- **Language**: C++
- **GUI Framework**: Win32 API
- **Controls**: Common Controls Library (comctl32)
- **Data Structure**: Vector of tuples for property storage
- **File I/O**: CSV-based file handling

### Algorithms Implemented
- **Bubble Sort**: For sorting properties by price in ascending order
- **Binary Search**: For exact price lookup (requires sorted data)
- **Linear Search**: For type and location searches

### System Requirements
- **Operating System**: Windows 7 or later
- **Compiler**: 
  - Visual Studio (MSVC) with C++ support, OR
  - MinGW-w64 (GCC for Windows)
- **Required Libraries**: comctl32.lib, gdi32.lib

## 🚀 Installation & Setup

### Option 1: Compile with MinGW (g++)

```bash
# Navigate to project directory

# Compile the application
g++ main.cpp -o RealEstateApp.exe -lcomctl32 -lgdi32 -mwindows

# Run the application
.\RealEstateApp.exe
```

### Option 2: Compile with Visual Studio

```bash
# Open Visual Studio Developer Command Prompt and navigate to project directory

# Compile with MSVC
cl /EHsc main.cpp /link comctl32.lib user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup

# Run the application
.\main.exe
```

## 📖 Usage Guide

### 1. Getting Started
- Launch the application
- You'll see the **Login** section by default
- Use the navigation buttons at the top to switch between sections

### 2. First Time Setup
1. Click **"Register"** button
2. Enter your desired username
3. Enter a password
4. Confirm your password
5. Click **"REGISTER"**
6. You'll be redirected to the login page

### 3. Login
1. Click **"Login"** button (if not already on login page)
2. Enter your username
3. Enter your password
4. Click **"LOGIN"**
5. Upon successful login, you'll be redirected to the Search Property section

### 4. Adding Properties
1. Click **"Add Property"** button in the navigation bar
2. Fill in all fields:
   - **Type**: e.g., House, Apartment, Villa, Land
   - **Location**: e.g., Mumbai, Delhi, Bangalore
   - **Price**: Enter numeric value
   - **Area**: Enter area in square feet (numeric)
   - **Owner**: Owner's name
3. Click **"Add Property"**
4. The property list will update automatically
5. Input fields will be cleared for the next entry

### 5. Searching Properties

#### Show All Properties (Sorted)
- Click **"Show All (Sorted)"** to display all properties sorted by price

#### Sort Existing View
- Click **"Sort by Price"** to sort currently displayed properties

#### Search by Type
1. Enter property type in the **"Type"** field
2. Click **"Search by Type"**

#### Search by Location
1. Enter location in the **"Location"** field
2. Click **"Search by Location"**

#### Search by Exact Price
1. Enter exact price in the **"Exact Price"** field
2. Click **"Search Exact Price"** (uses binary search)

#### Search by Price Range
1. Enter minimum price in the **"Min Price"** field
2. Enter maximum price in the **"Max Price"** field
3. Click **"Search Price Range"**

## 📂 Project Structure

```
Project/
│
├── main.cpp              # Main application source code
├── RealEstateApp.exe     # Compiled executable
├── properties.csv        # Property data storage (auto-generated)
├── users.csv             # User credentials storage (auto-generated)
└── README.md             # This file
```

## 🔧 Control IDs Reference

### Section Buttons
- `IDC_BTN_SECTION_LOGIN` (50)
- `IDC_BTN_SECTION_REGISTER` (51)
- `IDC_BTN_SECTION_SEARCH` (52)
- `IDC_BTN_SECTION_ADD` (53)

### Login Controls
- `IDC_LOGIN_USERNAME` (60)
- `IDC_LOGIN_PASSWORD` (61)
- `IDC_BTN_LOGIN` (62)

### Register Controls
- `IDC_REG_USERNAME` (70)
- `IDC_REG_PASSWORD` (71)
- `IDC_REG_CONFIRM_PASS` (72)
- `IDC_BTN_REGISTER` (73)

### Add Property Controls
- `IDC_TYPE` (101)
- `IDC_LOCATION` (102)
- `IDC_PRICE` (103)
- `IDC_AREA` (104)
- `IDC_OWNER` (105)
- `IDC_BTN_ADD` (201)

### Search Controls
- `IDC_SEARCH_TYPE` (110)
- `IDC_SEARCH_LOCATION` (111)
- `IDC_SEARCH_PRICE` (112)
- `IDC_SEARCH_MIN_PRICE` (113)
- `IDC_SEARCH_MAX_PRICE` (114)

### Action Buttons
- `IDC_BTN_SHOW` (202)
- `IDC_BTN_SORT` (203)
- `IDC_BTN_SEARCH_TYPE` (204)
- `IDC_BTN_SEARCH_LOC` (205)
- `IDC_BTN_SEARCH_RANGE` (206)
- `IDC_BTN_SEARCH_EXACT` (207)

## 💾 Data Format

### properties.csv
```
Type,Location,Price,Area,Owner
HOUSE,MUMBAI,5000000,1500,John Doe
APARTMENT,DELHI,3500000,1200,Jane Smith
VILLA,BANGALORE,8000000,2500,Bob Johnson
```

### users.csv
```
username,password
admin,admin123
user1,pass123
```

> ⚠️ **Note**: Passwords are stored in plain text. This is suitable for educational purposes only. In production, always use proper password hashing (e.g., bcrypt, SHA-256).

## 🎨 User Interface

- **Window Size**: 780 x 640 pixels (fixed size)
- **Background Color**: Alice Blue (RGB: 240, 248, 255)
- **Layout**: Section-based navigation with dedicated panels
- **ListView**: 6 columns (No., Type, Location, Price, Area, Owner)

## 🔒 Security Notes

⚠️ **Important**: This application is designed for educational purposes and includes the following security considerations:

1. **Plain Text Passwords**: User passwords are stored in plain text in `users.csv`. For production use, implement proper password hashing.
2. **File Permissions**: CSV files have no access restrictions. Consider implementing file encryption for sensitive data.
3. **Input Validation**: Basic validation is implemented, but additional sanitization may be needed for production.

## 🐛 Troubleshooting

### Application won't compile
- Ensure you have a C++ compiler installed (MinGW or Visual Studio)
- Check that comctl32 and gdi32 libraries are linked properly
- Verify all required Windows SDK components are installed

### CSV files not found
- Files are created automatically on first use
- Ensure the application has write permissions in the directory

### Search not returning results
- Type and Location searches are case-insensitive
- Exact price search requires the list to be sorted first
- Check that your search criteria match existing properties

## 📝 Future Enhancements

- [ ] Password encryption/hashing
- [ ] Property image support
- [ ] Export to PDF/Excel functionality
- [ ] Property editing and deletion
- [ ] Advanced filtering (multiple criteria)
- [ ] User roles (Admin/User)
- [ ] Property booking/reservation system
- [ ] Database integration (SQLite/MySQL)

## 👨‍💻 Author

**Sahil**  
Computer Science Student  



