//
// Created by matteo on 01/09/18.
//

#include <unordered_map>
#include <stack>
#include <vector>
#include <queue>
#include <stopwatch.h>
#include "MultiTreeManager.h"
#include <omp.h>
#include <MTSerialCreator.h>
#include <MTParallelCreator.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <zconf.h>
#include <iostream>
#include <algorithm>
#include <CGAL/bounding_box.h>
#include <MTVisualizer.h>
#include <core/multitree/SerialCreator.h>

MultiTreeManager::MultiTreeManager() : chaining_type_(ChainingType::LTR), creation_type_(CreationType::SERIAL), with_adoptions_(false),
                                       conduit_width_(2), conduit_depth_(5),
                                       multiple_adoptions_(false), deep_adoption_(false) {
    visualizer_ = new MTVisualizer();
    serial_creator_ = new MTSerialCreator(this);
    parallel_creator_ = new MTParallelCreator(this);
}

MultiTreeManager::~MultiTreeManager() {
    delete visualizer_;
    delete serial_creator_;
    delete parallel_creator_;
}

struct MyPosData {
    GLdouble x = 0;
    GLdouble y = 0;
    GLdouble scale = 1;
    GLdouble translationX = 0;
    GLdouble translationY = 0;
    bool to_display = false;
};

void controls(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS) {
        MyPosData *dataPtr = (MyPosData*)glfwGetWindowUserPointer(window);
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_SPACE:
                dataPtr->to_display = false;
                break;
            case GLFW_KEY_RIGHT_BRACKET:
                dataPtr->scale += 0.5;
                break;
            case GLFW_KEY_P:
                dataPtr->scale -= 0.5;
                break;
            case GLFW_KEY_A:
                dataPtr->translationX -= 0.05;
                break;
            case GLFW_KEY_D:
                dataPtr->translationX += 0.05;
                break;
            case GLFW_KEY_W:
                dataPtr->translationY += 0.05;
                break;
            case GLFW_KEY_S:
                dataPtr->translationY -= 0.05;
                break;
        }
    }
    if(action == GLFW_REPEAT) {
        MyPosData *dataPtr = (MyPosData*)glfwGetWindowUserPointer(window);
        switch (key) {
            case GLFW_KEY_RIGHT:
                dataPtr->x -= 1;
                break;
            case GLFW_KEY_LEFT:
                dataPtr->x += 1;
                break;
            case GLFW_KEY_DOWN:
                dataPtr->y -= 1;
                break;
            case GLFW_KEY_UP:
                dataPtr->y += 1;
                break;
        }
    }
}

GLFWwindow* initWindow(const int resX, const int resY)
{
    if(!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(resX, resY, "TEST", NULL, NULL);

    if(window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, controls);

    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEPTH_TEST); // Depth Testing
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return window;
}

void drawCube(GLFWwindow* window,std::vector<GLfloat> coords,std::vector<GLfloat> colors)
{
    std::vector<GLfloat> vertini = {-1,-1,-1,-1,-1,1,-1,1,1};
    GLfloat vertices[] =
            {
                    -1, -1, -1,   -1, -1,  1,   -1,  1,  1,   -1,  1, -1,
                    1, -1, -1,    1, -1,  1,    1,  1,  1,    1,  1, -1,
                    -1, -1, -1,   -1, -1,  1,    1, -1,  1,    1, -1, -1,
                    -1,  1, -1,   -1,  1,  1,    1,  1,  1,    1,  1, -1,
                    -1, -1, -1,   -1,  1, -1,    1,  1, -1,    1, -1, -1,
                    -1, -1,  1,   -1,  1,  1,    1,  1,  1,    1, -1,  1
            };

    //GLfloat colors[] =
            //{
             //       0, 0, 0,   0, 0, 1,   0, 1, 1
            //};

    MyPosData *dataPtr = (MyPosData*)glfwGetWindowUserPointer(window);
    static float alpha = 0;
    //attempt to rotate cube
    glRotatef(dataPtr->x, 0, 1, 0);
    glRotatef(dataPtr->y,1,0,0);

    /* We have a color array and a vertex array */
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, coords.data());
    glColorPointer(3, GL_FLOAT, 0, colors.data());

    /* Send data : 24 vertices */
    int size = coords.size();
    glScalef(dataPtr->scale,dataPtr->scale,dataPtr->scale);
    glTranslatef(dataPtr->translationX, dataPtr->translationY,0);
    glDrawArrays(GL_TRIANGLES, 0, coords.size()/3);

    /* Cleanup states */
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    alpha += 1;
}

