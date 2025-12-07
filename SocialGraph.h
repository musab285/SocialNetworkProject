#ifndef SOCIALGRAPH_H
#define SOCIALGRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::find in vectors

using namespace std;

// ---------------------------------------------------------
// 1. Custom Stack Implementation (Wrapper around Vector)
// ---------------------------------------------------------
template <typename T>
class MyStack {
private:
    vector<T> data;
public:
    void push(const T& val) {
        data.push_back(val);
    }
    
    void pop() {
        if (!data.empty()) {
            data.pop_back();
        }
    }
    
    T top() const {
        if (!data.empty()) return data.back();
        return T(); // Default return
    }
    
    bool empty() const {
        return data.empty();
    }

    // Helper to support the specific logic in getRecentUsers (copying logic)
    // The default copy constructor of this class works fine because vector handles it.
};

// ---------------------------------------------------------
// 2. Custom Queue Implementation (Wrapper around Vector)
// ---------------------------------------------------------
template <typename T>
class MyQueue {
private:
    vector<T> data;
public:
    void push(const T& val) {
        data.push_back(val);
    }
    
    void pop() {
        if (!data.empty()) {
            // Vector erase at beginning is O(N), but allows us to use 
            // vector libraries as requested without manual memory management 
            // for a linked list copy constructor.
            data.erase(data.begin());
        }
    }
    
    T front() const {
        if (!data.empty()) return data.front();
        return T();
    }
    
    bool empty() const {
        return data.empty();
    }
};

// ---------------------------------------------------------
// 3. Custom Map Implementation (Binary Search Tree)
// ---------------------------------------------------------
// Replaces std::map and std::unordered_map
template <typename K, typename V>
class MyMap {
private:
    struct Node {
        K key;
        V value;
        Node *left, *right;
        Node(K k, V v) : key(k), value(v), left(nullptr), right(nullptr) {}
    };

    Node* root;

    // Helper: Recursive delete
    void clear(Node* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    // Helper: In-order traversal for iteration
    void inOrder(Node* node, vector<pair<K, V>>& result) const {
        if (!node) return;
        inOrder(node->left, result);
        result.push_back({node->key, node->value});
        inOrder(node->right, result);
    }

    // Helper: Copy tree (needed for assignment if graph is copied, 
    // though not strictly used in the specific logic below, good for safety)
    Node* copyTree(Node* node) {
        if (!node) return nullptr;
        Node* newNode = new Node(node->key, node->value);
        newNode->left = copyTree(node->left);
        newNode->right = copyTree(node->right);
        return newNode;
    }

public:
    MyMap() : root(nullptr) {}
    
    // Copy Constructor
    MyMap(const MyMap& other) {
        root = copyTree(other.root);
    }

    // Destructor
    ~MyMap() {
        clear(root);
    }

    // Check if key exists
    bool contains(const K& key) const {
        Node* curr = root;
        while (curr) {
            if (key == curr->key) return true;
            if (key < curr->key) curr = curr->left;
            else curr = curr->right;
        }
        return false;
    }

    // Operator [] - Returns reference to value. Inserts if not found.
    V& operator[](const K& key) {
        if (!root) {
            root = new Node(key, V()); // V() creates default (0 for int, empty for vector)
            return root->value;
        }

        Node* curr = root;
        while (true) {
            if (key == curr->key) {
                return curr->value;
            } else if (key < curr->key) {
                if (!curr->left) {
                    curr->left = new Node(key, V());
                    return curr->left->value;
                }
                curr = curr->left;
            } else {
                if (!curr->right) {
                    curr->right = new Node(key, V());
                    return curr->right->value;
                }
                curr = curr->right;
            }
        }
    }

    bool empty() const {
        return root == nullptr;
    }

    // Helper to replace iterator logic
    // Returns a vector of key-value pairs sorted by key
    vector<pair<K, V>> getEntries() const {
        vector<pair<K, V>> result;
        inOrder(root, result);
        return result;
    }
};

// ---------------------------------------------------------
// Main Class
// ---------------------------------------------------------

class SocialGraph {
private:
    // Core Graph Structure: User Name -> List of Friends
    // Replaced map with MyMap
    MyMap<string, vector<string>> adjList;
    
