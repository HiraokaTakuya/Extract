/*
  Extract, a simle file extracter.
  Copyright (C) 2017 Hiraoka Takuya

  Extract is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Extract is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cassert>

class Timer {
private:
    std::chrono::time_point<std::chrono::system_clock> t_ = std::chrono::system_clock::now();

    auto elapsed() const -> decltype(std::chrono::system_clock::now() - t_) {
        return std::chrono::system_clock::now() - t_;
    }

public:
    void restart() { *this = Timer(); }
    int64_t elapsed_msec  () const { return std::chrono::duration_cast<std::chrono::milliseconds                        >(elapsed()).count(); }
    int64_t elapsed_sec   () const { return std::chrono::duration_cast<std::chrono::seconds                             >(elapsed()).count(); }
    float   elapsed_sec_f () const { return std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1   >>>(elapsed()).count(); }
    float   elapsed_msec_f() const { return std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1000>>>(elapsed()).count(); }
};

class Extracter {
private:
    std::ifstream ifs_;
    std::ofstream ofs_;
    int64_t origin_;
    int64_t output_file_size_;
    Timer timer_;

public:
    Extracter(const int argc, const char* argv[]) : timer_() {
        if (argc != 5) {
            std::cerr << "Usage " << argv[0] << " <input file> <output file> <origin> <output file size>"
                      << "\noutput file size: end: extract to the end."
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        // input file open
        ifs_.open(argv[1], std::ios::binary);
        if (!ifs_.is_open()) {
            std::cerr << argv[1] << " isn't opened." << std::endl;
            exit(EXIT_FAILURE);
        }

        // output file open
        ofs_.open(argv[2], std::ios::binary);
        if (!ofs_.is_open()) {
            std::cerr << argv[2] << " isn't opened." << std::endl;
            exit(EXIT_FAILURE);
        }

        // file size
        const int64_t file_size = ifs_.seekg(0, std::ios::end).tellg();
        ifs_.seekg(0, std::ios::beg);

        // origin
        std::istringstream iss0(argv[3]);
        if (!(iss0 >> origin_)) {
            std::cerr << argv[3] << " isn't number." << std::endl;
            exit(EXIT_FAILURE);
        }
        else if (origin_ < 0) {
            std::cerr << argv[3] << " is minus number." << std::endl;
            exit(EXIT_FAILURE);
        }
        else if (origin_ > file_size) {
            std::cerr << argv[3] << " is larger than file size." << std::endl;
            exit(EXIT_FAILURE);
        }

        // output file size
        if (std::string(argv[4]) == "end")
            output_file_size_ = file_size - origin_;
        else {
            std::istringstream iss1(argv[4]);
            if (!(iss1 >> output_file_size_)) {
                std::cerr << argv[4] << " isn't number."  << std::endl;
                exit(EXIT_FAILURE);
            }
            else if (output_file_size_ < 0) {
                std::cerr << argv[4] << " is minus number." << std::endl;
                exit(EXIT_FAILURE);
            }
            else if (output_file_size_ > file_size - origin_) {
                std::cerr << argv[4] << " is larger than file size." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    void process() {
        ifs_.seekg(origin_);
        std::istreambuf_iterator<char> isb(ifs_);
        std::ostreambuf_iterator<char> osb(ofs_);
        std::copy_n(isb, output_file_size_, osb);
        std::cout << "Elapsed: " << std::fixed << timer_.elapsed_sec_f() << " [sec]" << std::endl;
    }
};

int main(const int argc, const char *argv[]) {
    Extracter extracter(argc, argv);
    extracter.process();
}
