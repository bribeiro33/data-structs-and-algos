// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <vector>
#include <utility>

// A specialized version of the 'priority queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra posize_ter (see below), be sure to
            // initialize it here.
            explicit Node(const TYPE &val)
        : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;
    }; // Node


    // Description: Construct an empty pairing heap with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) : BaseClass{ comp } {
        root = nullptr;
        this->compare = comp;
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) : BaseClass{ comp } {
        root = nullptr;
        count = 0;
        this->compare = comp;
        while (start != end){
            push(*start);
            ++start;
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) : BaseClass{ other.compare } {
        this->compare = other.compare;
        count = other.count;
        root = copy(other.root);
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        // Hsize_t: Use the copy-swap method from the "Arrays and Containers" lecture.
        if(this != &rhs){
            clear();
            root = copy(rhs.root);
            this->compare = rhs.compare;
            this->count = rhs.count;

        }
         return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        clear();
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out of order and
    //              'rebuilds' the pairing heap by fixing the pairing heap invariant.
    //              You CANNOT delete 'old' nodes and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        if (empty()){
            return;
        }
        std::vector<TYPE> temp;

        while (!empty()) {
            temp.push_back(top());
            pop();
        }

        while (!temp.empty()) {
            push(temp.back());
            temp.pop_back();
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already done.
    //              You should implement push functionality entirely in the addNode()
    //              function, and this function calls addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the pairing heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the pairing heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        --count;
        if (root->child == nullptr) {
            delete root;
            root = nullptr;
        }
        
        else{
    
            Node * temp_root = root;
            Node * temp_child = root->child;
            temp_root->child = nullptr;
            std::deque<Node*> deque;
            
            //ptr shift
            Node* temp_node_a;
            Node* temp_node_b;

            while (temp_child) {
                temp_node_a = temp_child;
                temp_child = temp_child->sibling;
                deque.push_back(temp_node_a);
            }
            
            //delete
            temp_node_a = nullptr;
            temp_node_b = nullptr;
            
            //1 element in deque
            while (deque.size() > 1) {
                //acquire
                temp_node_a = deque.front();
                deque.pop_front();
                temp_node_b = deque.front();
                deque.pop_front();
                
                //meld
                Node* meldNode = meld(temp_node_a, temp_node_b);
                deque.push_back(meldNode);
                
                //delete
                temp_node_a = nullptr;
                temp_node_b = nullptr;
            }
            
            if (deque.empty()) {
                root = temp_root;
            }
            
            else {
                delete root;
                root = nullptr;
                root = deque.front();
            }
        }
        
    }


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return root->elt;
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return static_cast<size_t> (count);
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return (root == nullptr);
    } // empty()


    // Description: Updates the priority of an element already in the pairing heap by
    //              replacing the element refered to by the Node with new_value.
    //              Must masize_tain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        node->elt = new_value;
        updatePriorities();
    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE &val) {
        Node* node = new Node(val);
        if (!root){
            root = node;
        }
        
        else{
            root = meld(root, node);
        }
        ++count;
        return node;
    } // addNode()


private:
    // TODO: Add any additional member variables or member functions you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).

    // NOTE: For member variables, you are only allowed to add a "root posize_ter"
    //       and a "count" of the number of nodes.  Anything else (such as a deque)
    //       should be declared inside of member functions as needed.
    Node *root;
    int count = 0;
    
    Node * meld(Node* left, Node* right){
    
        if (right == nullptr) {
            return right;
        }

        else if (right == nullptr) {
            return left;
        }
        
        else if (this->compare(left->elt, right->elt)) {
            left->sibling = right->child;
            right->child = left;
            right->sibling = nullptr;
            right->parent = left;
            return right;
        }
        
        else {
            right->sibling = left->child;
            left->child = right;
            left->sibling = nullptr;
            right->parent = left->parent;
            left->parent = right;
            return left;
        }
    }
    
    void clear(){
        deletePQ(root);
        root = nullptr;
    }
    
    void deletePQ(Node *root) const {
        if (root != nullptr){
            deletePQ(root->child);
            deletePQ(root->sibling);
            delete root;
        }
    }
    
    Node* copy(const Node* node) {
        if (!node){
            return nullptr;
        }

        else{
            Node *newNode = new Node(node->elt);
            if ((newNode->child = copy(node->child)) != nullptr){
                newNode->child->parent = newNode;
            }
            if ((newNode->sibling = copy(node->sibling)) != nullptr){
                newNode->sibling->parent = newNode;
            }
            return newNode;
        }
    
    }
    

};


#endif // PAIRINGPQ_H
