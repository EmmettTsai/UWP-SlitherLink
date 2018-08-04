//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include <pplawait.h>
#include <regex>
#include <string>
#include <locale.h>
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
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Markup;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Shapes;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Web::Http::Headers;

/*
https://developercommunity.visualstudio.com/content/problem/263131/co-await-optimization-bugs-in-visual-studio-157-x6.html
There is a hidden cl.exe switch "/d2CoroOptsWorkaround" to disable optimizations on all coroutines being compiled. I recommend only using it until you get the Preview 5 bits as this will disable all optimizations on the coroutine, resulting in worse code generation. Sometime in the future, this switch will not do anything.

https://docs.microsoft.com/en-us/windows/uwp/design/style/icons
https://docs.microsoft.com/en-us/windows/uwp/design/style/segoe-ui-symbol-font

https://stackoverflow.com/questions/50511876/how-to-set-bitmapicon-or-pathicon-content-from-canvas-stored-in-resource-d

https://stackoverflow.com/questions/25188349/how-to-set-pathicon-from-code-behind

https://docs.microsoft.com/zh-tw/windows/uwp/networking/httpclient

https://msdn.microsoft.com/en-us/library/bb982382.aspx
https://blog.csdn.net/fengbingchun/article/details/54835571

https://www.cnblogs.com/SunboyL/archive/2013/03/31/stringandwstring.html
https://gist.github.com/1901/5684151
https://blog.csdn.net/ghevinn/article/details/9825727
https://creatorlxd.github.io/2017/11/05/string-and-wstring/

https://docs.microsoft.com/zh-tw/dotnet/standard/base-types/backreference-constructs-in-regular-expressions
https://docs.microsoft.com/zh-tw/dotnet/standard/base-types/regular-expression-language-quick-reference

https://blog.csdn.net/mycwq/article/details/18838151
http://tommyjswu-blog.logdown.com/posts/726230-cpp11-regex-expression

https://msdn.microsoft.com/en-us/library/ts4c4dw6.aspx
*/


MainPage::MainPage()
{
    InitializeComponent();
    InitView();

    mLoop = ref new Vector<byte>();
    mExtendedLoop = ref new Vector<GridItemInfo^>();

    mEnableSetSide = true;
    mEnableSetCell = false;

    mTransparentColor = ref new SolidColorBrush(Colors::Transparent);
    mInsideMarkColor = ref new SolidColorBrush(Colors::Green);
    mOutsideMarkColor = ref new SolidColorBrush(Colors::SkyBlue);
    mLineMarkColor = ref new SolidColorBrush(Colors::Black);
    mCrossMarkColor = ref new SolidColorBrush(Colors::Red);

    mUrl = "https://www.puzzle-loop.com";
    InitHttpClient();
}


void MainPage::InitView()
{
    //"Yellow" "Red"
    ShaderPanel->Children->Append(CreateShaderPair(Colors::Yellow, Colors::Red));
    //"Green" "Pink"
    ShaderPanel->Children->Append(CreateShaderPair(Colors::Green, Colors::Pink));
}


