#pragma once

#include <TinyUI/Foundation/Handle.h>

#include <atomic>
#include <cstdint>
#include <deque>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace tinyui {
    namespace detail {
        inline std::atomic<std::uint64_t> nextSlotMapOwner(1);

        inline std::uint64_t acquireSlotMapOwner() noexcept {
            const std::uint64_t id = nextSlotMapOwner.fetch_add(1, std::memory_order_relaxed);
            if (id == 0)
                std::terminate();

            return id;
        }
    }

    template <typename T, typename Tag>
    class SlotMap {
    public:
        using Id = Handle<Tag>;

        SlotMap() : owner_(detail::acquireSlotMapOwner()) {}

        SlotMap(const SlotMap&) = delete;
        SlotMap& operator=(const SlotMap&) = delete;

        SlotMap(SlotMap&&) = delete;
        SlotMap& operator=(SlotMap&&) = delete;

        template<typename... Args>
        [[nodiscard]] Id emplace(Args&&... args) {
            if (!freeIndices_.empty()) {
                const std::uint32_t index = freeIndices_.back();
                Slot& slot = slots_[index];

                slot.value.emplace(std::forward<Args>(args)...);
                freeIndices_.pop_back();

                ++size_;
                return {index, slot.generation, owner_};
            }

            if (slots_.size() >= Id::InvalidIndex)
                throw std::length_error("SlotMap capacity exceeded.");

            const std::uint32_t index = static_cast<std::uint32_t>(slots_.size());
            Slot& slot = slots_.emplace_back(std::in_place, std::forward<Args>(args)...);

            ++size_;
            return {index, slot.generation, owner_};
        }

        [[nodiscard]] T* get(Id id) noexcept {
            return const_cast<T*>(std::as_const(*this).get(id));
        }

        [[nodiscard]] const T* get(Id id) const noexcept {
            if (!id.valid() || id.owner != owner_)
                return nullptr;

            if (id.index >= slots_.size())
                return nullptr;

            const Slot& slot = slots_[id.index];
            if (slot.generation != id.generation || !slot.value)
                return nullptr;

            return std::addressof(*slot.value);
        }

        [[nodiscard]] bool contains(Id id) const noexcept {
            return get(id) != nullptr;
        }

        bool erase(Id id) {
            if (!contains(id))
                return false;

            Slot& slot = slots_[id.index];
            if (slot.generation != std::numeric_limits<std::uint32_t>::max()) {
                freeIndices_.push_back(id.index);
                ++slot.generation;
            }

            slot.value.reset();
            --size_;

            return true;
        }

        void clear() noexcept {
            slots_.clear();
            freeIndices_.clear();
            size_ = 0;
            owner_ = detail::acquireSlotMapOwner();
        }

        [[nodiscard]] std::size_t size() const noexcept {
            return size_;
        }

        [[nodiscard]] bool empty() const noexcept {
            return size_ == 0;
        }

    private:
        struct Slot {
            template <typename...Args>
            explicit Slot(std::in_place_t, Args&&... args) : value(std::in_place, std::forward<Args>(args)...) {}

            std::optional<T> value;
            std::uint32_t generation = 1;
        };

        static_assert(std::is_nothrow_destructible_v<T>);

        std::deque<Slot> slots_;
        std::vector<std::uint32_t> freeIndices_;

        std::size_t size_ = 0;
        std::uint64_t owner_;
    };
}