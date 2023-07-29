#pragma once

#include <memory>
#include <stdexcept>

// Stack implemented through linked list
template <typename T, typename Alloc = std::allocator<T>>
class CustomStack
{
private:
    struct Node
    {
        T data;
        Node *prev;
    };

public:
    using Allocator = typename Alloc::template rebind<Node>::other;

    CustomStack();
    ~CustomStack();

    // Check if stack is empty
    bool empty() { return head_ == nullptr; }

    // Push element to stack
    void push(T element);

    // Remove and return top element
    T pop();

    // Return top element
    T top();

    // Remove all elements from stack
    void clear();

private:
    Node *head_;
    Allocator allocator_;
};

template <typename T, typename Alloc>
CustomStack<T, Alloc>::CustomStack() : head_(nullptr) {}

template <typename T, typename Alloc>
CustomStack<T, Alloc>::~CustomStack()
{
    clear();
}

template <typename T, typename Alloc>
void CustomStack<T, Alloc>::push(T element)
{
    Node *newNode = allocator_.allocate(1);
    allocator_.construct(newNode, Node{element, head_});
    head_ = newNode;
}

template <typename T, typename Alloc>
T CustomStack<T, Alloc>::pop()
{
    if (empty())
    {
        throw std::out_of_range("Cannot pop from empty stack");
    }
    Node *top = head_;
    T value = top->data;
    head_ = head_->prev;
    allocator_.destroy(top);
    allocator_.deallocate(top, 1);
    return value;
}

template <typename T, typename Alloc>
T CustomStack<T, Alloc>::top()
{
    if (empty())
    {
        throw std::out_of_range("Cannot access top of empty stack");
    }
    return head_->data;
}

template <typename T, typename Alloc>
void CustomStack<T, Alloc>::clear()
{
    while (!empty())
    {
        pop();
    }
}
