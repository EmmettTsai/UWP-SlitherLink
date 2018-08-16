//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace SlitherLink
{
    public enum class GridItemType : int
    {
        Dot,
        Cell,
        HorizontailLine,
        VerticalLine,
    };

    public enum class GridItemState : int
    {
        None,
        InSide,
        OutSide,
        Cross,
        Line,
    };

    public enum class IndicatorState : int
    {
        None,
        LeftSet,
        RightSet,
        Erase,
    };

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
    private:
        GridItemType m_type;
        GridItemState m_state;
        int m_row;
        int m_column;
        int m_degree;
        bool m_handled;
    };

    /*
    https://www.puzzle-loop.com
    https://www.puzzle-loop.com/?size=0
    5x5 Loop Normal	    	0
    5x5 Loop Hard	    	4
    7x7 Loop Normal	    	10
    7x7 Loop Hard	    	11
    10x10 Loop Normal   	1
    10x10 Loop Hard		    5
    15x15 Loop Normal   	2
    15x15 Loop Hard		    6
    20x20 Loop Normal   	3
    20x20 Loop Hard		    7
    25x30 Loop Normal   	8
    25x30 Loop Hard		    9
    Special Daily Loop	    13
    Special Weekly Loop     12
    Special Monthly Loop	14
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

	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

    private:
        // https://msdn.microsoft.com/en-us/library/ts4c4dw6.aspx
        Windows::Foundation::Collections::IVector<Windows::Foundation::Collections::IVector<byte>^>^ mLoop;
        IndicatorState mIndicatorState;
        bool mEnableSetCell;
        bool mEnableSetSide;

        Windows::UI::Xaml::Media::SolidColorBrush^ mTransparentColor;
        Windows::UI::Xaml::Media::SolidColorBrush^ mInsideMarkColor;
        Windows::UI::Xaml::Media::SolidColorBrush^ mOutsideMarkColor;
        Windows::UI::Xaml::Media::SolidColorBrush^ mLineMarkColor;
        Windows::UI::Xaml::Media::SolidColorBrush^ mCrossMarkColor;

        Platform::String^ mUrl;
        Windows::Web::Http::Filters::HttpBaseProtocolFilter^ mHttpFilter;
        Windows::Web::Http::HttpClient^ mHttpClient;

        Platform::String^ mPuzzleID;

        void Init(int row, int col);
        void InitHttpClient();

        void Grid_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void Grid_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void Grid_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);

        Windows::UI::Xaml::Media::Geometry^ PathMarkupToGeometry(Platform::String^ pathMarkup);

        void SetInside(Windows::UI::Xaml::Controls::Border^ item);
        void SetOutside(Windows::UI::Xaml::Controls::Border^ item);
        void SetLine(Windows::UI::Xaml::Controls::Border^ item);
        void SetCross(Windows::UI::Xaml::Controls::Border^ item);
        void SetErase(Windows::UI::Xaml::Controls::Border^ item);

        void SetCell(Windows::UI::Xaml::Controls::Border^ item, IndicatorState state);
        void SetSide(Windows::UI::Xaml::Controls::Border^ item, IndicatorState state);

        void Update(Platform::Object^ sender, bool isLeft);
        void UpdateCell(Windows::UI::Xaml::Controls::Border^ item, bool isLeft);
        void UpdateSide(Windows::UI::Xaml::Controls::Border^ item, bool isLeft);

        void SideToggleButton_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SideToggleButton_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void CellToggleButton_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void CellToggleButton_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void OpenFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void LoadFromUrlButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        Concurrency::task<void> ReadTextFile(Windows::Storage::StorageFile^ file);
        Concurrency::task<bool> ReadHtmlFile(Windows::Storage::StorageFile^ file);

        bool ParseHtmlText(Platform::String^ content);
        LoopSize getLoopSize(int index);
    };
}
