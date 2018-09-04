//
// Created by matteo on 01/09/18.
//

#include <unordered_map>
#include <queue>
#include "MultiTreeManager.h"

MultiTreeNode* MultiTreeManager::meshToTree(Mesh mesh, MultiTreeManager::CreationMode mode, int max_depth) {
    if(mode == CreationMode::THIN)
        return meshToTreeThin(mesh,mode);
    else
        return meshToTreeNormal(mesh,mode,max_depth);
}

MultiTreeNode* MultiTreeManager::meshToTreeNormal(Mesh mesh, MultiTreeManager::CreationMode mode, int max_depth){

    Dual dual(mesh);
    FiniteDual finiteDual(dual,noborder<Mesh>(mesh));
    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(finiteDual);

    std::map<int,bool> inserted_map;
    for(int i = 0; i < boost::num_vertices(finiteDual); i++) {
        inserted_map.insert({i,false});
    }

    std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor, MultiTreeNode*> node_map;
    std::vector<MultiTreeNode*> tree_queue;
    auto * root = new MultiTreeNode;
    root->level = 0;
    root->value = 1;
    root->id = *vb;

    node_map.insert({*vb,root});
    tree_queue.push_back(root);

    inserted_map.at(*vb) = true;
    MultiTreeNode* cursor = tree_queue.front();

    bool balanced_ltr = false;
    bool to_flip = false;

    while (!tree_queue.empty()) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;
        MultiTreeNode* front_element = tree_queue.front();

        std::vector<MultiTreeNode*> tmp_queue;

        int state = 0;

        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(front_element->id,finiteDual);ai != ai_end; ++ai) {
            if(!inserted_map.at(*ai)) {
                inserted_map.at(*ai) = true;

                auto *curr_node = new MultiTreeNode;
                curr_node->id = *ai;
                curr_node->parent = front_element;
                curr_node->level = curr_node->parent->level + 1;

                curr_node->prev = cursor;
                curr_node->prev->next = curr_node;

                state++;

                switch (state) {
                    case 1:
                        curr_node->parent->left = curr_node;
                        break;
                    case 2:
                        curr_node->parent->right = curr_node;
                        break;
                    default:
                        curr_node->parent->mid = curr_node; // Reached only for the root...
                        break;
                }

                cursor = curr_node;
                node_map.insert({*ai,curr_node});
                if(mode == CreationMode::RTL || (mode == CreationMode::BALANCED && balanced_ltr) || (mode == CreationMode::HYPER_FLIP && to_flip))
                    tmp_queue.push_back(curr_node);
                else
                    tree_queue.push_back(curr_node);
            }
            else if (front_element->parent != nullptr) {
                MultiTreeNode* old_node = node_map.at(*ai);


                if(front_element->level - old_node->level < max_depth && front_element->level - old_node->level >= 0 && front_element->parent != old_node)
                    old_node->relatives.push_back(front_element);
            }
        }

        if(mode == CreationMode::RTL || (mode == CreationMode::BALANCED && balanced_ltr) || (mode == CreationMode::HYPER_FLIP && to_flip)) {
            std::reverse(tmp_queue.begin(), tmp_queue.end());
            for(auto &node : tmp_queue)
                tree_queue.push_back(node);
        }

        to_flip = !to_flip;
        tree_queue.erase(tree_queue.begin());
        if (mode == CreationMode::BALANCED) {
            int curr_level = front_element->level;

            if (tree_queue.front()->level != curr_level && front_element->prev != nullptr) {
                std::reverse(tree_queue.begin(), tree_queue.end());
                balanced_ltr = !balanced_ltr;
            }
        }
    }

    return root;
}

