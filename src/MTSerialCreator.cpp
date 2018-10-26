//
// Created by matteo on 26/10/18.
//

#include <MTSerialCreator.h>

MultiTreeNode* MTSerialCreator::createSerialTree(FiniteDual dual, MultiTreeManager::CreationMode::Type creation_mode,
                                                 int max_depth){
    MultiTreeNode *root = nullptr;

    switch (creation_mode) {
        case MultiTreeManager::CreationMode::LTR :
            root = createSerialTreeLTR(dual, max_depth);
            break;
        case MultiTreeManager::CreationMode::RTL :
            root = createSerialTreeRTL(dual, max_depth);
            break;
        case MultiTreeManager::CreationMode::BALANCED :
            root = createSerialTreeBAL(dual, max_depth);
            break;
        case MultiTreeManager::CreationMode::HYPER_FLIP :
            root = createSerialTreeHF(dual, max_depth);
            break;
        default:
            std::cerr << "Wrong mode inserted, aborting" << std::endl;
            break;
    }

    return root;
}

MultiTreeNode* MTSerialCreator::createSerialTreeLTR(FiniteDual dual, int max_depth){

    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(dual);

    std::vector<MultiTreeNode*> node_real(boost::num_vertices(dual),nullptr);

    MultiTreeNode* root = new MultiTreeNode;
    root->id = *vb;
    node_real.at(*vb) = root;

    MultiTreeNode* cursor = root;
    MultiTreeNode* front_element = root;

    while (front_element != nullptr) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

        int state = 0;

        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(front_element->id,dual);ai != ai_end; ++ai) {
            if(node_real.at(*ai) == nullptr) {

                auto *curr_node = new MultiTreeNode;
                curr_node->id = *ai;
                curr_node->parent = front_element;
                curr_node->level = curr_node->parent->level + 1;

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

                node_real.at(*ai) = curr_node;

                curr_node->prev = cursor;
                curr_node->prev->next = curr_node;
                cursor = curr_node;

            }
            else if(front_element->parent->id != *ai) {
                MultiTreeNode *old_node = node_real.at(*ai);

                if (front_element->level - old_node->level < max_depth &&
                    front_element->level - old_node->level >= 0)
                    old_node->relatives.push_back(front_element);
            }

        }

        front_element = front_element->next;
    }

    return root;
}

MultiTreeNode* MTSerialCreator::createSerialTreeRTL(FiniteDual dual, int max_depth){
    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(dual);

    std::vector<MultiTreeNode*> node_real(boost::num_vertices(dual),nullptr);

    MultiTreeNode* root = new MultiTreeNode;
    root->id = *vb;
    node_real.at(*vb) = root;

    MultiTreeNode* cursor = root;
    MultiTreeNode* front_element = root;

    while (front_element != nullptr) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

        int state = 0;
        std::vector<MultiTreeNode*> reverse_nodes;

        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(front_element->id,dual);ai != ai_end; ++ai) {
            if(node_real.at(*ai) == nullptr) {

                auto *curr_node = new MultiTreeNode;
                curr_node->id = *ai;
                curr_node->parent = front_element;
                curr_node->level = curr_node->parent->level + 1;

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

                node_real.at(*ai) = curr_node;
                reverse_nodes.push_back(curr_node);

            }
            else if(front_element->parent->id != *ai) {
                MultiTreeNode *old_node = node_real.at(*ai);

                if (front_element->level - old_node->level < max_depth &&
                    front_element->level - old_node->level >= 0)
                    old_node->relatives.push_back(front_element);
            }

        }

        std::reverse(reverse_nodes.begin(), reverse_nodes.end());
        for(auto &node : reverse_nodes) {
            node->prev = cursor;
            node->prev->next = node;
            cursor = node;
        }

        front_element = front_element->next;
    }

    return root;
}

MultiTreeNode* MTSerialCreator::createSerialTreeBAL(FiniteDual dual, int max_depth){
    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(dual);

    std::vector<MultiTreeNode*> node_real(boost::num_vertices(dual),nullptr);

    MultiTreeNode* root = new MultiTreeNode;
    root->id = *vb;
    node_real.at(*vb) = root;

    MultiTreeNode* cursor = root;
    MultiTreeNode* old_cursor = root;
    MultiTreeNode* front_element = root;

    bool to_flip = false;

    while (front_element != nullptr) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai, ai_end;

        int state = 0;
        std::vector<MultiTreeNode *> reverse_nodes;

        for (boost::tie(ai, ai_end) = boost::adjacent_vertices(front_element->id, dual); ai != ai_end; ++ai) {
            if (node_real.at(*ai) == nullptr) {

                auto *curr_node = new MultiTreeNode;
                curr_node->id = *ai;
                curr_node->parent = front_element;
                curr_node->level = curr_node->parent->level + 1;

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

                node_real.at(*ai) = curr_node;
                if (to_flip)
                    reverse_nodes.push_back(curr_node);
                else {
                    curr_node->prev = cursor;
                    curr_node->prev->next = curr_node;
                    cursor = curr_node;
                }

            } else if (front_element->parent->id != *ai) {
                MultiTreeNode *old_node = node_real.at(*ai);

                if (front_element->level - old_node->level < max_depth &&
                    front_element->level - old_node->level >= 0)
                    old_node->relatives.push_back(front_element);
            }

        }

        std::reverse(reverse_nodes.begin(), reverse_nodes.end());
        if (to_flip) {
            for (auto &node : reverse_nodes) {
                node->prev = cursor;
                node->prev->next = node;
                cursor = node;
            }
        }

        int curr_level = front_element->level;

        if (front_element->prev == nullptr) {
            front_element = cursor;
            to_flip = true;
        } else {
            if (front_element->prev->level != curr_level) {
                if(old_cursor == cursor)
                    front_element = nullptr;
                else {
                    front_element = cursor;
                    old_cursor = cursor;
                    to_flip = !to_flip;
                }

            } else
                front_element = front_element->prev;
        }
    }

    return root;
}

MultiTreeNode* MTSerialCreator::createSerialTreeHF(FiniteDual dual, int max_depth){

}