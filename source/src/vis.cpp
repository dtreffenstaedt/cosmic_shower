#include "vis.h"

#include <iostream>

#include <cstdio>

#include <libconfig.h++>

auto main(const int argc, const char* argv[]) -> int
{

    if (argc != 2) {
        std::cerr << "expected one parameter\n";
        return -1;
    }

    auto tree = construct_tree(argv[1]);

    auto box = draw_tree(tree);
    FILE* pngout { fopen("test.png", "wb") };
    gdImagePng(box.ptr, pngout);

    fclose(pngout);
    gdImageDestroy(box.ptr);
}

auto draw_tree(const Node& n) -> ImageBox
{
    if (n.children.empty()) {
        return draw_box(n);
    }

    std::vector<ImageBox> children;
    for (const auto& c : n.children) {
        children.push_back(draw_tree(c));
    }

    return draw_tree_node(n, children);
}

auto subtree(Node& root, const std::vector<Field>& fields) -> std::vector<Field>
{
    auto [rest, children] = find_children(root.name, fields);
    root.children = children;
    for (auto& c : root.children) {
        rest = subtree(c, rest);
    }
    return rest;
}

auto construct_tree(const std::string& dir) -> Node
{
    std::vector<Field> fields;
    Node root;
    for (const auto& name : all_files(dir)) {
        fields.push_back(get_field(name, dir));
    }

    for (const auto& field : fields) {
        if (is_root(field.parent, fields)) {
            root.name = field.parent;
            break;
        }
    }

    auto rest = subtree(root, fields);

    return root;
}

auto draw_box(const Node& node) -> ImageBox
{
    ImageBox box;
    box.w = 100;
    box.h = 40;
    box.ptr = gdImageCreate(box.w, box.h);
    int black = gdImageColorAllocate(box.ptr, 0, 0, 0);
    int white = gdImageColorAllocate(box.ptr, 255, 255, 255);
    int blue = gdImageColorAllocate(box.ptr, 200, 200, 255);
    int green = gdImageColorAllocate(box.ptr, 200, 255, 200);
    int rect[8] {};
    gdImageStringTTF(box.ptr, rect, white, "/usr/share/fonts/TTF/FreeMono.ttf", 30, 0, 0, 0, node.name.c_str());
    bool recreate = false;
    if (rect[2] >= box.w) {
        box.w = rect[2] + 10;
        recreate = true;
    }
    if (rect[5] >= box.h) {
        box.h = rect[5] + 10;
        recreate = true;
    }
    if (recreate) {
        gdImageDestroy(box.ptr);
        box.ptr = gdImageCreate(box.w, box.h);
        black = gdImageColorAllocate(box.ptr, 0, 0, 0);
        white = gdImageColorAllocate(box.ptr, 255, 255, 255);
        blue = gdImageColorAllocate(box.ptr, 200, 200, 255);
        green = gdImageColorAllocate(box.ptr, 200, 255, 200);
    }

    gdImageFill(box.ptr, 0, 0, white);

    gdImageStringTTF(box.ptr, rect, white, "/usr/share/fonts/TTF/FreeMono.ttf", 30, 0, 1, 19, node.name.c_str());
    gdImageFill(box.ptr, 0, 0, white);

    if (node.has_hits) {
        gdImageFilledRectangle(box.ptr, 0, 0, box.w / 2, box.h, green);
    }
    if (node.has_intensity) {
        gdImageFilledRectangle(box.ptr, box.w / 2, 0, box.w, box.h, blue);
    }

    int x = (box.w - (rect[4] - rect[0])) / 2;
    int y = (box.h + (rect[1] - rect[5])) / 2;

    gdImageStringTTF(box.ptr, rect, black, "/usr/share/fonts/TTF/FreeMono.ttf", 30, 0, x, y, node.name.c_str());

    gdImageRectangle(box.ptr, 0, 0, box.w - 1, box.h - 1, black);

    return box;
}

auto draw_tree_node(const Node& node, std::vector<ImageBox> boxes) -> ImageBox
{
    auto top = draw_box(node);
    ImageBox box;
    box.w = 10;
    for (const auto& b : boxes) {
        box.w += b.w + 10;
        if (box.h < b.h) {
            box.h = b.h;
        }
    }
    int y = top.h + 100;
    box.h += 100 + top.h;

    box.ptr = gdImageCreate(box.w, box.h);

    int white = gdImageColorAllocate(box.ptr, 255, 255, 255);
    gdImageFill(box.ptr, 0, 0, white);

    gdImageCopy(box.ptr, top.ptr, (box.w - top.w) / 2, 0, 0, 0, top.w, top.h);

    int black = gdImageColorAllocate(box.ptr, 0, 0, 0);

    int x = 10;
    for (const auto& b : boxes) {
        gdImageLine(box.ptr, box.w / 2, top.h, x + b.w / 2, y, black);
        gdImageCopy(box.ptr, b.ptr, x, y, 0, 0, b.w, b.h);
        x += b.w + 10;
    }

    return box;
}

auto all_files(const std::string& dir) -> std::vector<std::string>
{
    std::vector<std::string> files;
    for (const auto& d : std::filesystem::directory_iterator(dir)) {
        if (d.is_regular_file()) {
            files.push_back(d.path().filename().string());
        }
    }
    return files;
}

auto get_field(const std::string& name, const std::string& dir) -> Field
{
    libconfig::Config conf {};
    conf.readFile((dir + "/" + name).c_str());
    const libconfig::Setting& root = conf.getRoot();

    Field field;
    root.exists("parent");
    root.lookupValue("parent", field.parent);
    root.lookupValue("name", field.name);
    std::cout << "data/" + name + "/event_1/ground_intensity\n";
    std::cout << "data/" + name + "/event_1/detailed_hits\n";
    field.has_intensity = std::filesystem::exists("data/" + name + "/event_1/ground_intensity");
    field.has_hits = std::filesystem::exists("data/" + name + "/event_1/detailed_hits");
    return field;
}

auto is_root(const std::string& name, const std::vector<Field>& fields) -> bool
{
    for (const auto& field : fields) {
        if (field.name == name) {
            return false;
        }
    }
    return true;
}

auto find_children(const std::string& name, const std::vector<Field>& fields) -> std::tuple<std::vector<Field>, std::vector<Node>>
{
    std::vector<Node> children;
    std::vector<Field> rest;
    for (const auto& field : fields) {
        if (field.parent == name) {
            Node n;
            n.name = field.name;
            n.has_hits = field.has_hits;
            n.has_intensity = field.has_intensity;
            children.push_back(n);
        } else {
            rest.push_back(field);
        }
    }
    return std::make_tuple(rest, children);
}
