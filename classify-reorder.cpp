#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Defining packets, rules, dependency relation and
// packet-to-rule match predicate.
struct MyPacket
{
    int ip_src;
    int ip_dst;
    int id;
};

struct MyRule
{
    int ip_src_min;
    int ip_src_max;

    int ip_dst_min;
    int ip_dst_max;

    int priority;
    int action;
};

// Printing MyRule
ostream& operator<<(ostream& os, const MyRule& r)
{
    os << r.priority;
    return os;
}


struct IsDependencyPred
{
    bool operator()(const MyRule& r1, const MyRule& r2) const
    {
        // dependencies: (2,3), (3,4), (4,5)
        // oversimplified
        if (r1.priority == 5 && r2.priority == 4) return true;
        if (r1.priority == 3 && r2.priority == 2) return true;
        if (r1.priority == 4 && r2.priority == 3) return true;
        return false;
    }
};

struct MatchPacketPred
{
    MyPacket p;
    MatchPacketPred(const MyPacket& _p) : p(_p) {}
    bool operator()(const MyRule& r) const
    {
        // oversimplified
        return p.id == r.priority;
    }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Implementation of drlist

// Abstracting from the Rule and Packet implementation and relying on
// the user supplying the rule dependency check comparison.


// FIXME: concepts: Rule must be swappable


// Classify the given packet (return the matching rule) and then reorder the rules to speed up future classification
template<typename Rule, typename Packet, typename MatchPred, typename DependencyPred, typename BidirectionalIterator >
BidirectionalIterator classify_reorder(BidirectionalIterator first, BidirectionalIterator last, const Packet& p)
{
    // Matching the packet is equivalent to a find operation
    // with a custom predicate.
    auto curr = std::find_if(first, last, MatchPred(p));

    // For debugging purposes.
    // Fast implementations should not have this check.
    if(curr == last)
        throw std::range_error("Rule not found (dynamic list)");


    // Rearrange with MoveRecursivelyForward procedure (see the paper).
    // Tail recursion implemented as iteration.
    // We break the iteration into two steps: moving the found node, and
    // moving successive nodes. This is needed to return the found node.

    // Step1: move the found rule forward until encountering its dependency.
    // At this point, we store the node to return.

    auto pred = curr; // pred is the predecessor of curr
    --pred;

    while(curr != first)
    {
        // If the nodes are not dependencies,
        // then swap them and continue the procedure for the predecessor
        // node "pred", which after swap contains the moving rule "curr".
        // If the nodes are dependencies, end the Step 1.

        if(!DependencyPred()(*curr, *pred))
            std::swap(*curr, *pred);
        else
        {
            break;
        }

        --curr;
        --pred;
    }

    auto found = curr;

    // Terminate if we reach the beginning of the list.
    if(curr == first)
        return found;


    // Step2: recursively move forward: swap forward until encountering
    // a dependency, say a node z. At that point start moving forward z.
    // Repeat until reaching the beginning of the list.

    // Move one step closer to the head of the list to start moving
    // the encountered dependency node.
    // Recall that in Step1, we broke from the loop without decrementing.
    --curr;
    --pred;

    while(curr != first)
    {
        // If the nodes are not dependencies,
        // then swap them and continue the procedure for the predecessor
        // node "pred", which after swap contains the moving rule "curr".
        // If the nodes are dependencies, continue the procedure without
        // the swap: it has the effect that "curr" becomes the "pred"
        // after both iterators decrement.

        if(!DependencyPred()(*curr, *pred))
            std::swap(*curr, *pred);

        --curr;
        --pred;
    }

    return found;
}



int main()
{
    vector<MyRule> original_list;

    for(int i = 1; i < 6; ++i)
    {
        MyRule r;
        r.priority = i;
        original_list.push_back(r);
    }


    // Request the packet with id=5 for 5 times in the row.
    for(int i = 0; i < 5; ++i)
    {
        // FIXME: print the list to see changes

        MyPacket p;
        p.id = 5;
        
        auto matching = classify_reorder<MyRule, MyPacket, MatchPacketPred, IsDependencyPred>(original_list.begin(), original_list.end(), p);

        cout << "Found rule " << (*matching) << endl;
    }

    return 0;
}
