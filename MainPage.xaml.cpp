﻿//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include <pplawait.h>
#include "MainPage.xaml.h"

using namespace SlitherLink;

using namespace Platform;
using namespace Platform::Collections;
using namespace Concurrency;
using namespace Windows::Devices::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Markup;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Shapes;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Web::Http::Headers;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409
/*
https://developercommunity.visualstudio.com/content/problem/263131/co-await-optimization-bugs-in-visual-studio-157-x6.html
There is a hidden cl.exe switch "/d2CoroOptsWorkaround" to disable optimizations on all coroutines being compiled. I recommend only using it until you get the Preview 5 bits as this will disable all optimizations on the coroutine, resulting in worse code generation. Sometime in the future, this switch will not do anything.

https://docs.microsoft.com/en-us/windows/uwp/design/style/icons
https://docs.microsoft.com/en-us/windows/uwp/design/style/segoe-ui-symbol-font

<AppBarToggleButton Label="FontIcon" Click="AppBarButton_Click">
    <AppBarToggleButton.Icon>
        <FontIcon FontFamily="Candara" Glyph="&#x03A3;"/>
    </AppBarToggleButton.Icon>
</AppBarToggleButton>

<AppBarButton Label="BitmapIcon" Click="AppBarButton_Click">
    <AppBarButton.Icon>
        <BitmapIcon UriSource="ms-appx:///Assets/globe.png"/>
    </AppBarButton.Icon>
</AppBarButton>

<!-- App bar button with path icon. -->
<AppBarButton Label="PathIcon" Click="AppBarButton_Click">
    <AppBarButton.Icon>
        <PathIcon Data="F1 M 16,12 20,2L 20,16 1,16" HorizontalAlignment="Center"/>
    </AppBarButton.Icon>
</AppBarButton>

https://stackoverflow.com/questions/50511876/how-to-set-bitmapicon-or-pathicon-content-from-canvas-stored-in-resource-d

https://stackoverflow.com/questions/25188349/how-to-set-pathicon-from-code-behind

https://docs.microsoft.com/zh-tw/windows/uwp/networking/httpclient
*/

/*
https://www.puzzle-loop.com
https://www.puzzle-loop.com/?size=0
5x5 Loop Normal		0
5x5 Loop Hard		4
7x7 Loop Normal		10
7x7 Loop Hard		11
10x10 Loop Normal	1
10x10 Loop Hard		5
15x15 Loop Normal	2
15x15 Loop Hard		6
20x20 Loop Normal	3
20x20 Loop Hard		7
25x30 Loop Normal	8
25x30 Loop Hard		9
Special Daily Loop	13
Special Weekly Loop	12
Special Monthly Loop	14
*/