void MultiTreeManager::setCreationModes(MultiTreeManager::CreationType type, bool with_adoption){
    creation_type_ = type;
    with_adoptions_ = with_adoption;
}

void MultiTreeManager::configCreation(MultiTreeManager::ChainingType chaining) {
    serial_creator_->configCreation(chaining,with_adoptions_);
}

void MultiTreeManager::configCreation(MultiTreeManager::ChainingType body_chaining, MultiTreeManager::ChainingType tree_chaining) {
    parallel_creator_->configCreation(body_chaining,tree_chaining);
}

void MultiTreeManager::configAdoption(int width, int depth, bool multiple, bool deep) {
    conduit_width_ = width;
    conduit_depth_ = depth;
    multiple_adoptions_ = multiple;
    deep_adoption_ = deep;
}

void MultiTreeManager::chainTree(MultiTreeNode *root) {
    parallel_creator_->chainTree(root);
    if(!checkTreeIntegrity(root)) {
        std::cout << "NOT CORRECT TREE" << std::endl;
        exit(0);
    }
}

MultiTreeNode* MultiTreeManager::meshToTree(Mesh mesh) {

    /*---- Convert mesh to dual ----*/
    Dual dual(mesh);
    FiniteDual finiteDual(dual, noborder<Mesh>(mesh));
    MultiTreeNode* root = new MultiTreeNode;
    SerialCreator serialCreator;
    /*---- Create the tree ----*/
    switch(creation_type_) {
        case CreationType::SERIAL:
            //serialCreator.buildTree(mesh,root);
            //root = serial_creator_->createSerialTree(finiteDual);
            break;
        case CreationType::PARALLEL:
            root = parallel_creator_->createParallelTree(finiteDual);
            //root = meshToTreeNormal(mesh,ChainingType::LTR,100);
            break;
    }

    return root;
}

