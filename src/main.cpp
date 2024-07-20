#include <unistd.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <string>
#include <CLI/CLI.hpp>
using std::string;
#include "runner.hpp"

extern const char *LICENSE;

void limit_virtual_memory (float gigabytes) {
    size_t constexpr GIGA = 1024 * 1024 * 1024;
    struct rlimit rl;
    rl.rlim_cur = rl.rlim_max = size_t(std::round(gigabytes * GIGA));
    if (setrlimit(RLIMIT_AS, &rl) != 0) {
        std::cerr << "failed setrlimit" << std::endl;
        exit(1);
    }
    gigabytes = rl.rlim_cur;
    gigabytes /= GIGA;
    std::cout << "Set memory limit to " << gigabytes << "GB." << std::endl;
}

void report_resource_usage () {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    double utime = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
    double stime = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
    double max_rss = double(usage.ru_maxrss) / (1024*1024) ;
    std::cout << "USR_TIME/s:\t" << utime << std::endl;
    std::cout << "SYS_TIME/s:\t" << stime << std::endl;
    std::cout << "MAX_RSS/GB:\t" << max_rss << std::endl;
}

int main(int argc, char**argv) {
    std::string input_path;
    std::string output_path;
    float gigabytes = 16;
    int maxdepth = -1;
    bool eval = false;
    bool full = false;
    {
        bool license = false;
        CLI::App app{"Icecuber's Solution to ARC 2020 Challenge\n"
                     "Code: https://github.com/aaalgo/ARC-solution\n"
                     "Original: https://github.com/top-quarks/ARC-solution\n"
                     "Run with --license to show license\n"
                     "Original submission produced by combining depths: 3, 23, 33, 4\n"
        };

        argv = app.ensure_utf8(argv);

        app.add_option("-i,--input", input_path, "");
        app.add_option("-o,--output", output_path, "");
        app.add_option("-d,--depth", maxdepth, "")->capture_default_str();
        app.add_option("-g,--gigabytes", gigabytes)->capture_default_str();
        app.add_flag("--license", license, "");
        app.add_flag("--eval", eval, "run with eval==1, see original README.");
        app.add_flag("--full", full, "output all answers with scores");
        CLI11_PARSE(app, argc, argv);
        if (license) {
            std::cout << LICENSE << std::endl;
            return 0;
        }
        if (input_path.empty()) {
            std::cerr << "Please provide --input; run with -h to show usage." << std::endl;
            return 1;
        }
    }
{
    bool is_name = false;
    // determine if it is name 
    string name_or_path = input_path;
    if (name_or_path.size() == 8) {
        size_t pos = 0;
        try {
            long long ll = stoll(name_or_path, &pos, 16);
            if (pos == name_or_path.size()) {
                is_name = true;
            }
        }
        catch (...) {
        }
    }
    if (is_name) {
        input_path = "data/problems/" + name_or_path + ".json";
    }
}
  limit_virtual_memory(gigabytes);

  run(input_path, output_path, maxdepth, eval, full);
  report_resource_usage();
}


const char *LICENSE = R"V0G0N(
MIT License

Copyright (c) 2020 top-quarks

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
)V0G0N";
