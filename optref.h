#include <optional>

namespace Oggy
{
  /**@brief Optionnal reference implementation taken from https://vector-of-bool.github.io/2018/02/27/opt-ref.html */
  template <typename T>
  class opt_ref {
      T* _ptr = nullptr;

  public:
      opt_ref() noexcept = default;
      opt_ref(T& reference) noexcept
          : _ptr(std::addressof(reference)) {}
      opt_ref(std::nullopt_t) noexcept
          : _ptr(nullptr) {}

      explicit operator bool() const noexcept { return _ptr != nullptr; }

      T& operator*() noexcept {
          assert(_ptr != nullptr && "Dereferencing null opt_ref");
          return *_ptr;
      }

      T* operator->() noexcept {
          assert(_ptr != nullptr && "Dereferencing null opt_ref");
          return _ptr;
      }
  };
}//Oggy
