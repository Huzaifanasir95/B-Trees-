#pragma once
#include <iostream>
#include <string>
#include<queue>
using namespace std;


//struct for representing pair of values;
template <class T1, class T2>
struct Pair {
    T1 first;            //key
    T2 second;          // path

    Pair() : first(), second()
    {

    }
    Pair(const T1& a, const T2& b) {
        this->first = a;    //key
        this->second = b;   //path
    }
};


//Node class for B-tree
class TreeNode {
public:
    Pair<unsigned long long int, string>* keys;    // array to store key values
    int degree;     // max number of children
    TreeNode** childArray;//pointer to child array
    int numsKeys;    // number of keys
    bool leafFlag;      // indicates that whether the node is leaf or not
    TreeNode* nextDuplicate; // Pointer to the next node with the same key

    TreeNode() {
        this->degree = 0;
        this->leafFlag = true;
        this->keys = NULL;
        this->childArray = NULL;
        this->nextDuplicate = NULL;
        this->numsKeys = 0;

    }
    TreeNode(int var, bool flag) {
        this->degree = var;
        this->leafFlag = flag;
        this->nextDuplicate = NULL;
        this->numsKeys = 0;
        this->keys = new Pair<unsigned long long int, string>[2 * degree - 1]; // array of keys which is always 1 less than the order
        this->childArray = new TreeNode * [2 * degree];

    }

    void NonFullInsertion(unsigned long long int  key, const string& temp) {
        int var;   // var for tracking pos of insertion
        var = this->numsKeys - 1;    // we start from right most key


        // if node is leaf node then it is inserted in it directly
        if (this->leafFlag == true) {
            //keys are moved towards the right
            for (; var >= 0 && this->keys[var].first > key;) {
                this->keys[var + 1] = this->keys[var];
                var--;
            }
            // if key already exits in the node
            if (this->keys[var + 1].first == key) {
                // Handle duplicates by creating a linked list
                TreeNode* newNode = new TreeNode(degree, true);
                newNode->keys[0].first = key;
                newNode->keys[0].second = temp;
                newNode->nextDuplicate = nextDuplicate;
                this->nextDuplicate = newNode;
            }
            else {
                // inserting the key value at the correct position
                this->keys[var + 1] = Pair<unsigned long long int, string>(key, temp);
                this->numsKeys = this->numsKeys + 1;
            }


        }
        else {
            //if the node is an internal node then find the child to insert 
            for (; var >= 0 && this->keys[var].first > key;) {
                var--;
            }
            // checking if the child is full
            if (this->childArray[var + 1]->numsKeys == 2 * degree - 1)
            {
                // splitting the child to make room for insertion
                childSplitting(var + 1, this->childArray[var + 1]);

                // Adjusting the index if the key to insert is greater than the split key.
                if (this->keys[var + 1].first < key)
                    var++;
            }

            //recursion to insert into no full child node
            this->childArray[var + 1]->NonFullInsertion(key, temp);
        }
    }

