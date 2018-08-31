#pragma once

namespace SlitherLink
{
    public enum class GridItemState : int
    {
        None,
        InSide,
        OutSide,
        Cross,
        Line,
    };

    static GridItemState GetReverseState(GridItemState state)
    {
        switch (state)
        {
        case GridItemState::Line:
            return GridItemState::Cross;
        case GridItemState::Cross:
            return GridItemState::Line;
        case GridItemState::InSide:
            return GridItemState::OutSide;
        case GridItemState::OutSide:
            return GridItemState::InSide;
        }
        return GridItemState::None;
    }
}