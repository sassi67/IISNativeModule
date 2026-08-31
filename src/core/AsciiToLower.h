#ifndef CORE_ASCII_TO_LOWER_H_
#define CORE_ASCII_TO_LOWER_H_

namespace core {
    // Locale-independent ASCII case folding.
    //
    // Deliberately NOT std::towlower: that consults the active C locale,
    // so the same URL can fold differently depending on process
    // configuration (the Turkish dotted/dotless I is the classic way this
    // breaks a security comparison). URL path matching must behave
    // identically everywhere, so only A-Z is folded and every other code
    // point — including non-ASCII — is left untouched.
    //
    // Being constexpr and branch-simple, it also inlines into the
    // comparison loop instead of calling into the CRT once per character.
    constexpr auto AsciiToLower(wchar_t character) -> wchar_t {
        return (character >= L'A' && character <= L'Z')
            ? static_cast<wchar_t>(character - L'A' + L'a')
            : character;
    }
}
#endif // CORE_ASCII_TO_LOWER_H_
