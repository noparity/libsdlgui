#include "stdafx.h"
#include "drawing_routines.hpp"
#include "window.hpp"

void DrawChevron(Window* pWindow, const SDL_Rect& bounds, const SDL_Color& color, bool pointsUp)
{
    const int Size = 2;

    // start in the middle of the bounds
    SDL_Point midPoint;
    midPoint.x = bounds.x + (bounds.w / 2);
    midPoint.y = bounds.y + (bounds.h / 2);
    SDL_Point endPoint = midPoint;

    if (pointsUp)
    {
        endPoint.y += Size;
        midPoint.y -= Size;
    }
    else
    {
        endPoint.y -= Size;
        midPoint.y += Size;
    }

    for (int i = 0; i < 2; ++i)
    {
        if (i == 0)
            endPoint.x -= (Size * 2);
        else
            endPoint.x -= (Size * 4);

        pWindow->DrawLine(midPoint, endPoint, color);

        // move endPoint to the other side
        endPoint.x += (Size * 4);
        pWindow->DrawLine(midPoint, endPoint, color);

        // move down
        ++endPoint.y;
        ++midPoint.y;
    }
}

void DrawX(Window* pWindow, const SDL_Rect& bounds, const SDL_Color& color)
{
    const int Size = 5;

    // start in the middle of the bounds
    SDL_Point endOne;
    endOne.x = bounds.x + (bounds.w / 2);
    endOne.y = bounds.y + (bounds.h / 2);
    SDL_Point endTwo = endOne;

    endOne.x -= Size;
    endOne.y -= Size;
    endTwo.x += Size;
    endTwo.y += Size;

    pWindow->DrawLine(endOne, endTwo, color);
    endOne.x += 1;
    endTwo.x += 1;
    pWindow->DrawLine(endOne, endTwo, color);

    endOne.x += (Size * 2);
    endTwo.x -= (Size * 2);

    pWindow->DrawLine(endOne, endTwo, color);
    endOne.x -= 1;
    endTwo.x -= 1;
    pWindow->DrawLine(endOne, endTwo, color);
}
