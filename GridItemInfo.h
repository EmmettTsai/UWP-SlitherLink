#pragma once

#include "GridItemType.h"
#include "GridItemState.h"
#include "SolverGridItemState.h"

namespace SlitherLink
{
    public ref class GridItemInfo sealed
    {
    public:
        GridItemInfo()
        {
            ResetState();
        }

        void ResetState()
        {
            IsLocked = false;
            RecursiveFlag = false;
            Handled = false;
            State = GridItemState::None;
            StateSlotA = GridItemState::None;
            StateSlotB = GridItemState::None;
            SolverState = SolverGridItemState::None;
        }

        property GridItemType Type;
        property GridItemState State;
        property GridItemState StateSlotA;
        property GridItemState StateSlotB;
        property SolverGridItemState SolverState;
        property int Row;
        property int Column;
        property int Degree;
        property bool Handled;
        property bool IsExtended;
        property bool IsLocked;
        property bool RecursiveFlag;
        property bool IsColorBoundary;
        property Windows::UI::Xaml::Controls::Border^ View;
    };
}
