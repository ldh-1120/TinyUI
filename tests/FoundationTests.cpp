#include <TinyUI/Foundation/Handle.h>
#include <TinyUI/Foundation/Result.h>
#include <TinyUI/Foundation/SlotMap.h>

#include <catch2/catch_test_macros.hpp>

using namespace tinyui;

namespace {
    struct NodeTag {};
    using NodeId = Handle<NodeTag>;

    struct TestNode {
        explicit TestNode(int initialValue) : value(initialValue) {}

        TestNode(const TestNode &) = delete;
        TestNode &operator=(const TestNode &) = delete;

        TestNode(TestNode &&) = delete;
        TestNode &operator=(TestNode &&) = delete;

        int value;
    };
}

TEST_CASE("SlotMap creates and removes objects", "[foundation]") {
    SlotMap<TestNode, NodeTag> nodes;

    const NodeId first = nodes.emplace(42);
    REQUIRE(first.valid());
    REQUIRE(nodes.contains(first));
    REQUIRE(nodes.get(first)->value == 42);
    REQUIRE(nodes.size() == 1);

    REQUIRE(nodes.erase(first));
    REQUIRE_FALSE(nodes.contains(first));
    REQUIRE(nodes.get(first) == nullptr);
    REQUIRE(nodes.empty());
}

TEST_CASE("SlotMap rejects stale handles", "[foundation]") {
    SlotMap<TestNode, NodeTag> nodes;

    const NodeId oldId = nodes.emplace(10);
    REQUIRE(nodes.erase(oldId));

    const NodeId newId = nodes.emplace(20);
    REQUIRE(oldId.index == newId.index);
    REQUIRE(oldId.generation != newId.generation);

    REQUIRE(nodes.get(oldId) == nullptr);
    REQUIRE(nodes.get(newId)->value == 20);
    REQUIRE_FALSE(nodes.erase(oldId));
}

TEST_CASE("SlotMap isolates owners", "[foundation]") {
    SlotMap<TestNode, NodeTag> first;
    SlotMap<TestNode, NodeTag> second;

    const NodeId firstId = first.emplace(10);
    const NodeId secondId = second.emplace(20);

    REQUIRE(firstId.index == secondId.index);
    REQUIRE(firstId.owner != secondId.owner);

    REQUIRE(first.get(secondId) == nullptr);
    REQUIRE(second.get(firstId) == nullptr);
}

TEST_CASE("SlotMap preserves live object addresses", "[foundation]") {
    SlotMap<TestNode, NodeTag> nodes;

    const NodeId first = nodes.emplace(42);
    TestNode *original = nodes.get(first);
    REQUIRE(original != nullptr);

    for (int i = 0; i < 1000; ++i) {
        const NodeId inserted = nodes.emplace(i);
        REQUIRE(nodes.contains(inserted));
    }

    REQUIRE(nodes.get(first) == original);
    REQUIRE(original->value == 42);
}

TEST_CASE("SlotMap clear invalidates handles", "[foundation]") {
    SlotMap<TestNode, NodeTag> nodes;

    const NodeId oldId = nodes.emplace(42);
    nodes.clear();

    const NodeId newId = nodes.emplace(100);
    REQUIRE(oldId.owner != newId.owner);
    REQUIRE(nodes.get(oldId) == nullptr);
    REQUIRE(nodes.get(newId)->value == 100);
}