#pragma once

#include "GridItemInfo.h"

namespace SlitherLink
{
    public enum class Result : int
    {
        Invalid,
        Solved,
    };

    public enum class Direction : int
    {
        LeftTop,
        Top,
        RightTop,
        Right,
        RightBottom,
        Bottom,
        LeftBottom,
        Left,
        Count
    };

    public enum class RotateDegree : int
    {
        Counterclockwise360,
        Counterclockwise315,
        Counterclockwise270,
        Counterclockwise225,
        Counterclockwise180,
        Counterclockwise135,
        Counterclockwise90,
        Counterclockwise45,
        Zero,
        Clockwise45,
        Clockwise90,
        Clockwise135,
        Clockwise180,
        Clockwise225,
        Clockwise270,
        Clockwise315,
    };

    public ref class Solver sealed
    {
    public:
        Solver(int row, int col, Platform::String^ data);
        Platform::String^ Solve();
    private:
        int mLoopRowSize;
        int mLoopColSize;
        int mRowSize;
        int mColSize;
        int mRowStart;
        int mRowEnd;
        int mColStart;
        int mColEnd;
        int mUpdated;
        Platform::String^ mData;

        int mExtendedRowSize;
        int mExtendedColSize;
        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mExtendedLoop;

        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mGridZero;
        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mGridOne;
        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mGridTwo;
        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mGridThree;
        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mGridUnknown;

        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mQueue;
        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mDotSet;
        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mLeftBorderCellSet;
        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mTopBorderCellSet;
        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mRightBorderCellSet;
        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mBottomBorderCellSet;

        inline int Solver::GetDataAt(int i, int j);
        inline GridItemInfo^ GetExtendedLoopAt(int i, int j);
        inline GridItemInfo^ GetExtendedLoopAt(GridItemInfo^ info, int i, int j);
        inline GridItemInfo^ GetExtendedLoopAt(GridItemInfo^ info, Direction direction, int scale = 1);
        void InitExtendedLoop();

        Platform::String^ GetResult();

        bool SetLine(GridItemInfo^ info);
        bool SetCross(GridItemInfo^ info);
        bool SetInside(GridItemInfo^ info);
        bool SetOutside(GridItemInfo^ info);
        bool SetState(GridItemInfo^ info, GridItemState state);

        GridItemInfo^ GetLeftTop(GridItemInfo^ info);
        GridItemInfo^ GetTop(GridItemInfo^ info);
        GridItemInfo^ GetRightTop(GridItemInfo^ info);
        GridItemInfo^ GetRight(GridItemInfo^ info);
        GridItemInfo^ GetRightBottom(GridItemInfo^ info);
        GridItemInfo^ GetBottom(GridItemInfo^ info);
        GridItemInfo^ GetLeftBottom(GridItemInfo^ info);
        GridItemInfo^ GetLeft(GridItemInfo^ info);

        Direction GetReverseDirection(Direction direction);
        Direction RotateDirection(Direction direction, RotateDegree rotateDegree);
        GridItemState GetReverseState(GridItemState state);

        void ClearRecursiveFlag();
        void SetCellStateRecursive(GridItemInfo^ info, GridItemState state);

        void UpdateQueue(GridItemInfo^ info);
        void RuleZero();
        void RuleThreeThree();
        void RuleCheckSelf(GridItemInfo^ info);
        void RuleCheckCell(GridItemInfo^ info);
        void RuleCheckOne(GridItemInfo^ info);
        void RuleCheckTwo(GridItemInfo^ info);
        void RuleCheckThree(GridItemInfo^ info);
        void RuleCheckUnknown(GridItemInfo^ info);
        void RuleCornerHaveSameState(GridItemInfo^ info, Direction direction, GridItemState state);
        void RuleCornerHaveDifferentState(GridItemInfo^ info, Direction direction);
        void RuleSetCornerSameState(GridItemInfo^ info, Direction direction, GridItemState state, bool recursive = true);
        void RuleSetCornerDifferentState(GridItemInfo^ info, Direction direction, bool recursive = true);
        void RuleCycleTest();
        void RuleCycleTestForThree();
        void RuleColorTest();
    };
}
