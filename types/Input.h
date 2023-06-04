#pragma once

#include <input/input.h>

struct Input{
    InputKey inputKey;
    InputType inputType;
};

enum InputState{
    InputDown, InputUp, InputPress, InputRelease
};