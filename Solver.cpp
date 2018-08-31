#include "pch.h"
#include "Solver.h"

using namespace SlitherLink;

using namespace Platform;
using namespace Platform::Collections;
#if USE_DELEGATE
using namespace Windows::UI::Core;
#endif

Solver::Solver(int row, int col, String^ data)
{
    mLoopRowSize = row;
    mLoopColSize = col;
    mRowSize = 2 * row + 1;
    mColSize = 2 * col + 1;
    mExtendedRowSize = mRowSize + 4;
    mExtendedColSize = mColSize + 4;
    mRowStart = 2;
    mRowEnd = mExtendedRowSize - 3;
    mColStart = 2;
    mColEnd = mExtendedColSize - 3;
    mData = data;

    mExtendedLoop = ref new Vector<GridItemInfo^>();
    mGridZero = ref new Vector<GridItemInfo^>();
    mGridOne = ref new Vector<GridItemInfo^>();
    mGridTwo = ref new Vector<GridItemInfo^>();
    mGridThree = ref new Vector<GridItemInfo^>();
    mGridUnknown = ref new Vector<GridItemInfo^>();
    mQueue = ref new Vector<GridItemInfo^>();
    mDotSet = ref new Vector<GridItemInfo^>();
    mLeftBorderCellSet = ref new Vector<GridItemInfo^>();
    mTopBorderCellSet = ref new Vector<GridItemInfo^>();
    mRightBorderCellSet = ref new Vector<GridItemInfo^>();
    mBottomBorderCellSet = ref new Vector<GridItemInfo^>();
    myLogW(LOG_DEBUG, LTAG L"[%d][%s] mData->Length = %d, mData = %s", __LINE__, __funcw__, mData->Length(), mData->Data());
    InitExtendedLoop();
}


#if USE_DELEGATE
void Solver::SetMainDispatcher(CoreDispatcher^ dispatcher)
{
    mMainDispatcher = dispatcher;
}


void Solver::UpdateMainView(GridItemInfo^ info, GridItemState state)
{
    mMainDispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this, info, state]()
    {
        switch (state)
        {
        case GridItemState::Line:
            OnSetLine(OnGetExtendedLoopAt(info->Row, info->Column)->View, true);
            break;
        case GridItemState::Cross:
            OnSetCross(OnGetExtendedLoopAt(info->Row, info->Column)->View, true);
            break;
        case GridItemState::InSide:
            OnSetInside(OnGetExtendedLoopAt(info->Row, info->Column)->View, true);
            break;
        case GridItemState::OutSide:
            OnSetOutside(OnGetExtendedLoopAt(info->Row, info->Column)->View, true);
            break;
        }
    }));
}
#endif


int Solver::GetDataAt(int i, int j)
{
    switch (mData->Data()[i * mLoopColSize + j])
    {
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    }
    return -1;
}


GridItemInfo^ Solver::GetExtendedLoopAt(int i, int j)
{
    return mExtendedLoop->GetAt(i * mExtendedColSize + j);
}


GridItemInfo^ Solver::GetExtendedLoopAt(GridItemInfo^ info, int i, int j)
{
    return GetExtendedLoopAt(info->Row + i, info->Column + j);
}


GridItemInfo^ Solver::GetExtendedLoopAt(GridItemInfo^ info, Direction direction, int scale)
{
    switch (direction)
    {
    case Direction::LeftTop:
        return GetExtendedLoopAt(info->Row - scale, info->Column - scale);
    case Direction::Top:
        return GetExtendedLoopAt(info->Row - scale, info->Column);
    case Direction::RightTop:
        return GetExtendedLoopAt(info->Row - scale, info->Column + scale);
    case Direction::Right:
        return GetExtendedLoopAt(info->Row, info->Column + scale);
    case Direction::RightBottom:
        return GetExtendedLoopAt(info->Row + scale, info->Column + scale);
    case Direction::Bottom:
        return GetExtendedLoopAt(info->Row + scale, info->Column);
    case Direction::LeftBottom:
        return GetExtendedLoopAt(info->Row + scale, info->Column - scale);
    case Direction::Left:
        return GetExtendedLoopAt(info->Row, info->Column - scale);
    }
}


GridItemInfo^ Solver::GetLeftTop(GridItemInfo^ info)
{
    return GetExtendedLoopAt(info->Row - 1, info->Column - 1);
}


GridItemInfo^ Solver::GetTop(GridItemInfo^ info)
{
    return GetExtendedLoopAt(info->Row - 1, info->Column);
}


GridItemInfo^ Solver::GetRightTop(GridItemInfo^ info)
{
    return GetExtendedLoopAt(info->Row - 1, info->Column + 1);
}


GridItemInfo^ Solver::GetRight(GridItemInfo^ info)
{
    return GetExtendedLoopAt(info->Row, info->Column + 1);
}


GridItemInfo^ Solver::GetRightBottom(GridItemInfo^ info)
{
    return GetExtendedLoopAt(info->Row + 1, info->Column + 1);
}