MainPage::MainPage()
{
	InitializeComponent();
    mMap = ref new Vector<IVector<byte>^>();

    mEnableSetSide = true;
    mEnableSetCell = false;

    mUrl = "https://www.puzzle-loop.com";
    mHttpFilter = ref new HttpBaseProtocolFilter();
    mHttpFilter->CacheControl->ReadBehavior = HttpCacheReadBehavior::NoCache;
    mHttpFilter->CacheControl->WriteBehavior = HttpCacheWriteBehavior::NoCache;
    mHttpClient = ref new HttpClient(mHttpFilter);
    
    
    auto defaultHeaders = mHttpClient->DefaultRequestHeaders;

    //defaultHeaders->Append("Connection", "keep-alive");

    defaultHeaders->Append("Upgrade-Insecure-Requests", "1");

    auto userAgents = ref new Vector<String^>();
    userAgents->Append("Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    userAgents->Append("AppleWebKit/537.36 (KHTML, like Gecko)");
    userAgents->Append("Chrome/67.0.3396.99 Safari/537.36");
    for (String^ userAgent : userAgents)
    {
        if (!defaultHeaders->UserAgent->TryParseAdd(userAgent))
        {
            myLogW(LOG_ERROR, LTAG L"add userAgent fail: %s", userAgent->Data());
        }
        else
        {
            myLogW(LOG_INFO, LTAG L"add userAgent success: %s", userAgent->Data());
        }
    }

    auto mediaTypes = ref new Vector<String^>();
    mediaTypes->Append("text/html");
    mediaTypes->Append("application/xhtml+xml");
    mediaTypes->Append("application/xml;q=0.9");
    mediaTypes->Append("image/webp");
    mediaTypes->Append("image/apng");
    mediaTypes->Append("*/*;q=0.8");
    for (String^ mediaType : mediaTypes)
    {
        if (!defaultHeaders->Accept->TryParseAdd(mediaType))
        {
            myLogW(LOG_ERROR, LTAG L"add mediaType fail: %s", mediaType->Data());
        }
        else
        {
            myLogW(LOG_INFO, LTAG L"add mediaType success: %s", mediaType->Data());
        }
    }

    auto encodings = ref new Vector<String^>();
    encodings->Append("gzip");
    encodings->Append("deflate");
    encodings->Append("br");
    for (String^ encoding : encodings)
    {
        if (!defaultHeaders->AcceptEncoding->TryParseAdd(encoding))
        {
            myLogW(LOG_ERROR, LTAG L"add encoding fail: %s", encoding->Data());
        }
        else
        {
            myLogW(LOG_INFO, LTAG L"add encoding success: %s", encoding->Data());
        }
    }

    auto languages = ref new Vector<String^>();
    languages->Append("zh-TW");
    languages->Append("zh;q=0.9");
    languages->Append("en-US;q=0.8");
    languages->Append("en;q=0.7");
    languages->Append("zh-CN;q=0.6");
    languages->Append("ja;q=0.5");
    for (String^ language : languages)
    {
        if (!defaultHeaders->AcceptLanguage->TryParseAdd(language))
        {
            myLogW(LOG_ERROR, LTAG L"add language fail: %s", language->Data());
        }
        else
        {
            myLogW(LOG_INFO, LTAG L"add language success: %s", language->Data());
        }
    }

    auto cookies = ref new Vector<String^>();
    cookies->Append("ab=0");
    cookies->Append("ccv=yes");
    cookies->Append("ccs=dismiss;");
    cookies->Append("zoom=1.5");
    cookies->Append("OX_plg=pm");
    cookies->Append("hideRules=0");
    for (String^ cookie : cookies)
    {
        if (!defaultHeaders->Cookie->TryParseAdd(cookie))
        {
            myLogW(LOG_ERROR, LTAG L"add cookie fail: %s", cookie->Data());
        }
        else
        {
            myLogW(LOG_INFO, LTAG L"add cookie success: %s", cookie->Data());
        }
    }

#if false
    defaultHeaders->Accept->Append(ref new HttpMediaTypeWithQualityHeaderValue("text/html"));
    defaultHeaders->Accept->Append(ref new HttpMediaTypeWithQualityHeaderValue("application/xhtml+xml"));
    defaultHeaders->Accept->Append(ref new HttpMediaTypeWithQualityHeaderValue("application/xml;q=0.9"));
    defaultHeaders->Accept->Append(ref new HttpMediaTypeWithQualityHeaderValue("image/webp"));
    defaultHeaders->Accept->Append(ref new HttpMediaTypeWithQualityHeaderValue("image/apng"));
    defaultHeaders->Accept->Append(ref new HttpMediaTypeWithQualityHeaderValue("*/*;q=0.8"));

    defaultHeaders->AcceptEncoding->Append(ref new HttpContentCodingWithQualityHeaderValue("gzip"));
    defaultHeaders->AcceptEncoding->Append(ref new HttpContentCodingWithQualityHeaderValue("deflate"));
    defaultHeaders->AcceptEncoding->Append(ref new HttpContentCodingWithQualityHeaderValue("br"));

    defaultHeaders->AcceptLanguage->Append(ref new HttpLanguageRangeWithQualityHeaderValue("zh-TW"));
#endif
}


void MainPage::Init(int row, int col)
{
    int dotSize = 10;
    double halfDotSize = dotSize * 0.5;
    int cellSize = 30;
    mIndicatorState = IndicatorState::None;

    //Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->SetDesiredBoundsMode(Windows::UI::ViewManagement::ApplicationViewBoundsMode::UseCoreWindow);

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
#if true
    RootCanvas->Width = col * cellSize + (col + 1) * dotSize;
    RootCanvas->Height = row * cellSize + (row + 1) * dotSize;

    Border^ background = ref new Border();
    background->Width = RootCanvas->Width - dotSize;
    background->Height = RootCanvas->Height - dotSize;
    background->Background = ref new SolidColorBrush(Colors::White);
    Canvas::SetZIndex(background, 0);
    Canvas::SetLeft(background, halfDotSize);
    Canvas::SetTop(background, halfDotSize);
    RootCanvas->Children->Append(background);

    for (int i = 0; i < 2 * row + 1; i++)
    {
        for (int j = 0; j < 2 * col + 1; j++)
        {
            Border^ item = ref new Border();
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
                Canvas::SetZIndex(item, 1);
                Canvas::SetLeft(item, (dotSize + cellSize) * (j / 2));
                Canvas::SetTop(item, (dotSize + cellSize) * (i / 2));
            }
            else if (i % 2 == 1 && j % 2 == 1)
            {
                int dataRow = (i - 1) / 2;
                int dataCol = (j - 1) / 2;
                info->Type = GridItemType::Cell;
                item->Width = cellSize + dotSize;
                item->Height = cellSize + dotSize;
                item->Background = ref new SolidColorBrush(Colors::Transparent);

                TextBlock^ text = ref new TextBlock();
                text->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
                text->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;

                switch (mMap->GetAt(dataRow)->GetAt(dataCol))
                {
                case '0':
                    text->Text = "0";
                    break;
                case '1':
                    text->Text = "1";
                    break;
                case '2':
                    text->Text = "2";
                    break;
                case '3':
                    text->Text = "3";
                    break;
                default:
                    text->Text = "";
                }

                item->Child = text;

                Canvas::SetZIndex(item, 0);
                Canvas::SetLeft(item, dotSize + (cellSize + dotSize) * dataCol - halfDotSize);
                Canvas::SetTop(item, dotSize + (cellSize + dotSize) * dataRow - halfDotSize);
            }
            else if (i % 2 == 0)
            {
                info->Type = GridItemType::HorizontailLine;
                item->Width = cellSize;
                item->Height = dotSize;
                item->Background = ref new SolidColorBrush(Colors::Transparent);

                Canvas::SetZIndex(item, 1);
                Canvas::SetLeft(item, dotSize + (cellSize + dotSize) * (j - 1) / 2);
                Canvas::SetTop(item, (dotSize + cellSize) * i / 2);
            }
            else
            {
                info->Type = GridItemType::VerticalLine;
                item->Width = dotSize;
                item->Height = cellSize;
                item->Background = ref new SolidColorBrush(Colors::Transparent);

                Canvas::SetZIndex(item, 1);
                Canvas::SetLeft(item, (dotSize + cellSize) * j / 2);
                Canvas::SetTop(item, dotSize + (cellSize + dotSize) * (i - 1) / 2);
            }
            item->Tag = info;
            RootCanvas->Children->Append(item);
        }
    }
#else
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
#endif
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

    FontIcon^ icon = ref new FontIcon();
    icon->FontFamily = ref new Windows::UI::Xaml::Media::FontFamily("Segoe MDL2 Assets");
    icon->Glyph = L"\xED64";
    SideToggleButton->Icon = icon;

    SideToggleButton->Label = "Enable Side Marker";
}


