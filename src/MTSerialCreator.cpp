//
// Created by matteo on 26/10/18.
//

#include <MTSerialCreator.h>
#include <stack>
#include <queue>
#include <stopwatch.h>

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
            root = createSerialTreeDF(dual);
            break;
    }

    return root;
}

MultiTreeNode* MTSerialCreator::createSerialTreeLTR(FiniteDual dual){

    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(dual);
    //*vb = boost::graph_traits<FiniteDual>::vertex_descriptor(2000);

    std::vector<MultiTreeNode*> node_real(boost::num_vertices(dual),nullptr);

    MultiTreeNode* root = new MultiTreeNode;
    root->id_ = *vb;
    node_real.at(*vb) = root;

    MultiTreeNode* cursor = root;
    MultiTreeNode* front_element = root;

    while (front_element != nullptr) {
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

                node_real.at(*ai) = curr_node;

                curr_node->prev_ = cursor;
                curr_node->prev_->next_ = curr_node;
                cursor = curr_node;

            }
            else if(front_element->parent_->id_ != *ai) {
                MultiTreeNode *old_node = node_real[*ai];
                if (front_element->level_ - old_node->level_ >= 0) {
                    old_node->descendants_.push_back(front_element);
                    front_element->relatives_.emplace_back(old_node);
                }
            }

        }

        front_element = front_element->next_;
    }

    trimSerialTree(root);
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

MultiTreeNode* MTSerialCreator::createSerialTreeDF(FiniteDual dual) {

    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(dual);

    std::vector<bool> inserted_map(boost::num_vertices(dual),false);
    std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor, MultiTreeNode*> node_map;
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

    return root;
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
            if(!isChain(curr_node)) {
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
        curr_node->level_ = curr_node->parent_->level_ + 1;

        std::vector<int> to_delete;
        for (int i = 0; i < curr_node->relatives_.size(); ++i) {
            MultiTreeNode* desc = curr_node->relatives_.at(i);

            if(desc->level_ >= curr_node->level_) {
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