void MainPage::InitHttpClient()
{
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
    // TODO: need clear previous event handler?
    RootCanvas->Children->Clear();
    mExtendedLoop->Clear();

    int dotSize = 10;
    double halfDotSize = dotSize * 0.5;
    int cellSize = 30;
    mIndicatorState = IndicatorState::None;

    /*
    * 2*row+1 x 2*col+1
    * dot:   (2*i, 2*j)
    * cell:  (2*(i+1), 2*(j+1))
    * hLine: (2*i, 2*(j+1))
    * vLine: (2*(i+1), 2*j)
    *
    * 0 1 | 2=mColStart ... n-3=mColEnd | n-2 n-1 | n=mExtendedColSize
    **/

    mRowSize = 2 * row + 1;
    mColSize = 2 * col + 1;
    mExtendedRowSize = mRowSize + 4;
    mExtendedColSize = mColSize + 4;
    mRowStart = 2;
    mRowEnd = mExtendedRowSize - 3;
    mColStart = 2;
    mColEnd = mExtendedColSize - 3;

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

    for (int i = 0; i < mExtendedRowSize; i++)
    {
        for (int j = 0; j < mExtendedColSize; j++)
        {
            int innerI = i - 2;
            int innerJ = j - 2;
            Border^ view;
            bool isExtendedItem = !(i >= mRowStart && i <= mRowEnd && j >= mColStart && j <= mColEnd);

            GridItemInfo^ info = ref new GridItemInfo();
            info->Row = i;
            info->Column = j;
            info->State = GridItemState::None;
            info->Handled = false;
            info->IsExtended = isExtendedItem;

            if (!isExtendedItem)
            {
                view = ref new Border();
                view->PointerEntered += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &SlitherLink::MainPage::Grid_PointerEntered);
                view->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &SlitherLink::MainPage::Grid_PointerPressed);
                view->PointerReleased += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &SlitherLink::MainPage::Grid_PointerReleased);
                info->View = view;
            }

            if (i % 2 == 0 && j % 2 == 0)
            {
                info->Type = GridItemType::Dot;
                info->Degree = 0;
#if BUG_FIXED
                if (!isExtendedItem)
#else
                if (isExtendedItem)
                {
                }
                else
#endif
                {
                    view->Width = dotSize;
                    view->Height = dotSize;
                    view->Background = mLineMarkColor;
                    Canvas::SetZIndex(view, 1);
                    Canvas::SetLeft(view, (dotSize + cellSize) * (innerJ / 2));
                    Canvas::SetTop(view, (dotSize + cellSize) * (innerI / 2));
                }
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

                    switch (GetLoopAt(loopRow, loopCol))
                    {
                    case '0':
                        info->Degree = 0;
                        break;
                    case '1':
                        info->Degree = 1;
                        break;
                    case '2':
                        info->Degree = 2;
                        break;
                    case '3':
                        info->Degree = 3;
                        break;
                    default:
                        info->Degree = -1;
                    }

                    view->Width = cellSize + dotSize;
                    view->Height = cellSize + dotSize;
                    view->Background = mTransparentColor;

                    TextBlock^ text = ref new TextBlock();
                    text->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
                    text->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
                    text->Text = info->Degree > 0 ? info->Degree.ToString() : "";
                    view->Child = text;

                    Canvas::SetZIndex(view, 0);
                    Canvas::SetLeft(view, dotSize + (cellSize + dotSize) * loopCol - halfDotSize);
                    Canvas::SetTop(view, dotSize + (cellSize + dotSize) * loopRow - halfDotSize);
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
                else
                {
                    view->Width = cellSize;
                    view->Height = dotSize;
                    view->Background = mTransparentColor;

                    Canvas::SetZIndex(view, 1);
                    Canvas::SetLeft(view, dotSize + (cellSize + dotSize) * (innerJ - 1) / 2);
                    Canvas::SetTop(view, (dotSize + cellSize) * innerI / 2);
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
                else
                {
                    view->Width = dotSize;
                    view->Height = cellSize;
                    view->Background = mTransparentColor;

                    Canvas::SetZIndex(view, 1);
                    Canvas::SetLeft(view, (dotSize + cellSize) * innerJ / 2);
                    Canvas::SetTop(view, dotSize + (cellSize + dotSize) * (innerI - 1) / 2);
                }
            }
            if (!isExtendedItem)
            {
                view->Tag = info;
                RootCanvas->Children->Append(view);
            }
            mExtendedLoop->Append(info);
        }
    }
}


void MainPage::Grid_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    myLogW(LOG_VERBOSE, LTAG L"[%d][%s]", __LINE__, __funcw__);
    mIndicatorState = IndicatorState::None;
    Pointer^ pointer = e->Pointer;
    if (pointer->PointerDeviceType == PointerDeviceType::Mouse)
    {
        if (sender->GetType()->FullName == TypeName(Viewbox::typeid).Name)
        {
            myLogW(LOG_DEBUG, LTAG L"[%d][%s] is Viewbox", __LINE__, __funcw__);
            auto box = (Viewbox^)sender;
            auto item = (Border^)box->Parent;
            auto info = (GridItemInfo^)item->Tag;
            info->Handled = true;
            return;
        }
        auto item = (Border^)sender;
        PointerPoint^ pointerPoint = e->GetCurrentPoint(item);
        if (pointerPoint->Properties->IsLeftButtonPressed)
        {
            Update(sender, true);
        }
        else if (pointerPoint->Properties->IsRightButtonPressed)
        {
            Update(sender, false);
        }
    }
}


