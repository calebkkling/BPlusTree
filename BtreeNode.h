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
#include <fstream>
#include <sstream>
#include "SDLPlotter.h"
#include "rectangle.h"

using namespace std;

class Bptree {
private:

    //degree of the tree
    int order;



    struct node {
        node* parent;
        vector<int> key;
        bool isleaf;
        int value;
        vector<node*> children;

        node() {
            parent = nullptr;
            isleaf = true;
            vector<int> key[7];
            vector<node*> children[8];
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

        node(node* p,int k, bool l) {
            parent = p;
            key.push_back(k);
            isleaf = true;
//            value = -1;
        }

        ~node(){};
    };

    node* root = nullptr;

    //element that last visited
    node* last_visited = nullptr;

public:


    Bptree(int o){
        root = nullptr;
        order = o;

    }

    ~Bptree(){ //not implemented

    }

    node* search(int x) {
        node* v = root;
        int i;
        while (v) {
            for (i = 0; i < v->key.size() && x >= v->key[i] ; i++){
                if (x == v->key[i] && v->isleaf) {
                    last_visited = v;
                    return v;
                }
            }

            last_visited = v;
            if(hasChildren(v)){
                v = v->children[i];
            }else{
                return nullptr;
            }

            i = 0;


        }

        return nullptr;

    }



    node* merge(node*, node*);

    size_t findPos(vector<int> arr,int comp) {
        size_t i;
        for (i = 0; i < arr.size() && comp > arr[i]; i++) {}
        return i;
    }

    bool isFull(node* n) {
        return n->key.size() == this->order;
    }

    bool hasChildren(node* n) {
        return n->children.size()!= 0;
    }

    template <typename T>
    void shiftRight(vector<T> v,size_t pos){
//        T a;
//        v.push_back(a);
        for(size_t i = v.size(); i > pos; i--){
            v.insert(v.begin()+i,v[i-1]);
        }
    }



