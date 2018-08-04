#pragma once

namespace SlitherLink
{
    public ref class ShaderPair sealed
    {
    public:
        ShaderPair(Windows::UI::Xaml::Media::SolidColorBrush^ left, Windows::UI::Xaml::Media::SolidColorBrush^ right)
        {
            mLeft = left;
            mRight = right;
        }

        property Windows::UI::Xaml::Media::SolidColorBrush^ Left
        {
            Windows::UI::Xaml::Media::SolidColorBrush^ get()
            {
                return mLeft;
            }
        }
        property Windows::UI::Xaml::Media::SolidColorBrush^ Right
        {
            Windows::UI::Xaml::Media::SolidColorBrush^ get()
            {
                return mRight;
            }
        }
    private:
        Windows::UI::Xaml::Media::SolidColorBrush^ mLeft;
        Windows::UI::Xaml::Media::SolidColorBrush^ mRight;
    };
}