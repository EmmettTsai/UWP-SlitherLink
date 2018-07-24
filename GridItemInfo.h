#pragma once

#include "GridItemType.h"
#include "GridItemState.h"

namespace SlitherLink
{
    public ref class GridItemInfo sealed
    {
    public:
        property GridItemType Type
        {
            GridItemType get()
            {
                return m_type;
            }
            void set(GridItemType type)
            {
                m_type = type;
            }
        }
        property GridItemState State
        {
            GridItemState get()
            {
                return m_state;
            }
            void set(GridItemState state)
            {
                m_state = state;
            }
        }
        property int Row
        {
            int get()
            {
                return m_row;
            }
            void set(int row)
            {
                m_row = row;
            }
        }
        property int Column
        {
            int get()
            {
                return m_column;
            }
            void set(int column)
            {
                m_column = column;
            }
        }
        property int Degree
        {
            int get()
            {
                return m_degree;
            }
            void set(int degree)
            {
                m_degree = degree;
            }
        }
        property bool Handled
        {
            bool get()
            {
                return m_handled;
            }
            void set(bool handled)
            {
                m_handled = handled;
            }
        }
        property Windows::UI::Xaml::Controls::Border^ View
        {
            Windows::UI::Xaml::Controls::Border^ get()
            {
                return m_border;
            }
            void set(Windows::UI::Xaml::Controls::Border^ border)
            {
                m_border = border;
            }
        }
        property bool IsExtended
        {
            bool get()
            {
                return m_isExtended;
            }
            void set(bool isExtended)
            {
                m_isExtended = isExtended;
            }
        }
    private:
        GridItemType m_type;
        GridItemState m_state;
        Windows::UI::Xaml::Controls::Border^ m_border;
        int m_row;
        int m_column;
        int m_degree;
        bool m_handled;
        bool m_isExtended;
    };

}