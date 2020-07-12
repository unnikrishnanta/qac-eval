/*
Find Unique Characters in a File
Taken from https://stackoverflow.com/a/387720/937153
The code ignores whitespace and it is case insensitive."
*/

#include <iostream>
#include <set>
#include <vector>

int main() {
    std::set<char> seen_chars;
    std::set<char>::const_iterator iter;
    char ch;

    /* ignore whitespace and case */
    while ( std::cin.get(ch) ) {
        if (! isspace(ch) ) {
            seen_chars.insert(tolower(ch));
        }
    }

    std::cout << "Alphabet size = " << seen_chars.size() << "\n";
    for( iter = seen_chars.begin(); iter != seen_chars.end(); ++iter ) {
        std::cout << *iter;
    }
    std::cout<< std::endl;
    return 0;
}
