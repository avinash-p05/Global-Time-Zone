# Global Time Zone Application

A C++ command-line utility for accessing accurate global time zone information using the TimeZoneDB API.

## Features

- Display current UTC time
- View current time in major time zones around the world
- Get detailed information for specific time zones
- Convert times between different time zones with DST support
- Search for time zones by region or country name
- Save and manage favorite time zones for quick access

## Screenshots

![image](https://github.com/user-attachments/assets/fd51dfe5-b8e6-4515-940e-9d3c003dcf85)
![image](https://github.com/user-attachments/assets/66209ad0-20f4-4c7a-a6b0-889a7aef4ba9)
![image](https://github.com/user-attachments/assets/1510fa42-0485-4723-8290-ac69f37313f8)
![image](https://github.com/user-attachments/assets/d36af854-6258-4ff0-920c-d77298752812)
![image](https://github.com/user-attachments/assets/a7e74320-ff9a-4b70-9683-76539b915789)
![image](https://github.com/user-attachments/assets/885d3a71-3701-46f2-a91d-30b70b0a4c30)
![image](https://github.com/user-attachments/assets/5ce0260b-2d88-4c7a-a956-81456c7c35c3)
![image](https://github.com/user-attachments/assets/5b6c9796-979a-4245-801e-fa8c9834d2ac)

## Technologies Used

- C++17
- CURL for HTTP requests
- nlohmann/json for JSON parsing
- CMake build system

## Requirements

- C++17 compatible compiler
- CURL library
- CMake 3.10 or higher
- Internet connection for API access

## Building the Application

1. Clone the repository
```
git clone https://github.com/yourusername/global-timezone-app.git
cd global-timezone-app
```

2. Create a build directory
```
mkdir build
cd build
```

3. Configure with CMake
```
cmake ..
```

4. Build the application
```
cmake --build .
```

## Usage

The application provides a simple menu-driven interface:

1. View current UTC time
2. View current time in major time zones
3. View current time in a specific time zone
4. Convert time between time zones
5. Manage favorite time zones
6. View favorite time zones
7. Search time zones
8. Help
9. Exit

### Examples

- To view the current time in a specific zone, select option 3 and enter the time zone identifier (e.g., "America/New_York")
- To convert time between zones, select option 4 and follow the prompts for source zone, target zone, and the time to convert
- To search for time zones, select option 7 and enter a search term like "Tokyo" or "Australia"

## API Information

This application uses the TimeZoneDB API to retrieve time zone data. The free tier supports up to 1 request per second, which is sufficient for individual use.

If you're modifying the code, you'll need to register for a free API key at [TimeZoneDB](https://timezonedb.com/register) and replace the API_KEY constant in main.cpp.

## Architecture

The application follows a three-tier architecture:

1. **Client Application Layer** - User interface and application logic
2. **API Layer** - Communication with external services
3. **External Services Layer** - TimeZoneDB REST API

## Configuration

User configuration, including favorites, is stored in a JSON file at `data/user_config.json`. This file is created automatically when you add your first favorite time zone.

## Limitations

- Requires internet connection for full functionality
- Subject to API rate limits (1 request per second on free tier)
- Limited offline capabilities

## Future Improvements

- Graphical user interface
- Caching of user data and time zone data
- Calendar integration for scheduling across time zones


## Author

Avinash Pauskar

## Acknowledgments

- [TimeZoneDB](https://timezonedb.com) for providing the time zone API
- [nlohmann/json](https://github.com/nlohmann/json) for the JSON library
- [libcurl](https://curl.se/libcurl/) for HTTP request functionality