MultiTreeNode* MultiTreeManager::meshToTreeNormal(Mesh mesh, MultiTreeManager::ChainingType mode, int max_depth){
    GLFWwindow* window = initWindow(1024, 620);
    MyPosData my_pos_data;
    glfwSetWindowUserPointer(window,&my_pos_data);

    Stopwatch stopwatch;
    stopwatch.start();
    Dual dual(mesh);
    double elapsed = stopwatch.stop();
    std::cout << "Conversion took " << elapsed << " seconds" << std::endl;
    FiniteDual finiteDual(dual,noborder<Mesh>(mesh));
    boost::graph_traits<FiniteDual>::vertex_iterator vb,ve;
    boost::tie(vb,ve) = boost::vertices(finiteDual);
    *vb = boost::graph_traits<FiniteDual>::vertex_descriptor(15000);

    std::map<int,bool> inserted_map;
    stopwatch.start();
    for(int i = 0; i < boost::num_vertices(finiteDual); i++) {
        inserted_map.insert({i,false});
    }
    elapsed = stopwatch.stop();
    std::cout << "Map creation took " << elapsed << " seconds" << std::endl;

    std::unordered_map<boost::graph_traits<FiniteDual>::vertex_descriptor, MultiTreeNode*> node_map;
    std::vector<MultiTreeNode*> tree_queue;
    auto * root = new MultiTreeNode;
    root->level_ = 0;
    root->value_ = 1;
    root->id_ = *vb;

    node_map.insert({*vb,root});
    tree_queue.push_back(root);

    inserted_map.at(*vb) = true;
    MultiTreeNode* cursor = tree_queue.front();

    bool balanced_ltr = false;
    bool to_flip = false;
    std::vector<GLfloat> coords,colors;
    int count = 0;

    while (!tree_queue.empty()) {
        if (my_pos_data.to_display) {
            GLint windowWidth, windowHeight;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);
            glViewport(0, 0, windowWidth, windowHeight);

            // Draw stuff
            glClearColor(0.9, 0.9, 0.8, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glMatrixMode(GL_PROJECTION_MATRIX);
            glLoadIdentity();

            gluPerspective(60, (double) windowWidth / (double) windowHeight, 0.1, 100);

            glMatrixMode(GL_MODELVIEW_MATRIX);
            glTranslatef(0, 0, -5);

            drawCube(window,coords,colors);

            // Update Screen
            glfwSwapBuffers(window);

            // Check for any input, or window movement
            glfwPollEvents();

        } else {
            boost::graph_traits<FiniteDual>::adjacency_iterator ai, ai_end;
            MultiTreeNode *front_element = tree_queue.front();

            std::vector<MultiTreeNode *> tmp_queue;

            int state = 0;

            for (boost::tie(ai, ai_end) = boost::adjacent_vertices(front_element->id_, finiteDual); ai != ai_end; ++ai) {
                if (!inserted_map.at(*ai)) {
                    inserted_map.at(*ai) = true;

                    CGAL::Vertex_around_face_iterator<Mesh> vafb,vafe;
                    int lop = 0;
                    for(boost::tie(vafb,vafe) = CGAL::vertices_around_face(mesh.halfedge(*ai),mesh);vafb != vafe;++vafb) {
                        Point p = (Point)mesh.point(*vafb);
                        coords.push_back(p.x());
                        coords.push_back(p.y());
                        coords.push_back(p.z());

                        if(lop == 0)
                            colors.push_back(1.0);
                        else
                            colors.push_back(0.0);

                        if(lop == 1)
                            colors.push_back(1.0);
                        else
                            colors.push_back(0.0);

                        if(lop == 2)
                            colors.push_back(1.0);
                        else
                            colors.push_back(0.0);

                        lop++;
                    }
                    if(count == 250) {
                        my_pos_data.to_display = true;
                        count = 0;
                    } else
                        ++count;

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


                    node_map.insert({*ai, curr_node});
                    if (mode == ChainingType::RTL || (mode == ChainingType::BALANCED && balanced_ltr) ||
                        (mode == ChainingType::FLIP && to_flip))
                        tmp_queue.push_back(curr_node);
                    else {
                        curr_node->prev_ = cursor;
                        if (cursor != nullptr)
                            curr_node->prev_->next_ = curr_node;
                        cursor = curr_node;
                        tree_queue.push_back(curr_node);
                    }
                } else if (front_element->parent_ != nullptr) {

                    if (front_element->parent_->id_ != *ai) {
                        MultiTreeNode *old_node = node_map.at(*ai);


                        if (front_element->level_ - old_node->level_ < max_depth &&
                            front_element->level_ - old_node->level_ >= 0)
                            old_node->descendants_.push_back(front_element);
                    }
                }
            }

            if (mode == ChainingType::RTL || (mode == ChainingType::BALANCED && balanced_ltr) ||
                (mode == ChainingType::FLIP && to_flip)) {
                std::reverse(tmp_queue.begin(), tmp_queue.end());
                for (auto &node : tmp_queue) {
                    node->prev_ = cursor;
                    if (cursor != nullptr)
                        node->prev_->next_ = node;
                    cursor = node;
                    tree_queue.push_back(node);
                }
            }

            to_flip = !to_flip;
            tree_queue.erase(tree_queue.begin());
            if (mode == ChainingType::BALANCED) {
                int curr_level = front_element->level_;

                if (tree_queue.front()->level_ != curr_level) {
                    std::reverse(tree_queue.begin(), tree_queue.end());
                    balanced_ltr = !balanced_ltr;
                }
            }

            //front_element = front_element->next;
        }
    }

    return root;
}

