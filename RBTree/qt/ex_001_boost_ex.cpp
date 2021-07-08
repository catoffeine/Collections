#include <algorithm>
#include <iostream>
#include <vector>

#include <boost/asio/io_context.hpp>
#include <boost/type_index.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/lambda/lambda.hpp>

using namespace boost::lambda;

int main() {
    /*
    boost::asio::io_context ioc;

    boost::asio::signal_set signals(ioc, SIGTERM, SIGINT);

    signals.async_wait([&](auto, auto){std::cout << "This program is just about to quit" << std::endl; ioc.stop();});

    ioc.run();
    */

    struct MyStruct { int x; const int& y;};

    int a = 0;

    MyStruct ms{a, a};

    std::cout << boost::typeindex::type_id_with_cvr<decltype(ms.y)>().pretty_name() << std::endl;
    std::cout << typeid(ms.y).name() << std::endl;

    std::vector v{5,6,7,8,9};
    for_each(v.begin(), v.end(), std::cout << _1 << '\n');

    return 0;
}
