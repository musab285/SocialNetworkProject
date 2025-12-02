#ifndef SOCIALGRAPH_H
#define SOCIALGRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <stack>
#include <queue>
#include <map>

using namespace std;

class SocialGraph {
private:
    // Core Graph Structure: User Name -> List of Friends
    map<string, vector<string>> adjList;
    
    // Data Structures for specific features
    stack<string> recentUsersStack;
    queue<string> recRequestQueue;

public:
    // 1. Add User
    bool addUser(string name) {
        if (adjList.find(name) != adjList.end()) {
            return false; // User already exists
        }
        
        // Initialize empty friend list
        adjList[name] = vector<string>();
        
        // Push to Stack
        recentUsersStack.push(name);
        return true;
    }

    // 2. Create Friendship
    bool addFriendship(string u1, string u2) {
        if (adjList.find(u1) == adjList.end() || adjList.find(u2) == adjList.end()) {
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
        for (auto const& [user, friends] : adjList) {
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
        if (adjList.find(name) == adjList.end()) return "User not found!";

        // Add to Queue (Tracking requests)
        recRequestQueue.push(name);

        // Logic: Friend of a Friend who is NOT my friend
        unordered_map<string, int> potentialFriends;
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
                    potentialFriends[candidate]++;
                }
            }
        }

        if (potentialFriends.empty()) return "No new recommendations available.";

        string result = "Recommendations for " + name + ":\n";
        for (auto const& [candidate, mutualCount] : potentialFriends) {
            result += "- " + candidate + " (Mutual Friends: " + to_string(mutualCount) + ")\n";
        }
        return result;
    }

    // Helper: Get Recently Added (Stack)
    string getRecentUsers() {
        if (recentUsersStack.empty()) return "No recent users.";
        
        // We copy the stack to read it without destroying the original
        stack<string> temp = recentUsersStack; 
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
        
        queue<string> temp = recRequestQueue;
        string out = "";
        while(!temp.empty()) {
            out += temp.front() + "\n";
            temp.pop();
        }
        return out;
    }
};

#endif