GridItemInfo^ Solver::GetBottom(GridItemInfo^ info)
{
    return GetExtendedLoopAt(info->Row + 1, info->Column);
}


GridItemInfo^ Solver::GetLeftBottom(GridItemInfo^ info)
{
    return GetExtendedLoopAt(info->Row + 1, info->Column - 1);
}


GridItemInfo^ Solver::GetLeft(GridItemInfo^ info)
{
    return GetExtendedLoopAt(info->Row, info->Column - 1);
}


void Solver::InitExtendedLoop()
{
    for (int i = 0; i < mExtendedRowSize; i++)
    {
        for (int j = 0; j < mExtendedColSize; j++)
        {
            int innerI = i - 2;
            int innerJ = j - 2;
            bool isExtendedItem = !(i >= mRowStart && i <= mRowEnd && j >= mColStart && j <= mColEnd);

            GridItemInfo^ info = ref new GridItemInfo();
            info->Row = i;
            info->Column = j;
            info->State = GridItemState::None;
            info->Handled = false;
            info->IsExtended = isExtendedItem;

            if (i % 2 == 0 && j % 2 == 0)
            {
                info->Type = GridItemType::Dot;
                info->Degree = 0;
            }
            else if (i % 2 == 1 && j % 2 == 1)
            {
                info->Type = GridItemType::Cell;

                if (isExtendedItem)
                {
                    info->State = GridItemState::OutSide;
                    info->Degree = -1;
                }
                else
                {
                    int loopRow = (innerI - 1) / 2;
                    int loopCol = (innerJ - 1) / 2;
                    info->Degree = GetDataAt(loopRow, loopCol);
                    myLogW(LOG_DEBUG, LTAG L"[%d][%s] loopRow = %d, loopCol = %d, degree = %d", __LINE__, __funcw__, loopRow, loopCol, info->Degree);
                    switch (info->Degree)
                    {
                    case 0:
                        mGridZero->Append(info);
                        break;
                    case 1:
                        mGridOne->Append(info);
                        break;
                    case 2:
                        mGridTwo->Append(info);
                        break;
                    case 3:
                        mGridThree->Append(info);
                        break;
                    default:
                        mGridUnknown->Append(info);
                    }
                    if (j == mColStart + 1)
                    {
                        mLeftBorderCellSet->Append(info);
                    }
                    if (i == mRowStart + 1)
                    {
                        mTopBorderCellSet->Append(info);
                    }
                    if (j == mColEnd - 1)
                    {
                        mRightBorderCellSet->Append(info);
                    }
                    if (i == mRowEnd - 1)
                    {
                        mBottomBorderCellSet->Append(info);
                    }
                }
            }
            else if (i % 2 == 0)
            {
                info->Type = GridItemType::HorizontailLine;
                info->Degree = 0;
                if (isExtendedItem)
                {
                    info->State = GridItemState::Cross;
                }
            }
            else
            {
                info->Type = GridItemType::VerticalLine;
                info->Degree = 0;
                if (isExtendedItem)
                {
                    info->State = GridItemState::Cross;
                }
            }
            mExtendedLoop->Append(info);
        }
    }
    myLogW(LOG_DEBUG, LTAG L"[%d][%s] mGridZero->Size = %u", __LINE__, __funcw__, mGridZero->Size);
    myLogW(LOG_DEBUG, LTAG L"[%d][%s] mGridOne->Size = %u", __LINE__, __funcw__, mGridOne->Size);
    myLogW(LOG_DEBUG, LTAG L"[%d][%s] mGridTwo->Size = %u", __LINE__, __funcw__, mGridTwo->Size);
    myLogW(LOG_DEBUG, LTAG L"[%d][%s] mGridThree->Size = %u", __LINE__, __funcw__, mGridThree->Size);
    myLogW(LOG_DEBUG, LTAG L"[%d][%s] mGridUnknown->Size = %u", __LINE__, __funcw__, mGridUnknown->Size);
}


Platform::String^ Solver::GetResult()
{
    String^ result = L"";
    for (auto info : mExtendedLoop)
    {
        if (info->IsExtended)
        {
            continue;
        }
        switch (info->State)
        {
        case GridItemState::None:
            result += L"0";
            break;
        case GridItemState::Line:
        case GridItemState::InSide:
            result += L"1";
            break;
        case GridItemState::Cross:
        case GridItemState::OutSide:
            result += L"2";
            break;
        }
    }
    return result;
}


