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
    private:
        GridItemType m_type;
        GridItemState m_state;
        int m_row;
        int m_column;
        int m_degree;
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
        Windows::Foundation::Collections::IVector<Windows::Foundation::Collections::IVector<byte>^>^ mMap;
        IndicatorState mIndicatorState;
        bool mEnableSetCell;
        bool mEnableSetSide;

        Platform::String^ mUrl;
        Windows::Web::Http::Filters::HttpBaseProtocolFilter^ mHttpFilter;
        Windows::Web::Http::HttpClient^ mHttpClient;

        int mRow;
        int mCol;

        void Init(int row, int col);

        void Grid_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void Grid_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void Grid_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void Grid_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
        void Grid_RightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e);

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
        Concurrency::task<void> ReadHtmlFile(Windows::Storage::StorageFile^ file);

        std::wstring StringToWstring(const std::string& str);
        std::string WstringToString(const std::wstring & wstr);
        std::vector<std::string> SplitString(std::string strtem, char a);
        std::vector<std::wstring> SplitWString(std::wstring strtem, wchar_t a);
        
        Platform::String^ mHtmlContent;
        std::string mHtmlContentStdStr;
        std::wstring mHtmlContentStdWStr;
        void SearchButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
