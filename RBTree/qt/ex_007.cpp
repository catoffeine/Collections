#include <iostream>
#include <string>
#include <optional>
#include <string_view>
#include <iterator>

void func(std::string_view v) {
    std::cout << v << std::endl;
}

//void func2(const std::string& s) {
//    std::cout << s << std::endl;
//}


int main() {
//    std::optional<std::string_view> optv = std::make_optional("lalalala");
//    std::optional<std::string> opt1, opt2 = std::make_optional(std::string("message"));
//
//    opt1 = optv;

    func("asdfasdf");

    std::string str("0123456789");

    std::cout << std::string_view(str.c_str() + 2, 3) << std::endl;

    for (auto it = str.begin(); it != str.end(); it++) std::cout << *it << std::endl;

//    std::cout <<  opt1.value_or("opt1 is empty") << std::endl;
//    std::cout <<  opt2.value_or("opt2 is empty") << std::endl;
    return 0;
}