String^ Solver::Solve()
{
    RuleZero();
    RuleThreeThree();

    for (auto set : { mGridThree, mGridTwo, mGridOne })
    {
        for (auto info : set)
        {
            mQueue->Append(info);
            info->SolverState = SolverGridItemState::Queued;
        }
    }
    while (true)
    {
        mUpdated = false;
        while (true)
        {
            auto info = mQueue->GetAt(0);
            mQueue->RemoveAt(0);
            if (info->SolverState != SolverGridItemState::Completed)
            {
                info->SolverState = SolverGridItemState::Checked;
            }
            RuleCheckSelf(info);
            if (mQueue->Size > 0)
            {
                continue;
            }
            RuleCycleTest();
            RuleCycleTestForThree();
            if (mQueue->Size > 0)
            {
                continue;
            }
            RuleColorTest();
            if (mQueue->Size == 0)
            {
                break;
            }
        }
        if (!mUpdated)
        {
            break;
        }
        for (auto set : { mGridThree, mGridTwo, mGridOne, mGridUnknown })
        {
            unsigned int i = 0;
            while (i < set->Size)
            {
                auto info = set->GetAt(i);
                if (info->SolverState == SolverGridItemState::Completed)
                {
                    set->RemoveAt(i);
                    continue;
                }
                mQueue->Append(info);
                info->SolverState = SolverGridItemState::Queued;
                i++;
            }
        }
    }

    return GetResult();
}


void Solver::UpdateQueue(GridItemInfo^ info)
{
    mUpdated = true;
    switch (info->Type)
    {
        case GridItemType::HorizontailLine: {
            for (auto cell :
                {
                    GetExtendedLoopAt(info, -1, -2),
                    GetExtendedLoopAt(info, -1, 0),
                    GetExtendedLoopAt(info, -1, 2),
                    GetExtendedLoopAt(info, 1, 2),
                    GetExtendedLoopAt(info, 1, 0),
                    GetExtendedLoopAt(info, 1, -2)
                })
            {
                if (!cell->IsExtended
                    && cell->SolverState != SolverGridItemState::Completed
                    && cell->SolverState != SolverGridItemState::Queued)
                {
                    mQueue->Append(cell);
                }
            }
            break;
        }
        case GridItemType::VerticalLine: {
            for (auto cell :
                {
                    GetExtendedLoopAt(info, -2, -1),
                    GetExtendedLoopAt(info, -2, 1),
                    GetExtendedLoopAt(info, 0, 1),
                    GetExtendedLoopAt(info, 2, 1),
                    GetExtendedLoopAt(info, 2, -1),
                    GetExtendedLoopAt(info, 0, -1)
                })
            {
                if (!cell->IsExtended
                    && cell->SolverState != SolverGridItemState::Completed
                    && cell->SolverState != SolverGridItemState::Queued)
                {
                    mQueue->Append(cell);
                }
            }
            break;
        }
    }
}


bool Solver::SetLine(GridItemInfo^ info)
{
    if (info->State == GridItemState::Cross)
    {
        return false;
    }
    else if (info->State == GridItemState::None)
    {
        info->State = GridItemState::Line;
#if USE_DELEGATE
        UpdateMainView(info, info->State);
#endif
        switch (info->Type)
        {
        case GridItemType::HorizontailLine:
            for (auto vertex : { GetLeft(info), GetRight(info) })
            {
                if (++vertex->Degree == 1)
                {
                    mDotSet->Append(vertex);
                }
            }
            break;
        case GridItemType::VerticalLine:
            for (auto vertex : { GetTop(info), GetBottom(info) })
            {
                if (++vertex->Degree == 1)
                {
                    mDotSet->Append(vertex);
                }
            }
            break;
        }
        UpdateQueue(info);
    }
    return true;
}


bool Solver::SetCross(GridItemInfo^ info)
{
    if (info->State == GridItemState::Line)
    {
        return false;
    }
    else if (info->State == GridItemState::None)
    {
        info->State = GridItemState::Cross;
#if USE_DELEGATE
        UpdateMainView(info, info->State);
#endif
        UpdateQueue(info);
    }
    return true;
}


bool Solver::SetInside(GridItemInfo^ info)
{
    if (info->State == GridItemState::OutSide)
    {
        return false;
    }
    else if (info->State == GridItemState::None)
    {
        info->State = GridItemState::InSide;
#if USE_DELEGATE
        UpdateMainView(info, info->State);
#endif
    }
    return true;
}


bool Solver::SetOutside(GridItemInfo^ info)
{
    if (info->State == GridItemState::InSide)
    {
        return false;
    }
    else if (info->State == GridItemState::None)
    {
        info->State = GridItemState::OutSide;
#if USE_DELEGATE
        UpdateMainView(info, info->State);
#endif
    }
    return true;
}


bool Solver::SetState(GridItemInfo^ info, GridItemState state)
{
    switch (state)
    {
    case GridItemState::Line:
        return SetLine(info);
    case GridItemState::Cross:
        return SetCross(info);
    case GridItemState::InSide:
        return SetInside(info);
    case GridItemState::OutSide:
        return SetOutside(info);
    }
}


void Solver::RuleZero()
{
    for (auto info : mGridZero)
    {
        info->SolverState = SolverGridItemState::Completed;
        SetCross(GetLeft(info));
        SetCross(GetTop(info));
        SetCross(GetRight(info));
        SetCross(GetBottom(info));
    }
}