    // haven't tested yet
    void solveOverFlow(node* ptr) {
        while (ptr && ptr->key.size() >= order) {
            size_t pivot = ptr->key.size() / 2;

            if(!ptr->isleaf){
                //create a right children
                node* rc = new node();




                if(root == ptr && !hasChildren(root)){
                    for (int i = pivot; i < ptr->key.size(); i++) {
                        rc->key.push_back(ptr->key[i]);
                    }

                    ptr->key.erase(ptr->key.begin() + pivot, ptr->key.end());

                    node* temp = new node();
                    temp->isleaf = false;
                    rc->parent = temp;

                    //new root has the key that suppose to be pushed up
                    temp->key.push_back(rc->key[0]);


                    //store the children in new root
                    temp->children.push_back(ptr);
                    temp->children.push_back(rc);

                    this->root = temp;
                    ptr->parent = root;
                    ptr->isleaf = true;


                }else{
                    int pushed = ptr->key[pivot];
                    for (int i = pivot + 1; i < ptr->key.size(); i++) {
                        rc->key.push_back(ptr->key[i]);
                    }

                    ptr->key.erase(ptr->key.begin() + pivot, ptr->key.end());

                    if (hasChildren(ptr)) {
                        for (size_t i = pivot + 1,j = 0; i < ptr->children.size(); i++,j++) {
                            rc->children.push_back(ptr->children[i]);
                            rc->children[j]->parent = rc;
                        }


                        ptr->children.erase(ptr->children.begin() + pivot+1, ptr->children.end());
                    }

                    rc->isleaf = false;
                    ptr->isleaf = false;

                    if (root == ptr) {

                        node* temp = new node();
                        temp->isleaf = false;
                        rc->parent = temp;

                        temp->key.push_back(pushed);
                        //store the children in new root
                        temp->children.push_back(ptr);
                        temp->children.push_back(rc);
                        this->root = temp;
                        ptr->parent = root;

                    }

                    else {
                        size_t pos = findPos(ptr->parent->key, ptr->key[pivot]);
                        shiftRight(ptr->parent->key, pos);

                        ptr->parent->key.insert(ptr->parent->key.begin()+pos,pushed);
                        shiftRight(ptr->parent->children, pos + 1);

                        //ptr->parent->children.insert(ptr->parent->children.begin()+pos,ptr);
                        ptr->parent->children.insert(ptr->parent->children.begin()+pos+1,rc);
                        rc->parent = ptr->parent;
                    }



                }



            }else{

                size_t pos = findPos(ptr->parent->key, ptr->key[pivot]);

                shiftRight(ptr->parent->key,pos);
                ptr->parent->key.insert(ptr->parent->key.begin()+pos,ptr->key[pivot]);

                if(pos == 0){
                    node* left = new node();
                    //left->isleaf = true;
                    for(size_t i = 0; i < pivot; i++){
                        left->key.push_back(ptr->key[i]);
                    }
                    left->parent = ptr->parent;
                    size_t i = pivot,j = 0,size = ptr->key.size();
                    for(i,j; i < size; i++,j++){
						swap(ptr->key[j], ptr->key[i]);
                    }
                    ptr->key.erase(ptr->key.begin()+j,ptr->key.end());


                    shiftRight(ptr->parent->children,0);
					ptr->parent->children.insert(ptr->parent->children.begin(), left);
                    left->parent = ptr->parent;

                }else{
                    node* right = new node();
                    //right->isleaf = true;
                    for(size_t i = pivot; i < ptr->key.size(); i++){
                        right->key.push_back(ptr->key[i]);

                    }
                    ptr->key.erase(ptr->key.begin()+pivot,ptr->key.end());


                    shiftRight(ptr->parent->children,pos+1);
                    ptr->parent->children.insert(ptr->parent->children.begin()+pos+1,right);
                    right->parent = ptr->parent;



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


//            if(last_visited->isleaf){
            //make room for store item at position
//                last_visited->key.push_back(0);
//                last_visited->children.push_back(nullptr);
            shiftRight(last_visited->key,i);
//            shiftRight(last_visited->children,i);
//                rotate(last_visited->children.rbegin(),last_visited->children.rbegin()+i,last_visited->children.rend());

            //store the item
            last_visited->key.insert(last_visited->key.begin()+i,item);

//            last_visited->children[i]->key.push_back(item);
//            last_visited->children[i]->value = item;
//            last_visited->children[i]->isleaf = true;
//            last_visited = last_visited->children[i];
//            }else{
//                last_visited = last_visited->children[i];
//                shiftRight(last_visited->key,i);
//                last_visited->key.insert(last_visited->key.begin()+i,item);
//
//            }


        }

        solveOverFlow(last_visited);
    }

    void remove(int);


    int xDimension(){
        queue<node *> queue1, queue2;
        int count = 0;
        if(!root){
            return count;
        }
        queue1.push(root);
        while(!queue1.empty() || !queue2.empty()){
            while(!queue1.empty()){
                count+=10;
                if(hasChildren(queue1.front())){
                    for (auto i : queue1.front()->children) {
                        queue2.push(i);
                    }
                }
                for(auto i : queue1.front()->key){
                    count+=60;
                }
                queue1.pop();
            }
            if(!queue2.empty()){
                count=0;
            }
            while(!queue2.empty()){
                count+=10;
                if(hasChildren(queue2.front())){
                    for (auto i : queue2.front()->children) {
                        queue1.push(i);
                    }
                }
                for(auto i : queue1.front()->key){
                    count+=60;
                }
                queue2.pop();
            }
            if(!queue1.empty()){
                count = 0;
            }
        }
        return count;
    }

    void printDigit(SDL_Plotter &g, int a, point P, color color1, int scale) {
        ifstream input;
        int rows, cols;
        switch(a){
            case 0:
                input.open("0.txt");
                break;
            case 1:
                input.open("1.txt");
                break;
            case 2:
                input.open("2.txt");
                break;
            case 3:
                input.open("3.txt");
                break;
            case 4:
                input.open("4.txt");
                break;
            case 5:
                input.open("5.txt");
                break;
            case 6:
                input.open("6.txt");
                break;
            case 7:
                input.open("7.txt");
                break;
            case 8:
                input.open("8.txt");
                break;
            case 9:
                input.open("9.txt");
                break;
            default:
                break;
        }
        input >> rows >> cols;
        char letter[rows][cols];
        for(int r = 0; r < rows; r++){
            for(int c = 0; c < cols; c++){
                input >> letter[r][c];
            }
        }
        for(int y = 0; y < rows; y++){
            for(int x = 0; x < cols; x++){
                for(int r=0; r<scale; r++){
                    for(int c=0; c<scale; c++){
                        if(letter[y][x]=='1'){
                            g.plotPixel(P.x+scale*x+r, P.y+scale*y+c, color1.R, color1.G, color1.B);
                        }
                    }
                }
            }
        }
        input.close();

    }

    void plotNumber(const string message, int scale, point P, SDL_Plotter &g, color C) {
        size_t n=0;
        while(n<message.length()){
            switch(message[n]){
                case '0':
                    printDigit(g, 0, P, C, scale);
                    break;
                case '1':
                    printDigit(g, 1, P, C, scale);
                    break;
                case '2':
                    printDigit(g, 2, P, C, scale);
                    break;
                case '3':
                    printDigit(g, 3, P, C, scale);
                    break;
                case '4':
                    printDigit(g, 4, P, C, scale);
                    break;
                case '5':
                    printDigit(g, 5, P, C, scale);
                    break;
                case '6':
                    printDigit(g, 6, P, C, scale);
                    break;
                case '7':
                    printDigit(g, 7, P, C, scale);
                    break;
                case '8':
                    printDigit(g, 8, P, C, scale);
                    break;
                case '9':
                    printDigit(g, 9, P, C, scale);
                    break;
                default:
                    break;
            }
            n++;
            P.x+=(18*scale);
        }
    }

    void drawNode(SDL_Plotter& g, node * ptr, point P, color C){
        string arr;
        int num;
        stringstream ss;
        for (int i : ptr->key) {
            num = i;
            if(0<=num && num<10){
                ss << "00";
            }else if(10<=num && num<100){
                ss << "0";
            }
            ss << num;
            ss >> arr;
            ss.clear();
            rectangle rec(P, point(P.x+60, P.y+25));
            rec.drawOutline(g);
            plotNumber(arr, 1, point(P.x+5, P.y+2), g, C);
            P.x+=60;
        }
    }

    void levelOrder(ostream& os){
        queue<node *> queue1, queue2;
        if(!root){
            os << "NULL" << endl;
            return;
        }
        queue1.push(root);
        while(!queue1.empty() || !queue2.empty()){
            while(!queue1.empty()){
                if(hasChildren(queue1.front())){
                    for (auto i : queue1.front()->children) {
                        queue2.push(i);
                    }
                }
                for(int i = 0; i<queue1.front()->key.size(); i++){
                    if(!queue1.front()->isleaf && queue1.front()!=root){
                        os << "K: ";
                    }
                    os << queue1.front()->key[i] << " ";
                }
                queue1.pop();
                os << "   ";
            }
            os << endl;
            while(!queue2.empty()){
                if(hasChildren(queue2.front())){
                    for (auto i : queue2.front()->children) {
                        queue1.push(i);
                    }
                }
                for(int i = 0; i<queue2.front()->key.size(); i++){
                    if(!queue2.front()->isleaf && queue2.front()!=root){
                        os << "K: ";
                    }
                    os << queue2.front()->key[i] << " ";
                }
                queue2.pop();
                os << "   ";
            }
            os << endl;
        }
        os << endl;
    }

    void drawGraph(SDL_Plotter &plotter){
        plotter.clear();
        queue<node *> queue1, queue2;
        queue1.push(root);
        int x=5, y=5;
        while(!queue1.empty() || !queue2.empty()){
            while(!queue1.empty()){
                if(hasChildren(queue1.front())){
                    for (auto i : queue1.front()->children) {
                        queue2.push(i);
                    }
                }
                drawNode(plotter, queue1.front(), point(x, y), color(0, 0, 0));
                x+=queue1.front()->key.size()*60+10;
                queue1.pop();
            }
            y+=50;
            x=5;
            while(!queue2.empty()){
                if(hasChildren(queue2.front())){
                    for (auto i : queue2.front()->children) {
                        queue1.push(i);
                    }
                }
                drawNode(plotter, queue2.front(), point(x, y), color(0, 0, 0));
                x+=queue2.front()->key.size()*60+10;
                queue2.pop();
            }
            y+=50;
            x = 5;
        }
        plotter.update();
    }





};

#endif //B_TREE_BTREENODE_H