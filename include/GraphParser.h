//
// Created by matteo on 19/07/18.
//

#ifndef MANIFOLD_SPLITTING_GRAPHPARSER_H
#define MANIFOLD_SPLITTING_GRAPHPARSER_H


#include <string>

class GraphParser {
public:

    GraphParser();
    virtual ~GraphParser();

    void convertDotToMetis(std::string filename);
    void convertDotToZoltanGraph(std::string filename, std::string out_filename);

private:

};


#endif //MANIFOLD_SPLITTING_GRAPHPARSER_H
