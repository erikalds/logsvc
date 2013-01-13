#define BOOST_TEST_MODULE "dummy empty test module"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(emptyTest)

BOOST_AUTO_TEST_CASE(empty)
{
  BOOST_WARN("empty test");
}

BOOST_AUTO_TEST_SUITE_END()
