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

template<typename Rule, typename Packet, class MatchPred, class DependencyPred, typename List = std::list<Rule> >
class drlist
{
    List rules;
public:
    drlist(const List& _rules) : rules(_rules) {}

    inline constexpr Rule match_simple(const Packet& p)
    {
        // Matching the packet is equivalent to a find operation
        // with a custom predicate.
        auto curr = std::find_if(rules.begin(), rules.end(), MatchPred(p));
        Rule found = *curr;

        // Rearrange with MoveRecursivelyForward procedure (see the paper).
        // Tail recursion implemented as iteration.
        auto pred = curr; // pred is the predecessor of curr
        --pred;

        while(curr != rules.begin())
        {
            // If the nodes are not dependencies,
            // then swap them and continue the procedure for the predecessor
            // node "pred", which after swap contains the moving rule "curr".
            // If the nodes are dependencies, end the Step 1.

            if(!DependencyPred()(*curr, *pred))
                std::swap(*curr, *pred);

            --curr;
            --pred;
        }

       return found;
    }


    inline const Rule& match(const Packet& p)
    {
        // Matching the packet is equivalent to a find operation
        // with a custom predicate.
        auto curr = std::find_if(rules.begin(), rules.end(), MatchPred(p));

        // For debugging purposes.
        // Fast implementations should not have this check.
        if(curr == rules.end())
            throw std::range_error("Rule not found (dynamic list)");


        // Rearrange with MoveRecursivelyForward procedure (see the paper).
        // Tail recursion implemented as iteration.
        // We break the iteration into two steps: moving the found node, and
        // moving successive nodes. This is needed to return the found node.

        // Step1: move the found rule forward until encountering its dependency.
        // At this point, we store the node to return.

        auto pred = curr; // pred is the predecessor of curr
        --pred;

        while(curr != rules.begin())
        {
            // If the nodes are not dependencies,
            // then swap them and continue the procedure for the predecessor
            // node "pred", which after swap contains the moving rule "curr".
            // If the nodes are dependencies, end the Step 1.

            if(!DependencyPred()(*curr, *pred))
                std::swap(*curr, *pred);
            else
                break;

            --curr;
            --pred;
        }

        auto found = curr;

        // Terminate if we reach the beginning of the list.
        if(curr == rules.begin())
            return *found;


        // Step2: recursively move forward: swap forward until encountering
        // a dependency, say a node z. At that point start moving forward z.
        // Repeat until reaching the beginning of the list.


        // Decrement once more after Step1
        // (we broke from the loop without decrementing)


        --curr;
        --pred;

        while(curr != rules.begin())
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

        return *found;
    }

    // For debugging purposes
    void print_current_list() const
    {
        for(auto it = rules.begin(); it != rules.end(); ++it)
            cout << (*it) << ' ';
        cout << endl;
    }
};


int main()
{
    vector<MyRule> original_list;

    for(int i = 1; i < 6; ++i)
    {
        MyRule r;
        r.priority = i;
        original_list.push_back(r);
    }

    drlist<MyRule, MyPacket, MatchPacketPred, IsDependencyPred, vector<MyRule> >
        dynamic_list(original_list);

    dynamic_list.print_current_list();

    // Request the packet with id=5 for 5 times in the row.
    for(int i = 0; i < 5; ++i)
    {
        MyPacket p;
        p.id = 5;
        
        const MyRule& matching = dynamic_list.match_simple(p);
        cout << "Found rule " << matching << endl;

        dynamic_list.print_current_list();
    }

    return 0;
}