void MultiTreeManager::treeAdoption(MultiTreeNode *root) {
    std::stack<MultiTreeNode*> depth_stack;
    depth_stack.emplace(root);

    while(!depth_stack.empty()) {
        MultiTreeNode* curr_node = depth_stack.top();
        std::vector<MultiTreeNode*> curr_children;

        int children_count = 0;

        if(curr_node->mid_) {
            curr_children.push_back(curr_node->mid_);
            ++children_count;
        }

        if(curr_node->right_) {
            curr_children.push_back(curr_node->right_);
            ++children_count;
        }

        if(curr_node->left_) {
            curr_children.push_back(curr_node->left_);
            ++children_count;
        }

        depth_stack.pop();

        if(children_count == 1 && curr_node->descendants_.size() == 1 && isChannel(curr_node)) {
            MultiTreeNode* descendant = curr_node->descendants_[0];
            if(curr_node->level_ != descendant->level_) {

                if (!(descendant->parent_->left_ && descendant->parent_->right_)) {

                    if (descendant->parent_->left_)
                        descendant->parent_->left_ = nullptr;

                    if (descendant->parent_->right_)
                        descendant->parent_->right_ = nullptr;

                    if(curr_node->level_ == descendant->parent_->level_) {
                        descendant->parent_->descendants_.emplace_back(descendant);
                        descendant->relatives_.emplace_back(descendant->parent_);
                    }
                    else {
                        descendant->parent_->relatives_.emplace_back(descendant);
                        descendant->descendants_.emplace_back(descendant->parent_);
                    }

                    descendant->parent_ = curr_node;

                    if (!curr_node->left_) {
                        curr_node->left_ = descendant;
                        if (deep_adoption_)
                            curr_children.emplace_back(descendant);
                    } else {
                        curr_node->right_ = descendant;
                        if (deep_adoption_)
                            curr_children.insert(curr_children.begin(), descendant);
                    }

                    curr_node->descendants_.clear();
                    descendant->relatives_.erase(
                            std::remove(descendant->relatives_.begin(), descendant->relatives_.end(), curr_node),
                            descendant->relatives_.end());

                    adjustDescendants(descendant);
                }
            }

            if(deep_adoption_) {
                for(auto &elem : curr_children)
                    depth_stack.push(elem);
            }

        } else
            for(auto &elem : curr_children)
                depth_stack.push(elem);
    }

    /*if(!checkTreeIntegrity(root)) {
        std::cout << "NOT CORRECT TREE" << std::endl;
        exit(0);
    }*/
}

bool MultiTreeManager::checkTreeIntegrity(MultiTreeNode *root) {
    std::queue<MultiTreeNode*> new_tree;
    new_tree.push(root);
    int count = 0;
    while(!new_tree.empty()) {
        MultiTreeNode* curr_node = new_tree.front();
        int smol = 1;

        if(curr_node->left_ != nullptr) {
            if(curr_node->left_->level_ != curr_node->level_ + 1)
                return false;
            new_tree.push(curr_node->left_);
            ++smol;
        }
        if(curr_node->right_ != nullptr) {
            if(curr_node->right_->level_ != curr_node->level_ + 1)
                return false;
            new_tree.push(curr_node->right_);
            ++smol;
        }
        if(curr_node->mid_ != nullptr) {
            if(curr_node->mid_->level_ != curr_node->level_ + 1)
                return false;
            new_tree.push(curr_node->mid_);
        }

        if(curr_node->next_)
            if(curr_node->next_->level_ < curr_node->level_)
                return false;
        if(curr_node->prev_)
            if(curr_node->prev_->level_ > curr_node->level_)
                return false;

        for(auto &desc : curr_node->descendants_) {
            ++smol;
            if (desc->level_ < curr_node->level_)
                return false;
        }

        for(auto &rel : curr_node->relatives_) {
            ++smol;
            if (rel->level_ > curr_node->level_)
                return false;
        }

        if(smol != 3)
            return false;
        new_tree.pop();
        ++count;
    }

    if(count != 69666)
        return false;

    return true;
}

