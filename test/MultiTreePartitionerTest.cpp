//
// Created by matteo on 10/08/18.
//
#include <MultiTreePartitioner.h>
#include <cstdio>
#include <InputManager.h>
#include <stopwatch.h>

int main (int argc, char* argv[]) {

    /*MultiTreeNode* node = new MultiTreeNode;
    node->value = 1;
    node->id = 0;
    node->level = 0;
    MultiTreeNode* node1 = new MultiTreeNode;
    node1->value = 1;
    node1->id = 1;
    node1->level = 1;
    MultiTreeNode* node2 = new MultiTreeNode;
    node2->value = 1;
    node2->id = 2;
    node2->level = 1;
    MultiTreeNode* node3 = new MultiTreeNode;
    node3->value = 1;
    node3->id = 3;
    node3->level = 2;
    MultiTreeNode* node4 = new MultiTreeNode;
    node4->value = 1;
    node4->id = 4;
    node4->level = 2;

    node->prev = nullptr;
    node->next = node1;
    node1->prev = node;
    node1->next = node2;
    node2->prev = node1;
    node2->next = node3;
    node3->prev = node2;
    node3->next = node4;
    node4->prev = node3;
    node4->next = nullptr;

    node->left = node1;
    node->right = node2;
    node1->left = nullptr;
    node1->right = nullptr;
    node2->left = node3;
    node2->right = node4;
    node3->left = nullptr;
    node3->right = nullptr;
    node4->left = nullptr;
    node4->right = nullptr;

    node->parent = nullptr;
    node1->parent = node;
    node2->parent = node;
    node3->parent = node2;
    node4->parent = node2;

    MultiTreePartitioner partitioner;
    partitioner.partitionByNumber(node4);*/

    InputManager input_manager;
    std::string input_filename = "../../data/WatermarkingBenchmark/bunny.off";//full_border_quads.off";
    input_manager.readMeshFromOff(input_filename);

    MultiTreeNode* root = input_manager.meshToMultiTree();
    MultiTreeNode* copy = root;
    /*while(copy != nullptr) {
        std::cout << "ID: " << copy->id << std::endl;
        std::cout << "Level: " << copy->level << std::endl;
        if(copy->parent != nullptr)
            std::cout << "Parent ID: " << copy->parent->id << std::endl;
        if(copy->left != nullptr)
            std::cout << "Left ID: " << copy->left->id << std::endl;
        if(copy->right != nullptr)
            std::cout << "Right ID: " << copy->right->id << std::endl;
        if(copy->mid != nullptr)
            std::cout << "Mid ID: " << copy->mid->id << std::endl;
        if(copy->next != nullptr)
            std::cout << "Next ID: " << copy->next->id << std::endl;
        if(copy->prev != nullptr)
            std::cout << "Prev ID: " << copy->prev->id << std::endl;
        std::cout << "Siblings: " << std::endl;
        for (int i = 0; i < copy->siblings.size(); i++) {
            std::cout << "\t" << copy->siblings.at(i)->id << std::endl;
        }
        std::cout << "Relatives: " << std::endl;
        for (int i = 0; i < copy->relatives.size(); i++) {
            std::cout << "\t" << copy->relatives.at(i)->id << std::endl;
        }
        std::cout << std::endl;
        copy = copy->next;
    }*/
    int i = 1;
    while (copy->next != nullptr) {
        copy = copy->next;
        i++;
    }

    int thresh = i / 2;
    MultiTreePartitioner partitioner;
    Stopwatch stopwatch;
    stopwatch.start();
    partitioner.partitionByNumber(copy,thresh);
    double elapsed = stopwatch.stop();
    std::cout << elapsed << std::endl;

}