void MainPage::Grid_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    myLogW(LOG_VERBOSE, LTAG L"[%d][%s]", __LINE__, __funcw__);
    mIndicatorState = IndicatorState::None;
}


void MainPage::Grid_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    myLogW(LOG_VERBOSE, LTAG L"[%d][%s]", __LINE__, __funcw__);
    Pointer^ pointer = e->Pointer;
    if (pointer->PointerDeviceType == PointerDeviceType::Mouse)
    {
        auto item = (Border^)sender;
        auto info = (GridItemInfo^)item->Tag;
        if (info->Handled)
        {
            info->Handled = false;
            return;
        }

        PointerPoint^ pointerPoint = e->GetCurrentPoint(item);
        bool leftPress = pointerPoint->Properties->IsLeftButtonPressed;
        bool rightPress = pointerPoint->Properties->IsRightButtonPressed;

        myLogW(LOG_DEBUG, LTAG L"[%d][%s] pos = (%d, %d)", __LINE__, __funcw__, info->Row, info->Column);
        switch (info->Type)
        {
        case GridItemType::Cell:
            myLogW(LOG_DEBUG, LTAG L"[%d][%s] type is Cell", __LINE__, __funcw__);
            if (!mEnableSetCell)
            {
                return;
            }
            break;
        case GridItemType::HorizontailLine:
        case GridItemType::VerticalLine:
            myLogW(LOG_DEBUG, LTAG L"[%d][%s] type is Side", __LINE__, __funcw__);
            if (!mEnableSetSide)
            {
                return;
            }
            break;
        }
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


void MainPage::SetInside(Windows::UI::Xaml::Controls::Border^ item)
{
    if (!mEnableSetCell)
    {
        return;
    }
    auto info = (GridItemInfo^)item->Tag;
    info->State = GridItemState::InSide;
    item->Background = mInsideMarkColor;
}


void MainPage::SetOutside(Windows::UI::Xaml::Controls::Border^ item)
{
    if (!mEnableSetCell)
    {
        return;
    }
    auto info = (GridItemInfo^)item->Tag;
    info->State = GridItemState::OutSide;
    item->Background = mOutsideMarkColor;
}


void MainPage::SetLine(Windows::UI::Xaml::Controls::Border^ item)
{
    if (!mEnableSetSide)
    {
        return;
    }
    auto info = (GridItemInfo^)item->Tag;
    info->State = GridItemState::Line;
    item->Background = mLineMarkColor;
    item->Child = nullptr;
}


void MainPage::SetCross(Windows::UI::Xaml::Controls::Border^ item)
{
    if (!mEnableSetSide)
    {
        return;
    }
    auto info = (GridItemInfo^)item->Tag;
    info->State = GridItemState::Cross;
    item->Background = mTransparentColor;

    auto viewBox = ref new Viewbox();
    FontIcon^ icon = ref new FontIcon();
    icon->FontFamily = ref new Windows::UI::Xaml::Media::FontFamily("Segoe MDL2 Assets");
    icon->Glyph = L"\xE8BB";
    icon->FontWeight = Windows::UI::Text::FontWeights::Bold;
    icon->Foreground = mCrossMarkColor;
    viewBox->Child = icon;
    viewBox->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &SlitherLink::MainPage::Grid_PointerPressed);
    item->Child = viewBox;
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
        myLogW(LOG_DEBUG, LTAG L"[%d][%s]", __LINE__, __funcw__);
        item->Child = nullptr;
        myLogW(LOG_DEBUG, LTAG L"[%d][%s]", __LINE__, __funcw__);
        break;
    }
    info->State = GridItemState::None;
    myLogW(LOG_DEBUG, LTAG L"[%d][%s]", __LINE__, __funcw__);
    item->Background = mTransparentColor;
    myLogW(LOG_DEBUG, LTAG L"[%d][%s]", __LINE__, __funcw__);
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


