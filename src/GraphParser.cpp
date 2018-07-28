//
// Created by matteo on 19/07/18.
//

#include <iostream>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>
#include <unordered_set>
#include <map>
#include "GraphParser.h"

GraphParser::GraphParser() {
    //TODO
}

GraphParser::~GraphParser() {

}

void GraphParser::convertDotToMetis(std::string filename) {
    std::cout << "GraphParser::convertDotToMetis" << std::endl;

    std::ifstream infile(filename);
    std::cout << "OPENING AT: " << filename.c_str() << std::endl;
    if (!infile) {
        std::cerr << "Unable to open file at" << filename.c_str() << std::endl;
        return;
    }

    std::string headTag;
    getline(infile,headTag);
    if(headTag.find("graph G")==std::string::npos) {
        std::cerr << "Incorrect file format" << std::endl;
        return;
    }

    std::string line;
    int numVertices = 0;
    int numEdges = 0;
    std::map<int,std::unordered_set<int>> graphMap;

    while (std::getline(infile,line)) {
        if(line.find("--")!=std::string::npos)
            break;
        std::replace(line.begin(),line.end(),'[',' ');
        std::replace(line.begin(),line.end(),']',' ');
        std::replace(line.begin(),line.end(),';',' ');

        std::istringstream iss(line);
        int a;
        if (!(iss >> a)) break; // error
        numVertices++;
        std::unordered_set<int> tmp;
        graphMap.insert({a,tmp});
    }

    std::cout << "There are " << numVertices << " vertices" << std::endl;
    do {
        std::replace(line.begin(), line.end(), '-', ' ');
        std::istringstream iss(line);
        int a,b;
        if(!(iss >> a >> b)) break;
        graphMap[a].insert(b);
        graphMap[b].insert(a);
        numEdges++;
    } while(std::getline(infile,line));


    std::string output_filename;
    output_filename = filename.substr(0,filename.find(".dot")) + std::string(".graph");
    std::cout << "Saving at " << output_filename << std::endl;

    std::ofstream outfile(output_filename);
    outfile << numVertices << " " << numEdges << std::endl;
    for(std::pair<int,std::unordered_set<int>> element : graphMap) {
        //std::cout << element.first << " :: " << std::endl;
        for (int elementIn : element.second) {
            //std::cout << " - - " << elementIn << std::endl;
            outfile << elementIn+1 << " ";
        }
        outfile << std::endl;
    }
}