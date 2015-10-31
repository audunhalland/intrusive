
#include <intrusive/list.hpp>

#include <iostream>

class element {
public:
    element() : value(0) {}
    element(int value) : value(value) {}

    int value;

    intrusive::listnode nodeA;
    intrusive::listnode nodeB;
};

template <intrusive::listnode element::*N>
void expect(std::string test_name,
            intrusive::list<element, N> &list,
            size_t size,
            std::string expected)
{
    std::string output;

    if (!list.empty()) {
        typename intrusive::list<element, N>::iterator i = list.begin();
        output += std::to_string(i->value);
        while (++i != list.end()) {
            output += ", " + std::to_string(i->value);
        }
    }

    if (list.size() != size) {
        std::cout << "failed: " << test_name << " size=" << list.size() << " expected=" << size << std::endl;
    } else if (output != expected) {
        std::cout << "failed: " << test_name << " elements=[" << output << "] expected=[" << expected << "]" << std::endl;
    } else {
        std::cout << "success: " << test_name << " elements=[" << output << "]" << std::endl;
    }
}

int main() {
    element one(1);
    element two(2);
    element three(3);

    typedef intrusive::list<element, &element::nodeA> alist;
    typedef intrusive::list<element, &element::nodeB> blist;

    alist listA;
    blist listB;

    listA.push_back(one);
    listA.push_back(two);
    listB.push_back(one);
    listB.push_back(two);

    expect("1 listA", listA, 2, "1, 2");
    expect("2 listB", listB, 2, "1, 2");

    {
        /* create a new list using property nodeA */
        alist listA2;
        element four(4);

        listA.push_back(four);
        listA2.push_back(two); // two disappears from listA
        listA2.push_back(three);

        expect("3 listA", listA, 2, "1, 4");
        expect("4 listB", listB, 2, "1, 2");
        expect("5 listA2", listA2, 2, "2, 3");

        // "four" goes out of scope, disappears from listA
    }

    expect("6 listA", listA, 1, "1");
    expect("7 listB", listB, 2, "1, 2");

    {
        /* insert a range */
        const size_t n = 10;
        element elements[n];

        alist list1;
        alist list2;

        for (size_t i = 0; i < n; ++i) {
            elements[i].value = i;
            list1.push_back(elements[i]);
        }

        expect("8 list1", list1, 10, "0, 1, 2, 3, 4, 5, 6, 7, 8, 9");
        expect("9 list2", list2, 0, "");

        list2.insert(list2.end(), alist::iterator(elements[3]), alist::iterator(elements[8]));

        expect("10 list1", list1, 5, "0, 1, 2, 8, 9");
        expect("11 list2", list2, 5, "3, 4, 5, 6, 7");

        for (int i = 0; i < 2; ++i) {
            list2.insert(list2.begin(), list1.begin(), list1.end());

            expect("12 list1", list1, 0, "");
            expect("13 list2", list2, 10, "0, 1, 2, 8, 9, 3, 4, 5, 6, 7");
        }

        list2.clear();

        expect("14 list2", list1, 0, "");
    }

    return 0;
}
