#include <iostream>
#include <list>
#include <vector>
#include <algorithm>

#include "classify_reorder.hpp"

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