    // Data Structures for specific features
    // Replaced stack/queue with MyStack/MyQueue
    MyStack<string> recentUsersStack;
    MyQueue<string> recRequestQueue;

public:
    // 1. Add User
    bool addUser(string name) {
        // Replaced find()!=end() with contains()
        if (adjList.contains(name)) {
            return false; // User already exists
        }
        
        // Initialize empty friend list
        // operator[] will create the entry automatically
        adjList[name] = vector<string>();
        
        // Push to Stack
        recentUsersStack.push(name);
        return true;
    }

    // 2. Create Friendship
    bool addFriendship(string u1, string u2) {
        if (!adjList.contains(u1) || !adjList.contains(u2)) {
            return false; // One or both don't exist
        }
        if (u1 == u2) return false; // Cannot be friends with self

        // Check if already friends
        vector<string>& friends1 = adjList[u1];
        if (find(friends1.begin(), friends1.end(), u2) != friends1.end()) {
            return false; // Already friends
        }

        // Undirected Graph connection
        adjList[u1].push_back(u2);
        adjList[u2].push_back(u1);
        return true;
    }

    // 3. Logic to Get Network String (for GUI Display)
    string getNetworkString() {
        if (adjList.empty()) return "Network is empty.";
        
        string output = "";
        
        // MyMap doesn't support C++11 range-based for loops directly without 
        // writing complex iterators. We use getEntries() to get a vector 
        // and iterate that instead. Logic remains identical.
        auto allEntries = adjList.getEntries();

        for (auto const& entry : allEntries) {
            string user = entry.first;
            vector<string> friends = entry.second;

            output += user + " is connected to: ";
            if (friends.empty()) {
                output += "[No Friends]";
            } else {
                for (const auto& f : friends) {
                    output += f + ", ";
                }
                // Remove last comma
                if(output.length() > 2) output = output.substr(0, output.length()-2);
            }
            output += "\n";
        }
        return output;
    }

    // 4. Recommend Friends
    // Returns a formatted string of recommendations
    string recommendFriends(string name) {
        if (!adjList.contains(name)) return "User not found!";

        // Add to Queue (Tracking requests)
        recRequestQueue.push(name);

        // Logic: Friend of a Friend who is NOT my friend
        // Replaced unordered_map with MyMap
        MyMap<string, int> potentialFriends;
        const vector<string>& myFriends = adjList[name];

        for (const string& friendName : myFriends) {
            const vector<string>& friendsOfFriend = adjList[friendName];
            for (const string& candidate : friendsOfFriend) {
                // Candidate cannot be me
                if (candidate == name) continue;
                
                // Candidate cannot be someone I'm already friends with
                bool alreadyFriend = false;
                for(const string& existing : myFriends) {
                    if(existing == candidate) {
                        alreadyFriend = true;
                        break;
                    }
                }

                if (!alreadyFriend) {
                    // MyMap [] operator initializes int to 0 if not found, then increments
                    potentialFriends[candidate]++;
                }
            }
        }

        if (potentialFriends.empty()) return "No new recommendations available.";

        string result = "Recommendations for " + name + ":\n";
        
        // Convert map to list to iterate
        auto potentialList = potentialFriends.getEntries();

        for (auto const& entry : potentialList) {
            string candidate = entry.first;
            int mutualCount = entry.second;
            result += "- " + candidate + " (Mutual Friends: " + to_string(mutualCount) + ")\n";
        }
        return result;
    }

    // Helper: Get Recently Added (Stack)
    string getRecentUsers() {
        if (recentUsersStack.empty()) return "No recent users.";
        
        // We copy the stack to read it without destroying the original
        // MyStack uses vector, so default copy constructor works perfectly
        MyStack<string> temp = recentUsersStack; 
        string out = "";
        while(!temp.empty()) {
            out += temp.top() + "\n";
            temp.pop();
        }
        return out;
    }

    // Helper: Get Request Queue
    string getRecQueue() {
        if (recRequestQueue.empty()) return "No pending requests.";
        
        // MyQueue uses vector, so default copy constructor works perfectly
        MyQueue<string> temp = recRequestQueue;
        string out = "";
        while(!temp.empty()) {
            out += temp.front() + "\n";
            temp.pop();
        }
        return out;
    }
};

#endif