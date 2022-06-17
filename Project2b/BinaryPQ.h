// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#ifndef BINARYPQ_H
#define BINARYPQ_H

#include <vector>
#include <algorithm>
#include <stdlib.h>
#include "Eecs281PQ.h"

using namespace std;

// A specialized version of the 'heap' ADT implemented as a binary heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) : BaseClass{ comp } {
        // TODO: Implement this function.
        this->compare = comp;
    } // BinaryPQ


    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) : BaseClass{ comp } {
        // TODO: Implement this function.
        this->compare = comp;
        vector<TYPE> binary(start, end);
        for (uint32_t i = 0; i < binary.size(); ++i) {
            push(binary[i]);
        }
    } // BinaryPQ


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automaticslly.
    virtual ~BinaryPQ() {
    } // ~BinaryPQ()


    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        int size = static_cast<int>(data.size());
        for (int i = 0; i < size; ++i) {
            fixUp(i);
        }
    } // updatePriorities()


    // Description: Add a new element to the heap.
    // Runtime: O(log(n))
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE &val) {
        // TODO: Implement this function.
        data.push_back(val);
        fixUp((int) size() - 1);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        swap(data[0], data[data.size()-1]);
        data.pop_back();
        if (size() > 1) {
            fixDown(0);
        }
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function.
        return data[0];
    } // top()


    // Description: Get the number of elements in the heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function.  Might be very simple,
        return static_cast<size_t> (data.size());
    } // size()


    // Description: Return true if the heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return data.empty();
    } // empty()


private:
    // Note: This vector *must* be used for your heap implementation.
    std::vector<TYPE> data;
    // NOTE: You are not allowed to add any member variables.  You don't need
    //       a "heapSize", since you can call your own size() member function,
    //       or check data.size().

    // TODO: Add any additional member functions or data you require here.
    //       For instance, you might add fixUp() and fixDown().
    
//    TYPE &getElement(std::size_t i) {
//        //int num = i - 1;
//        //TYPE val = num;
//        return data[i - 1];
//    }
//    const TYPE &getElement(std::size_t i) const {
//        return data[i - 1];
//    }
    
    void fixUp(int k){
        //messy but it works, keep track of k
        size_t kdivided = static_cast<size_t>(k/2);
        size_t k2 = static_cast<size_t>(k);
        while(k > 0 && this->compare(data[kdivided], data[k2])){
            swap(data[k2], data[kdivided]);
            k2 /= 2;
            k /= 2;
            kdivided = k2/2;
        }
    }
    
    void fixDown(int i){
        //messy but it works, keep track of i and size_t of i and j
        int size = static_cast<int>(data.size());
        while(2 * i < size){
            int j = i * 2;
            size_t j2 = static_cast<size_t>(j);
            size_t jplus = static_cast<size_t>(j + 1);
            size_t i2 = static_cast<size_t>(i);
            
            if((j < size - 1) && this->compare(data[j2], data[jplus])){
                ++j;
                ++j2; 
            }
            
            if(!this->compare(data[i2],data[j2])){
                break;
            }
            swap(data[i2],data[j2]);
            i = j;
            i2 = j2;
        }
    }
}; // BinaryPQ


#endif // BINARYPQ_H
