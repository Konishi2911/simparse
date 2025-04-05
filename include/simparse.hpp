#include <iostream>
#include <string>
#include <iterator>

namespace simparse {

template<typename T>
concept CharIterator = 
    std::forward_iterator<T> && 
    std::same_as<std::iter_value_t<T>, char>;


template<std::invocable<char> F>
auto satisfy(F&& cond) {
    return [=]<CharIterator I>(I& str_iter) -> char {
        if (*str_iter == '\0') {
            throw std::runtime_error("End of string reached.");
        }
        auto s = *str_iter; 
        if (cond(s)) {
            ++str_iter;
            return s;
        } else {
            throw std::runtime_error("Condition not satisfied.");
        }
    };
}

/// @brief Parses a specified number of characters from the input iterator.
/// @tparam F The type of the parser function.
/// @param n The number of characters to parse.
template<typename F>
auto rep(size_t n, F&& parser) {
    return [=]<CharIterator I>(I& str_iter) {
        std::string result;
        for (size_t i = 0; i < n; ++i) {
            result += parser(str_iter);
        }
        return result;
    };
}

/// @brief Ignores the underlying parser and returns an empty string.
/// @tparam F The type of the parser function.
/// @param parser The parser function to ignore.
/// @return A parser function that ignores the underlying parser.
template<typename F>
auto ignore(F&& parser) {
    return [=]<CharIterator I>(I& str_iter) {
        parser(str_iter);
        return std::string{};
    };
}

/// @brief Parses zero or more characters from the input iterator.
/// @tparam F The type of the parser function.
/// @param parser The parser function to use.
/// @return A parser function that parses zero or more characters.
/// @note This parser will consume characters until the parser fails.
///       It will return the concatenated result of all successful parses.
///       If the parser fails immediately, it will return an empty string.
///       If the parser fails after some successful parses, it will return
///       the concatenated result of those successful parses.
template<typename F>
auto many(F&& parser) {
    return [=]<CharIterator I>(I& str_iter) {
        std::string result;
        while (true) {
            try {
                result += parser(str_iter);
            } catch (std::runtime_error&) {
                break;
            }
        }
        return result;
    };
}

/// @brief Creates a parser that matches any characters except for the given character.
/// @tparam F The type of the parser function.
/// @param c The character to exclude.
/// @return A parser function that matches any character except for the given character.
/// @note This parser will consume characters until it encounters the excluded character.
template<typename F>
auto exclude(char c) {
    return satisfy([=](char s) { return c != s; });
}

/// @brief Creates a parser that matches a specific character.
/// @tparam F The type of the parser function.
/// @param c The character to match.
/// @return A parser function that matches the given character.
/// @note This parser will consume characters until the specified character is matched.
///       If the character is not matched, it will throw an exception.
inline auto character(char c) {
    return satisfy([=](char s) { return c == s; });
}

/// @brief Creates a parser that matches a specific string.
/// @tparam F The type of the parser function.
/// @param str The string to match.
/// @return A parser function that matches the given string.
/// @note This parser will consume characters until the entire string is matched.
///       If the string is not matched, it will throw an exception.
inline auto string(std::string str) {
    return [=]<CharIterator I>(I& str_iter) {
        for (char c : str) {
            if (*str_iter != c) {
                throw std::runtime_error("String not matched: \"" + str + "\"");
            }
            ++str_iter;
        }
        return str;
    };
}

/// @brief Backtraces the parser to the last successful position.
/// @tparam F The type of the parser function.
/// @param parser The parser function to use.
/// @return A parser function that parses with the given parser object.
template<typename F>
auto back(F&& parser) {
    return [=]<CharIterator I>(I& str_iter) {
        auto pos = str_iter;
        try {
            return parser(str_iter);
        } catch (const std::runtime_error&) {
            str_iter = pos;
            throw;
        }
    };
}

/// @brief Peeks at the parser without consuming characters.
/// @tparam F The type of the parser function.
/// @param parser The parser function to use.
/// @return A parser function that peeks at the given parser object.
/// @note This parser will not consume any characters from the input iterator.
///       It will return the result of the parser without modifying the iterator.
///       If the parser fails, it will throw an exception and the iterator will not be modified.
template<typename F>
auto peek(F&& parser) {
    return [=]<CharIterator I>(I& str_iter) {
        auto pos = str_iter;
        try {
            auto str = parser(str_iter);
            str_iter = pos;
            return str;
        } catch (const std::runtime_error&) {
            str_iter = pos;
            throw;
        }
    };
}

/// @brief Concatenates the parsers.
/// @tparam F The type of the first parser function.
/// @tparam G The type of the second parser function.
/// @param f The first parser function.
/// @param g The second parser function.
/// @return A parser function that concatenates the results of the two parsers.
/// @note This parser will return the concatenated result of both parsers.
///       If either parser fails, it will throw an exception.
template<typename F, typename G>
auto operator+(F&& f, G&& g) {
    return [=]<CharIterator I>(I& str_iter) {
        auto result = f(str_iter);
        result += g(str_iter);
        return result;
    };
}

template<typename F, typename G>
auto operator|(F&& f, G&& g) {
    return [=]<CharIterator I>(I& str_iter) {
        try {
            return f(str_iter);
        } catch (const std::runtime_error&) {
            return g(str_iter);
        }
    };
}


/// @brief Parses a single character from the input iterator.
/// @tparam I The type of the input iterator.
/// @param str_iter The input iterator to parse from.
/// @return The parsed character.
inline auto any_char = []<CharIterator I>(I& str_iter) -> char {
    return satisfy([](auto) { return true; })(str_iter);
}; 

/// @brief Parses a single digit character from the input iterator.
/// @tparam I The type of the input iterator.
/// @param str_iter The input iterator to parse from.
/// @return The parsed digit character.
inline auto digit = []<CharIterator I>(I& str_iter) -> char {
    return satisfy([](char c) { return std::isdigit(c); })(str_iter);
};

/// @brief Parses a single alphabet character from the input iterator.
/// @tparam I The type of the input iterator.
/// @param str_iter The input iterator to parse from.
/// @return The parsed alphabet character.
inline auto alphabet = []<CharIterator I>(I& str_iter) -> char {
    return satisfy([](char c) { return std::isalpha(c); })(str_iter);
};

/// @brief Parses a single alphanumeric character from the input iterator.
/// @tparam I The type of the input iterator.
/// @param str_iter The input iterator to parse from.
/// @return The parsed alphanumeric character.
inline auto alphanumeric = []<CharIterator I>(I& str_iter) -> char {
    return satisfy([](char c) { return std::isalnum(c); })(str_iter);
};

/// @brief Parses a single whitespace character from the input iterator.
/// @tparam I The type of the input iterator.
/// @param str_iter The input iterator to parse from.
/// @return The parsed whitespace character.
inline auto whitespace = []<CharIterator I>(I& str_iter) -> char {
    return satisfy([](char c) { return std::isspace(c); })(str_iter);
};

}