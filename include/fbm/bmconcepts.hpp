#pragma once

#include <concepts>

template <typename T>
concept SizeByte = requires(T t) { sizeof(t) == 1; };

template <typename T>
concept IsUnsigned = requires(T t) { std::is_unsigned_v<T> && !std::same_as<T, bool>; };

template <typename T, std::size_t N>
concept IsStdArrayOfAtleastSize = requires(T a) {
    { a.size() } -> std::convertible_to<std::size_t>;
    requires T::size() >= N;
};

template <typename T, std::size_t N>
concept IsStdArrayOfSize = requires(T a) {
    { a.size() } -> std::convertible_to<std::size_t>;
    requires T::size() == N;
};
