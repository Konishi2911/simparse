#include "simparse.hpp"
#include <gtest/gtest.h>

TEST(ParseTests, AnyChar) {
    std::string str = "abc";
    auto it = str.begin();
    char result = simparse::any_char(it);

    EXPECT_EQ(result, 'a');
    EXPECT_EQ(it, str.begin() + 1);

    result = simparse::any_char(it);
    EXPECT_EQ(result, 'b');
    EXPECT_EQ(it, str.begin() + 2);

    result = simparse::any_char(it);
    EXPECT_EQ(result, 'c');
    EXPECT_EQ(it, str.begin() + 3);

    EXPECT_THROW(simparse::any_char(it), std::runtime_error);
}

TEST(ParseTests, Reputation) {
    std::string str = "abc";
    auto it = str.begin();
    auto parser = simparse::rep(2, simparse::any_char);
    std::string result = parser(it);

    EXPECT_EQ(result, "ab");
    EXPECT_EQ(it, str.begin() + 2);
    
    EXPECT_THROW(parser(it), std::runtime_error);
}

TEST(ParseTests, String) {
    std::string str = "abcdef";
    auto it = str.begin();
    auto parser = simparse::string("abc");
    std::string result = parser(it);

    EXPECT_EQ(result, "abc");
    EXPECT_EQ(it, str.begin() + 3);

    EXPECT_THROW(parser(it), std::runtime_error);
}

TEST(ParseTests, OrParse) {
    std::string str = "abcdef";
    auto it = str.begin();
    auto parser = simparse::string("abc") | simparse::string("def");

    std::string result = parser(it);
    EXPECT_EQ(result, "abc");
    EXPECT_EQ(it, str.begin() + 3);

    result = parser(it);
    EXPECT_EQ(result, "def");
    EXPECT_EQ(it, str.end());
    EXPECT_THROW(parser(it), std::runtime_error);
}

TEST(ParseTests, Ignore) {
    std::string str = "   abc   ";
    auto it = str.begin();
    auto parser = simparse::ignore(simparse::many(simparse::whitespace)) + simparse::string("abc") + simparse::ignore(simparse::many(simparse::whitespace));
    std::string result = parser(it);

    EXPECT_EQ(result, "abc");
    EXPECT_EQ(it, str.end());
}

TEST(ParseTests, Many) {
    std::string str = "abcabc";
    auto it = str.begin();
    auto parser = simparse::many(simparse::any_char);
    std::string result = parser(it);

    EXPECT_EQ(result, "abcabc");
    EXPECT_EQ(it, str.end());
}

TEST(ParseTests, Back) {
    std::string str = "abc";
    auto it = str.begin();
    auto parser = simparse::back(simparse::string("acb"));

    EXPECT_THROW(parser(it), std::runtime_error);
    EXPECT_EQ(it, str.begin());
}

TEST(ParseTests, Peek) {
    std::string str = "abc";
    auto it = str.begin();
    auto parser = simparse::peek(simparse::string("ab"));

    std::string result = parser(it);
    EXPECT_EQ(result, "ab");
    EXPECT_EQ(it, str.begin());
}

TEST(ParseTests, Whitespace) {
    std::string str = "   abc   ";
    auto it = str.begin();
    auto parser = simparse::many(simparse::whitespace);

    std::string result = parser(it);
    EXPECT_EQ(result, "   ");
    EXPECT_EQ(it, str.begin() + 3);

    result = parser(it);
    EXPECT_EQ(result, "");
    EXPECT_EQ(it, str.begin() + 3);
}

TEST(ParseTests, ExampleTest) {
    std::string str = "VARIABLES= \"var1\", \"var2\" ,\"var3\" , \"var4\"";
    auto it = str.begin();
    auto label = simparse::back( 
        simparse::string("VARIABLES") 
        + simparse::many(simparse::whitespace)
        + simparse::string("=")
        + simparse::many(simparse::whitespace)
    );
    auto item = simparse::back(
        simparse::ignore(simparse::string("\""))
        + simparse::many(simparse::alphanumeric)
        + simparse::ignore(simparse::string("\""))
        + simparse::ignore(
            simparse::many(simparse::whitespace)
            + simparse::many(simparse::string(","))
            + simparse::many(simparse::whitespace)
        )
    );
    
    std::string lb = label(it);
    EXPECT_EQ(lb, "VARIABLES= ");
    EXPECT_EQ(it, str.begin() + 11);

    std::string item1 = item(it);
    EXPECT_EQ(item1, "var1");

    std::string item2 = item(it);
    EXPECT_EQ(item2, "var2");

    std::string item3 = item(it);
    EXPECT_EQ(item3, "var3");

    std::string item4 = item(it);
    EXPECT_EQ(item4, "var4");

    EXPECT_THROW(item(it), std::runtime_error);
}

TEST(ParseTests, ExampleTest2) {
    auto label_parser =  
        simparse::ignore( 
            simparse::many(simparse::whitespace) 
        ) +
        simparse::many(simparse::alphabet) + 
        simparse::ignore(
            simparse::many(simparse::whitespace) + 
            simparse::string("=") + 
            simparse::many(simparse::whitespace)
        );

    auto single_entry_parser = simparse::back(
        simparse::ignore(simparse::many(simparse::string("\""))) + 
        simparse::many(simparse::alphanumeric | simparse::whitespace) + 
        simparse::ignore(
            simparse::many(simparse::string("\"")) + 
            simparse::many(simparse::string(","))
        )
    );

    auto labels = std::string("I = 1, J = 2, K = 3");
    auto it = labels.begin();

    auto label1 = label_parser(it);
    auto var1 = single_entry_parser(it);
    EXPECT_EQ(label1, "I");
    EXPECT_EQ(var1, "1");

    auto label2 = label_parser(it);
    auto var2 = single_entry_parser(it);
    EXPECT_EQ(label2, "J");
    EXPECT_EQ(var2, "2");

    auto label3 = label_parser(it);
    auto var3 = single_entry_parser(it);
    EXPECT_EQ(label3, "K");
    EXPECT_EQ(var3, "3");

    EXPECT_THROW(label_parser(it), std::runtime_error);
}