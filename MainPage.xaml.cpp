//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace SlitherLink;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Shapes;

using namespace Windows::Devices::Input;
using namespace Windows::UI::Input;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();

    int row = 50;
    int col = 40;
    int dotSize = 10;
    double halfDotSize = dotSize * 0.5;
    int cellSize = 30;
    mIndicatorState = IndicatorState::None;
    mEnableSetCell = false;
    mEnableSetSide = true;
    SideToggleButton->IsChecked = true;
    CellToggleButton->IsChecked = false;

    Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->SetDesiredBoundsMode(Windows::UI::ViewManagement::ApplicationViewBoundsMode::UseCoreWindow);
    return;
    /*
     * 2*row+1 x 2*col+1
     * dot:   (2*i, 2*j)
     * cell:  (2*(i+1), 2*(j+1))
     * hLine: (2*i, 2*(j+1))
     * vLine: (2*(i+1), 2*j)
     *
     **/

    //ColumnDefinition^ colDef = ref new ColumnDefinition();
    //rowDef->Height = GridLength(30);
    //rowDef->Height = GridLength::Auto;

    //ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &SlitherLink::MainPage::OnPointerEntered);
    //m_PointerEntered = ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &SlitherLink::MainPage::OnTapped);
    Grid^ grid = ref new Grid();
    for (int i = 0; i < 2 * row + 1; i++)
    {
        RowDefinition^ rowDef = ref new RowDefinition();
        rowDef->Height = GridLength::Auto;
        grid->RowDefinitions->Append(rowDef);
        for (int j = 0; j < 2 * col + 1; j++)
        {
            ColumnDefinition^ colDef = ref new ColumnDefinition();
            colDef->Width = GridLength::Auto;
            grid->ColumnDefinitions->Append(colDef);

            //Rectangle^ item = ref new Rectangle();
            Border^ item = ref new Border();
            //Canvas^ canvas = ref new Canvas();
            item->PointerEntered += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &SlitherLink::MainPage::Grid_PointerEntered);
            item->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &SlitherLink::MainPage::Grid_PointerPressed);
            item->PointerReleased += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &SlitherLink::MainPage::Grid_PointerReleased);

            item->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &SlitherLink::MainPage::Grid_Tapped);
            item->RightTapped += ref new Windows::UI::Xaml::Input::RightTappedEventHandler(this, &SlitherLink::MainPage::Grid_RightTapped);


            GridItemInfo^ info = ref new GridItemInfo();
            info->Row = i;
            info->Column = j;
            info->State = GridItemState::None;

            if (i % 2 == 0 && j % 2 == 0)
            {
                info->Type = GridItemType::Dot;
                item->Width = dotSize;
                item->Height = dotSize;
                item->Background = ref new SolidColorBrush(Colors::Black);
            }
            else if (i % 2 == 1 && j % 2 == 1)
            {
                info->Type = GridItemType::Cell;
                item->Width = cellSize;
                item->Height = cellSize;
                //item->Margin = Windows::UI::Xaml::Thickness(-halfDotSize, -halfDotSize, -halfDotSize, -halfDotSize);
                //item->Padding = Windows::UI::Xaml::Thickness(halfDotSize, halfDotSize, halfDotSize, halfDotSize);
                item->Background = ref new SolidColorBrush(Colors::Transparent);

                TextBlock^ text = ref new TextBlock();
                text->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
                text->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
                text->Text = ((i + j) % 4).ToString();

                item->Child = text;
            }
            else if (i % 2 == 0)
            {
                info->Type = GridItemType::HorizontailLine;
                item->Width = cellSize;
                item->Height = dotSize;
                item->Background = ref new SolidColorBrush(Colors::Transparent);
            }
            else
            {
                info->Type = GridItemType::VerticalLine;
                item->Width = dotSize;
                item->Height = cellSize;
                item->Background = ref new SolidColorBrush(Colors::Transparent);
            }
            item->Tag = info;

            grid->Children->Append(item);
            grid->SetRow(item, i);
            grid->SetColumn(item, j);
        }
    }
    RootGrid->Children->Append(grid);
}


void MainPage::Grid_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    myLogW(LOG_VERBOSE, LTAG L"[%d][%s]", __LINE__, __funcw__);
}


void MainPage::Grid_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    myLogW(LOG_VERBOSE, LTAG L"[%d][%s]", __LINE__, __funcw__);
}


void MainPage::Grid_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    myLogW(LOG_VERBOSE, LTAG L"[%d][%s]", __LINE__, __funcw__);
    Pointer^ pointer = e->Pointer;
    if (pointer->PointerDeviceType == PointerDeviceType::Mouse)
    {
        auto item = (Border^)sender;
        auto info = (GridItemInfo^)item->Tag;
        PointerPoint^ pointerPoint = e->GetCurrentPoint(item);
        bool leftPress = pointerPoint->Properties->IsLeftButtonPressed;
        bool rightPress = pointerPoint->Properties->IsRightButtonPressed;


        myLogW(LOG_DEBUG, LTAG L"[%d][%s] leftPress = %d", __LINE__, __funcw__, leftPress);
        myLogW(LOG_DEBUG, LTAG L"[%d][%s] rightPress = %d", __LINE__, __funcw__, rightPress);

        if ((leftPress && rightPress) || !(leftPress || rightPress))
        {
            mIndicatorState = IndicatorState::None;
            return;
        }

        if (mIndicatorState == IndicatorState::None)
        {
            if (info->State == GridItemState::None)
            {
                mIndicatorState = leftPress ? IndicatorState::LeftSet : IndicatorState::RightSet;
            }
            else
            {
                mIndicatorState = IndicatorState::Erase;
            }
        }

        switch (info->Type)
        {
        case GridItemType::Cell:
            SetCell(item, mIndicatorState);
            break;
        case GridItemType::HorizontailLine:
        case GridItemType::VerticalLine:
            SetSide(item, mIndicatorState);
            break;
        }
    }
}


