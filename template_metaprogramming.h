/*
    1/ Don't deduce - T myFunc(const std::vector<T>& myVec,  dont_deduce<T> value1, dont_deduce<T> value2)
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 1/ Don't deduce (https://artificial-mind.net/blog/2020/09/26/dont-deduce)
// Template functions pose problems for different parameters when template deduction comes into play, which we can fix
// by forcing any of the parameters as deducible type

template <class T>
struct dont_deduce_t 
{
    using type = T;
};

template <class T>
using dont_deduce = typename dont_deduce_t<T>::type;

// USE CASES

template<typename T>
//T myFunc(const std::vector<T>& myVec, T value1, T value2)
// with dont_deduce the T is deduced only from the first parameter, forcing the other T references to that type
T myFunc(const std::vector<T>& myVec,  dont_deduce<T> value1, dont_deduce<T> value2)
{
}
