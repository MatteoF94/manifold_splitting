//
// Created by matteo on 07/11/18.
//

#include "MTParallelCreator.h"

#include <queue>
#include <stack>

MultiTreeNode* MTParallelCreator::createParallelTree(FiniteDual dual) {
    /*---- Initialize the dual graph and select a starting face ----*/
    //Dual dual(mesh);
    //FiniteDual finiteDual(dual,noborder<Mesh>(mesh));
    //boost::graph_traits<FiniteDual>::vertex_descriptor root_dsc = *mesh.faces_begin();
    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(dual);

    /*---- Initialize the utility vectors ----*/
    std::vector<bool> inserted_map(boost::num_vertices(dual),false);
    std::vector<MultiTreeNode*> node_real(boost::num_vertices(dual), nullptr);
    std::vector<std::vector<DualVertexDescriptor>> relatives_id(boost::num_vertices(dual));

    /*---- Setup the tree root ----*/
    MultiTreeNode* root = new MultiTreeNode;
    root->id_ = *vb;
    root->level_ = 0;
    inserted_map[*vb] = true;
    node_real[0] = root;

    /*---- Create the first generation of the tree ----*/
    boost::graph_traits<FiniteDual>::adjacency_iterator aib_first,aie_first;
    int state_first = 0;
    for (boost::tie(aib_first,aie_first)=boost::adjacent_vertices(root->id_,dual); aib_first != aie_first; ++aib_first) {

        MultiTreeNode *curr_node = new MultiTreeNode;
        curr_node->id_ = *aib_first;
        curr_node->parent_ = root;
        curr_node->level_ = curr_node->parent_->level_ + 1;

        state_first++;

        switch (state_first) {
            case 1:
                curr_node->parent_->left_ = curr_node;
                break;
            case 2:
                curr_node->parent_->right_ = curr_node;
                break;
            default:
                curr_node->parent_->mid_ = curr_node;
                break;
        }

        inserted_map[*aib_first] = true;
        node_real[*aib_first] = curr_node;
    }

    /*---- Parallel phase setup ----*/
    MultiTreeNode* left_node = root->left_;
    MultiTreeNode* right_node = root->right_;
    MultiTreeNode* mid_node = root->mid_;
    omp_lock_t insertion_lock;
    omp_init_lock(&insertion_lock);

    #pragma omp parallel num_threads(3) shared(inserted_map,node_real)
    {
        MultiTreeNode *front_element = nullptr;
        MultiTreeNode *cursor = nullptr;

        switch (omp_get_thread_num()) {
            case 0:
                front_element = left_node;
                break;
            case 1:
                front_element = right_node;
                break;
            case 2:
                front_element = mid_node;
                break;
            default:
                std::cerr << "Process number different than expected" << std::endl;
                exit(1);
        }
        cursor = front_element;

        switch(branch_chaining_type_){
            case MultiTreeManager::ChainingType::LTR:
                createParallelTreeBodyLTR(front_element,dual,insertion_lock,&inserted_map,&node_real,&relatives_id);
                break;
            case MultiTreeManager::ChainingType::RTL:
                createParallelTreeBodyRTL(front_element,dual,insertion_lock,&inserted_map,&node_real,&relatives_id);
                break;
            case MultiTreeManager::ChainingType::BALANCED:
                createParallelTreeBodyBAL(front_element,dual,insertion_lock,&inserted_map,&node_real,&relatives_id);
                break;
            case MultiTreeManager::ChainingType::FLIP:
                createParallelTreeBodyFLIP(front_element,dual,insertion_lock,&inserted_map,&node_real,&relatives_id);
                break;
            default:
                std::cerr << "*ERROR* :: accepted modes for parallel creation are LTR, RTL, BALANCED and FLIP" << std::endl;
                exit(1);
        }
    }

    /*std::queue<MultiTreeNode*> coda;
    std::vector<int> count(5804,0);
    coda.push(root);
    int c = 0;
    while(!coda.empty()) {
        MultiTreeNode* curr_node = coda.front();
        count[curr_node->id]++;
        c++;
        if(curr_node->left != nullptr) {
            coda.push(curr_node->left);
        }
        if(curr_node->mid != nullptr) {
            coda.push(curr_node->mid);
        }
        if(curr_node->right != nullptr) {
            coda.push(curr_node->right);
        }
        coda.pop();
    }
    std::cout << "CODA: " << c << std::endl;

    for(int i = 0; i < count.size(); ++i)  {
        int elem = count[i];
        if(elem != 1)
            std::cout << "Elem " << i << " counted times " << elem << std::endl;
    }*/

    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < relatives_id.size(); ++i) {
        std::vector<boost::graph_traits<FiniteDual>::vertex_descriptor> elem = relatives_id[i];
        if (!elem.empty()) {
            for (boost::graph_traits<FiniteDual>::vertex_descriptor dsc : elem) {
                node_real[i]->descendants_.emplace_back(node_real[dsc]);
            }
        }
    }

    switch(tree_chaining_type_) {
        case MultiTreeManager::ChainingType::LTR:
            chainParallelTreeLTR(root);
            break;
        case MultiTreeManager::ChainingType::RTL:
            chainParallelTreeRTL(root);
            break;
        case MultiTreeManager::ChainingType::BALANCED:
            chainParallelTreeBAL(root);
            break;
        case MultiTreeManager::ChainingType::FLIP:
            chainParallelTreeFLIP(root);
            break;
        default:
            std::cerr << "*ERROR* :: accepted modes for parallel chaining are LTR, RTL, BALANCED and FLIP" << std::endl;
            exit(1);
    }

    return root;
}