void Solver::RuleThreeThree()
{
    for (auto head : mGridThree)
    {
        GridItemInfo^ tail;
        GridItemInfo^ next;
        for (tail = head; (next = GetExtendedLoopAt(tail, Direction::Right, 2))->Degree == 3; tail = next)
        {
            SetLine(GetLeft(next));
            SetCross(GetExtendedLoopAt(next, -2, -1));
            SetCross(GetExtendedLoopAt(next, 2, -1));
        }
        if (tail != head)
        {
            SetLine(GetLeft(head));
            SetLine(GetRight(tail));
        }

        for (tail = head; (next = GetExtendedLoopAt(tail, Direction::Bottom, 2))->Degree == 3; tail = next)
        {
            SetLine(GetTop(next));
            SetCross(GetExtendedLoopAt(next, -1, -2));
            SetCross(GetExtendedLoopAt(next, -1, 2));
        }
        if (tail != head)
        {
            SetLine(GetTop(head));
            SetLine(GetBottom(tail));
        }

        for (tail = GetExtendedLoopAt(head, Direction::RightBottom, 2); tail->Degree == 2; tail = GetExtendedLoopAt(tail, Direction::RightBottom, 2));
        if (tail->Degree == 3)
        {
            SetLine(GetLeft(head));
            SetLine(GetTop(head));
            SetCross(GetExtendedLoopAt(head, -2, -1));
            SetCross(GetExtendedLoopAt(head, -1, -2));

            SetLine(GetRight(tail));
            SetLine(GetBottom(tail));
            SetCross(GetExtendedLoopAt(tail, 2, 1));
            SetCross(GetExtendedLoopAt(tail, 1, 2));
        }

        for (tail = GetExtendedLoopAt(head, Direction::LeftBottom, 2); tail->Degree == 2; tail = GetExtendedLoopAt(tail, Direction::LeftBottom, 2));
        if (tail->Degree == 3)
        {
            SetLine(GetRight(head));
            SetLine(GetTop(head));
            SetCross(GetExtendedLoopAt(head, -2, 1));
            SetCross(GetExtendedLoopAt(head, -1, 2));

            SetLine(GetLeft(tail));
            SetLine(GetBottom(tail));
            SetCross(GetExtendedLoopAt(tail, 2, -1));
            SetCross(GetExtendedLoopAt(tail, 1, -2));
        }
    }
}


void Solver::RuleCheckSelf(GridItemInfo^ info)
{
    switch (info->Type)
    {
    case GridItemType::Cell:
        RuleCheckCell(info);
        break;
    }
}


void Solver::RuleCheckCell(GridItemInfo^ info)
{
    auto left = GetLeft(info);
    auto top = GetTop(info);
    auto right = GetRight(info);
    auto bottom = GetBottom(info);

    int lineCount = 0;
    int crossCount = 0;

    for (auto side : { left, top, right, bottom })
    {
        switch (side->State)
        {
        case GridItemState::Line:
            lineCount++;
            break;
        case GridItemState::Cross:
            crossCount++;
        }
    }

    if (lineCount == info->Degree)
    {
        info->SolverState = SolverGridItemState::Completed;
        if (info->Degree + crossCount != 4)
        {
            for (auto side : { left, top, right, bottom })
            {
                if (side->State == GridItemState::None)
                {
                    SetCross(side);
                }
            }
        }
    }
    else if (info->Degree + crossCount == 4)
    {
        info->SolverState = SolverGridItemState::Completed;
        for (auto side : { left, top, right, bottom })
        {
            if (side->State == GridItemState::None)
            {
                SetLine(side);
            }
        }
    }

    if (info->SolverState == SolverGridItemState::Completed)
    {
        for (auto direction : { Direction::LeftTop, Direction::RightTop, Direction::RightBottom, Direction::LeftBottom })
        {
            auto sideA = GetExtendedLoopAt(info, RotateDirection(direction, RotateDegree::Counterclockwise45));
            auto sideB = GetExtendedLoopAt(info, RotateDirection(direction, RotateDegree::Clockwise45));

            if (sideA->State == sideB->State)
            {
                RuleCornerHaveSameState(info, direction, sideA->State);
            }
            else
            {
                RuleCornerHaveDifferentState(info, direction);
            }
        }
    }
    else
    {
        for (auto direction : { Direction::LeftTop, Direction::RightTop, Direction::RightBottom, Direction::LeftBottom })
        {
            auto cell = GetExtendedLoopAt(info, direction, 2);
            auto reverseDirection = RotateDirection(direction, RotateDegree::Clockwise180);
            auto sideA = GetExtendedLoopAt(cell, RotateDirection(reverseDirection, RotateDegree::Counterclockwise45));
            auto sideB = GetExtendedLoopAt(cell, RotateDirection(reverseDirection, RotateDegree::Clockwise45));

            if (sideA->State != GridItemState::None && sideB->State != GridItemState::None)
            {
                if (sideA->State == sideB->State)
                {
                    RuleCornerHaveSameState(cell, reverseDirection, sideA->State);
                }
                else
                {
                    RuleCornerHaveDifferentState(cell, reverseDirection);
                }
            }
        }
    }

    switch (info->Degree)
    {
    case 1:
        RuleCheckOne(info);
        break;
    case 2:
        RuleCheckTwo(info);
        break;
    case 3:
        RuleCheckThree(info);
        break;
    default:
        RuleCheckUnknown(info);
    }
}


