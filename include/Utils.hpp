#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <sstream>

namespace sg {
	
static uint64_t getHash( const std::string &name )
{
	static std::hash<std::string> hasher;
	return hasher( name );
}
    
static std::string toFormattedString( const uint32_t input, const uint8_t length )
{
    std::ostringstream ss_out;
    ss_out << std::setw( length ) << std::setfill('0') << input;
    return ss_out.str();
}

}