bool MultiTreeManager::isChannel(MultiTreeNode* node) {

    if(!multiple_adoptions_ && node->is_adopted_)
        return false;

    std::queue<MultiTreeNode*> breadth_queue;
    breadth_queue.emplace(node);
    int curr_level = node->level_;
    int count = 0;

    while(!breadth_queue.empty()) {
        MultiTreeNode* curr_node = breadth_queue.front();

        if(curr_node->level_ - node->level_ >= conduit_depth_)
            return true;

        if(curr_node->level_ != curr_level) {
            count = 0;
            curr_level = curr_node->level_;
        }

        if(curr_node->left_) {
            breadth_queue.emplace(curr_node->left_);
            ++count;
        }

        if(curr_node->right_) {
            breadth_queue.emplace(curr_node->right_);
            ++count;
        }

        if(count > conduit_width_) return false;

        breadth_queue.pop();
    }

    return false;
}

void MultiTreeManager::adjustDescendants(MultiTreeNode *node) {
    //std::cout << "Adjusting" << std::endl;
    std::queue<MultiTreeNode*> node_queue;
    node_queue.emplace(node);

    while(!node_queue.empty()) {
        MultiTreeNode* curr_node = node_queue.front();

        curr_node->level_ = curr_node->parent_->level_ + 1;
        curr_node->is_adopted_ = true;

        std::vector<int> to_delete;
        for (int i = 0; i < curr_node->relatives_.size(); ++i) {
            MultiTreeNode* relative = curr_node->relatives_[i];

            if(relative->level_ >= curr_node->level_) {
                relative->descendants_.erase(std::remove(relative->descendants_.begin(),relative->descendants_.end(),curr_node),relative->descendants_.end());
                relative->relatives_.emplace_back(curr_node);
                curr_node->descendants_.emplace_back(relative);
                to_delete.emplace_back(i);
            }
        }

        for (int elim : to_delete) {
            curr_node->relatives_[elim] = curr_node->relatives_.back();
            curr_node->relatives_.pop_back();
        }

        if(curr_node->left_)
            node_queue.emplace(curr_node->left_);
        if(curr_node->right_)
            node_queue.emplace(curr_node->right_);
        if(curr_node->mid_)
            node_queue.emplace(curr_node->mid_);

        node_queue.pop();
    }
}

void MultiTreeManager::regenerateTree(std::vector<MultiTreeNode*>* tree_roots, std::vector<int> group_ids) {

    /*---- Adjust cuts obtained from descendants ----*/
    linkTrees(tree_roots);

    /*---- Set new levels, chains and utility fields ----*/
    regenerateTree(tree_roots->front(),group_ids);
}