void Solver::RuleCheckOne(GridItemInfo^ info)
{

}


void Solver::RuleCheckTwo(GridItemInfo^ info)
{
    for (auto direction : { Direction::LeftTop, Direction::RightTop, Direction::RightBottom, Direction::LeftBottom })
    {
        auto reverseDirection = GetReverseDirection(direction);
        auto head = GetExtendedLoopAt(info, reverseDirection, 2);

        auto sideA = GetExtendedLoopAt(head, RotateDirection(direction, RotateDegree::Counterclockwise45));
        auto sideB = GetExtendedLoopAt(head, RotateDirection(direction, RotateDegree::Clockwise45));
        auto sideC = GetExtendedLoopAt(info, RotateDirection(reverseDirection, RotateDegree::Counterclockwise45));
        auto sideD = GetExtendedLoopAt(info, RotateDirection(reverseDirection, RotateDegree::Clockwise45));
        bool patternMatch = false;
        if (sideA->State == GridItemState::Line || sideB->State == GridItemState::Line
            || sideC->State == GridItemState::Cross || sideD->State == GridItemState::Cross)
        {
            auto tail = info;
            auto next = GetExtendedLoopAt(tail, direction, 2);
            while (true)
            {
                if (direction == Direction::RightBottom || direction == Direction::LeftBottom)
                {
                    sideA = GetExtendedLoopAt(tail, RotateDirection(direction, RotateDegree::Counterclockwise45));
                    sideB = GetExtendedLoopAt(tail, RotateDirection(direction, RotateDegree::Clockwise45));
                    sideC = GetExtendedLoopAt(next, RotateDirection(reverseDirection, RotateDegree::Counterclockwise45));
                    sideD = GetExtendedLoopAt(next, RotateDirection(reverseDirection, RotateDegree::Clockwise45));
                    if (sideA->State == GridItemState::Cross || sideB->State == GridItemState::Cross
                        || sideC->State == GridItemState::Line || sideD->State == GridItemState::Line)
                    {
                        patternMatch = true;
                        break;
                    }
                }
                tail = next;
                if (tail->Degree == 3)
                {
                    patternMatch = true;
                    break;
                }
                else if (tail->Degree != 2)
                {
                    break;
                }
                next = GetExtendedLoopAt(next, direction, 2);
            }
        }
        if (patternMatch)
        {
            RuleSetCornerDifferentState(head, direction);
        }
    }
}


void Solver::RuleCheckThree(GridItemInfo^ info)
{
    for (auto direction : { Direction::LeftTop, Direction::RightTop, Direction::RightBottom, Direction::LeftBottom })
    {
        auto next = GetExtendedLoopAt(info, direction, 2);
        auto reverseDirection = GetReverseDirection(direction);
        auto sideA = GetExtendedLoopAt(next, RotateDirection(reverseDirection, RotateDegree::Counterclockwise45));
        auto sideB = GetExtendedLoopAt(next, RotateDirection(reverseDirection, RotateDegree::Clockwise45));
        if (sideA->State == GridItemState::Line || sideB->State == GridItemState::Line)
        {
            RuleSetCornerSameState(info, reverseDirection, GridItemState::Line);
            RuleSetCornerDifferentState(info, direction);
        }
    }
}


void Solver::RuleCheckUnknown(GridItemInfo^ info)
{

}


