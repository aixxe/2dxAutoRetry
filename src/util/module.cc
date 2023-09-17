#include <ranges>
#include <windows.h>
#include "module.h"

using namespace util;

module::module(void* base)
{
    _start = reinterpret_cast<std::uint8_t*>(base);

    auto const dos = PIMAGE_DOS_HEADER(_start);
    auto const nt = PIMAGE_NT_HEADERS(_start + dos->e_lfanew);

    _size = nt->OptionalHeader.SizeOfImage;
    _end = (_start + _size);
}

module::module(std::string_view name)
{
    auto const handle = GetModuleHandleA(name.data());

    if (handle == nullptr)
        return;

    *this = module { handle };
}

auto module::start() const -> std::uint8_t*
    { return _start; }

auto module::end() const -> std::uint8_t*
    { return _end; }

auto module::size() const -> std::size_t
    { return _size; }

auto module::path() const -> std::filesystem::path
{
    auto path = std::string(MAX_PATH, '\0');

    if (GetModuleFileNameA(HMODULE(_start), path.data(), MAX_PATH) == 0)
        return std::filesystem::current_path();

    return { path };
}

auto module::scan(std::string_view bytes) const -> std::uint8_t*
{
    using pattern_byte = std::pair<std::uint8_t, bool>;

    auto pattern = std::views::split(bytes, ' ') |
                   std::views::transform([] (auto&& range) -> pattern_byte
    {
        auto const chunk = std::string_view(range);
        auto byte = std::uint8_t {};

        if (chunk == "?" || chunk == "??")
            return { 0, true };

        std::from_chars(chunk.data(), chunk.data() + chunk.size(), byte, 16);

        return { byte, false };
    });

    auto region = std::span<std::uint8_t> { _start, _size };

    auto const result = std::ranges::search(region, pattern, [] (auto&& a, auto&& b)
        { return b.second || a == b.first; });

    if (result.empty())
        return nullptr;

    return result.data();
}

module::operator bool() const
    { return _start != nullptr; }

auto module::operator*() const -> std::uint8_t*
    { return _start; }