void MTParallelCreator::createParallelTreeBodyLTR(MultiTreeNode* front_element, FiniteDual dual, omp_lock_t insertion_lock, std::vector<bool>* inserted_map, std::vector<MultiTreeNode*>* node_real, std::vector<std::vector<DualVertexDescriptor>>* relatives_id){

    MultiTreeNode* cursor = nullptr;
    cursor = front_element;

    while (front_element != nullptr) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

        int state = 0;

        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(front_element->id_,dual);ai != ai_end; ++ai) {

            bool flag = false;
            //omp_set_lock(&insertion_lock);
#pragma omp critical
            {
                if (!inserted_map->operator[](*ai)) {
                    inserted_map->operator[](*ai) = true;
                    flag = true;
                }
            }
            //omp_unset_lock(&insertion_lock);

            if(flag) {

                auto *curr_node = new MultiTreeNode;
                curr_node->id_ = *ai;
                curr_node->parent_ = front_element;
                curr_node->level_ = curr_node->parent_->level_ + 1;

                state++;

                switch (state) {
                    case 1:
                        curr_node->parent_->left_ = curr_node;
                        break;
                    case 2:
                        curr_node->parent_->right_ = curr_node;
                        break;
                    default:
                        curr_node->parent_->mid_ = curr_node; // Reached only for the root...
                        break;
                }

                node_real->operator[](*ai) = curr_node;

                curr_node->prev_ = cursor;
                cursor->next_ = curr_node;
                cursor = curr_node;

            } else {
                if(front_element->parent_->id_ != *ai) {
                    relatives_id->operator[](*ai).emplace_back(front_element->id_);
                }
            }
        }

        front_element = front_element->next_;
    }
}

void MTParallelCreator::createParallelTreeBodyRTL(MultiTreeNode *front_element, FiniteDual dual,
                                                  omp_lock_t insertion_lock, std::vector<bool> *inserted_map,
                                                  std::vector<MultiTreeNode *> *node_real,
                                                  std::vector<std::vector<DualVertexDescriptor>> *relatives_id){

    MultiTreeNode* cursor = nullptr;
    cursor = front_element;

    while (front_element != nullptr) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

        int state = 0;
        std::vector<MultiTreeNode*> reverse_nodes;

        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(front_element->id_,dual);ai != ai_end; ++ai) {

            bool flag = false;
            #pragma omp critical
            {
            //omp_set_lock(&insertion_lock);
                if(!inserted_map->operator[](*ai)) {
                    inserted_map->operator[](*ai) = true;
                    flag = true;
                }
            }
            //omp_unset_lock(&insertion_lock);

            if(flag) {

                auto *curr_node = new MultiTreeNode;
                curr_node->id_ = *ai;
                curr_node->parent_ = front_element;
                curr_node->level_ = curr_node->parent_->level_ + 1;

                state++;

                switch (state) {
                    case 1:
                        curr_node->parent_->left_ = curr_node;
                        break;
                    case 2:
                        curr_node->parent_->right_ = curr_node;
                        break;
                    default:
                        curr_node->parent_->mid_ = curr_node; // Reached only for the root...
                        break;
                }

                node_real->operator[](*ai) = curr_node;
                reverse_nodes.push_back(curr_node);

            } else {
                if(front_element->parent_->id_ != *ai) {
                    relatives_id->operator[](*ai).emplace_back(front_element->id_);
                }
            }
        }

        std::reverse(reverse_nodes.begin(), reverse_nodes.end());
        for(auto &node : reverse_nodes) {
            node->prev_ = cursor;
            node->prev_->next_ = node;
            cursor = node;
        }

        front_element = front_element->next_;
    }
}