void SlitherLink::MainPage::SideToggleButton_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    mEnableSetSide = false;
    PathIcon^ icon = ref new PathIcon();
    String^ data = "M 36.666668,84.061299 C 31.919265,82.097643 27.844946,76.443872 26.884074,70.486382 26.367817,67.285539 21.603698,56.116668 16.297143,45.666668 c -8.5232126,-16.784446 -10.0359368,-19 -12.972712,-19 C 0.00237399,26.666668 0,26.657146 0,13.333334 0,0.88888892 0.17777778,0 2.6666668,0 5.1111112,0 5.3333335,0.88888892 5.3333335,10.666667 V 21.333334 H 42.666668 80.000003 V 10.666667 C 80.000003,0.88888892 80.222225,0 82.666669,0 c 2.48889,0 2.666667,0.88888892 2.666667,13.333334 0,13.312741 -0.0051,13.333334 -3.31414,13.333334 -2.713016,0 -4.046628,1.45105 -7.352527,8 -2.910153,5.764983 -4.783393,8 -6.705054,8 -2.514253,0 -2.435853,-0.45724 1.371721,-8 l 4.038386,-8 H 42.675584 11.979445 l 8.020556,16 8.020555,16 h 7.864896 c 7.208457,0 7.806693,0.222573 7.167548,2.666667 -0.546327,2.089159 -1.818688,2.666667 -5.875174,2.666667 -5.041269,0 -5.177825,0.124673 -5.177825,4.727273 0,5.986922 4.896651,11.272728 10.442802,11.272728 4.691746,0 8.143833,3.23376 4.815513,4.510956 -2.853907,1.095146 -7.314724,0.905766 -10.591648,-0.44966 z M 58.761449,83.06982 C 42.315237,74.425092 46.43926,50.744943 64.782102,48.499203 74.527698,47.306035 83.596079,54.669854 84.834135,64.782102 86.537063,78.691332 71.070396,89.539851 58.761449,83.06982 Z m 13.238553,-4.389763 c 1.625626,-1.039679 0.308987,-2.970125 -7.033805,-10.312916 -7.342791,-7.342792 -9.273238,-8.659431 -10.312917,-7.033806 -1.789105,2.79742 -1.547897,8.084905 0.540731,11.853239 3.18482,5.746102 11.937507,8.60715 16.805991,5.493483 z m 7.190682,-7.455771 c 4.004328,-10.435112 -8.80032,-22.325171 -17.857349,-16.581863 -1.625789,1.030957 -0.385625,2.898193 6.62815,9.979584 4.745481,4.791231 9.031288,8.711329 9.524015,8.711329 0.492726,0 1.260058,-0.949072 1.705184,-2.10905 z";
    icon->Data = PathMarkupToGeometry(data);
    SideToggleButton->Icon = icon;

    SideToggleButton->Label = "Disable Side Marker";
}


