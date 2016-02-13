#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

// indicates that something failed within the SDL subsystem.
// this exception is likely non-recoverable, e.g. game requires
// accelerated graphics but the user's hardware doesn't support it.
class SDLException : public std::runtime_error
{
public:
    SDLException(const std::string& message) : std::runtime_error(message)
    {
    }
};

#endif // EXCEPTIONS_HPP
