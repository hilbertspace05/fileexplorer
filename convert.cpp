#include "convert.h"
#include <unicode/ucnv.h>
#include <unicode/ustring.h>
#include <iostream>
#include <memory>
#include <stdexcept>

std::string convertToUTF8(const std::string& str, const std::string& sourceEncoding) {
    UErrorCode status = U_ZERO_ERROR;

    // Open the converter for the source encoding
    UConverter* conv = ucnv_open(sourceEncoding.c_str(), &status);
    if (U_FAILURE(status)) {
        std::cerr << "Failed to open converter for encoding: " << sourceEncoding << std::endl;
        ucnv_close(conv); // Ensure conv is closed in case of failure
        throw std::runtime_error("Failed to open converter for " + sourceEncoding);
    }

    // Convert from source encoding to UChar (Unicode)
    int32_t unicodeLength = str.length() * 2; // Initial estimate
    std::unique_ptr<UChar[]> unicodeStr(new UChar[unicodeLength]);
    ucnv_toUChars(conv, unicodeStr.get(), unicodeLength, str.c_str(), str.length(), &status);
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        status = U_ZERO_ERROR;
        unicodeLength = ucnv_toUChars(conv, nullptr, 0, str.c_str(), str.length(), &status);
        unicodeStr.reset(new UChar[unicodeLength]);
        status = U_ZERO_ERROR;
        ucnv_toUChars(conv, unicodeStr.get(), unicodeLength, str.c_str(), str.length(), &status);
    }
    if (U_FAILURE(status)) {
        std::cerr << "Failed to convert from " << sourceEncoding << " to Unicode" << std::endl;
        ucnv_close(conv);
        throw std::runtime_error("Conversion to Unicode failed");
    }

    // Convert from UChar (Unicode) to UTF-8
    int32_t utf8Length = unicodeLength * 4; // Initial estimate
    std::unique_ptr<char[]> utf8Str(new char[utf8Length]);
    u_strToUTF8(utf8Str.get(), utf8Length, &utf8Length, unicodeStr.get(), unicodeLength, &status);
    if (U_FAILURE(status)) {
        std::cerr << "Failed to convert from Unicode to UTF-8" << std::endl;
        ucnv_close(conv);
        throw std::runtime_error("Conversion to UTF-8 failed");
    }

    ucnv_close(conv);
    return std::string(utf8Str.get(), utf8Length); // Use utf8Length to correctly construct the string
}