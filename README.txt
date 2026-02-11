# Movie Query Client

A C++ command-line client that retrieves a movie dataset over HTTP and executes
simple column-selection queries with an optional WHERE-style filter.

This project demonstrates:

- TCP networking with Boost.Asio
- Manual HTTP request/response handling
- Stream-based parsing
- Tokenization and string processing
- In-memory filtering and formatting

---

## Features

- Fetch movie dataset over HTTP
- Select specific columns to print
- Optional WHERE-style filtering
- Interactive query loop
- Works with any HTTP-hosted dataset

---

## Setup

### 1. Start a local HTTP server (recommended for testing)

From the project directory:

### macOS / Linux / WSL
python3 -m http.server 8000

### Windows
py -m http.server 8000

This serves the included dataset file over HTTP at:

http://localhost:8000/movies_small.txt

---

### 2. Build

Example with g++:

g++ -std=c++17 main.cpp -o movie_client -lboost_system

(Adjust flags depending on your Boost installation.)

---

### 3. Run

./movie_client

When prompted, provide the dataset URL followed by your query parameters.

Example:

"http://localhost:8000/movies_small.txt" title year rating

---

## Example Queries

Select specific columns:

title year rating "http://localhost:8000/movies_small.txt"

Filter with WHERE:

title year where title Batman "http://localhost:8000/movies_small.txt"

---

## How It Works

1. Opens a TCP connection using Boost.Asio
2. Sends an HTTP GET request
3. Skips HTTP headers
4. Parses the dataset into `Movie` objects
5. Executes selection and filtering locally in memory

The dataset is loaded once per run and reused for all queries.

---

## Project Structure

main.cpp           Client logic and networking  
Movie.h            Movie record parsing  
movies_small.txt   Sample dataset for local testing  

---

## Notes

- Designed for experimentation with networking and parsing
- Works with any compatible text dataset served over HTTP
- Easily adaptable to different data sources
