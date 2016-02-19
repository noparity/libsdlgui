#ifndef DIMENTIONS_HPP
#define DIMENTIONS_HPP

struct Dimentions
{
    int W;
    int H;
    Dimentions(int w, int h) : W(w), H(h) {}
};

inline bool operator==(const Dimentions& lhs, const Dimentions& rhs)
{
    return lhs.W == rhs.W && lhs.H == rhs.H;
}

inline bool operator!=(const Dimentions& lhs, const Dimentions& rhs)
{
    return !(lhs == rhs);
}

#endif // DIMENTIONS_HPP
