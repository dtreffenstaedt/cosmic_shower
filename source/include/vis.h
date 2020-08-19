#ifndef VIS_H
#define VIS_H

#include <algorithm>
#include <filesystem>
#include <gd.h>
#include <iostream>
#include <iterator>
#include <string>
#include <tuple>
#include <vector>

struct ImageBox {
    gdImagePtr ptr { nullptr };
    int w { 0 };
    int h { 0 };
};

struct Node {
    std::string name;
    bool has_hits;
    bool has_intensity;
    std::vector<Node> children;
};

struct Field {
    std::string name;
    std::string parent;
    bool has_hits;
    bool has_intensity;
};

[[nodiscard]] auto draw_box(const Node& node) -> ImageBox;
[[nodiscard]] auto draw_tree_node(const Node& node, std::vector<ImageBox> boxes) -> ImageBox;
[[nodiscard]] auto draw_tree(const Node& n) -> ImageBox;

[[nodiscard]] auto get_field(const std::string& name, const std::string& dir) -> Field;
[[nodiscard]] auto construct_tree(const std::string& dir) -> Node;
[[nodiscard]] auto find_children(const std::string& name, const std::vector<Field>& fields) -> std::tuple<std::vector<Field>, std::vector<Node>>;

[[nodiscard]] auto subtree(Node& root, const std::vector<Field>& fields) -> std::vector<Field>;
[[nodiscard]] auto all_files(const std::string& dir) -> std::vector<std::string>;

[[nodiscard]] auto is_root(const std::string& name, const std::vector<Field>& fields) -> bool;

#endif // VIS_H