    void childSplitting(int var, TreeNode* temp) {    // var= index to split the child  temp= child node to be spilt


        TreeNode* node = new TreeNode(temp->degree, temp->leafFlag);  // creating a node with same degree and same leaf flag as the child
        node->numsKeys = degree - 1;

        for (int i = 0; i < degree - 1; i++) { // Copy the rightmost keys from the full child to the new node.
            node->keys[i] = temp->keys[i + degree];
        }
        // if the child is an internal node then copy the rightmost degree children too
        if (temp->leafFlag == false) {
            for (int j = 0; j < degree; j++) {
                node->childArray[j] = temp->childArray[j + degree];
            }
        }


        temp->numsKeys = degree - 1;  // num keys in the original 
        // move the pointers of children in the current node to make space for the newchild
        for (int i = numsKeys; i >= var + 1; i--) {
            this->childArray[i + 1] = this->childArray[i];
        }
        this->childArray[var + 1] = node;// insert the new node as a child of the current node


        // move the keys in the current node to make space for the split key from the child
        for (int i = numsKeys - 1; i >= var; i--) {
            this->keys[i + 1] = this->keys[i];
        }
        this->keys[var] = temp->keys[degree - 1]; // copy the split key from the child to the current node
        this->numsKeys = this->numsKeys + 1;  // increment the number of keys in the current node
    }
    void print() {
        int i;
        for (i = 0; i < this->numsKeys; i++)
        {
            // if the node is not a leaf then recurcion print the child nodes
            if (this->leafFlag == false) {
                this->childArray[i]->print();
            }
            cout << " " << this->keys[i].first << this->keys[i].second;

        }
        //if the node is not a leaf then recursion to print the last child node
        if (this->leafFlag == false) {
            this->childArray[i]->print();

        }
    }
    TreeNode* search(unsigned long long int  key) {
        int i = 0;
        while (i < this->numsKeys && key > this->keys[i].first) {
            i++;
        }
        if (this->keys[i].first == key) {
            cout << "found";
            return this;   // returns  current node
        }

        // if the node is a leaf and the key is not found ret nullptr
        if (this->leafFlag == true) {
            cout << "not";
            return nullptr;
        }

        return this->childArray[i]->search(key);  // recusrion to search for key in child node
    }
    int KeyFind(unsigned long long int  temp) {
        int index = 0;
        while (index < this->numsKeys && this->keys[index].first < temp) {
            ++index;
        }
        return index; // returuns the index of the key in the array
    }
    void deletion2(unsigned long long int  temp) {
        int ind;
        ind = KeyFind(temp);  // index of key

        // key is found
        if (ind < this->numsKeys && this->keys[ind].first == temp) {
            if (this->leafFlag == true) {// if node is leaf node then simple remove
                LeafRemoval(ind);
            }
            else { // else remove the key from interna l ndoe
                NonLeafRemoval(ind);
            }
        }
        else {
            if (this->leafFlag == true) {   // if key is not found and node is also the leaf node
                cout << "The key " << temp << " does not exist" << endl;
                return;
            }
            // Determine whether the key is to be deleted from the leftmost or rightmost child

            bool flag;
            if (ind == this->numsKeys) {
                flag = true;
            }
            else {
                flag = false;
            }
            if (this->childArray[ind]->numsKeys < degree) {
                fill(ind);  // child to delete is less in ekys and then filing it
            }
            // recursion to delte the key from the corect child
            if (flag == true && ind > this->numsKeys) {
                this->childArray[ind - 1]->deletion2(temp);
            }
            else {
                this->childArray[ind]->deletion2(temp);

            }
        }
        return;
    }
    void LeafRemoval(int index) { // index of key to remove
        if (nextDuplicate) {  // if duplicate is present then delete that firsr
            TreeNode* temp = nextDuplicate;
            this->nextDuplicate = nextDuplicate->nextDuplicate;
            delete temp;
        }
        // If there is no nextDuplicate then shift keys to the left to remove the key
        else {
            for (int i = index + 1; i < this->numsKeys; ++i) {
                this->keys[i - 1] = keys[i];
            }
            this->numsKeys--; // num of keys dec
        }
        return;
    }
    void NonLeafRemoval(int index) {  // indx to remove the key
        int temp;
        temp = this->keys[index].first; // temp=key from that index

        // if the left child has enough keys find the predecessor replace the key and delete the predecessor
        if (this->childArray[index]->numsKeys >= degree) {
            int temp1;
            temp1 = Predecessor(index);
            this->keys[index] = Pair<unsigned long long int, string>(temp1, "");
            this->childArray[index]->deletion2(temp1);
        }
        // if the left child does not have enough keys but the right child does find the successor  replace the key  and delete the successor
        else if (childArray[index + 1]->numsKeys >= degree) {
            int temp2;
            temp2 = Successor(index);
            this->keys[index] = Pair<unsigned long long int, string>(temp2, "");
            this->childArray[index + 1]->deletion2(temp2);
        }
        // if both the left and right children have the minimum number of keys perform a merge

        else {
            if (nextDuplicate) { // if duplicate is present deleteiing that
                TreeNode* node = nextDuplicate;
                this->nextDuplicate = nextDuplicate->nextDuplicate;
                delete node;
            }
            // if there is no nextDuplicate performing merge 

            else {
                merge(index);
                this->childArray[index]->deletion2(temp);
            }
        }
        return;
    }
    int Predecessor(int index) { // index of key
        // start from the left child and traverse to the rightmost leaf to find the predecessor
        TreeNode* temp = childArray[index];
        while (temp->leafFlag == NULL) {
            temp = temp->childArray[temp->numsKeys];
        }
        return temp->keys[temp->numsKeys - 1].first;  // Return the rightmost key in the leaf node
    }

