// fixme: ifdef

#include <algorithm>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Implementation of classify-reorder algorithm

// FIXME: concepts: Rule must be swappable

// Abstracting from the Rule and Packet implementation and relying on
// the user supplying the rule dependency check comparison.



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

template<typename Rule, typename DependencyPred, typename BidirectionalIterator >
BidirectionalIterator cr_insert(BidirectionalIterator first, BidirectionalIterator last, const Rule& r)
{
    // find the first position where r has no inversions with the nodes in this prefix
    // insert r there
    // find all rules succeeding r that should be placed in front of it
    //     move them directly in front of r
}

// fixme: insertion algorithm
// * first, naive reference implementation
// * then, use stable_partition algorithm

// fixme: deletion algorithm: no implementation, but highlight it's datastructure responsibility, and no further handling is needed for delete