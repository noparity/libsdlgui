#ifndef DIMENTIONS_HPP
#define DIMENTIONS_HPP

namespace libsdlgui
{
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

} // namespace libsdlgui

#endif // DIMENTIONS_HPP
