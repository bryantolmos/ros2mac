#include <iostream>
#include "../include/state_buffer.hpp"

int main() {
    buffer::StateBuffer buffer;
    buffer::RobotState test_state;
    test_state.dof = 7;
    buffer.write(test_state);

    buffer::RobotState read_state;
    bool ok = buffer.read(read_state);

    std::cout << "StateBuffer size: " << sizeof(buffer::StateBuffer) 
              << " | Read valid: " << std::boolalpha << ok 
              << " | DOF: " << read_state.dof << "\n";
    return 0;
}
