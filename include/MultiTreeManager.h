//
// Created by matteo on 01/09/18.
//

#ifndef MANIFOLD_SPLITTING_MULTITREEMANAGER_H
#define MANIFOLD_SPLITTING_MULTITREEMANAGER_H

#include <types.h>

class MultiTreeManager {
public:
    struct CreationMode {
        enum Type {
            LTR, RTL, BALANCED, HYPER_FLIP, THIN, DF
        };

        Type t_;

        CreationMode(Type t) : t_(t) {}

        operator Type() const { return t_; }

    private:
        template<typename T>
        operator T() const;
    };

    MultiTreeNode *meshToTree(Mesh mesh, CreationMode mode, int max_depth = 100);

    void addAreasToTree(MultiTreeNode *root, std::map<boost::graph_traits<Mesh>::face_descriptor, double> areas);

    void regenerateTree(MultiTreeNode *root, std::vector<int> group_ids);

    void linkTrees(std::vector<MultiTreeNode*>* tree_roots);

private:
    MultiTreeNode *meshToTreeNormal(Mesh mesh, CreationMode mode, int max_depth);

    MultiTreeNode *meshToTreeDF(Mesh mesh, int max_depth, int mode);

    MultiTreeNode *meshToTreeThin(Mesh mesh, CreationMode mode);

    bool isAncestorOf(MultiTreeNode *candidate_anc, MultiTreeNode *candidate_des);

};

#endif //MANIFOLD_SPLITTING_MULTITREEMANAGER_H
