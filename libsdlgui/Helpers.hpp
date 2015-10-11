#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <boost/lexical_cast.hpp>

template<typename T>
std::string ToString(const T& in)
{
	return boost::lexical_cast<std::string>(in);
}

std::string SafeGetEnv(const std::string& name);

#endif // HELPERS_HPP