task<bool> SlitherLink::MainPage::ReadTextFile(StorageFile^ file)
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

    int row = -1;
    int col = -1;

    sscanf_s(ptr, "%d %d%*[\r\n]%n", &row, &col, &pos);
    if (row < 0 || col < 0)
    {
        delete buf;
        co_return false;
    }
    ptr += pos;
    myLogW(LOG_INFO, LTAG L"row = %d, col = %d, pos = %d", row, col, pos);
    mLoopRowSize = row;
    mLoopColSize = col;
    mLoop->Clear();
    for (int i = 0; i < row; i++)
    {
        myLogW(LOG_DEBUG, LTAG L"------------- row = %d -------------", i);
        for (int j = 0; j < col; j++)
        {
            char ch;
            //sscanf_s(ptr, "%c", &ch);
            //ptr++;
            ch = *ptr++;
            myLogW(LOG_DEBUG, LTAG L"[%d] ch = %c", j, ch);
            mLoop->Append(ch);
        }
        sscanf_s(ptr, "%*[\r\n]%n", &pos);
        myLogW(LOG_DEBUG, LTAG L"[%d] pos = %d", i, pos);
        ptr += pos;
    }

    delete buf;

    co_return true;
}


task<bool> SlitherLink::MainPage::ReadHtmlFile(StorageFile^ file)
{
    IRandomAccessStream^ stream = co_await file->OpenReadAsync();
    myLogW(LOG_INFO, LTAG L"------------- stream->Size = %llu -------------", stream->Size);
    DataReader^ reader = ref new DataReader(stream->GetInputStreamAt(0));

    unsigned int numBytes = co_await reader->LoadAsync(stream->Size);
    myLogW(LOG_INFO, LTAG L"------------- numBytes = %u -------------", numBytes);

    String^ content = reader->ReadString(numBytes);
    myLog(LOG_INFO, TAG "html content length = %u", content->Length());
    co_return ParseHtmlText(content);
}


void SlitherLink::MainPage::OpenFileButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FileOpenPicker^ picker = ref new FileOpenPicker();
    picker->FileTypeFilter->Append(".txt");

    create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            create_task(ReadTextFile(file)).then([this](bool result) {
                if (!result)
                {
                    myLogW(LOG_WARN, LTAG L"OpenFile ReadTextFile fail");
                    return;
                }
                this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]() {
                    Init(mLoopRowSize, mLoopColSize);
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
#if false
    FileOpenPicker^ picker = ref new FileOpenPicker();
    picker->FileTypeFilter->Append(".html");

    create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            create_task(ReadHtmlFile(file)).then([this](bool result) {
                if (!result)
                {
                    myLogW(LOG_WARN, LTAG L"LoadFromUrl ReadHtmlFile fail");
                    return;
                }
                this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]() {
                    Init(mLoopRowSize, mLoopColSize);
                    PuzzleInfo->Text = mPuzzleInfo;
                }));

            });
        }
    });
#else
    int index = LoopSizeBox->SelectedIndex;
    String^ loopName = (String^)LoopSizeBox->SelectedItem;
    int size = (int)getLoopSize(index);
    myLogW(LOG_INFO, LTAG L"selected index = %d, loop size = %d, name = %s", index, size, loopName->Data());
    Uri^ uri = ref new Uri(mUrl + "/?size=" + size);
    create_task(mHttpClient->GetAsync(uri)).then([this](HttpResponseMessage^ response) {
        create_task(response->Content->ReadAsStringAsync()).then([this](String^ responseBodyAsText) {
            this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, responseBodyAsText]() {
                if (!ParseHtmlText(responseBodyAsText))
                {
                    myLogW(LOG_WARN, LTAG L"LoadFromUrl ParseHtmlText fail");
                    return;
                }

                this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]() {
                    Init(mLoopRowSize, mLoopColSize);
                    PuzzleInfo->Text = mPuzzleInfo;
                }));
            }));
        });
    });
