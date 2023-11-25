#include "gtest/gtest.h"

#include "InputGenerator/RegexGenerator.h"
#include "Objects/FiniteAutomaton.h"
#include "Objects/Regex.h"

using std::string;

TEST(TestCaseName, Test_random_regex_parsing) {
	RegexGenerator rg(15, 10, 5, 3);
	for (int i = 0; i < 30; i++) {
		string str = rg.generate_regex();
		Regex r1(str);
		string r1_str = r1.to_txt();
		Regex r2(r1_str);
		ASSERT_EQ(true, Regex::equivalent(r1, r2));
	}
}

TEST(TestArden, Test_arden_on_random_regex) {
	RegexGenerator rg;
	for (int i = 0; i < 30; i++) {
		string rgx_str = rg.generate_regex();
		Regex r1(rgx_str), r2(rgx_str);
		ASSERT_TRUE(Regex::equivalent(r1, r2.to_ilieyu().to_regex()));
	}
}