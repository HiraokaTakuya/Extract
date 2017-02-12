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
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cassert>

class Timer {
private:
    std::chrono::time_point<std::chrono::system_clock> t_ = std::chrono::system_clock::now();

    auto elapsed() -> decltype(std::chrono::system_clock::now() - t_) const {
        return std::chrono::system_clock::now() - t_;
    }

public:
    void restart() { *this = Timer(); }
    auto elapsed_msec() -> decltype(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed()).count()) const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed()).count();
    }
    auto elapsed_sec() -> decltype(std::chrono::duration_cast<std::chrono::seconds>(elapsed()).count()) const {
        return std::chrono::duration_cast<std::chrono::seconds>(elapsed()).count();
    }
    auto elapsed_sec_f() -> decltype(std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1>>>(elapsed()).count()) const {
        return std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1>>>(elapsed()).count();
    }
    auto elapsed_msec_f() -> decltype(std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1000>>>(elapsed()).count()) const {
        return std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1000>>>(elapsed()).count();
    }
};

class Extracter {
private:
    std::ifstream ifs_;
    std::ofstream ofs_;
    size_t file_size_;
    size_t origin_;
    size_t output_file_size_;
    char buf_[BUFSIZ];
    Timer timer_;

public:
    Extracter(const int argc, const char* argv[]) : timer_() {
        if (argc != 5) {
            std::cout << "Usage " << argv[0] << " <input file> <output file> <origin> <output file size>"
                      << "\noutput file size: end: extract to the end."
                      << std::endl;
        }

        // input file open
        {
            ifs_.open(argv[1], std::ios::binary);
            if (!ifs_.is_open()) {
                std::cerr << argv[1] << " isn't opened." << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        // output file open
        {
            ofs_.open(argv[2], std::ios::binary);
            if (!ofs_.is_open()) {
                std::cerr << argv[2] << " isn't opened." << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        // file size
        {
            file_size_ = static_cast<size_t>(ifs_.seekg(0, std::ios::end).tellg());
            ifs_.seekg(0, std::ios::beg);     // ストリームのポインタを一番前に戻して、これから先で使いやすいようにする
        }

        // origin
        {
            std::istringstream iss(argv[3]);
            int64_t tmp_origin;
            if (!(iss >> tmp_origin)) {
                std::cerr << argv[3] << " isn't number." << std::endl;
                exit(EXIT_FAILURE);
            }
            else if (tmp_origin < 0) {
                std::cerr << argv[3] << " is minus number." << std::endl;
                exit(EXIT_FAILURE);
            }
            else if (tmp_origin > (int64_t)file_size_) {
                std::cerr << argv[3] << " is larger than file size." << std::endl;
                exit(EXIT_FAILURE);
            }
            origin_ = tmp_origin;
        }

        // output file size
        {
            if (std::string(argv[4]) == "end")
                output_file_size_ = file_size_ - origin_;
            else {
                std::istringstream iss(argv[4]);
                int64_t tmp_output_file_size;
                if (!(iss >> tmp_output_file_size)) {
                    std::cerr << argv[4] << " isn't number."  << std::endl;
                    exit(EXIT_FAILURE);
                }
                else if (tmp_output_file_size < 0) {
                    std::cerr << argv[4] << " is minus number." << std::endl;
                    exit(EXIT_FAILURE);
                }
                else if (tmp_output_file_size > int64_t(file_size_ - origin_)) {
                    std::cerr << argv[4] << " is larger than file size." << std::endl;
                    exit(EXIT_FAILURE);
                }
                output_file_size_ = tmp_output_file_size;
            }
        }
    }
    void process() {
        ifs_.seekg(origin_);
        size_t remaining_file_size = output_file_size_;
        while (true) {
            if (remaining_file_size >= BUFSIZ) {
                ifs_.read(buf_, BUFSIZ);
                ofs_.write(buf_, BUFSIZ);
                remaining_file_size -= BUFSIZ;
            }
            else {
                // 最後のファイル出力。 remaining_file_size == 0 の時もあり得るが問題ない。
                ifs_.read(buf_, remaining_file_size);
                ofs_.write(buf_, remaining_file_size);
                break;
            }
        }
        std::cout << "Elapsed: " << std::fixed << timer_.elapsed_sec_f() << std::endl;
    }
};

int main(const int argc, const char *argv[]) {
    Extracter extracter(argc, argv);
    extracter.process();
}
