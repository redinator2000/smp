#ifndef SMP_HPP
#define SMP_HPP

#include <string>
#include <unordered_map>
#include <vector>

/*
smp file format. Removes the need for escape sequences by only allowing matched {}[]
The code here just converts std::unordered_map<std::string, std::string> or std::vector<std::string> to human readable strings.
Contains no type information, since it is intended to be used with typed languages.
For more complex data structures, or just structs composing structs, make a Sump/Samp for the sub-component, convert it to a string, and use that string as a value for the big component.
*/

namespace smp
{
using Sump = std::unordered_map<std::string, std::string>;
using Samp = std::vector<std::string>; //order is preserved

Sump string_to_sump(std::string_view); //reserves '{' '}' ':'
std::string sump_to_string(const Sump &); //reserves '{' '}' ':'
Samp string_to_samp(std::string_view); //reserves '[' ']'
std::string samp_to_string(const Samp &); //reserves '[' ']'

std::string bracket_indent(std::string_view); //searches for all {}[] to do a nested indentation style, especially useful if the value of a Sump/Samp is another Sump/Samp
std::string bracket_indent_simple_linevalues(std::string_view); //puts values on its own line with no tab, makes it easier for external tools to scrape

std::string sanitize_for_sump(std::string); //if {}[] mismatch, replace {}[] with (). Since matching {}[] are still valid, it is easy to nest Sump/Samp.
                                            //If the string comes from sump_to_string or samp_to_string, sanitization is unnecessary
}

/*
example of a stringified sump put through bracket_indent. {data} is a key for another sub-sump. {punches} is a key for a sub-samp.

"""
{tile_type}:{punchcard}
{position}:{0,0}
{data}:{
    {punches}:{
        [0000000001]
        [0000000010]
        [0000000100]
        [0000001000]
    }
    {channels}:{4}
}
"""

When saving a sump/samp to file, use extension .smp
*/

#endif // SMP_HPP
