#include <catch2/catch.hpp>
#include "csv_routes.hpp"
#include "graph/graph.hpp"
#include "graph/views/depth_first_search.hpp"
#include "graph/container/dynamic_graph.hpp"

#define TEST_OPTION_OUTPUT (1)
#define TEST_OPTION_GEN (2)
#define TEST_OPTION_TEST (3)
#define TEST_OPTION TEST_OPTION_TEST

using std::cout;
using std::endl;

using std::ranges::forward_range;
using std::remove_reference_t;
using std::is_const_v;
using std::is_lvalue_reference_v;
using std::forward_iterator;
using std::input_iterator;

using std::graph::vertex_t;
using std::graph::vertex_id_t;
using std::graph::vertex_value_t;
using std::graph::vertex_edge_range_t;
using std::graph::vertex_reference_t;
using std::graph::edge_t;
using std::graph::edge_value_t;
using std::graph::edge_reference_t;

using std::graph::graph_value;
using std::graph::vertices;
using std::graph::edges;
using std::graph::vertex_id;
using std::graph::vertex_value;
using std::graph::target_id;
using std::graph::target;
using std::graph::edge_value;
using std::graph::degree;
using std::graph::find_vertex;
using std::graph::find_vertex_edge;

using std::graph::views::dfs_vertex_range;
using std::graph::views::dfs_edge_range;

using routes_vol_graph_traits = std::graph::container::vol_graph_traits<double, std::string, std::string>;
using routes_vol_graph_type   = std::graph::container::dynamic_adjacency_graph<routes_vol_graph_traits>;

template <typename G>
constexpr auto find_frankfurt_id(const G& g) {
  return find_city_id(g, "Frankf\xC3\xBCrt");
}

template <typename G>
auto find_frankfurt(G&& g) {
  return find_city(g, "Frankf\xC3\xBCrt");
}


//template <typename _Iter>
//concept io_iterator = requires(_Iter __i) {
//  { *__i } -> std::__detail::__can_reference;
//}
//&&std::weakly_incrementable<_Iter>;

