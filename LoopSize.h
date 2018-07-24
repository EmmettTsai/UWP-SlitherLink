#pragma once

namespace SlitherLink
{
    /*
    https://www.puzzle-loop.com
    https://www.puzzle-loop.com/?size=0
    5x5 Loop Normal         0
    5x5 Loop Hard           4
    7x7 Loop Normal         10
    7x7 Loop Hard           11
    10x10 Loop Normal       1
    10x10 Loop Hard         5
    15x15 Loop Normal       2
    15x15 Loop Hard         6
    20x20 Loop Normal       3
    20x20 Loop Hard         7
    25x30 Loop Normal       8
    25x30 Loop Hard         9
    Special Daily Loop      13
    Special Weekly Loop     12
    Special Monthly Loop    14
    */
    public enum class LoopSize
    {
        Normal_5x5,
        Normal_10x10,
        Normal_15x15,
        Normal_20x20,
        Hard_5x5,
        Hard_10x10,
        Hard_15x15,
        Hard_20x20,
        Normal_25x30,
        Hard_25x30,
        Normal_7x7,
        Hard_7x7,
        Special_Weekly,
        Special_Daily,
        Special_Monthly,
    };
}