void MTParallelCreator::createParallelTreeBodyBAL(MultiTreeNode *front_element, FiniteDual dual,
                                                  omp_lock_t insertion_lock, std::vector<bool> *inserted_map,
                                                  std::vector<MultiTreeNode *> *node_real,
                                                  std::vector<std::vector<DualVertexDescriptor>> *relatives_id) {

    MultiTreeNode* old_cursor = front_element;
    MultiTreeNode* cursor = front_element;
    bool to_flip = false;

    while (front_element != nullptr) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

        int state = 0;
        std::vector<MultiTreeNode*> reverse_nodes;

        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(front_element->id_,dual);ai != ai_end; ++ai) {

            bool flag = false;
            //omp_set_lock(&insertion_lock);
            #pragma omp critical
            {
                if (!inserted_map->operator[](*ai)) {
                    inserted_map->operator[](*ai) = true;
                    flag = true;
                }
            }
            //omp_unset_lock(&insertion_lock);

            if(flag) {

                auto *curr_node = new MultiTreeNode;
                curr_node->id_ = *ai;
                curr_node->parent_ = front_element;
                curr_node->level_ = curr_node->parent_->level_ + 1;

                state++;

                switch (state) {
                    case 1:
                        curr_node->parent_->left_ = curr_node;
                        break;
                    case 2:
                        curr_node->parent_->right_ = curr_node;
                        break;
                    default:
                        curr_node->parent_->mid_ = curr_node; // Reached only for the root...
                        break;
                }

                node_real->operator[](*ai) = curr_node;
                if (to_flip)
                    reverse_nodes.push_back(curr_node);
                else {
                    curr_node->prev_ = cursor;
                    curr_node->prev_->next_ = curr_node;
                    cursor = curr_node;
                }

            } else {
                if(front_element->parent_->id_ != *ai) {
                    relatives_id->operator[](*ai).emplace_back(front_element->id_);
                }
            }
        }

        if (to_flip) {
            std::reverse(reverse_nodes.begin(), reverse_nodes.end());
            for (auto &node : reverse_nodes) {
                node->prev_ = cursor;
                cursor->next_ = node;
                cursor = node;
            }
        }

        int curr_level = front_element->level_;

        if (front_element->parent_->parent_ == nullptr) {
            if(old_cursor == cursor)
                front_element = nullptr;
            else
                front_element = cursor;
            to_flip = true;
        } else {
            if (front_element->prev_->level_ != curr_level) {
                if(old_cursor == cursor)
                    front_element = nullptr;
                else {
                    front_element = cursor;
                    old_cursor = cursor;
                    to_flip = !to_flip;
                }

            } else
                front_element = front_element->prev_;
        }

    }
}

