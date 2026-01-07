#include "smp.hpp"

namespace smp
{
Sump string_to_sump(std::string_view str)
{
    enum class Stage
    {
        finding_first,
        writing_first,
        finding_colon,
        finding_second,
        writing_second
    };

    Sump sump;
    std::pair<std::string, std::string> builder;
    int bracket_depth = 0;
    Stage stage = Stage::finding_first;
    bool comment_line = false;
    for(unsigned int i = 0; i < str.length(); i++)
    {
        char c = str[i];
        if(c == '#' && i + 1 < str.length() && str[i + 1] == '#')
            comment_line = true;
        if(comment_line)
        {
            if(c == '\n')
                comment_line = false;
        }
        else
        {
            switch(stage)
            {
            case Stage::finding_first:
                if(c == '{')
                {
                    stage = Stage::writing_first;
                    bracket_depth = 1;
                }
                break;

            case Stage::writing_first:
                if(c == '{')
                    bracket_depth++;
                else if(c == '}')
                    bracket_depth--;
                if(bracket_depth == 0)
                    stage = Stage::finding_colon;
                else
                    builder.first += c;
                break;

            case Stage::finding_colon:
                if(c == ':')
                    stage = Stage::finding_second;
                else if(c == '{')//corrupted
                {
                    stage = Stage::writing_first;
                    builder = std::pair<std::string, std::string>();
                    bracket_depth = 1;
                }
                break;

            case Stage::finding_second:
                if(c == '{')
                {
                    stage = Stage::writing_second;
                    bracket_depth = 1;
                }
                break;

            case Stage::writing_second:
                if(c == '{')
                    bracket_depth++;
                else if(c == '}')
                    bracket_depth--;
                if(bracket_depth == 0)
                {
                    sump.insert(builder);
                    stage = Stage::finding_first;
                    builder = std::pair<std::string, std::string>();
                }
                else
                    builder.second += c;
                break;
            }
        }
    }
    return sump;
}
std::string sump_to_string(const Sump & sump)
{
    std::string out;
    for(const auto & element : sump)
    {
        out += '{';
        out += element.first;
        out += '}';
        out += ':';
        out += '{';
        out += element.second;
        out += '}';
    }
    return out;
}

Samp string_to_samp(std::string_view str)
{
    bool writing_element = false;
    int bracket_depth = 0;
    bool comment_line = false;
    Samp samp = {};
    std::string builder = {};
    for(unsigned int i = 0; i < str.length(); i++)
    {
        char c = str[i];
        if(c == '#' && i + 1 < str.length() && str[i + 1] == '#')
            comment_line = true;
        if(comment_line)
        {
            if(c == '\n')
                comment_line = false;
        }
        else if(writing_element)
        {
            if(c == '[')
                bracket_depth++;
            else if(c == ']')
                bracket_depth--;
            if(bracket_depth == 0)
            {
                samp.push_back(builder);
                builder = "";
                writing_element = false;
            }
            else
                builder += c;
        }
        else
        {
            if(c == '[')
            {
                writing_element = true;
                bracket_depth = 1;
            }
            else if(c == ']')//corrupted
            {
                writing_element = false;
                builder = "";
                bracket_depth = 0;
            }
        }
    }
    return samp;
}
std::string samp_to_string(const Samp & samp)
{
    std::string out;
    for(std::string s : samp)
    {
        out += '[';
        out += s;
        out += ']';
    }
    return out;
}

std::string sump_get_string(const Sump & sump, std::string_view key)
{
    thread_local static std::string static_key;
    static_key.reserve(key.size());
    static_key.assign(key.data(), key.size());

    auto p = sump.find(static_key);
    if(p == sump.end())
        return "";
    else
        return p->second;
}

std::string bracket_indent(std::string_view in)
{
    int tab = 0;
    std::string out;
    out.reserve(in.size() + in.size() / 8);
    for(unsigned int c = 0; c < in.size(); c++)
    {
        if(in[c] == '{' || in[c] == '[')
        {
            if(c > 0 && (in[c - 1] == '{' || in[c - 1] == '['))
            {
                out += '\n';
                tab++;
                if(tab > 0)
                    out += std::string(tab * 4, ' ');
            }
        }
        out += in[c];
        if(in[c] == '}' || in[c] == ']')
        {
            if(c + 1 < in.size() && (in[c + 1] == '}' || in[c + 1] == ']'))
            {
                out += '\n';
                tab--;
                if(tab > 0)
                    out += std::string(tab * 4, ' ');
            }
            else if(c + 1 < in.size() && (in[c + 1] == '{' || in[c + 1] == '['))
            {
                out += '\n';
                if(tab > 0)
                    out += std::string(tab * 4, ' ');
            }
        }
    }
    return out;
}
std::string bracket_indent_simple_linevalues(std::string_view in)
{
    enum class Stage
    {
        outside, key, colon, value
    };
    Stage stage = Stage::outside;
    std::string out;
    out.reserve(in.size() + in.size() / 16);
    for(const auto & c : in)
    {
        if(c == '}')
        {
            if(stage == Stage::key)
                stage = Stage::colon;
            else if(stage == Stage::value)
            {
                stage = Stage::outside;
                out += '\n';
                out += c;
                out += '\n';
                continue;
            }
        }
        out += c;
        if(c == '{')
        {
            if(stage == Stage::outside)
                stage = Stage::key;
            else if(stage == Stage::colon)
            {
                stage = Stage::value;
                out += '\n';
            }
        }
    }
    return out;
}
char char_sanitize_for_sump(char c)
{
    switch(c)
    {
    case '{':
    case '[':
        return '(';
    case '}':
    case ']':
        return ')';
    case ':':
        return ';';
    default:
        return c;
    }
}
bool correct_brackets(std::string_view str)//only for [] {}
{
    int level = 0;
    for(char c : str)
    {
        switch(c)
        {
        case '{':
        case '[':
            level++;
            break;
        case '}':
        case ']':
            level--;
            if(level < 0)
                return false;
            break;
        default:;
        }
    }
    return level == 0;
}
std::string sanitize_for_sump(std::string str)
{
    if(correct_brackets(str))
        return str;
    else
    {
        for(unsigned int i = 0; i < str.size(); i++)
            str[i] = char_sanitize_for_sump(str[i]);
        return str;
    }
}
}