void Solver::RuleCornerHaveSameState(GridItemInfo^ info, Direction direction, GridItemState state)
{
    auto next = GetExtendedLoopAt(info, direction, 2);
    if (next->IsExtended)
    {
        RuleSetCornerSameState(next, GetReverseDirection(direction), GridItemState::None, false);
        return;
    }

    if (next->Degree == 2)
    {
        RuleCornerHaveDifferentState(next, RotateDirection(direction, RotateDegree::Counterclockwise90));
        RuleCornerHaveDifferentState(next, RotateDirection(direction, RotateDegree::Clockwise90));
    }

    if (state == GridItemState::Line)
    {
        RuleSetCornerSameState(next, GetReverseDirection(direction), GridItemState::Cross, false);

        if (next->Degree == 1)
        {
            RuleSetCornerDifferentState(next, direction);
        }
        else if (next->Degree == 2)
        {
            next->SolverState = SolverGridItemState::Completed;
            RuleSetCornerSameState(next, direction, GridItemState::Line);
        }
    }
    else if (state == GridItemState::Cross)
    {
        if (next->Degree == 1)
        {
            RuleSetCornerSameState(next, GetReverseDirection(direction), GridItemState::Cross, false);
            RuleSetCornerDifferentState(next, direction);
        }
        else if (next->Degree == 2)
        {
            RuleSetCornerSameState(next, GetReverseDirection(direction), GridItemState::None, false);
            RuleSetCornerSameState(next, direction, GridItemState::None);
        }
        else if (next->Degree == 3)
        {
            RuleSetCornerSameState(next, GetReverseDirection(direction), GridItemState::Line, false);
            RuleSetCornerDifferentState(next, direction);
        }
        else
        {
            RuleSetCornerSameState(next, GetReverseDirection(direction), GridItemState::None, false);
        }
    }
    else if(state == GridItemState::None)
    {
        if (next->Degree == 1)
        {
            RuleSetCornerSameState(next, GetReverseDirection(direction), GridItemState::Cross, false);
            RuleSetCornerDifferentState(next, direction);
        }
        else if (next->Degree == 2)
        {
            RuleSetCornerSameState(next, GetReverseDirection(direction), GridItemState::None, false);
            RuleSetCornerSameState(next, direction, GridItemState::None);
        }
        else if (next->Degree == 3)
        {
            RuleSetCornerSameState(next, GetReverseDirection(direction), GridItemState::Line, false);
            RuleSetCornerDifferentState(next, direction);
        }
        else
        {
            RuleSetCornerSameState(next, GetReverseDirection(direction), GridItemState::None, false);
        }
    }
}


void Solver::RuleCornerHaveDifferentState(GridItemInfo^ info, Direction direction)
{
    auto next = GetExtendedLoopAt(info, direction, 2);
    RuleSetCornerDifferentState(next, GetReverseDirection(direction), false);
    if (next->IsExtended)
    {
        return;
    }

    if (next->Degree == 1)
    {
        RuleSetCornerSameState(next, direction, GridItemState::Cross);
    }
    else if (next->Degree == 2)
    {
        RuleSetCornerDifferentState(next, direction);
    }
    else if(next->Degree == 3)
    {
        RuleSetCornerSameState(next, direction, GridItemState::Line);
    }
}


void Solver::RuleSetCornerSameState(GridItemInfo^ info, Direction direction, GridItemState state, bool recursive)
{
    auto sideA = GetExtendedLoopAt(info, RotateDirection(direction, RotateDegree::Counterclockwise45));
    auto sideB = GetExtendedLoopAt(info, RotateDirection(direction, RotateDegree::Clockwise45));
    if (state != GridItemState::None)
    {
        SetState(sideA, state);
        SetState(sideB, state);
    }
    else
    {
        SetState(sideA, sideB->State);
        SetState(sideB, sideA->State);
    }
    if (recursive)
    {
        RuleCornerHaveSameState(info, direction, state);
    }
}


void Solver::RuleSetCornerDifferentState(GridItemInfo^ info, Direction direction, bool recursive)
{
    auto sideA = GetExtendedLoopAt(info, RotateDirection(direction, RotateDegree::Counterclockwise45));
    auto sideB = GetExtendedLoopAt(info, RotateDirection(direction, RotateDegree::Clockwise45));
    SetState(sideA, GetReverseState(sideB->State));
    SetState(sideB, GetReverseState(sideA->State));
    if (recursive)
    {
        RuleCornerHaveDifferentState(info, direction);
    }
}


Direction Solver::RotateDirection(Direction direction, RotateDegree rotateDegree)
{
    return (Direction)(((int)direction + (int)rotateDegree) % (int)Direction::Count);
}


Direction Solver::GetReverseDirection(Direction direction)
{
    return RotateDirection(direction, RotateDegree::Clockwise180);
}


void Solver::RuleCycleTest()
{
    auto handledDotSet = ref new Vector<GridItemInfo^>();
    unsigned int i = 0;
    while (i < mDotSet->Size)
    {
        auto head = mDotSet->GetAt(i);
        if (head->Degree == 2)
        {
            for (auto side :
                {
                    GetLeft(head),
                    GetTop(head),
                    GetRight(head),
                    GetBottom(head)
                })
            {
                if (side->State == GridItemState::None)
                {
                    SetCross(side);
                }
            }
            mDotSet->RemoveAt(i);
            continue;
        }
        if (!head->Handled)
        {
            auto tail = head;
            do
            {
                for (auto direction :
                    {
                        Direction::Left,
                        Direction::Top,
                        Direction::Right,
                        Direction::Bottom
                    })
                {
                    if (GetExtendedLoopAt(tail, direction)->State == GridItemState::Line)
                    {
                        auto next = GetExtendedLoopAt(tail, direction, 2);
                        if (!next->Handled)
                        {
                            tail->Handled = true;
                            handledDotSet->Append(tail);
                            tail = next;
                            break;
                        }
                    }
                }
            } while (tail->Degree == 2);
            for (auto direction :
                {
                    Direction::Left,
                    Direction::Top,
                    Direction::Right,
                    Direction::Bottom
                })
            {
                if (tail == GetExtendedLoopAt(head, direction, 2))
                {
                    SetCross(GetExtendedLoopAt(head, direction));
                    break;
                }
            }
            tail->Handled = true;
            handledDotSet->Append(tail);
        }
        i++;
    }
    for (auto dot : handledDotSet)
    {
        dot->Handled = false;
    }
}