void MultiTreeManager::regenerateTree(MultiTreeNode *root, std::vector<int> group_ids){
    std::queue<MultiTreeNode*> tree_queue;
    int curr_group = group_ids[root->id_];
    tree_queue.emplace(root);
    MultiTreeNode* cursor = nullptr;
    root->parent_ = nullptr;

    while(!tree_queue.empty()) {
        MultiTreeNode* curr_node = tree_queue.front();

        /*---- Reset the node member values ----*/
        curr_node->value_ = 1;
        curr_node->valid_ = true;
        curr_node->propagated_ = false;
        curr_node->prev_ = cursor;
        curr_node->next_ = nullptr;
        if(curr_node->parent_)
            curr_node->level_ = curr_node->parent_->level_ + 1;
        else
            curr_node->level_ = 0;

        if(cursor)
            curr_node->prev_->next_ = curr_node;

        /*---- Node's children can belong to another cut, so they must be separated ----*/
        if(curr_node->left_) {
            if(group_ids[curr_node->left_->id_] == curr_group) {
                tree_queue.emplace(curr_node->left_);
            } else {
                curr_node->left_ = nullptr;
            }
        }

        if(curr_node->right_) {
            if(group_ids[curr_node->right_->id_] == curr_group) {
                tree_queue.emplace(curr_node->right_);
            } else {
                curr_node->right_ = nullptr;
            }
        }

        if(curr_node->mid_) {
            if(group_ids[curr_node->mid_->id_] == curr_group) {
                tree_queue.emplace(curr_node->mid_);
            } else {
                curr_node->mid_ = nullptr;
            }
        }

        /*---- Descendants and relatives can exist only in the tree itself, so the others will be cut ----*/
        std::vector<MultiTreeNode*> desc_tmp,rels_tmp;
        for(auto &descendant : curr_node->descendants_) {
            if(group_ids[descendant->id_] == curr_group)
                desc_tmp.emplace_back(descendant);
        }
        curr_node->descendants_.clear();
        curr_node->descendants_.insert(curr_node->descendants_.end(),desc_tmp.begin(),desc_tmp.end());

        for(auto &relative : curr_node->relatives_) {
            if(group_ids[relative->id_] == curr_group)
                rels_tmp.emplace_back(relative);
        }
        curr_node->relatives_.clear();
        curr_node->relatives_.insert(curr_node->relatives_.end(),rels_tmp.begin(),rels_tmp.end());

        /*---- Move the cursor ----*/
        cursor = curr_node;
        tree_queue.pop();
    }
}

void MultiTreeManager::linkTrees(std::vector<MultiTreeNode*>* tree_roots) {

    for(unsigned int i = 0; i < tree_roots->size()-1; ++i) {
        MultiTreeNode* curr_root = tree_roots->operator[](i);

        if(!curr_root->left_) {
            curr_root->left_ = tree_roots->operator[](i+1);
            tree_roots->operator[](i+1)->parent_ = curr_root;
            continue;
        }

        if(!curr_root->right_) {
            curr_root->right_ = tree_roots->operator[](i+1);
            tree_roots->operator[](i+1)->parent_ = curr_root;
            continue;
        }
    }
}


/*-----------------------------------------------------------------*/
/*------------------------ Utility methods ------------------------*/
/*-----------------------------------------------------------------*/
void MultiTreeManager::addAreasToTree(MultiTreeNode* root, std::map<boost::graph_traits<Mesh>::face_descriptor,double> areas) {
    MultiTreeNode* curr_node = root;

    while(curr_node != nullptr) {
        double curr_area = areas[curr_node->id_];
        curr_node->area_ = curr_area;
        curr_node = curr_node->next_;
    }
}

void MultiTreeManager::addBordersToTree(MultiTreeNode *root) {

    while(root) {
        int children_count = 0;

        if(root->left_)
            ++children_count;

        if(root->right_)
            ++children_count;

        if(root->mid_)
            ++children_count;

        if(children_count == 0 && (root->relatives_.size() + root->descendants_.size()) < 2)
            root->is_border_ = true;

        if(children_count == 0 && root->relatives_.size() == 1 && root->descendants_.size() == 1 && root->descendants_[0] == root->relatives_[0])
            root->is_border_ = true;

        if(children_count == 1 && (root->relatives_.size() + root->descendants_.size()) == 0)
            root->is_border_ = true;

        if(children_count == 2 && !root->parent_)
            root->is_border_ = true;

        root = root->next_;
    }
}

/*-----------------------------------------------------------------*/
/*--------------------- Visualisation methods ---------------------*/
/*-----------------------------------------------------------------*/
void MultiTreeManager::visualizeMultiTree(MultiTreeNode *root, std::map<boost::graph_traits<Mesh>::face_descriptor,Point> centroids) {
    visualizer_->visualize3DTree(root,centroids);
}

void MultiTreeManager::compareMultiTrees(MultiTreeNode *root_a,MultiTreeNode *root_b, std::map<boost::graph_traits<Mesh>::face_descriptor,Point> centroids) {
    visualizer_->compare3DTrees(root_a,root_b,centroids);
}
