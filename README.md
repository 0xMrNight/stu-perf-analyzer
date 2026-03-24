# Smart Student Performance Analyzer

An application designed to manage student records and provide performance insights. 

This project combines the performance of a **C-based backend** with the accessibility of a Python web interface**.

Developed for `BACSE104`: Structured and Object Oriented Programming - Digital Assignment 1

## 🚀 Features

- **Binary File Storage:** Data is persisted in a binary `.dat` file using C structures for high efficiency.
- **Student Management:**
  - Add new records (RegNo, Name, 3 Subject Marks).
  - Update existing marks.
  - Delete records.
  - Search by Registration Number.
- **Performance Insights:**
  - Real-time calculation of Total, Average, and Grade (A/B/C/Fail).
  - Automatic identification of the Class Topper.
  - Class statistics including overall average and grade distribution.
- **Modern Web UI:** Responsive interface built with Bootstrap 5.

## 🏗️ Architecture

The application uses a **hybrid architecture**:
1. **Backend (C):** A menu-driven and CLI-enabled C program (`main.c`) handles all data processing and file I/O operations.
2. **Frontend (Python/Flask):** A Flask application (`app.py`) serves the web interface. It communicates with the C backend using the `subprocess` module, passing user input and parsing the output for display.
3. **Deployment (Render):** A custom `build.sh` script compiles the C backend and sets up the Python environment during deployment.

## Local Setup

### Prerequisites
- GCC (C Compiler)
- Python 3.x

### Installation
1. **Clone the repository:**
   ```bash
   git clone https://github.com/0xMrNight/stu-perf-analyzer
   cd stu-perf-analyzer
   ```

2. **Run the build script:**
   This will compile the C backend into an `analyzer` executable and install Flask dependencies.
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

3. **Start the application:**
   ```bash
   gunicorn app:app
   ```
4. Open your browser and navigate to `http://127.0.0.1:8000`.

