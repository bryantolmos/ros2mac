// Copyright 2026 Bryant Olmos
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace buffer {

static constexpr size_t CACHE_LINE_SIZE = 128;

struct alignas(CACHE_LINE_SIZE) RobotState {
    static constexpr size_t MAX_DOF = 10;
    size_t dof{0};
    uint64_t timestamp_ns{0};
    std::array<double, MAX_DOF> q{}; // joint positions in radians
    std::array<double, MAX_DOF> qd{}; // joint velocities in radians/sec
    std::array<double, MAX_DOF> tau{}; // joint torques in Nm
};

static_assert(sizeof(RobotState) % CACHE_LINE_SIZE == 0, "RobotState must be a multiple of CACHE_LINE_SIZE.");
static_assert(alignof(RobotState) == CACHE_LINE_SIZE, "RobotState must align to CACHE_LINE_SIZE.");

class alignas(CACHE_LINE_SIZE) StateBuffer {
private:
    alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> sequence_{0};
    alignas(CACHE_LINE_SIZE) RobotState state_{};

public:
    StateBuffer() = default;
    ~StateBuffer() = default;

    StateBuffer(const StateBuffer&) = delete;
    StateBuffer& operator=(const StateBuffer&) = delete;
    StateBuffer(StateBuffer&&) = delete;
    StateBuffer& operator=(StateBuffer&&) = delete;

    void write(const RobotState& new_state) noexcept {
        uint64_t seq = sequence_.load(std::memory_order_relaxed);

        sequence_.store(seq + 1, std::memory_order_relaxed); // write in progress (odd)
        std::atomic_thread_fence(std::memory_order_release); // guarantee that the write to sequence_ is visible before the write to state_

        state_ = new_state;

        sequence_.store(seq + 2, std::memory_order_release); // write complete (even)
    }

    bool read(RobotState& output_state) const noexcept {
        uint64_t prev_sequence = sequence_.load(std::memory_order_acquire);

        // write in progress
        if (prev_sequence & 1) { 
            return false;
        }

        output_state = state_;

        std::atomic_thread_fence(std::memory_order_seq_cst); // ensure that the read of state_ is completed before reading sequence_ again

        uint64_t post_sequence = sequence_.load(std::memory_order_relaxed);
        return (prev_sequence == post_sequence);
    }
};
static_assert(alignof(StateBuffer) == CACHE_LINE_SIZE, "StateBuffer must align to CACHE_LINE_SIZE.");
} // namespace buffer