#endif
}


bool SlitherLink::MainPage::ParseHtmlText(Platform::String^ content)
{
    if (content == nullptr || content->IsEmpty())
    {
        return false;
    }

    std::wstring pattern;
    std::wregex regex;
    std::wsmatch matchs;
    std::wstring str(content->Data());
    int row;
    int col;

    // get puzzle info
    pattern.assign(L"<div class=\"puzzleInfo\">[\r\n]*.*?</p>");
    regex = std::wregex(pattern);
    if (std::regex_search(str, matchs, regex))
    {
        std::wstring substr = matchs[0].str();
        pattern.assign(L"<p>.*?(?=</p>)");
        regex.assign(pattern);
        if (std::regex_search(substr, matchs, regex))
        {
            mPuzzleInfo = ref new String(matchs[0].str().substr(wcslen(L"<p>")).c_str());
            myLogW(LOG_INFO, LTAG L"puzzle info = %s", mPuzzleInfo->Data());
        }
        else
        {
            myLog(LOG_ERROR, TAG "search puzzle info 2 fail");
            return false;
        }
    }
    else
    {
        myLog(LOG_ERROR, TAG "search puzzle info 1 fail");
        return false;
    }

    // get height
    pattern.assign(L"name=\"h\" value=\"\\d+(?=\")");
    regex.assign(pattern);
    if (std::regex_search(str, matchs, regex))
    {
        row = std::stoi(matchs[0].str().substr(wcslen(L"name=\"h\" value=\"")).c_str());
        myLog(LOG_INFO, TAG "row = %d", row);
    }
    else
    {
        myLog(LOG_ERROR, TAG "search height fail");
        return false;
    }

    // get width
    pattern.assign(L"name=\"w\" value=\"\\d+(?=\")");
    regex.assign(pattern);
    if (std::regex_search(str, matchs, regex))
    {
        col = std::stoi(matchs[0].str().substr(wcslen(L"name=\"w\" value=\"")).c_str());
        myLog(LOG_INFO, TAG "col = %d", col);
    }
    else
    {
        myLog(LOG_ERROR, TAG "search width fail");
        return false;
    }

    // get loop
    mLoopRowSize = row;
    mLoopColSize = col;
    mLoop->Clear();
    pattern.assign(L"<td align=\"center\" >\\d?<(?=/td>)");
    regex.assign(pattern);
    int count = 0;
    int len = wcslen(L"<td align=\"center\" >");
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            if (std::regex_search(str, matchs, regex))
            {
                char ch = matchs[0].str().substr(len, 1)[0];
                myLog(LOG_DEBUG, TAG "search map at (%d, %d) is '%c'", i, j, ch);
                if (ch == '<')
                {
                    ch = ' ';
                }
                mLoop->Append(ch);
            }
            else
            {
                myLog(LOG_ERROR, TAG "search loop at (%d, %d) fail", i, j);
                return false;
            }
            str = matchs.suffix();
        }
    }
    myLog(LOG_INFO, TAG "ParseHtmlText done");
    return true;
}


LoopSize SlitherLink::MainPage::getLoopSize(int index)
{
    switch (index)
    {
    default:
    case 0: return LoopSize::Normal_5x5;
    case 1: return LoopSize::Hard_5x5;
    case 2: return LoopSize::Normal_7x7;
    case 3: return LoopSize::Hard_7x7;
    case 4: return LoopSize::Normal_10x10;
    case 5: return LoopSize::Hard_10x10;
    case 6: return LoopSize::Normal_15x15;
    case 7: return LoopSize::Hard_15x15;
    case 8: return LoopSize::Normal_20x20;
    case 9: return LoopSize::Hard_20x20;
    case 10: return LoopSize::Normal_25x30;
    case 11: return LoopSize::Hard_25x30;
    case 12: return LoopSize::Special_Daily;
    case 13: return LoopSize::Special_Weekly;
    case 14: return LoopSize::Special_Monthly;
    }
}