void MainPage::Grid_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
    myLogW(LOG_VERBOSE, LTAG L"[%d][%s]", __LINE__, __funcw__);
    Update(sender, true);
}


void MainPage::Grid_RightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e)
{
    myLogW(LOG_VERBOSE, LTAG L"[%d][%s]", __LINE__, __funcw__);
    Update(sender, false);
}


void MainPage::SetInside(Windows::UI::Xaml::Controls::Border^ item)
{
    if (!mEnableSetCell)
    {
        return;
    }
    auto info = (GridItemInfo^)item->Tag;
    info->State = GridItemState::InSide;
    item->Background = ref new SolidColorBrush(Colors::Green);
}


void MainPage::SetOutside(Windows::UI::Xaml::Controls::Border^ item)
{
    if (!mEnableSetCell)
    {
        return;
    }
    auto info = (GridItemInfo^)item->Tag;
    info->State = GridItemState::OutSide;
    item->Background = ref new SolidColorBrush(Colors::SkyBlue);
}


void MainPage::SetLine(Windows::UI::Xaml::Controls::Border^ item)
{
    if (!mEnableSetSide)
    {
        return;
    }
    auto info = (GridItemInfo^)item->Tag;
    info->State = GridItemState::Line;
    item->Background = ref new SolidColorBrush(Colors::Black);
}


void MainPage::SetCross(Windows::UI::Xaml::Controls::Border^ item)
{
    if (!mEnableSetSide)
    {
        return;
    }
    auto info = (GridItemInfo^)item->Tag;
    info->State = GridItemState::Cross;
    item->Background = ref new SolidColorBrush(Colors::Red);
}


void MainPage::SetErase(Windows::UI::Xaml::Controls::Border^ item)
{
    auto info = (GridItemInfo^)item->Tag;
    switch (info->Type)
    {
    case GridItemType::Cell:
        if (!mEnableSetCell)
        {
            return;
        }
        break;
    case GridItemType::HorizontailLine:
    case GridItemType::VerticalLine:
        if (!mEnableSetSide)
        {
            return;
        }
        break;
    }
    info->State = GridItemState::None;
    item->Background = ref new SolidColorBrush(Colors::Transparent);
}


void MainPage::SetCell(Windows::UI::Xaml::Controls::Border^ item, IndicatorState state)
{
    switch (state)
    {
    case IndicatorState::LeftSet:
        SetInside(item);
        break;
    case IndicatorState::RightSet:
        SetOutside(item);
        break;
    case IndicatorState::Erase:
        SetErase(item);
        break;
    }
}


void MainPage::SetSide(Windows::UI::Xaml::Controls::Border^ item, IndicatorState state)
{
    switch (state)
    {
    case IndicatorState::LeftSet:
        SetLine(item);
        break;
    case IndicatorState::RightSet:
        SetCross(item);
        break;
    case IndicatorState::Erase:
        SetErase(item);
        break;
    }
}


void MainPage::Update(Object^ sender, bool isLeft)
{
    Border^ item = (Border^)sender;
    GridItemInfo^ info = (GridItemInfo^)(item->Tag);
    switch (info->Type)
    {
    case GridItemType::Cell:
        UpdateCell(item, isLeft);
        break;
    case GridItemType::HorizontailLine:
    case GridItemType::VerticalLine:
        UpdateSide(item, isLeft);
        break;
    }
}


void MainPage::UpdateCell(Border^ item, bool isLeft)
{
    GridItemInfo^ info = (GridItemInfo^)(item->Tag);
    if (isLeft)
    {
        switch (info->State)
        {
        case GridItemState::InSide:
            SetErase(item);
            break;
        case GridItemState::None:
        default:
            SetInside(item);
            break;
        }
    }
    else
    {
        switch (info->State)
        {
        case GridItemState::OutSide:
            SetErase(item);
            break;
        case GridItemState::None:
        default:
            SetOutside(item);
            break;
        }
    }
}


void MainPage::UpdateSide(Border^ item, bool isLeft)
{
    GridItemInfo^ info = (GridItemInfo^)(item->Tag);
    if (isLeft)
    {
        switch (info->State)
        {
        case GridItemState::Line:
            SetErase(item);
            break;
        case GridItemState::None:
        default:
            SetLine(item);
            break;
        }
    }
    else
    {
        switch (info->State)
        {
        case GridItemState::Cross:
            SetErase(item);
            break;
        case GridItemState::None:
        default:
            SetCross(item);
            break;
        }
    }
}


void SlitherLink::MainPage::SideToggleButton_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    mEnableSetSide = true;
}


void SlitherLink::MainPage::SideToggleButton_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    mEnableSetSide = false;
}


void SlitherLink::MainPage::CellToggleButton_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    mEnableSetCell = true;
}


void SlitherLink::MainPage::CellToggleButton_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    mEnableSetCell = false;
}
