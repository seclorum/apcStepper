/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   back_svg;
    const int            back_svgSize = 3231;

    extern const char*   ConthraxSemiBold_otf;
    const int            ConthraxSemiBold_otfSize = 77528;

    extern const char*   cuttlefish_jpg;
    const int            cuttlefish_jpgSize = 778500;

    extern const char*   shadow_png;
    const int            shadow_pngSize = 24119;

    extern const char*   playcircle_svg;
    const int            playcircle_svgSize = 919;

    extern const char*   stopcircle_svg;
    const int            stopcircle_svgSize = 623;

    extern const char*   shift_svg;
    const int            shift_svgSize = 562;

    extern const char*   button_svg;
    const int            button_svgSize = 1423;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 8;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
