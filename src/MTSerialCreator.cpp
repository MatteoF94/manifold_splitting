//
// Created by matteo on 26/10/18.
//

#include <MTSerialCreator.h>
#include <stack>
#include <queue>
#include <boost/circular_buffer.hpp>
#include <stopwatch.h>
#include <core/multitree/Concatenator.h>
#include <core/multitree/AdoptionHandler.h>
#include <core/multitree/SerialCreator.h>

MultiTreeNode* MTSerialCreator::createSerialTree(FiniteDual dual){
    MultiTreeNode *root = nullptr;

    switch (chaining_type_) {
        case MultiTreeManager::ChainingType::LTR :
            root = createSerialTreeLTR(dual);
            break;
        case MultiTreeManager::ChainingType::RTL :
            root = createSerialTreeRTL(dual);
            break;
        case MultiTreeManager::ChainingType::BALANCED :
            root = createSerialTreeBAL(dual);
            break;
        case MultiTreeManager::ChainingType::FLIP :
            root = createSerialTreeFLIP(dual);
            break;
        case MultiTreeManager::ChainingType::DF :
            //root = createSerialTreeDF(dual);
            break;
    }

    return root;
}

MultiTreeNode* MTSerialCreator::createSerialTreeLTR(FiniteDual dual){

    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(dual);
    //*vb = boost::graph_traits<FiniteDual>::vertex_descriptor(2000);

    std::vector<MultiTreeNode*> node_real(boost::num_vertices(dual),nullptr);

    MultiTreeNode *root = new MultiTreeNode;
    root->id_ = *vb;
    node_real[*vb] = root;

    MultiTreeNode* cursor = root;
    MultiTreeNode* front_element = root;

    while (front_element) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

        int state = 0;

        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(front_element->id_,dual);ai != ai_end; ++ai) {
            if(node_real.at(*ai) == nullptr) {

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

                node_real[*ai] = curr_node;

                curr_node->prev_ = cursor;
                curr_node->prev_->next_ = curr_node;
                cursor = curr_node;

            }
            else if(front_element->parent_->id_ != *ai) {
                MultiTreeNode *old_node = node_real[*ai];
                if (front_element->level_ - old_node->level_ >= 0) {
                    old_node->descendants_.push_back(front_element);
                    front_element->relatives_.emplace_back(old_node);

                    if(std::find(old_node->relatives_.begin(),old_node->relatives_.end(),front_element) != old_node->relatives_.end()) {
                        old_node->relatives_.erase(std::remove(old_node->relatives_.begin(),old_node->relatives_.end(),front_element),old_node->relatives_.end());
                        front_element->descendants_.erase(std::remove(front_element->descendants_.begin(),front_element->descendants_.end(),old_node),front_element->descendants_.end());
                    }
                }
            }

        }

        front_element = front_element->next_;
    }
    if(tree_manager_->checkTreeIntegrity(root))
        std::cout << "CORRECT TREE" << std::endl;
    if(with_adoptions_) {
        AdoptionHandler adoptionHandler;
        //adoptionHandler.configHandler(true,true,10,2);
        //adoptionHandler.adoptBranches(root);
        tree_manager_->treeAdoption(root);
        //trimSerialTree(root);
        //trimSerialTree(root);
        //tree_manager_->chainTree(root);
        /*SerialCreator serialCreator;
        serialCreator.configCreator();
        Concatenator concatenator;
        concatenator.setConcatenationType(ConcatenationType::Balanced);
        concatenator.concatenateTree(root);*/
    }
    return root;
}

