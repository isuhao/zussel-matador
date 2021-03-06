//
// Created by sascha on 12/23/16.
//

#include "TestSuiteTestUnit.hpp"
#include "FirstTestUnit.hpp"
#include "SecondTestUnit.hpp"

bool TestSuiteTestUnit::test_create()
{
  matador::test_suite ts;

  const matador::test_suite::summary &smry = ts.test_summary();
  bool result = smry.asserts == 0;
  result &= smry.errors == 0;
  result &= smry.failures == 0;
  result &= smry.succeeded == 0;
  result &= smry.tests == 0;
  return result;
}

bool TestSuiteTestUnit::test_init()
{
  char *argv1[] = {
    (char*)"",
    (char*)"list"
  };

  matador::test_suite ts;

  ts.init(2, argv1);

  const matador::test_suite::test_suite_args &args = ts.test_args();

  bool result = args.cmd == matador::test_suite::LIST;
  result &= args.initialized;
  result &= !args.brief;
  result &= args.unit_args.empty();

  char *argv2[] = {
    (char*)"",
    (char*)"list",
    (char*)"brief"
  };

  ts.init(3, argv2);

  result &= args.cmd == matador::test_suite::LIST;
  result &= args.initialized;
  result &= args.brief;
  result &= args.unit_args.empty();

  char *argv3[] = {
    (char*)"",
    (char*)"exec",
    (char*)"all"
  };

  ts.init(3, argv3);

  result &= args.cmd == matador::test_suite::EXECUTE;
  result &= args.initialized;
  result &= args.unit_args.empty();

  return result;
}

bool TestSuiteTestUnit::test_list()
{
  matador::test_suite ts;
  ts.register_unit(new FirstTestUnit);
  ts.register_unit(new SecondTestUnit);

  bool result = ts.size() == 2;
  result &= !ts.empty();

  char *argv1[] = {
    (char*)"",
    (char*)"list"
  };

  ts.init(2, argv1);
  ts.quiet();

  const matador::test_suite::test_suite_args &args = ts.test_args();

  result &= args.cmd == matador::test_suite::LIST;
  result &= args.initialized;
  result &= !args.brief;
  result &= args.unit_args.empty();

  ts.run();

  char *argv2[] = {
    (char*)"",
    (char*)"list",
    (char*)"brief"
  };

  ts.init(3, argv2);

  result &= args.cmd == matador::test_suite::LIST;
  result &= args.initialized;
  result &= args.brief;
  result &= args.unit_args.empty();

  ts.run();

  return result;
}

bool TestSuiteTestUnit::test_method()
{
  matador::test_suite ts;
  ts.register_unit(new FirstTestUnit);
  ts.register_unit(new SecondTestUnit);

  char *argv[] = {
    (char*)"",
    (char*)"exec",
    (char*)"first:sub1"
  };

  ts.init(3, argv);
  ts.quiet();

  const matador::test_suite::test_suite_args &args = ts.test_args();

  bool result = args.cmd == matador::test_suite::EXECUTE;
  result &= args.initialized;
  result &= args.unit_args.size() == 1;

  auto &unit = args.unit_args.front();

  result &= unit.unit == "first";
  result &= unit.tests.size() == 1;

  ts.run();

  // Todo: check summary

  return result;
}

bool TestSuiteTestUnit::test_unit()
{
  matador::test_suite ts;
  ts.register_unit(new FirstTestUnit);
  ts.register_unit(new SecondTestUnit);

  char *argv[] = {
    (char*)"",
    (char*)"exec",
    (char*)"first"
  };

  ts.init(3, argv);
  ts.quiet();

  const matador::test_suite::test_suite_args &args = ts.test_args();

  bool result = args.cmd == matador::test_suite::EXECUTE;
  result &= args.initialized;
  result &= args.unit_args.size() == 1;

  ts.run();

  // Todo: check summary

  return result;
}

bool TestSuiteTestUnit::test_suite()
{
  matador::test_suite ts;
  ts.register_unit(new FirstTestUnit);
  ts.register_unit(new SecondTestUnit);

  char *argv[] = {
    (char*)"",
    (char*)"exec",
    (char*)"all"
  };

  ts.init(3, argv);
  ts.quiet();

  const matador::test_suite::test_suite_args &args = ts.test_args();

  bool result = args.cmd == matador::test_suite::EXECUTE;
  result &= args.initialized;
  result &= args.unit_args.empty();

  ts.run();

  // Todo: check summary

  return result;
}