void Solver::RuleCycleTestForThree()
{
    auto handledDotSet = ref new Vector<GridItemInfo^>();
    unsigned int i = 0;
    while (i < mGridThree->Size)
    {
        auto info = mGridThree->GetAt(i);
        if (info->SolverState == SolverGridItemState::Completed)
        {
            mGridThree->RemoveAt(i);
            continue;
        }
        for (auto direction : { Direction::LeftTop, Direction::RightTop, Direction::RightBottom, Direction::LeftBottom })
        {
            auto sideA = GetExtendedLoopAt(info, RotateDirection(direction, RotateDegree::Counterclockwise45));
            auto sideB = GetExtendedLoopAt(info, RotateDirection(direction, RotateDegree::Clockwise45));
            if (sideA->State != GridItemState::Line || sideB->State != GridItemState::Line)
            {
                continue;
            }
            auto reverseDirection = RotateDirection(direction, RotateDegree::Clockwise180);
            auto head = GetExtendedLoopAt(info, reverseDirection);
            if (head->Degree != 1)
            {
                continue;
            }

            auto tail = head;
            do
            {
                for (auto direction :
                    {
                        Direction::Left,
                        Direction::Top,
                        Direction::Right,
                        Direction::Bottom
                    })
                {
                    if (GetExtendedLoopAt(tail, direction)->State == GridItemState::Line)
                    {
                        auto next = GetExtendedLoopAt(tail, direction, 2);
                        if (!next->Handled)
                        {
                            tail->Handled = true;
                            handledDotSet->Append(tail);
                            tail = next;
                            break;
                        }
                    }
                }
            } while (tail->Degree == 2);

            auto dotA = GetExtendedLoopAt(info, RotateDirection(direction, RotateDegree::Counterclockwise90));
            auto dotB = GetExtendedLoopAt(info, RotateDirection(direction, RotateDegree::Clockwise90));

            for (auto direction :
                {
                    Direction::Left,
                    Direction::Top,
                    Direction::Right,
                    Direction::Bottom
                })
            {
                auto dot = GetExtendedLoopAt(tail, direction, 2);
                if (dot == dotA || dot == dotB)
                {
                    SetCross(GetExtendedLoopAt(tail, direction));
                    break;
                }
            }
            tail->Handled = true;
            handledDotSet->Append(tail);
        }
        for (auto dot : handledDotSet)
        {
            dot->Handled = false;
        }
        i++;
    }
}


