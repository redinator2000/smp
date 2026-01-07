smp file format. Removes the need for escape sequences by only allowing matched {}[]
The code here just converts std::unordered_map<std::string, std::string> or std::vector<std::string> to human readable strings.
Contains no type information, since it is intended to be used with typed languages.
For more complex data structures, or just structs composing structs, make a Sump/Samp for the sub-component, convert it to a string, and use that string as a value for the big component.

example of a stringified sump put through bracket_indent. {data} is a key for another sub-sump. {punches} is a key for a sub-samp.

```
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
```

When saving a sump/samp to file, use extension `.smp`