inline byte SlitherLink::MainPage::GetLoopAt(int i, int j)
{
    return mLoop->GetAt(i * mLoopColSize + j);
}


inline GridItemInfo^ SlitherLink::MainPage::GetExtendedLoopAt(int i, int j)
{
    return mExtendedLoop->GetAt(i * mExtendedColSize + j);
}


Windows::UI::Xaml::Controls::RadioButton^ SlitherLink::MainPage::CreateShaderPair(Windows::UI::Color left, Windows::UI::Color right)
{
    auto rectStyle = dynamic_cast<Windows::UI::Xaml::Style^>(this->Resources->Lookup("RectangleStyle1"));
    auto buttonStyle = dynamic_cast<Windows::UI::Xaml::Style^>(this->Resources->Lookup("RadioButtonStyle1"));

    Rectangle^ leftRect = ref new Rectangle();
    leftRect->Style = rectStyle;
    leftRect->Drop += ref new Windows::UI::Xaml::DragEventHandler(this, &SlitherLink::MainPage::Rectangle_Drop);
    leftRect->DragStarting += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::UIElement ^, Windows::UI::Xaml::DragStartingEventArgs ^>(this, &SlitherLink::MainPage::Rectangle_DragStarting);
    leftRect->DragEnter += ref new Windows::UI::Xaml::DragEventHandler(this, &SlitherLink::MainPage::Rectangle_DragEnter);
    leftRect->Fill = ref new SolidColorBrush(left);

    Rectangle^ rightRect = ref new Rectangle();
    rightRect->Style = rectStyle;
    rightRect->Drop += ref new Windows::UI::Xaml::DragEventHandler(this, &SlitherLink::MainPage::Rectangle_Drop);
    rightRect->DragStarting += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::UIElement ^, Windows::UI::Xaml::DragStartingEventArgs ^>(this, &SlitherLink::MainPage::Rectangle_DragStarting);
    rightRect->DragEnter += ref new Windows::UI::Xaml::DragEventHandler(this, &SlitherLink::MainPage::Rectangle_DragEnter);
    rightRect->Fill = ref new SolidColorBrush(right);

    StackPanel^ panel = ref new StackPanel();
    panel->Orientation = Windows::UI::Xaml::Controls::Orientation::Horizontal;
    panel->Children->Append(leftRect);
    panel->Children->Append(rightRect);

    RadioButton^ button = ref new RadioButton();
    button->Style = buttonStyle;
    button->Margin = Thickness(10, 10, 10, 10);
    button->Content = panel;

    return button;
}


void SlitherLink::MainPage::Rectangle_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    auto sourceRect = (Rectangle^)mDragObject;
    auto targetRect = (Rectangle^)sender;

    auto sourceBrush = (SolidColorBrush^)sourceRect->Fill;
    auto targetBrush = (SolidColorBrush^)targetRect->Fill;

    auto sourceColor = sourceBrush->Color;
    auto targetColor = targetBrush->Color;

    String^ msg = "from ("
        + sourceColor.A + "," + sourceColor.R + "," + sourceColor.G + "," + sourceColor.B + ")"
        + " to ("
        + targetColor.A + "," + targetColor.R + "," + targetColor.G + "," + targetColor.B + ")";

    auto dialog = ref new Windows::UI::Popups::MessageDialog(msg);
    dialog->ShowAsync();
}


void SlitherLink::MainPage::Rectangle_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    auto sourceRect = (Rectangle^)mDragObject;
    auto targetRect = (Rectangle^)sender;

    auto sourceBrush = (SolidColorBrush^)sourceRect->Fill;
    auto targetBrush = (SolidColorBrush^)targetRect->Fill;

    auto sourceColor = sourceBrush->Color;
    auto targetColor = targetBrush->Color;

    if (sourceBrush->Equals(targetBrush))
    {
        return;
    }

    e->AcceptedOperation = Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move;
}


void SlitherLink::MainPage::Rectangle_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args)
{
    mDragObject = sender;
}
