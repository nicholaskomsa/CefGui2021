#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

template<typename... Bases>
struct LambdaOverload : public Bases... {
    using Bases::operator()...;

    LambdaOverload(const Bases&... bases) : Bases(bases)... { }
};

template<typename ...Args>
void outd(Args... args) {


    auto deduce = LambdaOverload(
        [](const char* c) ->std::string { return c; },
        [](const std::string& str)->const std::string& { return str; },
        [](const auto& t) { return std::to_string(t); }
    );

    std::string o = (( deduce(args) + " " ) + ...);
    OutputDebugStringA(o.c_str());
}