MultiTreeNode* MTSerialCreator::createSerialTreeRTL(FiniteDual dual){
    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(dual);

    std::vector<MultiTreeNode*> node_real(boost::num_vertices(dual),nullptr);

    MultiTreeNode* root = new MultiTreeNode;
    root->id_ = *vb;
    node_real.at(*vb) = root;

    MultiTreeNode* cursor = root;
    MultiTreeNode* front_element = root;

    while (front_element != nullptr) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;

        int state = 0;
        std::vector<MultiTreeNode*> reverse_nodes;

        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(front_element->id_,dual);ai != ai_end; ++ai) {
            if(node_real.at(*ai) == nullptr) {

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

                node_real.at(*ai) = curr_node;
                reverse_nodes.push_back(curr_node);

            }
            else if(front_element->parent_->id_ != *ai) {
                MultiTreeNode *old_node = node_real[*ai];
                if (front_element->level_ - old_node->level_ >= 0) {
                    old_node->descendants_.push_back(front_element);
                    front_element->relatives_.emplace_back(old_node);
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

    return root;
}

MultiTreeNode* MTSerialCreator::createSerialTreeBAL(FiniteDual dual){
    std::cout << "YO" << std::endl;
    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(dual);

    std::vector<MultiTreeNode*> node_real(boost::num_vertices(dual),nullptr);

    MultiTreeNode* root = new MultiTreeNode;
    root->id_ = *vb;
    node_real.at(*vb) = root;

    MultiTreeNode* cursor = root;
    MultiTreeNode* old_cursor = root;
    MultiTreeNode* front_element = root;

    bool to_flip = false;

    while (front_element != nullptr) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai, ai_end;

        int state = 0;
        std::vector<MultiTreeNode *> reverse_nodes;

        for (boost::tie(ai, ai_end) = boost::adjacent_vertices(front_element->id_, dual); ai != ai_end; ++ai) {
            if (node_real.at(*ai) == nullptr) {

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

                node_real.at(*ai) = curr_node;
                if (to_flip)
                    reverse_nodes.push_back(curr_node);
                else {
                    curr_node->prev_ = cursor;
                    curr_node->prev_->next_ = curr_node;
                    cursor = curr_node;
                }

            } else if (front_element->parent_->id_ != *ai) {
                MultiTreeNode *old_node = node_real[*ai];
                if (front_element->level_ - old_node->level_ >= 0) {
                    old_node->descendants_.push_back(front_element);
                    front_element->relatives_.emplace_back(old_node);
                }
            }

        }

        if (to_flip) {
            std::reverse(reverse_nodes.begin(), reverse_nodes.end());
            for (auto &node : reverse_nodes) {
                node->prev_ = cursor;
                node->prev_->next_ = node;
                cursor = node;
            }
        }

        int curr_level = front_element->level_;

        if (front_element->prev_ == nullptr) {
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

    return root;
}

MultiTreeNode* MTSerialCreator::createSerialTreeFLIP(FiniteDual dual){
    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(dual);

    std::vector<MultiTreeNode*> node_real(boost::num_vertices(dual),nullptr);

    MultiTreeNode* root = new MultiTreeNode;
    root->id_ = *vb;
    node_real.at(*vb) = root;

    MultiTreeNode* cursor = root;
    MultiTreeNode* old_cursor = root;
    MultiTreeNode* front_element = root;

    bool to_flip = false;

    while (front_element != nullptr) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai, ai_end;

        int state = 0;
        std::vector<MultiTreeNode *> reverse_nodes;

        for (boost::tie(ai, ai_end) = boost::adjacent_vertices(front_element->id_, dual); ai != ai_end; ++ai) {
            if (node_real.at(*ai) == nullptr) {

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

                node_real.at(*ai) = curr_node;
                if (to_flip)
                    reverse_nodes.push_back(curr_node);
                else {
                    curr_node->prev_ = cursor;
                    curr_node->prev_->next_ = curr_node;
                    cursor = curr_node;
                }

            } else if (front_element->parent_->id_ != *ai) {
                MultiTreeNode *old_node = node_real[*ai];
                if (front_element->level_ - old_node->level_ >= 0) {
                    old_node->descendants_.push_back(front_element);
                    front_element->relatives_.emplace_back(old_node);
                }
            }

        }

        std::reverse(reverse_nodes.begin(), reverse_nodes.end());
        if (to_flip) {
            for (auto &node : reverse_nodes) {
                node->prev_ = cursor;
                node->prev_->next_ = node;
                cursor = node;
            }
        }

        to_flip = !to_flip;
        front_element = front_element->next_;
    }

    return root;
}

std::vector<int> MTSerialCreator::createSerialTreeDF(FiniteDual dual, Mesh mesh) {

    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(dual);

    std::vector<bool> inserted_map(boost::num_vertices(dual),false);
    /*std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor, MultiTreeNode*> node_map;
    std::stack<MultiTreeNode*> tree_stack;

    auto * root = new MultiTreeNode;
    root->level_ = 0;
    root->value_ = 1;
    root->id_ = *vb;

    node_map.insert({*vb,root});
    tree_stack.push(root);

    inserted_map[*vb] = true;
    MultiTreeNode* cursor = root;

    bool flip = false;

    while (!tree_stack.empty()) {
        boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;
        MultiTreeNode* front_element = tree_stack.top();

        std::vector<MultiTreeNode*> tmp_queue;

        int state = 0;

        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(front_element->id_,dual);ai != ai_end; ++ai) {
            if(!inserted_map[*ai]) {
                inserted_map[*ai] = true;

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

                node_map.insert({*ai,curr_node});
                tmp_queue.push_back(curr_node);
            }
        }

        tree_stack.pop();

        std::reverse(tmp_queue.begin(), tmp_queue.end());

        for (auto &node : tmp_queue) {
            node->prev_ = cursor;
            if(cursor != nullptr)
                node->prev_->next_ = node;
            cursor = node;
            tree_stack.push(node);
        }
    }

    return root;*/

    std::stack<boost::graph_traits<Mesh>::face_descriptor> ids;
    std::vector<boost::graph_traits<Mesh>::face_descriptor> parents(boost::num_vertices(dual),*vb);
    ids.emplace(*vb);
    boost::graph_traits<FiniteDual>::adjacency_iterator ai,ai_end;
    int count = 0;
    std::vector<int> groups(boost::num_vertices(dual),-1);
    int curr_group = 0;
    bool swap_direction = false;
    groups[*vb] = curr_group;

    while(!ids.empty()) {
        boost::graph_traits<Mesh>::face_descriptor curr_index = ids.top();
        ids.pop();
        if(!inserted_map[curr_index]) {
            inserted_map[curr_index] = true;
            groups[curr_index] = curr_group;
            ++count;
        } else {
            if(curr_group != groups[curr_index])
                swap_direction = !swap_direction;
            continue;
        }

        std::vector<boost::graph_traits<Mesh>::face_descriptor> curr_neigh;
        //std::cout << curr_index << std::endl;
        if(curr_index == 8)
            int urca = 0;

        boost::circular_buffer<boost::graph_traits<Mesh>::face_descriptor> neighbours(3);
        for (boost::tie(ai,ai_end)=boost::adjacent_vertices(curr_index,dual);ai != ai_end; ++ai) {
            neighbours.push_back(*ai);

            /*std::cout << *ai << std::endl;
            CGAL::Vertex_around_face_iterator<Mesh> vafb,vafe;
            for(boost::tie(vafb,vafe) = CGAL::vertices_around_face(mesh.halfedge(*ai),mesh);vafb != vafe;++vafb) {

                Point p = mesh.point(*vafb);
                std::cout << p.x() << " " << p.y() << " " << p.z() << std::endl;
            }*/
        }

        if(parents[curr_index] != curr_index) {
            while(*neighbours.begin() != parents[curr_index]) {
                neighbours.rotate(neighbours.begin()+1);
            }
        } else {
            std::reverse(neighbours.begin(),neighbours.end());
            boost::graph_traits<Mesh>::face_descriptor first_child = neighbours[0];
            if(!inserted_map[first_child]) {
                //inserted_map[first_child] = true;
                //groups[first_child] = curr_group;
                //++count;
                ids.emplace(first_child);
                parents[first_child] = curr_index;
            }
        }

        /*std::cout << "- " << neighbours[0] << std::endl;
        std::cout << "- " << neighbours[1] << std::endl;
        std::cout << "- " << neighbours[2] << std::endl;*/

        boost::graph_traits<Mesh>::face_descriptor cw_child;
        if(!swap_direction)
            cw_child = neighbours[1];
        else
            cw_child = neighbours[2];

        if(!inserted_map[cw_child] || true) {
            //inserted_map[cw_child] = true;
            //groups[cw_child] = curr_group;
            //++count;
            ids.emplace(cw_child);
            parents[cw_child] = curr_index;
        }

        boost::graph_traits<Mesh>::face_descriptor ccw_child;
        if(!swap_direction)
            ccw_child = neighbours[2];
        else
            ccw_child = neighbours[1];
        if(!inserted_map[ccw_child] || true) {
            //inserted_map[ccw_child] = true;
            //groups[ccw_child] = curr_group;
            //++count;
            ids.emplace(ccw_child);
            parents[ccw_child] = curr_index;
        }

        int miao = 0;
        if(count > 8708) {
            count = 0;
            ++curr_group;
            bool flag = false;

            while(!flag && !ids.empty()) {
                int num_children = 0;
                boost::graph_traits<Mesh>::face_descriptor conf_ind = ids.top();
                if(inserted_map[conf_ind]) {
                    ids.pop();
                    continue;
                }
                for (boost::tie(ai,ai_end)=boost::adjacent_vertices(conf_ind,dual);ai != ai_end; ++ai) {
                    if (!inserted_map[*ai]) {
                        ++num_children;
                    }
                }
                if(num_children != 0) {
                    flag = true;
                    //std::stack<boost::graph_traits<Mesh>::face_descriptor> empty_stack;
                    //ids.swap(empty_stack);
                    //ids.emplace(conf_ind);
                } else
                    ids.pop();
            }
        }
    }

    return groups;
}

void MTSerialCreator::trimSerialTree(MultiTreeNode *root) {
    std::stack<MultiTreeNode*> depth_stack;

    depth_stack.push(root);

    while(!depth_stack.empty()) {
        MultiTreeNode* curr_node = depth_stack.top();
        std::vector<MultiTreeNode*> curr_push;

        int children_count = 0;

        if(curr_node->mid_ != nullptr) {
            curr_push.push_back(curr_node->mid_);
            ++children_count;
        }

        if(curr_node->right_ != nullptr) {
            curr_push.push_back(curr_node->right_);
            ++children_count;
        }

        if(curr_node->left_ != nullptr) {
            curr_push.push_back(curr_node->left_);
            ++children_count;
        }

        depth_stack.pop();

        if(children_count == 1 && curr_node->descendants_.size() == 1) {
            if(!tree_manager_->isChannel(curr_node)) {
                for(auto &elem : curr_push)
                    depth_stack.push(elem);
            } else {
                MultiTreeNode* relative = curr_node->descendants_.at(0);

                if(relative->parent_->left_ == nullptr || relative->parent_->right_ == nullptr) {
                    if(relative->parent_->left_ != nullptr) relative->parent_->left_ = nullptr;
                    if(relative->parent_->right_ != nullptr) relative->parent_->right_ = nullptr;
                    relative->parent_ = curr_node;
                    if (curr_node->left_ == nullptr)
                        curr_node->left_ = relative;
                    else
                        curr_node->right_ = relative;
                    curr_node->descendants_.clear();
                    relative->relatives_.clear();
                    std::cout << "Curr node: " << relative->id_ << std::endl;

                    if(relative->id_ == 65953)
                        int adsadsadas = 0;
                    adjustDescendants(relative);
                }
            }
        } else
            for(auto &elem : curr_push)
                depth_stack.push(elem);
    }

    std::queue<MultiTreeNode*> new_tree;
    new_tree.push(root);
    int count = 0;
    while(!new_tree.empty()) {
        MultiTreeNode* curr_node = new_tree.front();

        if(curr_node->left_ != nullptr)
            new_tree.push(curr_node->left_);
        if(curr_node->right_ != nullptr)
            new_tree.push(curr_node->right_);
        if(curr_node->mid_ != nullptr)
            new_tree.push(curr_node->mid_);

        new_tree.pop();
        ++count;
        if(!new_tree.empty()) {
            curr_node->next_ = new_tree.front();
            curr_node->next_->prev_ = curr_node;
        }
    }

    std::cout << "Num elements: " << count << std::endl;
    //exit(0);
}

void MTSerialCreator::adjustDescendants(MultiTreeNode *node) {
    std::queue<MultiTreeNode*> node_queue;
    node_queue.push(node);

    while(!node_queue.empty()) {
        MultiTreeNode* curr_node = node_queue.front();
        //std::cout << curr_node->id_ << std::endl;
        if(curr_node->id_ == 69068)
            int asd = 0;
        curr_node->level_ = curr_node->parent_->level_ + 1;

        std::vector<int> to_delete;
        for (int i = 0; i < curr_node->relatives_.size(); ++i) {
            MultiTreeNode* desc = curr_node->relatives_.at(i);

            if(desc->level_ > curr_node->level_) {
                desc->descendants_.erase(std::find(desc->descendants_.begin(),desc->descendants_.end(),curr_node));
                desc->relatives_.push_back(curr_node);
                curr_node->descendants_.push_back(desc);
                to_delete.push_back(i);
            }
        }

        for(int i = 0; i < to_delete.size(); ++i) {
            int elim = to_delete.at(i);
            curr_node->relatives_[elim] = curr_node->relatives_.back();
            curr_node->relatives_.pop_back();
        }

        if(curr_node->left_ != nullptr)
            node_queue.push(curr_node->left_);
        if(curr_node->right_ != nullptr)
            node_queue.push(curr_node->right_);
        if(curr_node->mid_ != nullptr)
            node_queue.push(curr_node->mid_);

        node_queue.pop();
    }
}

bool MTSerialCreator::isChain(MultiTreeNode* node) {
    std::queue<MultiTreeNode*> depth_queue;
    depth_queue.push(node);
    int curr_level = node->level_;
    int count = 0;

    Stopwatch stopwatch;
    stopwatch.start();

    while(!depth_queue.empty()) {
        MultiTreeNode* curr_node = depth_queue.front();

        if(curr_node->level_ - node->level_ >= 1)
            return true;

        if(curr_node->level_ != curr_level) {
            count = 0;
            curr_level = curr_node->level_;
        }

        if(curr_node->left_ != nullptr) {
            depth_queue.push(curr_node->left_);
            ++count;
        }

        if(curr_node->right_ != nullptr) {
            depth_queue.push(curr_node->right_);
            ++count;
        }

        if(curr_node->mid_ != nullptr) {
            depth_queue.push(curr_node->mid_);
            ++count;
        }

        if(count > 2) return false;

        depth_queue.pop();
    }

    return false;
}