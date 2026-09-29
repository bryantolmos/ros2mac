#include <iostream>
#include <mujoco/mujoco.h>

int main() {
    std::cout << "MuJoCo framework linked successfully!" << mj_versionString() << std::endl;
    return 0;
}
