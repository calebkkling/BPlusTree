/*
 * Group project B+ Tree
 *
 *
 *
 *
 *
 *
 * Last Modified: 11/25/2019
 *
 *
 *11/27/2019
 * -add utility function,insertion and solve for overflow(neither checked)
 *
 * 12/2/2019
 * -modified insert,solve for overflow,level order traversal and search
 * -initial value to -1 is unnecessary, a node with no parents(disconnected node) is not yet have value stored
 */

#ifndef B_TREE_BTREENODE_H
#define B_TREE_BTREENODE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <algorithm>

using namespace std;

class Bptree
{
private:
    //degree of the tree
    int order;

    struct node
    {
        node *parent;
        vector<int> key;
        bool isleaf;
        int value;
        vector<node *> children;

        node()
        {
            parent = nullptr;
            isleaf = true;
            vector<int> key[7];
            vector<node *> children[7];
            value = -1; //value initialized to -1 to show that nothing is stored in the node
            // (Like with the visualization, only positive numbers can be added to the tree)
        }

        //        node(node* p, vector<int> k, const vector<node*> c) {
        //            parent = p;
        //            if (!k.empty()) {
        //                for (int i = 0; i < order - 1; i++) {
        //                    key.push_back(k[i]);
        //                }
        //            }
        //            isleaf = false;
        //            for (auto i : c) {
        //                children.push_back(i);
        //            }
        //            value = -1;
        //        }

        node(node *p, int k, bool l)
        {
            parent = p;
            key.push_back(k);
            isleaf = true;
            //            value = -1;
        }

        ~node(){};
    };

    node *root = nullptr;

    //element that last visited
    node *last_visited = nullptr;

public:
    Bptree(int o)
    {
        root = nullptr;
        order = o;
    }

    ~Bptree()
    { //not implemented
        if(hasChildren(root)){

        }
    }

    node *search(int x)
    {
        node *v = root;
        int i;
        while (v)
        {
            for (i = 0; i < v->key.size() && x > v->key[i]; i++)
            {
                if (x == v->key[i] && v->isleaf)
                {
                    last_visited = v;
                    return v;
                }
            }
            last_visited = v;
            if (hasChildren(v))
            {
                v = v->children[i];
            }
            else
            {
                return nullptr;
            }

            i = 0;
        }

        return nullptr;
    }

    node *merge(node *, node *);

    size_t findPos(vector<int> arr, int comp)
    {
        size_t i;
        for (i = 0; i < arr.size() && comp > arr[i]; i++)
        {
        }
        return i;
    }

    bool isFull(node *n)
    {
        return n->key.size() == this->order;
    }

    bool hasChildren(node *n)
    {
        return !n->children.empty();
    }

    template <typename T>
    void shiftRight(vector<T> v, size_t pos)
    {
        T a();
        v.push_back(a);
        for (size_t i = v.size(); i > pos; i--)
        {
            swap(v[i], v[i - 1]);
        }
    }

