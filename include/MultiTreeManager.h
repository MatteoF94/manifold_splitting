//
// Created by matteo on 01/09/18.
//

#ifndef MANIFOLD_SPLITTING_MULTITREEMANAGER_H
#define MANIFOLD_SPLITTING_MULTITREEMANAGER_H

#include <types.h>
#include "MTVisualizer.h"

class MTSerialCreator;
class MTParallelCreator;

class MultiTreeManager {
public:
    enum class ChainingType {LTR,RTL,BALANCED,FLIP,DF};
    enum class CreationType {SERIAL,PARALLEL};

    MultiTreeManager();
    ~MultiTreeManager();

    void setCreationType(CreationType type);
    void configCreation(ChainingType chaining);
    void configCreation(ChainingType body_chaining, ChainingType tree_chaining);

    /*---- Creation methods ----*/
    MultiTreeNode *meshToTree(Mesh mesh);
    void trimTree(MultiTreeNode *root);

    /*---- Recursive tree methods ----*/
    void regenerateTree(std::vector<MultiTreeNode*>* tree_roots, std::vector<int> group_ids);

    /*---- Utility methods ----*/
    void addAreasToTree(MultiTreeNode *root, std::map<boost::graph_traits<Mesh>::face_descriptor, double> areas);
    void addBordersToTree(MultiTreeNode *root);

    /*---- Visualisation methods ----*/
    void visualizeMultiTree(MultiTreeNode *root, std::map<boost::graph_traits<Mesh>::face_descriptor,Point> centroids);
    void compareMultiTrees(MultiTreeNode *root_a,MultiTreeNode *root_b, std::map<boost::graph_traits<Mesh>::face_descriptor,Point> centroids);

private:
    /*---- Recursive tree methods ----*/
    void linkTrees(std::vector<MultiTreeNode*>* tree_roots);
    void regenerateTree(MultiTreeNode *root, std::vector<int> group_ids);

    MultiTreeNode *meshToTreeNormal(Mesh mesh, ChainingType chaining_type, int max_depth);

    /*---- Configuration variables ----*/
    ChainingType chaining_type_;
    CreationType creation_type_;

    /*---- Delegation variables ----*/
    MTVisualizer* visualizer_;
    MTSerialCreator* serial_creator_;
    MTParallelCreator* parallel_creator_;
};

#endif //MANIFOLD_SPLITTING_MULTITREEMANAGER_H
