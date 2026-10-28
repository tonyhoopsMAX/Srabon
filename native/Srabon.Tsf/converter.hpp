#pragma once

#include <string>
#include <vector>

namespace srabon {

// Native transliteration port for the in-process TSF service. Rules intentionally
// mirror the existing Srabon.Converter prototype; they remain provisional.
std::vector<std::wstring> Candidates(const std::wstring& input, std::size_t limit = 8);

} // namespace srabon
