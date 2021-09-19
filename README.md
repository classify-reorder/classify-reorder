# The algorithm classify-reorder for packet classification in networked systems

A simple algorithm for packet classification with boosted performance for skewed packet sequences. Drop-in replacement for linked-list-of-rules that uses no additional memory.


If packets often match the same rules (a realistic assumption, see study Feldmann FIXME), the algorithm serves them faster. The algorithm self-adjusts the order of rules to discover patterns in the packet sequence, even if the patterns rapidly change and are unpredictable.

# Example use case:

You keep your rules in a linked list, and classify packets by finding the first rule that matches the packet. You chose this implementation for simplicity, and your list of rules is small (say, a hundred). How classify-reorder performs? This depends on the structure of packet sequence: with repeated hits to the same rule ("high locality"), the performance improves hundreds times, and for worst-case adversarial input your performance drops 2x ("no locality", unrealistic input in most cases).

Code before: std::list and for loop to find

Code after: classify_reorder with hpp only library no dependencies

# Parametrized design: works with any data structure and any packets and rules

Code with vector and list and deque

Code with various classification criterias
