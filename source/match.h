#if !defined(FUNCDB_MATCH_H)
#define FUNCDB_MATCH_H

#include <variant>

namespace funcdb {

template <typename... Ts>
struct With : Ts... {
  using Ts::operator()...;
};

template <typename... Ts>
With(Ts...) -> With<Ts...>;

template <typename V, typename W>
decltype(auto) Match(V&& variant, W&& with) {
  return std::visit(with, variant);
}

}  // namespace funcdb

#endif  // FUNCDB_MATCH_H
