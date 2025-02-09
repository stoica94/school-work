/* Copyright 2018 <Stoica George-Ovidiu>
  Copy this and a baby duck will cry */
#include <iostream>
#ifndef DEQUE_H_
#define DEQUE_H_

template <typename T>
class Node {
 public:
    T info;
    Node *next, *prev;

    /* List of constructors */
    Node() {
        next = prev = nullptr;
    }
    explicit Node(T info, Node *next = nullptr, Node *prev = nullptr):
        info(info), next(next), prev(prev) {}

    explicit Node(const Node &other) {
        info = other.info;
        next = other.next;
        prev = other.prev;
    }

    Node& operator=(const Node &other) {
        if (this != &other) {
            info = other.info;
            next = other.next;
            prev = other.prev;
        }
        return *this;
    }
};

template <typename T>
class Deque {
    Node<T> *head, *tail;
    unsigned int size;

 public:
    Deque() : size(0) {
        head = tail = nullptr;
    }
    ~Deque() {
        if (head != nullptr) {
            Node<T> *temp = head;
            while (temp != nullptr) {
                head = head->next;
                delete temp;
                temp = head;
            }
        }
    }

    /**
    * Checks if the list is empty
    * @return True if list is empty, False otherwise
    */
    bool isEmpty() {
        return ((head == nullptr) && (tail == nullptr));
    }

    /**
    * Adds an element at the end of the list
    */
    void push_back(T info) {
        Node<T> *aux = new Node<T>(info);
        if (tail == nullptr) {
            tail = head = aux;
            ++size;
            return;
        }
        aux->prev = tail;
        tail->next = aux;
        tail = aux;
        ++size;
    }

    /**
    * Adds an element at the front of the list
    */
    void push_front(T info) {
        Node<T> *aux = new Node<T>(info);
        if (head == nullptr) {
            head = tail = aux;
            ++size;
            return;
        }
        aux->next = head;
        head->prev = aux;
        head = aux;
        ++size;
    }

    /**
    * Removes the last element of the List
    */
    void pop_back() {
        if (tail == nullptr) {
            return;
        }

        Node<T> *temp = tail;
        tail = tail->prev;
        if (tail == nullptr) {
            head = nullptr;
        } else {
            tail->next = nullptr;
        }
        delete temp;
        if (size > 0) {
            --size;
        }
    }

    /**
    * Removes the first element of the list
    */
    void pop_front() {
        if (head == nullptr) {
            return;
        }
        Node<T> *temp = head;
        head = head->next;
        if (head == nullptr) {
            tail = nullptr;
        } else {
            head->prev = nullptr;  //
        }
        delete temp;
        if (size > 0) {
            --size;
        }
    }

    /**
    * Returns the number of elements in the List
    * @return Number of elements in the list
    */
    unsigned int getSize() {
        return size;
    }

    /**
    * Returns the first element of the list
    * @return The first element of the list
    */
    T getFront() {
        return head->info;
    }

    /**
    * Returns the last element of the list
    * @return The last element of the list
    */
    T getBack() {
        return tail->info;
    }

    /**
    * Prints the elements of the list to stdout
    */
    void printDeq() {
        if (head == nullptr) {
            return;
        }
        Node<T> *temp = head;
        while (temp->next != nullptr) {
            std::cout << temp->info << "; ";
            temp = temp->next;
        }
        std::cout << temp->info;
    }
};

#endif  // DEQUE_H_
