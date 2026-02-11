/*
 * Movie Query Client
 *
 * Copyright (c) 2025 Tyler Stephenson
 *
 * Downloads a remote movie dataset over HTTP and executes simple
 * column-selection queries with an optional WHERE-style filter.
 *
 * Demonstrates: Boost.Asio networking, HTTP parsing, tokenization,
 * and in-memory filtering/formatting.
 */

#include <algorithm>
#include <boost/asio.hpp>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>


#include "Movie.h"

// Namespace aliases used to keep networking and string utilities readable.
using namespace std;
using namespace boost::asio::ip;
using namespace boost::algorithm;

std::string userInput;

// Alias to a vector-of-strings
using StrVec = std::vector<std::string>;

/**
 * Tokenizes a query string into whitespace-delimited terms after normalizing
 * commas/quotes to spaces.
 */
StrVec split(std::string str) {
    std::replace(str.begin(), str.end(), ',', ' ');
    std::replace(str.begin(), str.end(), '"', ' ');
    std::istringstream is(str);
    StrVec wordList;
    std::string word;
    while (is >> word) {
        wordList.push_back(word);
    }
    return wordList;
}

/**
 * Sends an HTTP GET request to (host, path), skips the HTTP headers, then
 * parses the response body into Movie records.
 *
 * Assumes the first body line is a header row that should be ignored.
 */
std::vector<Movie> process(std::istream& is, std::ostream& os,
                           std::vector<std::string> hostAndPath) {
    std::string host = hostAndPath[0];
    std::string path = hostAndPath[1];

    os << "GET " << path << " HTTP/1.1\r\n";
    os << "Host: " << host << "\r\n";
    os << "Connection: Close\r\n\r\n";


    std::string line;

    // Consume HTTP headers (up to the blank line).
    while (std::getline(is, line) && !line.empty() && line != "\r") {
    }

    std::getline(is, line);

    std::vector<Movie> movieDB;
    Movie placeholder;

    while (is >> placeholder) {
        movieDB.push_back(placeholder);
    }
    return movieDB;
}

/**
 * Extracts the host and path from the query's dataset URL.
 *
 * Expected to find a URL containing "http://" and ending in ".txt".
 * Returns {host, path}.
 */
std::vector<std::string> getHostAndPath() {
    std::vector<std::string> result;

    int end = userInput.find(".txt");
    int httpStart = userInput.find("http://");
    std::string searchString =
        userInput.substr(httpStart + 7, end - httpStart - 3);

    int hostPathSeperator = searchString.find('/');
    std::string host = searchString.substr(0, hostPathSeperator);
    std::string path = searchString.substr(hostPathSeperator);

    result.push_back(host);
    result.push_back(path);

    return result;
}

/**
 * Establishes a TCP connection to the dataset host and builds the in-memory
 * movie database from the HTTP response.
 */
std::vector<Movie> networkHandler() {
    std::string host, path;

    std::vector<std::string> hostAndPath = getHostAndPath();

    host = hostAndPath[0];

    tcp::iostream stream(host, "8000");

    std::vector<Movie> movieDB;

    movieDB = process(stream, stream, hostAndPath);

    return movieDB;
}

/**
 * Prints selected columns for movies that match a simple WHERE condition.
 *
 * The WHERE condition is interpreted as: movie[colName] contains traitValue.
 */
void moviesWhere(std::vector<Movie> movieDB, std::vector<std::string> cols) {
    std::string desiredTrait = cols[cols.size() - 1];

    std::string desiredTraitName = cols[cols.size() - 3];

    for (Movie m : movieDB) {
        size_t traitFound = ((m.getCol(desiredTraitName)).find(desiredTrait));

        if (traitFound != std::string::npos) {
            std::string output;

            for (size_t i = 0; i < cols.size() - 4; i++) {
                output = output + m.getCol(cols[i]) +
                         (i < cols.size() - 1 ? " " : "");
            }
            trim(output);
            std::cout << output << std::endl;
        }
    }
}

// Prints selected columns for every movie (no filtering).
void moviesNoWhere(std::vector<Movie> movieDB, std::vector<std::string> cols) {
    // loops through each movie in the master DB
    for (Movie m : movieDB) {
        // string to be outputted.
        std::string output;

        // loops through the column list and adds that trait to output
        for (size_t i = 0; i < cols.size(); i++) {
            output =
                output + m.getCol(cols[i]) + (i < cols.size() - 1 ? " " : "");
        }

        // function from boost algorithm to remove leading or trailing spaces.
        trim(output);

        // print the output.
        std::cout << output << std::endl;
    }
}
/**
 * Dispatches printing based on whether the parsed query includes a WHERE clause.
 */
void printMoviesByCol(std::vector<Movie> movieDB,
                      std::vector<std::string> cols) {
    if (cols.size() >= 4 && cols[cols.size() - 4] == "where") {
        moviesWhere(movieDB, cols);
    } else {
        moviesNoWhere(movieDB, cols);
    }
}

/**
 * Parses the user query into tokens representing requested columns and
 * an optional WHERE clause, removing the dataset URL from the query first.
 */
std::vector<std::string> parseCategories() {
    std::string input = userInput;
    std::vector<std::string> result;

    // find the start and end of the path / host and then removes them.
    int findHostAndPathStart = input.find("\"http");
    int findHostAndPathEnd = input.find(".txt");
    std::string cleaned = input.substr(0, findHostAndPathStart) + " " +
                          input.substr(findHostAndPathEnd + 4);

    // returns a cleaned list of traits and maybe the where claus.
    return split(cleaned);
}
/**
 * Main loop: read queries until "exit", fetch the dataset, then execute
 * the parsed selection/filter against the in-memory database.
 */
int main() {
    bool shouldQuit = false;

    td::vector<Movie> movieDB = networkHandler();
    while (!shouldQuit) {
        getline(std::cin, userInput);
        std::cout << "query> ";
        if (userInput == "exit") {
            shouldQuit = true;

        } else if (userInput.size() < 50) {
            std::cout << "Try again";
        } else {

            s
            std::vector<std::string> categoriesToGrab;

            categoriesToGrab = parseCategories();
            printMoviesByCol(movieDB, categoriesToGrab);
        }
    }

    return 0;
}