MultiTreeNode* MultiTreeManager::meshToTreeThin(Mesh mesh, MultiTreeManager::CreationMode mode) {
    Dual dual(mesh);
    FiniteDual finiteDual(dual,noborder<Mesh>(mesh));
    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(finiteDual);

    std::map<int,bool> inserted_map;
    std::map<int,int> lane_map;
    for(int i = 0; i < boost::num_vertices(finiteDual); i++) {
        inserted_map.insert({i,false});
        lane_map.insert({i,0});
    }

    std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor, MultiTreeNode*> node_map;
    std::vector<MultiTreeNode*> tree_queue;
    auto * root = new MultiTreeNode;
    root->level = 0;
    root->value = 1;
    root->id = *vb;

    node_map.insert({*vb,root});
    tree_queue.push_back(root);

    inserted_map.at(*vb) = true;
    MultiTreeNode* cursor = tree_queue.front();
    boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

    int state = 0;

    for (boost::tie(ai,ai_end)=boost::adjacent_vertices(root->id,finiteDual);ai != ai_end; ++ai) {
        if (!inserted_map.at(*ai)) {
            inserted_map.at(*ai) = true;

            auto *curr_node = new MultiTreeNode;
            curr_node->id = *ai;
            curr_node->parent = root;
            curr_node->level = curr_node->parent->level + 1;

            curr_node->prev = cursor;
            curr_node->prev->next = curr_node;

            state++;

            switch (state) {
                case 1:
                    curr_node->parent->left = curr_node;
                    lane_map[curr_node->id] = 1;
                    break;
                case 2:
                    curr_node->parent->right = curr_node;
                    lane_map[curr_node->id] = 2;
                    break;
                default:
                    curr_node->parent->mid = curr_node; // Reached only for the root...
                    lane_map[curr_node->id] = 3;
                    break;
            }

            cursor = curr_node;
            node_map.insert({*ai,curr_node});
            tree_queue.push_back(curr_node);
        }
    }

    tree_queue.erase(tree_queue.begin());

    while (!tree_queue.empty()) {

        MultiTreeNode* front_element = tree_queue.front();
        bool ok = false;

        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(front_element->id,finiteDual);ai != ai_end; ++ai) {
            std::cout << *ai << std::endl;
            if(!inserted_map.at(*ai)) {
                boost::graph_traits<FiniteDual>::adjacency_iterator aai, aai_end;
                int curr_level = lane_map[front_element->id];

                for (boost::tie(aai, aai_end) = boost::adjacent_vertices(*ai, finiteDual); aai != aai_end; ++aai) {
                    std::cout << *aai << std::endl;
                    if(*aai != front_element->id) {
                        if (lane_map[*aai] != curr_level) {
                            ok = true;
                            break;
                        }
                    }
                }
            }

            if(ok)
                break;
        }

        if(ai != ai_end) {
            if(!inserted_map.at(*ai)) {
                inserted_map.at(*ai) = true;

                auto *curr_node = new MultiTreeNode;
                curr_node->id = *ai;
                curr_node->parent = front_element;
                curr_node->level = curr_node->parent->level + 1;

                curr_node->prev = cursor;
                curr_node->prev->next = curr_node;

                curr_node->parent->left = curr_node;

                cursor = curr_node;
                node_map.insert({*ai,curr_node});
                lane_map[curr_node->id] = lane_map[front_element->id];

                tree_queue.push_back(curr_node);
            }
            else if (front_element->parent != nullptr) {
                MultiTreeNode* old_node = node_map.at(*ai);

                if(!isAncestorOf(old_node,front_element) && old_node->level >= front_element->level)
                    old_node->relatives.push_back(front_element);
            }
        }

        tree_queue.erase(tree_queue.begin());
    }

    return root;
}

bool MultiTreeManager::isAncestorOf(MultiTreeNode *candidate_anc, MultiTreeNode *candidate_des){

    int anc_lvl = candidate_anc->level;

    while(candidate_des->level != anc_lvl) {
        if(candidate_des->parent == candidate_anc)
            return true;
        candidate_des = candidate_des->parent;
    }

    return false;
}

void MultiTreeManager::addAreasToTree(MultiTreeNode* root, std::map<boost::graph_traits<Mesh>::face_descriptor,double> areas) {

    MultiTreeNode* curr_node = root;

    while(curr_node != nullptr) {
        double curr_area = areas[curr_node->id];
        curr_node->area = curr_area;
        curr_node = curr_node->next;
    }
}