    int Successor(int index) {  // index of key
        //start from the right child and traverse to the leftmost leaf to find the successor
        TreeNode* temp = childArray[index + 1];
        while (temp->leafFlag == NULL) {
            temp = temp->childArray[0];
        }
        return temp->keys[0].first;  // Return the leftmost key in the leaf node.
    }
    //fill the child node at the given index by borrowing a key from its left or right sibling
    void fill(int index) {  // index of key
        if (index != 0 && this->childArray[index - 1]->numsKeys >= this->degree) { // if left has enough keys borrow
            PrevBorrow(index);
        }
        else if (index != this->numsKeys && this->childArray[index + 1]->numsKeys >= this->degree) {// if right has enough keys borrow
            NextBorrow(index);
        }
        else { // both have less?? merge then
            if (index != this->numsKeys) { // currentis not = rightmost node then merge right
                merge(index);
            }
            else {  // curent== reight most merge left
                merge(index - 1);
            }
        }
        return;
    }

    void PrevBorrow(int index) {  // index of child 
        TreeNode* sibl = this->childArray[index - 1]; // left sibling of child node
        TreeNode* node = this->childArray[index];  // child noed


        // Shift keys in the child to make space for the borrowed key
        for (int i = node->numsKeys - 1; i >= 0; --i) {
            node->keys[i + 1] = node->keys[i];
        }
        // Shift child pointers if the node is not a leaf
        if (node->leafFlag == NULL) {
            for (int i = node->numsKeys; i >= 0; --i) {
                node->childArray[i + 1] = node->childArray[i];
            }
        }
        // Copy the key from the parent to the child
        node->keys[0] = keys[index - 1];
        // Copy the child pointer from the left to the child if it not a leaf
        if (node->leafFlag == NULL) {
            node->childArray[0] = sibl->childArray[sibl->numsKeys];
        }
        // Update the parent key with the rightmost key of the left sibling
        this->keys[index - 1] = sibl->keys[sibl->numsKeys - 1];

        node->numsKeys += 1;   // keys in child
        sibl->numsKeys -= 1; // keys in siblins

        return;
    }

    void NextBorrow(int index) {    // indx of child 
        TreeNode* sibl = this->childArray[index + 1]; // right sibling
        TreeNode* node = this->childArray[index];   // child node

        //copy key from parent to child
        node->keys[(node->numsKeys)] = keys[index];
        // Copy the child pointer from the right sibling to the child if it not a leaf.
        if ((node->leafFlag) == NULL) {
            node->childArray[(node->numsKeys) + 1] = sibl->childArray[0];
        }
        // update the paretnt key with leftmost key of right sibling
        this->keys[index] = sibl->keys[0];
        // shift keys in rifht to fill gap
        for (int i = 1; i < sibl->numsKeys; ++i) {
            sibl->keys[i - 1] = sibl->keys[i];
        }
        // shift child in the right sibling if its not a leaf
        if (sibl->leafFlag == NULL) {
            for (int i = 1; i <= sibl->numsKeys; ++i) {
                sibl->childArray[i - 1] = sibl->childArray[i];
            }
        }

        node->numsKeys += 1; // keys in child
        sibl->numsKeys -= 1; // keys in sibling

        return;
    }

