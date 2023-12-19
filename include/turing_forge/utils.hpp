#pragma once

#include <ctime>


namespace Turingforge::Utils {

    void timestamp () {
# define TIME_SIZE 40
        static char time_buffer[TIME_SIZE];
        const struct std::tm *tm_ptr;
        std::time_t now;
        now = std::time (nullptr);
        tm_ptr = std::localtime ( &now );
        std::strftime (time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm_ptr);
        std::cout << time_buffer << "\n";
# undef TIME_SIZE
    }

}