TEST_CASE("dfs vertex test", "[dynamic][dfs][vertex]") {
  init_console();
  using G  = routes_vol_graph_type;
  auto&& g = load_ordered_graph<G>(TEST_DATA_ROOT_DIR "germany_routes.csv", name_order_policy::source_order_found);

  auto frankfurt    = find_frankfurt(g);
  auto frankfurt_id = find_frankfurt_id(g);

  SECTION("bfs_vertex_range is an input view") {
    dfs_vertex_range<G, void> dfs(g, frankfurt_id);
    auto                      it1 = dfs.begin();
    using I                       = decltype(it1);

    auto it2 = it1;            // copyable
    I    it3(it1);             // copy-constuctible
    auto it4 = std::move(it2); // movable
    I    it5(std::move(it3));  // move-constuctible
    I    it6;                  // default-constructible

    using I2 = std::remove_cvref_t<I>;
    static_assert(std::move_constructible<I2>);
    static_assert(std::copyable<I2>);
    static_assert(std::movable<I2>);
    static_assert(std::swappable<I2>);
    static_assert(std::is_default_constructible_v<I2>);

    //static_assert(std::input_iterator<I2>);
    static_assert(std::input_or_output_iterator<I2>);
    static_assert(std::indirectly_readable<I2>);
    static_assert(std::input_iterator<I2>);

    using Rng = decltype(dfs);
    static_assert(std::is_constructible_v<Rng>);
    static_assert(std::ranges::range<Rng>);
    static_assert(std::movable<Rng>);
    static_assert(std::derived_from<Rng, std::ranges::view_base>);
    static_assert(std::ranges::enable_view<Rng>);
    static_assert(std::ranges::view<decltype(dfs)>);

    auto it8  = std::ranges::begin(dfs);
    auto it9  = std::ranges::end(dfs);
    auto n    = std::ranges::size(dfs);
    auto empt = std::ranges::empty(dfs);
  }

#if TEST_OPTION == TEST_OPTION_OUTPUT
  SECTION("bfs_vertex_range output") {
    dfs_vertex_range dfs(g, frankfurt_id);
    dfs_vertex_range dfs(g, frankfurt_id);

    int cnt = 0;
    cout << '[' << frankfurt_id << "] " << vertex_value(g, **frankfurt) << " (seed)" << endl;
    for (auto&& [uid, u] : dfs) {
      ostream_indenter indent(static_cast<int>(dfs.depth()));
      cout << indent << '[' << uid << "] " << vertex_value(g, u) << endl;
      ++cnt;
    }
    REQUIRE(cnt == 9);
    /* Output:
        [0] Frankfürt (seed)
          [1] Mannheim
            [2] Karlsruhe
              [3] Augsburg
                [8] München
          [4] Würzburg
            [5] Nürnberg
              [9] Stuttgart
            [7] Erfurt
          [6] Kassel
    */
  }
#elif TEST_OPTION == TEST_OPTION_GEN
  SECTION("bfs_vertex_range generate content test") {
    using namespace std::graph;
    using std::cout;
    using std::endl;
    ostream_indenter indent;

    cout << endl
         << indent << "dfs_vertex_range dfs(g, frankfurt_id);" << endl
         << indent << "auto             city     = dfs.begin();" << endl
         << indent << "int              city_cnt = 0;" << endl;

    int city_cnt = 0;
    for (auto&& [uid, u] : dfs_vertex_range(g, frankfurt_id)) {
      ++city_cnt;
      cout << endl << indent << "if(city != dfs.end()) {\n";
      ++indent;
      cout << indent << "auto&& [uid, u] = *city;\n"
           << indent << "REQUIRE(" << uid << " == uid);\n"
           << indent << "REQUIRE(\"" << vertex_value(g, u) << "\" == vertex_value(g, u));\n"
           << indent << "++city_cnt;\n"
           << indent << "++city;\n";
      --indent;
      cout << indent << "}\n";
    }
    cout << endl << indent << "REQUIRE(" << city_cnt << " == city_cnt);" << endl << endl;
  }
#elif TEST_OPTION == TEST_OPTION_TEST
  SECTION("bfs_vertex_range test content") {

    dfs_vertex_range<G, void> dfs(g, frankfurt_id);
    auto                      city     = dfs.begin();
    int                       city_cnt = 0;

    if (city != dfs.end()) {
      auto&& [uid, u] = *city;
      REQUIRE(1 == uid);
      REQUIRE("Mannheim" == vertex_value(g, u));
      ++city_cnt;
      ++city;
    }

    if (city != dfs.end()) {
      auto&& [uid, u] = *city;
      REQUIRE(2 == uid);
      REQUIRE("Karlsruhe" == vertex_value(g, u));
      ++city_cnt;
      ++city;
    }

    if (city != dfs.end()) {
      auto&& [uid, u] = *city;
      REQUIRE(3 == uid);
      REQUIRE("Augsburg" == vertex_value(g, u));
      ++city_cnt;
      ++city;
    }

    if (city != dfs.end()) {
      auto&& [uid, u] = *city;
      REQUIRE(8 == uid);
      REQUIRE("München" == vertex_value(g, u));
      ++city_cnt;
      ++city;
    }

    if (city != dfs.end()) {
      auto&& [uid, u] = *city;
      REQUIRE(4 == uid);
      REQUIRE("Würzburg" == vertex_value(g, u));
      ++city_cnt;
      ++city;
    }

    if (city != dfs.end()) {
      auto&& [uid, u] = *city;
      REQUIRE(5 == uid);
      REQUIRE("Nürnberg" == vertex_value(g, u));
      ++city_cnt;
      ++city;
    }

    if (city != dfs.end()) {
      auto&& [uid, u] = *city;
      REQUIRE(9 == uid);
      REQUIRE("Stuttgart" == vertex_value(g, u));
      ++city_cnt;
      ++city;
    }

    if (city != dfs.end()) {
      auto&& [uid, u] = *city;
      REQUIRE(7 == uid);
      REQUIRE("Erfurt" == vertex_value(g, u));
      ++city_cnt;
      ++city;
    }

    if (city != dfs.end()) {
      auto&& [uid, u] = *city;
      REQUIRE(6 == uid);
      REQUIRE("Kassel" == vertex_value(g, u));
      ++city_cnt;
      ++city;
    }

    REQUIRE(9 == city_cnt);
  }
#endif

  SECTION("bfs_vertex_range with vertex value function") {
    int                                city_cnt = 0;
    auto                               vvf      = [&g](vertex_reference_t<G> u) { return vertex_value(g, u); };
    dfs_vertex_range<G, decltype(vvf)> dfs(g, frankfurt_id, vvf);
    //cout << '[' << frankfurt_id << "] " << vertex_value(g, **frankfurt) << " (seed)" << endl;
    for (auto&& [uid, u, city_name] : dfs) {
      //ostream_indenter indent(size(dfs));
      //cout << indent << "[" << uid << "] " << city_name << endl;
      ++city_cnt;
    }
    REQUIRE(9 == city_cnt);
  }

  SECTION("bfs_vertex_range can do cancel_all") {
    int                                city_cnt = 0;
    dfs_vertex_range<G> dfs(g, frankfurt_id);
    for (auto&& [uid, u] : dfs) {
      ++city_cnt;
      if (uid == 2) // Karlsruhe
        dfs.cancel(std::graph::views::cancel_search::cancel_all);
    }
    REQUIRE(2 == city_cnt);
  }
  SECTION("bfs_vertex_range can do cancel_branch #1") {
    //cout << '[' << frankfurt_id << "] " << vertex_value(g, **frankfurt) << " (seed)" << endl;
    int                 city_cnt = 0;
    dfs_vertex_range<G> dfs(g, frankfurt_id);
    for (auto&& [uid, u] : dfs) {
      ostream_indenter indent(size(dfs));
      //cout << indent << "[" << uid << "] " << vertex_value(g,u) << endl;
      ++city_cnt;
      if (uid == 4) // Wurzburg
        dfs.cancel(std::graph::views::cancel_search::cancel_branch);
    }
    REQUIRE(6 == city_cnt);
  }
}