void SlitherLink::MainPage::CellToggleButton_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    mEnableSetCell = true;

    FontIcon^ icon = ref new FontIcon();
    icon->FontFamily = ref new Windows::UI::Xaml::Media::FontFamily("Segoe MDL2 Assets");
    icon->Glyph = L"\xE790";
    CellToggleButton->Icon = icon;

    CellToggleButton->Label = "Enable Cell Shader";
}


void SlitherLink::MainPage::CellToggleButton_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    mEnableSetCell = false;
    FontIcon^ icon = ref new FontIcon();
    icon->FontFamily = ref new Windows::UI::Xaml::Media::FontFamily("Segoe MDL2 Assets");
    icon->Glyph = L"\xF570";
    CellToggleButton->Icon = icon;

    CellToggleButton->Label = "Disable Cell Shader";
}


task<void> SlitherLink::MainPage::ReadFile(StorageFile^ file)
{
    IRandomAccessStream^ stream = co_await file->OpenReadAsync();
    myLogW(LOG_INFO, LTAG L"------------- stream->Size = %llu -------------", stream->Size);
    DataReader^ reader = ref new DataReader(stream->GetInputStreamAt(0));

    Array<byte>^ bytes = ref new Array<byte>(stream->Size);
    unsigned int numBytes = co_await reader->LoadAsync(stream->Size);
    myLogW(LOG_INFO, LTAG L"------------- numBytes = %u -------------", numBytes);

    reader->ReadBytes(bytes);
    char *buf = new char[numBytes];

    for (int i = 0; i < numBytes; i++)
    {
        buf[i] = (char)bytes[i];
    }
    char *ptr = buf;
    int pos;

    int row;
    int col;

    sscanf_s(ptr, "%d %d%*[\r\n]%n", &row, &col, &pos);
    ptr += pos;
    myLogW(LOG_INFO, LTAG L"row = %d, col = %d, pos = %d", row, col, pos);

    for (int i = 0; i < row; i++)
    {
        myLogW(LOG_DEBUG, LTAG L"------------- row = %d -------------", i);
        auto arr = ref new Vector<byte>();
        for (int j = 0; j < col; j++)
        {
            char ch;
            //sscanf_s(ptr, "%c", &ch);
            //ptr++;
            ch = *ptr++;
            myLogW(LOG_DEBUG, LTAG L"[%d] ch = %c", j, ch);
            arr->Append(ch);
        }
        sscanf_s(ptr, "%*[\r\n]%n", &pos);
        myLogW(LOG_DEBUG, LTAG L"[%d] pos = %d", i, pos);
        ptr += pos;
        mMap->Append(arr);
    }

    delete buf;

    co_return;
}


void SlitherLink::MainPage::OpenFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FileOpenPicker^ picker = ref new FileOpenPicker();
    picker->FileTypeFilter->Append(".txt");

    create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            create_task(ReadFile(file)).then([this]() {
                int row = mMap->Size;
                if (row < 0)
                {
                    return;
                }
                int col = mMap->GetAt(0)->Size;
                if (col < 0)
                {
                    return;
                }
                myLogW(LOG_DEBUG, LTAG L"mMap: row = %d, col = %d", row, col);
                this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, row, col]() {
                    Init(row, col);
                }));
            });
        }
    });
}


Geometry^ SlitherLink::MainPage::PathMarkupToGeometry(String^ pathMarkup)
{
    String^ xaml =
        "<Path " +
        "xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'>" +
        "<Path.Data>" + pathMarkup + "</Path.Data></Path>";
    auto path = (Windows::UI::Xaml::Shapes::Path^)XamlReader::Load(xaml);
    // Detach the PathGeometry from the Path
    Geometry^ geometry = path->Data;
    path->Data = nullptr;
    return geometry;
}


void SlitherLink::MainPage::LoadFromUrlButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Uri^ uri = ref new Uri(mUrl);
    create_task(mHttpClient->GetAsync(uri)).then([this](HttpResponseMessage^ response) {
        create_task(response->Content->ReadAsStringAsync()).then([this](String^ responseBodyAsText) {
            this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, responseBodyAsText]() {
                //HtmlResult->Text = responseBodyAsText;
            }));
        });
    });
}