    // haven't tested yet
    void solveOverFlow(node *ptr)
    {
        while (ptr && ptr->key.size() >= order)
        {
            size_t pivot = ptr->key.size() / 2;

            if (!ptr->isleaf)
            {
                //create a right children
                node *rc = new node();

                //                if(!hasChildren(root)){
                //                    size_t i = pivot+1;
                //                }

                //right part after the split
                for (size_t i = pivot; i < ptr->key.size(); i++)
                {
                    rc->key.push_back(ptr->key[i]);
                }

                ptr->key.erase(ptr->key.begin() + pivot, ptr->key.end());

                if (hasChildren(ptr))
                {
                    for (size_t i = pivot; i < ptr->children.size(); i++)
                    {
                        rc->children.push_back(ptr->children[i]);
                    }

                    ptr->children.erase(ptr->children.begin() + pivot, ptr->children.end());
                }

                if (root == ptr)
                {
                    node *temp = new node();
                    temp->isleaf = false;
                    rc->parent = temp;

                    //new root has the key that suppose to be pushed up
                    temp->key.push_back(rc->key[0]);

                    //store the children in new root
                    temp->children.push_back(ptr);
                    temp->children.push_back(rc);

                    this->root = temp;
                    ptr->parent = root;
                }
                else
                {
                    size_t pos = findPos(ptr->parent->key, ptr->key[pivot]);
                    shiftRight(ptr->parent->key, pos);

                    ptr->parent->key[pos] = ptr->key[pivot];
                    shiftRight(ptr->parent->children, pos + 1);

                    ptr->parent->children[pos] = ptr;
                    ptr->parent->children[pos + 1] = rc;
                    rc->parent = ptr->parent;
                }
            }
            else
            {

                size_t pos = findPos(ptr->parent->key, ptr->key[pivot]);

                shiftRight(ptr->parent->key, pos);
                ptr->parent->key[pos] = ptr->key[pivot];

                if (pos == 0)
                {
                    node *left = new node();
                    left->isleaf = false;
                    for (size_t i = 0; i < pivot; i++)
                    {
                        left->key.push_back(ptr->key[i]);
                    }
                    left->parent = ptr->parent;
                    size_t i = pivot, j = 0;
                    for (i, j; i < ptr->key.size(); i++, j++)
                    {
                        ptr->key[j] = ptr->key[i];
                    }
                    ptr->key.erase(ptr->key.begin() + j, ptr->key.end());

                    shiftRight(ptr->parent->children, 0);
                    ptr->parent->children[0] = left;
                }
                else
                {
                    node *right = new node();
                    right->isleaf = false;
                    for (size_t i = pivot; i < ptr->key.size(); i++)
                    {
                        right->key.push_back(ptr->key[i]);
                    }
                    ptr->key.erase(ptr->key.begin() + pivot, ptr->key.end());

                    shiftRight(ptr->parent->children, pos + 1);
                    ptr->parent->children[pos + 1] = right;
                }
            }

            ptr = ptr->parent;
        }
    }

    //insert success or not, not tested as well
    bool insert(int item) {

        node* y = search(item);
        if (y) {
            return false;
        }
        if (root == nullptr) {
            root = new node();
            root->isleaf = false;
            root->key.push_back(item);
            last_visited = root;

        }else if(!hasChildren(root)){
            size_t pos = findPos(root->key,item);

            shiftRight(root->key,pos);
            root->key.insert(root->key.begin()+pos,item);
        }
        else {


            int i = findPos(last_visited->key, item);



            shiftRight(last_visited->key,i);

            last_visited->key.insert(last_visited->key.begin()+i,item);


        }

        solveOverFlow(last_visited);
    }

    void remove(int val){ //INCOMPLETE
        if(search(val)){  //if value is not found in the Tree, don't do anything
            node *v = root;
            bool deleted = false;
            int i;
            //find the node and remove node
            //using same method to get to node as search
            while (v)
            {
                for (i = 0; i < v->key.size() && val > v->key[i]; i++)
                {
                    if (val == v->key[i] && v->isleaf)
                    {

                        v = nullptr;
                        deleted = true;

                        //Check if node needs to replace the deleted one
                        if(last_visited == v){
                            if(hasChildren(last_visited)){
                                last_visited->key[i] = last_visited->children[i]->key[i];
                            }
                        }

                        //Shift a lot of stuff

                        break; //exits for loop once node is deleted

                    }
                }
                if(deleted){
                    break;  //exits while loop after node is deleted
                }
                last_visited = v;
                if (hasChildren(v))
                {
                    v = v->children[i];
                }

                i = 0;
            }

        }
    }

    void levelOrder(ostream &os)
    {
        queue<node *> traversalQueue;
        traversalQueue.push(root);
        while (!traversalQueue.empty())
        {
            node *current = traversalQueue.front();
            traversalQueue.pop();
            for (auto i : current->key)
            {
                os << current->key[i] << " ";
            }
            for (size_t j = 0; j < current->children.size(); j++)
            {
                traversalQueue.push(current->children[j]);
            }
        }
    }
};

#endif //B_TREE_BTREENODE_H