void MTParallelCreator::createParallelTreeBodyFLIP(MultiTreeNode *front_element, FiniteDual dual,
                                                   omp_lock_t insertion_lock, std::vector<bool> *inserted_map,
                                                   std::vector<MultiTreeNode *> *node_real,
                                                   std::vector<std::vector<DualVertexDescriptor>> *relatives_id){

    MultiTreeNode* cursor = front_element;
    bool to_flip = false;

    while (front_element != nullptr) {
        //std::cout << front_element->id << std::endl;
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

        int state = 0;
        std::vector<MultiTreeNode*> reverse_nodes;

        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(front_element->id_,dual);ai != ai_end; ++ai) {

            bool flag = false;
            //omp_set_lock(&insertion_lock);
            #pragma omp critical
            {
                if (!inserted_map->operator[](*ai)) {
                    inserted_map->operator[](*ai) = true;
                    flag = true;
                }
            }
            omp_unset_lock(&insertion_lock);

            if(flag) {

                auto *curr_node = new MultiTreeNode;
                curr_node->id_ = *ai;
                curr_node->parent_ = front_element;
                curr_node->level_ = curr_node->parent_->level_ + 1;

                state++;

                switch (state) {
                    case 1:
                        curr_node->parent_->left_ = curr_node;
                        break;
                    case 2:
                        curr_node->parent_->right_ = curr_node;
                        break;
                    default:
                        curr_node->parent_->mid_ = curr_node; // Reached only for the root...
                        break;
                }

                node_real->operator[](*ai) = curr_node;
                if (to_flip)
                    reverse_nodes.push_back(curr_node);
                else {
                    curr_node->prev_ = cursor;
                    curr_node->prev_->next_ = curr_node;
                    cursor = curr_node;
                }

            } else {
                if(front_element->parent_->id_ != *ai) {
                    relatives_id->operator[](*ai).emplace_back(front_element->id_);
                }
            }
        }

        if (to_flip) {
            std::reverse(reverse_nodes.begin(), reverse_nodes.end());
            for (auto &node : reverse_nodes) {
                node->prev_ = cursor;
                cursor->next_ = node;
                cursor = node;
            }
        }

        to_flip = !to_flip;
        front_element = front_element->next_;
    }
}

void MTParallelCreator::chainParallelTreeLTR(MultiTreeNode *root) {
    std::queue<MultiTreeNode*> tree_queue;
    tree_queue.push(root);
    MultiTreeNode* cursor = nullptr;
    //MultiTreeNode* curr_node = root;

    while(!tree_queue.empty()) {
        MultiTreeNode* curr_node = tree_queue.front();
        curr_node->prev_ = cursor;
        curr_node->next_ = nullptr;

        if(cursor != nullptr)
            curr_node->prev_->next_ = curr_node;

        if(curr_node->left_ != nullptr) {
            tree_queue.push(curr_node->left_);
        }

        if(curr_node->right_ != nullptr) {
            tree_queue.push(curr_node->right_);
        }

        if(curr_node->mid_ != nullptr) {
            tree_queue.push(curr_node->mid_);
        }

        cursor = curr_node;
        //curr_node = curr_node->next;
        tree_queue.pop();
    }
}

void MTParallelCreator::chainParallelTreeRTL(MultiTreeNode *root){
    std::queue<MultiTreeNode*> tree_queue;
    tree_queue.push(root);
    MultiTreeNode* cursor = nullptr;

    while(!tree_queue.empty()) {
        MultiTreeNode* curr_node = tree_queue.front();
        curr_node->prev_ = cursor;
        curr_node->next_ = nullptr;

        if(cursor != nullptr)
            curr_node->prev_->next_ = curr_node;

        if(curr_node->mid_ != nullptr) {
            tree_queue.push(curr_node->mid_);
        }

        if(curr_node->right_ != nullptr) {
            tree_queue.push(curr_node->right_);
        }

        if(curr_node->left_ != nullptr) {
            tree_queue.push(curr_node->left_);
        }

        cursor = curr_node;
        tree_queue.pop();
    }
}