void Solver::RuleColorTest()
{
    for (auto cell : mLeftBorderCellSet)
    {
        switch (GetLeft(cell)->State)
        {
        case GridItemState::Line:
            SetCellStateRecursive(cell, GridItemState::InSide);
            break;
        case GridItemState::Cross:
            SetCellStateRecursive(cell, GridItemState::OutSide);
            break;
        }
    }
    for (auto cell : mTopBorderCellSet)
    {
        switch (GetTop(cell)->State)
        {
        case GridItemState::Line:
            SetCellStateRecursive(cell, GridItemState::InSide);
            break;
        case GridItemState::Cross:
            SetCellStateRecursive(cell, GridItemState::OutSide);
            break;
        }
    }
    for (auto cell : mRightBorderCellSet)
    {
        switch (GetRight(cell)->State)
        {
        case GridItemState::Line:
            SetCellStateRecursive(cell, GridItemState::InSide);
            break;
        case GridItemState::Cross:
            SetCellStateRecursive(cell, GridItemState::OutSide);
            break;
        }
    }
    for (auto cell : mBottomBorderCellSet)
    {
        switch (GetBottom(cell)->State)
        {
        case GridItemState::Line:
            SetCellStateRecursive(cell, GridItemState::InSide);
            break;
        case GridItemState::Cross:
            SetCellStateRecursive(cell, GridItemState::OutSide);
            break;
        }
    }
    ClearRecursiveFlag();

    for (int i = mRowStart + 1; i < mRowEnd; i += 2)
    {
        for (int j = mColStart + 1; j < mColEnd; j += 2)
        {
            auto cell = GetExtendedLoopAt(i, j);
            for (auto direction : { Direction::Left, Direction::Top, Direction::Right, Direction::Bottom })
            {
                if (direction == Direction::Right && j != mColEnd - 1)
                {
                    continue;
                }
                if (direction == Direction::Bottom && i != mRowEnd - 1)
                {
                    continue;
                }

                auto side = GetExtendedLoopAt(cell, direction);
                auto next = GetExtendedLoopAt(cell, direction, 2);
                if (side->State == GridItemState::None
                    && cell->State != GridItemState::None
                    && next->State != GridItemState::None)
                {
                    if (cell->State == next->State)
                    {
                        SetCross(side);
                    }
                    else
                    {
                        SetLine(side);
                    }
                }
            }
        }
    }

    unsigned int i = 0;
    auto insideDirectionSet = ref new Vector<Direction>();
    auto outsideDirectionSet = ref new Vector<Direction>();
    while (i < mGridThree->Size)
    {
        auto info = mGridThree->GetAt(i);
        if (info->SolverState == SolverGridItemState::Completed)
        {
            mGridThree->RemoveAt(i);
            continue;
        }
        for (auto direction : { Direction::Left, Direction::Top, Direction::Right, Direction::Bottom })
        {
            switch (GetExtendedLoopAt(info, direction, 2)->State)
            {
            case GridItemState::InSide:
                insideDirectionSet->Append(direction);
                break;
            case GridItemState::OutSide:
                outsideDirectionSet->Append(direction);
                break;
            }
        }
        if (insideDirectionSet->Size > 1)
        {
            for (auto insideDirection : insideDirectionSet)
            {
                SetLine(GetExtendedLoopAt(info, insideDirection));
            }
            if (outsideDirectionSet->Size == 1)
            {
                SetCross(GetExtendedLoopAt(info, outsideDirectionSet->GetAt(0)));
            }
        }
        else if (outsideDirectionSet->Size > 1)
        {
            for (auto outsideDirection : outsideDirectionSet)
            {
                SetLine(GetExtendedLoopAt(info, outsideDirection));
            }
            if (insideDirectionSet->Size == 1)
            {
                SetCross(GetExtendedLoopAt(info, insideDirectionSet->GetAt(0)));
            }
        }
        insideDirectionSet->Clear();
        outsideDirectionSet->Clear();
        i++;
    }
    i = 0;
    while (i < mGridOne->Size)
    {
        auto info = mGridOne->GetAt(i);
        if (info->SolverState == SolverGridItemState::Completed)
        {
            mGridOne->RemoveAt(i);
            continue;
        }
        for (auto direction : { Direction::Left, Direction::Top, Direction::Right, Direction::Bottom })
        {
            switch (GetExtendedLoopAt(info, direction, 2)->State)
            {
            case GridItemState::InSide:
                insideDirectionSet->Append(direction);
                break;
            case GridItemState::OutSide:
                outsideDirectionSet->Append(direction);
                break;
            }
        }
        if (insideDirectionSet->Size > 1)
        {
            for (auto insideDirection : insideDirectionSet)
            {
                SetCross(GetExtendedLoopAt(info, insideDirection));
            }
            if (outsideDirectionSet->Size == 1)
            {
                SetLine(GetExtendedLoopAt(info, outsideDirectionSet->GetAt(0)));
            }
        }
        else if (outsideDirectionSet->Size > 1)
        {
            for (auto outsideDirection : outsideDirectionSet)
            {
                SetCross(GetExtendedLoopAt(info, outsideDirection));
            }
            if (insideDirectionSet->Size == 1)
            {
                SetLine(GetExtendedLoopAt(info, insideDirectionSet->GetAt(0)));
            }
        }
        insideDirectionSet->Clear();
        outsideDirectionSet->Clear();
        i++;
    }
}


void Solver::ClearRecursiveFlag()
{
    for (int i = mRowStart + 1; i < mRowEnd; i += 2)
    {
        for (int j = mColStart + 1; j < mColEnd; j += 2)
        {
            auto info = GetExtendedLoopAt(i, j);
            info->RecursiveFlag = false;
        }
    }
}


void Solver::SetCellStateRecursive(GridItemInfo^ info, GridItemState state)
{
    if (info->RecursiveFlag)
    {
        return;
    }
    auto stack = ref new Vector<GridItemInfo^>();
    SetState(info, state);
    stack->Append(info);
    while (stack->Size > 0)
    {
        auto cell = stack->GetAt(0);
        cell->RecursiveFlag = true;
        stack->RemoveAt(0);

        for (auto direction : { Direction::Bottom, Direction::Right, Direction::Top, Direction::Left })
        {
            auto nextCell = GetExtendedLoopAt(cell, direction, 2);
            if (!nextCell->RecursiveFlag && !nextCell->IsExtended)
            {
                auto side = GetExtendedLoopAt(cell, direction);
                if (side->State == GridItemState::Cross)
                {
                    SetState(nextCell, cell->State);
                    stack->InsertAt(0, nextCell);
                }
                else if (side->State == GridItemState::Line)
                {
                    SetState(nextCell, GetReverseState(cell->State));
                    stack->InsertAt(0, nextCell);
                }
            }
        }
    }
}