TEST_CASE("dfs edge test", "[dynamic][dfs][edge]") {
  init_console();
  using G  = routes_vol_graph_type;
  auto&& g = load_ordered_graph<G>(TEST_DATA_ROOT_DIR "germany_routes.csv", name_order_policy::source_order_found);

  auto frankfurt    = find_frankfurt(g);
  auto frankfurt_id = find_frankfurt_id(g);

  SECTION("bfs_edge_range is an input view") {
    dfs_edge_range<G, void> dfs(g, frankfurt_id);
    auto                    it1 = dfs.begin();
    using I                     = decltype(it1);

    auto it2 = it1;            // copyable
    I    it3(it1);             // copy-constuctible
    auto it4 = std::move(it2); // movable
    I    it5(std::move(it3));  // move-constuctible
    //I    it6;                  // default-constructible

    using I2 = std::remove_cvref_t<I>;
    static_assert(std::move_constructible<I2>);
    static_assert(std::movable<I2>);
    static_assert(std::copyable<I2>);
    static_assert(std::swappable<I2>);
    static_assert(std::is_default_constructible_v<I2>);

    static_assert(std::input_or_output_iterator<I2>);
    static_assert(std::indirectly_readable<I2>);
    static_assert(std::input_iterator<I2>);

    using Rng = decltype(dfs);
    static_assert(std::ranges::range<Rng>);
    static_assert(std::movable<Rng>);
    static_assert(std::derived_from<Rng, std::ranges::view_base>);
    static_assert(std::ranges::enable_view<Rng>);
    static_assert(std::ranges::view<decltype(dfs)>);

    auto it8  = std::ranges::begin(dfs);
    auto it9  = std::ranges::end(dfs);
    auto n    = std::ranges::size(dfs);
    auto empt = std::ranges::empty(dfs);
  }

#if TEST_OPTION == TEST_OPTION_OUTPUT
  SECTION("dfs_edge_range output") {
    dfs_edge_range dfs(g, frankfurt_id);
    int            cnt = 0;
    cout << "\n[" << frankfurt_id << "] " << vertex_value(g, **frankfurt) << " (seed)" << endl;
    for (auto&& [vid, uv] : dfs) {
      ostream_indenter indent(static_cast<int>(dfs.depth()));
      cout << indent << "--> [" << vid << "] " << vertex_value(g, target(g, uv)) << ' ' << edge_value(g, uv) << "km"
           << endl;
      ++cnt;
    }
    REQUIRE(cnt == 9);
    /* Output:
        [0] Frankfürt (seed)
          --> [1] Mannheim 85km
            --> [2] Karlsruhe 80km
              --> [3] Augsburg 250km
                --> [8] München 84km
          --> [4] Würzburg 217km
            --> [5] Nürnberg 103km
              --> [9] Stuttgart 183km
            --> [7] Erfurt 186km
          --> [6] Kassel 173km
    */
  }
#elif TEST_OPTION == TEST_OPTION_GEN
  SECTION("bfs_edge_range generate content test") {
    using namespace std::graph;
    using std::cout;
    using std::endl;
    ostream_indenter indent;

    cout << endl
         << indent << "dfs_edge_range dfs(g, frankfurt_id);" << endl
         << indent << "auto           route    = dfs.begin();" << endl
         << indent << "int            city_cnt = 0;" << endl;

    int city_cnt = 0;
    for (auto&& [vid, uv] : dfs_edge_range(g, frankfurt_id)) {
      ++city_cnt;
      cout << endl << indent << "if(route != dfs.end()) {\n";
      ++indent;
      cout << indent << "auto&& [vid, uv] = *route;\n"
           << indent << "REQUIRE(" << vid << " == vid);\n"
           << indent << "REQUIRE(" << edge_value(g, uv) << " == edge_value(g, uv));\n"
           << indent << "REQUIRE(\"" << vertex_value(g, target(g, uv)) << "\" == vertex_value(g, target(g, uv)));\n"
           << indent << "++city_cnt;\n"
           << indent << "++route;\n";
      --indent;
      cout << indent << "}\n";
    }
    cout << endl << indent << "REQUIRE(" << city_cnt << " == city_cnt);" << endl << endl;
  }
#elif TEST_OPTION == TEST_OPTION_TEST
  SECTION("bfs_edge_range test content") {
    dfs_edge_range<G, void> dfs(g, frankfurt_id);
    auto                    route    = dfs.begin();
    int                     city_cnt = 0;

    if (route != dfs.end()) {
      auto&& [vid, uv] = *route;
      REQUIRE(1 == vid);
      REQUIRE(85 == edge_value(g, uv));
      REQUIRE("Mannheim" == vertex_value(g, target(g, uv)));
      ++city_cnt;
      ++route;
    }

    if (route != dfs.end()) {
      auto&& [vid, uv] = *route;
      REQUIRE(2 == vid);
      REQUIRE(80 == edge_value(g, uv));
      REQUIRE("Karlsruhe" == vertex_value(g, target(g, uv)));
      ++city_cnt;
      ++route;
    }

    if (route != dfs.end()) {
      auto&& [vid, uv] = *route;
      REQUIRE(3 == vid);
      REQUIRE(250 == edge_value(g, uv));
      REQUIRE("Augsburg" == vertex_value(g, target(g, uv)));
      ++city_cnt;
      ++route;
    }

    if (route != dfs.end()) {
      auto&& [vid, uv] = *route;
      REQUIRE(8 == vid);
      REQUIRE(84 == edge_value(g, uv));
      REQUIRE("München" == vertex_value(g, target(g, uv)));
      ++city_cnt;
      ++route;
    }

    if (route != dfs.end()) {
      auto&& [vid, uv] = *route;
      REQUIRE(4 == vid);
      REQUIRE(217 == edge_value(g, uv));
      REQUIRE("Würzburg" == vertex_value(g, target(g, uv)));
      ++city_cnt;
      ++route;
    }

    if (route != dfs.end()) {
      auto&& [vid, uv] = *route;
      REQUIRE(5 == vid);
      REQUIRE(103 == edge_value(g, uv));
      REQUIRE("Nürnberg" == vertex_value(g, target(g, uv)));
      ++city_cnt;
      ++route;
    }

    if (route != dfs.end()) {
      auto&& [vid, uv] = *route;
      REQUIRE(9 == vid);
      REQUIRE(183 == edge_value(g, uv));
      REQUIRE("Stuttgart" == vertex_value(g, target(g, uv)));
      ++city_cnt;
      ++route;
    }

    if (route != dfs.end()) {
      auto&& [vid, uv] = *route;
      REQUIRE(7 == vid);
      REQUIRE(186 == edge_value(g, uv));
      REQUIRE("Erfurt" == vertex_value(g, target(g, uv)));
      ++city_cnt;
      ++route;
    }

    if (route != dfs.end()) {
      auto&& [vid, uv] = *route;
      REQUIRE(6 == vid);
      REQUIRE(173 == edge_value(g, uv));
      REQUIRE("Kassel" == vertex_value(g, target(g, uv)));
      ++city_cnt;
      ++route;
    }

    REQUIRE(9 == city_cnt);
  }
#endif

  SECTION("dfs_edge_range with vertex value function") {
    auto                             evf = [&g](edge_reference_t<G> uv) { return edge_value(g, uv); };
    dfs_edge_range<G, decltype(evf)> dfs(g, frankfurt_id, evf);
    int                              city_cnt = 0;
    //cout << "\n[" << frankfurt_id << "] " << vertex_value(g, **frankfurt) << " (seed)" << endl;
    for (auto&& [vid, uv, km] : dfs) {
      ostream_indenter indent(dfs.size());
      //cout << indent << "--> [" << vid << "] " << vertex_value(g, target(g, uv)) << ' ' << km << "km" << endl;
      ++city_cnt;
    }
    REQUIRE(city_cnt == 9);
    /* Output:
        [0] Frankfürt (seed)
          --> [1] Mannheim 85km
            --> [2] Karlsruhe 80km
              --> [3] Augsburg 250km
                --> [8] München 84km
          --> [4] Würzburg 217km
            --> [5] Nürnberg 103km
              --> [9] Stuttgart 183km
            --> [7] Erfurt 186km
          --> [6] Kassel 173km
    */
  }

  SECTION("dfs_edge_range with no-EVF and Sourced") {
    dfs_edge_range<G, void, true> dfs(g, frankfurt_id);
    int                           city_cnt = 0;
    //cout << "\n[" << frankfurt_id << "] " << vertex_value(g, **frankfurt) << " (seed)" << endl;
    for (auto&& [uid, vid, uv] : dfs) {
      ostream_indenter indent(dfs.size());
      //cout << indent << "[" << uid << "] --> [" << vid << "] " << vertex_value(g, target(g, uv)) << ' ' << edge_value(g,uv) << "km" << endl;
      ++city_cnt;
    }
    REQUIRE(city_cnt == 9);
    /*
    [0] Frankfürt (seed)
      [0] --> [1] Mannheim 85km
        [1] --> [2] Karlsruhe 80km
          [2] --> [3] Augsburg 250km
            [3] --> [8] München 84km
      [0] --> [4] Würzburg 217km
        [4] --> [5] Nürnberg 103km
          [5] --> [9] Stuttgart 183km
        [4] --> [7] Erfurt 186km
      [0] --> [6] Kassel 173km
    */
  }

  SECTION("dfs_edge_range with EVF and Sourced") {
    auto                                   evf = [&g](edge_reference_t<G> uv) { return edge_value(g, uv); };
    dfs_edge_range<G, decltype(evf), true> dfs(g, frankfurt_id, evf);
    int                                    city_cnt = 0;
    //cout << "\n[" << frankfurt_id << "] " << vertex_value(g, **frankfurt) << " (seed)" << endl;
    for (auto&& [uid, vid, uv, km] : dfs) {
      ostream_indenter indent(dfs.size());
      //cout << indent << "[" << uid << "] --> [" << vid << "] " << vertex_value(g, target(g, uv)) << ' ' << km << "km" << endl;
      ++city_cnt;
    }
    REQUIRE(city_cnt == 9);
    /*
    [0] Frankfürt (seed)
      [0] --> [1] Mannheim 85km
        [1] --> [2] Karlsruhe 80km
          [2] --> [3] Augsburg 250km
            [3] --> [8] München 84km
      [0] --> [4] Würzburg 217km
        [4] --> [5] Nürnberg 103km
          [5] --> [9] Stuttgart 183km
        [4] --> [7] Erfurt 186km
      [0] --> [6] Kassel 173km
    */
  }

  SECTION("dfs_edge_range can do cancel_all") {
    int                 city_cnt = 0;
    dfs_edge_range<G> dfs(g, frankfurt_id);
    for (auto&& [vid, uv] : dfs) {
      ++city_cnt;
      if (vid == 2) // Karlsruhe
        dfs.cancel(std::graph::views::cancel_search::cancel_all);
    }
    REQUIRE(2 == city_cnt);
  }
  SECTION("dfs_edge_range can do cancel_branch") {
    //cout << '[' << frankfurt_id << "] " << vertex_value(g, **frankfurt) << " (seed)" << endl;
    int                 city_cnt = 0;
    dfs_edge_range<G> dfs(g, frankfurt_id);
    for (auto&& [vid, uv] : dfs) {
      ostream_indenter indent(size(dfs));
      //cout << indent << "[" << uid << "] " << vertex_value(g,u) << endl;
      ++city_cnt;
      if (vid == 4) // Wurzburg
        dfs.cancel(std::graph::views::cancel_search::cancel_branch);
    }
    REQUIRE(6 == city_cnt);
  }
}
