// is_order_correct
// * naive implementation O(n^2)
// * is there a linear-time implementation?

// how to manage header-only libraries? how to split them over files?


// fixme: should work for functions too, not only struct DependencyPred

template<typename DependencyPred, typename BidirectionalIterator >
bool is_correct_order(BidirectionalIterator first, BidirectionalIterator last)
{
    for(auto i = first; i != last; ++i)
    {
        auto preceeding_wrong = std::find_if(first, i-1, 
            [&](auto& x) { return DependencyPred()(x,*i); });
        


        if(preceeding_wrong == i) // the last in the sequence, not found
            return false;
 
        auto succeeding_wrong = std::find_if(first, i-1, 
            [&](auto& x) { return DependencyPred()(*i,x); });
        
        if(succeeding_wrong == last) // the last in the sequence, not found
            return false;

    }
    return true;
}