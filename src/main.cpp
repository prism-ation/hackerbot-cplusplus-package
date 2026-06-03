#include "hackerbot/Hackerbot.hpp"

#include <iostream>

int main()
{
    const hackerbot::Hackerbot hackerbot;
    std::cout << hackerbot.name() << '\n';
    return 0;
}