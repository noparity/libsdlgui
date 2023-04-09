#ifndef SCROLLEVENTDATA_HPP
#define SCROLLEVENTDATA_HPP

namespace libsdlgui::detail
{
    enum class ScrollDirection
    {
        Decrement,
        Increment
    };

    enum class ScrollMagnitude
    {
        Large,
        Small
    };

    enum class ScrollOrientation
    {
        Horizontal,
        Vertical
    };

    // contains data for a scroll event
    class ScrollEventData
    {
    private:
        uint32_t m_value;
        ScrollDirection m_dir;
        ScrollMagnitude m_mag;
        ScrollOrientation m_or;

    public:
        ScrollEventData(uint32_t newValue, ScrollDirection direction, ScrollMagnitude magnitude, ScrollOrientation orientation) :
            m_value(newValue), m_dir(direction), m_mag(magnitude), m_or(orientation)
        {
            // empty
        }

        ScrollDirection Direction() const { return m_dir; }

        ScrollMagnitude Magnitude() const { return m_mag; }

        uint32_t NewValue() const { return m_value; }

        ScrollOrientation Orientation() const { return m_or; }
    };

} // namespace libsdlgui::detail

#endif SCROLLEVENTDATA_HPP