void MTParallelCreator::chainParallelTreeBAL(MultiTreeNode *root){
    std::vector<MultiTreeNode*> active_stack,passive_stack;
    //std::vector<MultiTreeNode*> stacc(5804);
    active_stack.push_back(root);
    MultiTreeNode* cursor = root;
    MultiTreeNode* prev_cursor = root;
    bool flip = false;

    while(!active_stack.empty()) {
        MultiTreeNode* curr_node = active_stack.back();

        if(curr_node->prev_ != nullptr) {
            curr_node->prev_->next_ = curr_node;
        }

        if(!flip) {
            if (curr_node->left_ != nullptr) {
                passive_stack.push_back(curr_node->left_);
            }
        } else {
            if(curr_node->mid_ != nullptr) {
                passive_stack.push_back(curr_node->mid_);
            }
        }

        if(curr_node->right_ != nullptr) {
            passive_stack.push_back(curr_node->right_);
        }

        if(!flip) {
            if (curr_node->mid_ != nullptr) {
                passive_stack.push_back(curr_node->mid_);
            }
        } else {
            if (curr_node->left_ != nullptr) {
                passive_stack.push_back(curr_node->left_);
            }
        }

        active_stack.pop_back();
        if(active_stack.empty()) {
            for(auto &elem : passive_stack) {
                cursor->next_ = elem;
                elem->prev_ = cursor;
                cursor = elem;
            }

            if(passive_stack.empty())
                cursor->next_ = nullptr;

            active_stack.insert(active_stack.end(),passive_stack.begin(),passive_stack.end());
            passive_stack.clear();
            flip = !flip;
        }
    }

    /*while(!active_stack.empty()) {
        MultiTreeNode* curr_node = active_stack.back();
        stacc[curr_node->id] = curr_node;
        if(curr_node->id == 4424)
            int miaoo = 1;
        active_stack.pop_back();
        if(active_stack.empty()) {
            curr_node->prev = cursor;
            cursor = prev_cursor;
        } else {
            curr_node->prev = active_stack.back();
        }

        if(curr_node->prev != nullptr) {
            curr_node->prev->next = curr_node;
        }

        if(!flip) {
            if (curr_node->left != nullptr) {
                passive_stack.push_back(curr_node->left);
            }
        } else {
            if(curr_node->mid != nullptr) {
                passive_stack.push_back(curr_node->mid);
            }
        }

        if(curr_node->right != nullptr) {
            passive_stack.push_back(curr_node->right);
        }

        if(!flip) {
            if (curr_node->mid != nullptr) {
                passive_stack.push_back(curr_node->mid);
            }
        } else {
            if (curr_node->left != nullptr) {
                passive_stack.push_back(curr_node->left);
            }
        }

        if(active_stack.empty()) {
            if(passive_stack.empty())
                curr_node->next = nullptr;
            else
                prev_cursor = passive_stack.back();
            active_stack.insert(active_stack.end(),passive_stack.begin(),passive_stack.end());
            passive_stack.clear();
            flip = !flip;
        }
    }*/

    /*MultiTreeNode* miao = root;
    std::vector<int> count(5804,0);
    while(miao != nullptr) {
        count[miao->id]++;
        miao = miao->next;
    }

    for(int i = 0; i < count.size(); ++i)  {
        int elem = count[i];
        if(elem != 1)
            std::cout << "Elem " << i << " counted times " << elem << std::endl;
    }

    int hello = 2;*/
}

void MTParallelCreator::chainParallelTreeFLIP(MultiTreeNode *root){
    std::queue<MultiTreeNode*> tree_queue;
    tree_queue.push(root);
    MultiTreeNode* cursor = nullptr;
    bool flip = false;

    while(!tree_queue.empty()) {
        MultiTreeNode* curr_node = tree_queue.front();
        curr_node->prev_ = cursor;
        curr_node->next_ = nullptr;

        if(cursor != nullptr)
            curr_node->prev_->next_ = curr_node;

        if(flip) {
            if (curr_node->left_ != nullptr) {
                tree_queue.push(curr_node->left_);
            }
        } else {
            if(curr_node->mid_ != nullptr) {
                tree_queue.push(curr_node->mid_);
            }
        }

        if(curr_node->right_ != nullptr) {
            tree_queue.push(curr_node->right_);
        }

        if(flip) {
            if (curr_node->mid_ != nullptr) {
                tree_queue.push(curr_node->mid_);
            }
        } else {
            if (curr_node->left_ != nullptr) {
                tree_queue.push(curr_node->left_);
            }
        }

        cursor = curr_node;
        flip = !flip;
        tree_queue.pop();
    }
}

void MTParallelCreator::chainTree(MultiTreeNode *root) {
    switch(tree_chaining_type_) {
        case MultiTreeManager::ChainingType::LTR:
            chainParallelTreeLTR(root);
            break;
        case MultiTreeManager::ChainingType::RTL:
            chainParallelTreeRTL(root);
            break;
        case MultiTreeManager::ChainingType::BALANCED:
            chainParallelTreeBAL(root);
            break;
        case MultiTreeManager::ChainingType::FLIP:
            chainParallelTreeFLIP(root);
            break;
        default:
            std::cerr << "*ERROR* :: accepted modes for parallel chaining are LTR, RTL, BALANCED and FLIP" << std::endl;
            exit(1);
    }
}