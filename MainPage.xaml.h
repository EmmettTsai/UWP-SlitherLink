//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "LoopSize.h"
#include "IndicatorState.h"
#include "GridItemInfo.h"

namespace SlitherLink
{
    public enum class StateSlot : int
    {
        Default,
        SlotA,
        SlotB,
    };

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public ref class MainPage sealed
    {
    public:
        MainPage();

    private:
        StateSlot mStateSlot;
        Windows::Foundation::Collections::IVector<byte>^ mLoop;
        Windows::Foundation::Collections::IVector<GridItemInfo^>^ mExtendedLoop;
        Windows::Foundation::Collections::IVector<Platform::String^>^ mRule;
        IndicatorState mIndicatorState;
        bool mEnableSetCell;
        bool mEnableSetSide;

        Windows::UI::Xaml::Media::SolidColorBrush^ mTransparentColor;
        Windows::UI::Xaml::Media::SolidColorBrush^ mInsideMarkColor;
        Windows::UI::Xaml::Media::SolidColorBrush^ mOutsideMarkColor;
        Windows::UI::Xaml::Media::SolidColorBrush^ mLineMarkColor;
        Windows::UI::Xaml::Media::SolidColorBrush^ mCrossMarkColor;
        Windows::UI::Xaml::Media::SolidColorBrush^ mCurrentLeftMarkCellColor;
        Windows::UI::Xaml::Media::SolidColorBrush^ mCurrentRightMarkCellColor;

        Platform::String^ mUrl;
        Windows::Web::Http::Filters::HttpBaseProtocolFilter^ mHttpFilter;
        Windows::Web::Http::HttpClient^ mHttpClient;

        Platform::String^ mPuzzleInfo;
        int mLoopRowSize;
        int mLoopColSize;
        int mRowSize;
        int mColSize;
        int mRowStart;
        int mRowEnd;
        int mColStart;
        int mColEnd;
        int mExtendedRowSize;
        int mExtendedColSize;
        Platform::String^ mParsedResult;

        void InitView();
        void Init(int row, int col);
        void InitHttpClient();

        void Grid_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void Grid_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void Grid_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);

        Windows::UI::Xaml::Media::Geometry^ PathMarkupToGeometry(Platform::String^ pathMarkup);

        void SetInside(Windows::UI::Xaml::Controls::Border^ item, bool force = false);
        void SetOutside(Windows::UI::Xaml::Controls::Border^ item, bool force = false);
        void SetLine(Windows::UI::Xaml::Controls::Border^ item, bool force = false);
        void SetCross(Windows::UI::Xaml::Controls::Border^ item, bool force = false);
        void SetErase(Windows::UI::Xaml::Controls::Border^ item, bool force = false);

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

        Concurrency::task<bool> ReadTextFile(Windows::Storage::StorageFile^ file);
        Concurrency::task<bool> ReadHtmlFile(Windows::Storage::StorageFile^ file);

        bool ParseHtmlText(Platform::String^ content);
        LoopSize getLoopSize(int index);

        inline byte GetLoopAt(int i, int j);
        inline GridItemInfo^ GetExtendedLoopAt(int i, int j);

        Windows::UI::Xaml::Controls::RadioButton^ mMainShaderPair;
        Windows::UI::Xaml::Controls::RadioButton^ CreateShaderPair(Windows::UI::Color left, Windows::UI::Color right);
        Windows::UI::Xaml::Controls::RadioButton^ CreateShaderPair(Windows::UI::Xaml::Media::SolidColorBrush^ left, Windows::UI::Xaml::Media::SolidColorBrush^ right);
        void ReplaceColor(Windows::UI::Color oldColor, Windows::UI::Color newColor);
        void RadioButton_OnChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        Platform::Object^ mDragObject;
        void Rectangle_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
        void Rectangle_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
        void Rectangle_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args);
        void LockButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UnlockButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResetToLockedButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResetGameButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StateSlot_Default_Button_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StateSlot_A_Button_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StateSlot_B_Button_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UpdateStateSlot(GridItemInfo^ info, GridItemState state);
        void MergeSlotButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ClearSlotA_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ClearSlotB_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SaveFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResetScaleButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ScaleSlider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
    };
}
