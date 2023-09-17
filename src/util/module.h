#pragma once

#include <filesystem>

namespace util
{
    class module
    {
        public:
            module() = default;

            explicit module(void* base);
            explicit module(std::string_view name);

            [[nodiscard]] auto start() const -> std::uint8_t*;
            [[nodiscard]] auto end() const -> std::uint8_t*;
            [[nodiscard]] auto size() const -> std::size_t;
            [[nodiscard]] auto path() const -> std::filesystem::path;
            [[nodiscard]] auto scan(std::string_view pattern) const -> std::uint8_t*;

            [[nodiscard]] explicit operator bool() const;
            [[nodiscard]] auto operator*() const -> std::uint8_t*;
        private:
            std::uint8_t* _start = nullptr;
            std::uint8_t* _end = nullptr;
            std::size_t _size = 0;
    };
}