    void merge(int index) {   // index of child
        TreeNode* sibl = this->childArray[index + 1]; // child right sibling
        TreeNode* node = this->childArray[index];  // child

        // copy key from parent to child node
        node->keys[degree - 1] = this->keys[index];
        for (int i = 0; i < sibl->numsKeys; ++i) {
            node->keys[i + degree] = sibl->keys[i];
        }
        // coy child pointer from right sibiing to the child if not leaf
        if (node->leafFlag == NULL) {
            for (int i = 0; i <= sibl->numsKeys; ++i) {
                node->childArray[i + degree] = sibl->childArray[i];
            }
        }
        // shift keys in pareat to fil gap
        for (int i = index + 1; i < this->numsKeys; ++i) {
            this->keys[i - 1] = keys[i];
        }
        // shift child pointer in parent to fll gap
        for (int i = index + 2; i <= this->numsKeys; ++i) {
            this->childArray[i - 1] = this->childArray[i];
        }
        node->numsKeys += sibl->numsKeys + 1;
        this->numsKeys--;
        // delete right as it has been merged 
        delete (sibl);
        return;
    }

    void levelOrderTraversal() {
        if (this == NULL) { // empty check
            cout << "Empty" << endl;
            return;
        }

        queue<TreeNode*> temp;
        temp.push(this);   // enqueue the root node

        while (temp.empty() == NULL) {
            int num;
            num = temp.size(); // num of node
            for (int i = 0; i < num; ++i) { // process each node at current lvl
                TreeNode* node = temp.front(); // dequeue a node from the front
                temp.pop();

                cout << "[ ";
                for (int j = 0; j < node->numsKeys; ++j) {
                    cout << node->keys[j].first << " " << node->keys[j].second << " ";
                }
                cout << "] ";
                // enqueue child node if the current node is not a leaf
                if (node->leafFlag == NULL) {
                    for (int j = 0; j <= node->numsKeys; ++j) {
                        if (node->childArray[j]) {
                            temp.push(node->childArray[j]);
                        }
                    }
                }
            }
            cout << endl;
        }
    }

};

class BTree {
public:
    TreeNode* root; // root of B-tree
    int degree;

    BTree()
    {
        degree = 0;
        root = NULL;
    }
    BTree(int var)
    {
        this->degree = var;
        this->root = NULL;
    }
    void set_order(int var)
    {
        degree = var;
    }
    void print()
    {
        if (this->root != NULL)
            this->root->print();  // prints in assemding order :) maybe i am cheems
    }

    TreeNode* search(unsigned long long int  key)
    {
        if (this->root == NULL) {
            return NULL;
        }
        else {
            return root->search(key);
        }

    }
    void levelOrderTraversal() {
        if (this->root == NULL) {  // empty check
            cout << "Empty" << endl;
            return;
        }

        root->levelOrderTraversal();
    }

    TreeNode* insert(unsigned long long int  key, const string& str)
    {
        if (this->root == NULL) { // empty chcek if empty then create new root and insert
            this->root = new TreeNode(degree, true);
            this->root->keys[0] = Pair<unsigned long long int, string>(key, str);
            this->root->numsKeys = 1;
            this->root->childArray = NULL;
        }
        else {
            // if root is full then create new root and spilt the current root
            if (this->root->numsKeys == 2 * degree - 1) {
                TreeNode* temp = new TreeNode(degree, false);

                temp->childArray[0] = this->root;
                // split child and insert key into corect child
                temp->childSplitting(0, root);

                int i = 0;
                if (temp->keys[0].first < key) {
                    i++;
                }
                temp->childArray[i]->NonFullInsertion(key, str);
                // root updated
                this->root = temp;
            }
            else
                this->root->NonFullInsertion(key, str);
        }
        return this->root;
    }
    void deletion(unsigned long long int  temp) {
        if (this->root == NULL) {
            //tree is empty
            return;
        }

        this->root->deletion2(temp);
        // if root becomes empty update teh root
        if (this->root->numsKeys == 0) {
            TreeNode* node = this->root;
            if (this->root->leafFlag) {
                this->root = NULL;
            }
            else {
                this->root = root->childArray[0];
            }
            delete node;
        }
        return;
    }
};


