#pragma once

/*
 * Created by Kami-Kaze on 4/15/2021.
 */

#include <cstdint>

namespace Engine
{
    using MouseCode = int16_t;

    namespace Mouse
    {
        enum : MouseCode
        {
            None                   = -1,

            // From glfw3.h
            Button0                = 0,
            Button1                = 1,
            Button2                = 2,
            Button3                = 3,
            Button4                = 4,
            Button5                = 5,
            Button6                = 6,
            Button7                = 7,

            ButtonLeft             = Button0,
            ButtonRight            = Button1,
            ButtonMiddle           = Button2
        };
    }
}