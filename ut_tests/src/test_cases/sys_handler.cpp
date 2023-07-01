#include "utils/utils.hpp"

namespace ut_tests
{
  namespace
  {
    void crash();
    void what_can_go_wrong() noexcept { crash(); }
    void crash() { throw std::runtime_error{ "absolute crash" }; }

    void bad_alloc_kindof() noexcept
    {
      if (auto newHandler = std::get_new_handler())
        newHandler();
    }

    void term_handler() noexcept
    {
      auto exc = std::current_exception();
      if (!exc)
      {
        std::cerr << "not an exception";
        return;
      }

      try
      {
        std::rethrow_exception(exc);
      }
      catch (std::exception& e)
      {
        std::cerr << e.what();
      }
      catch (...)
      {
        std::cerr << "unknown exception";
      }
    }
  }

  TEST(sys_handler, t_simple_term)
  {
    utils::set_terminate(42, [] { std::cerr << "hello"; });
    EXPECT_EXIT(std::terminate(), testing::ExitedWithCode(42), "hello");
  }

  TEST(sys_handler, t_keep_prev)
  {
    std::set_terminate(term_handler);
    utils::set_terminate(42, [] {});
    EXPECT_EXIT(what_can_go_wrong(), testing::ExitedWithCode(42), "absolute crash");
  }

  TEST(sys_handler, t_abnormal_termination)
  {
    utils::set_terminate(42, term_handler);
    EXPECT_EXIT(what_can_go_wrong(), testing::ExitedWithCode(42), "absolute crash");
  }

  TEST(sys_handler, t_alloc_failure)
  {
    utils::set_new([] { std::cerr << "bad alloc"; });
    utils::set_terminate(-100, [] {});
    EXPECT_EXIT(bad_alloc_kindof(), testing::ExitedWithCode(-100), "bad alloc");
  }

}