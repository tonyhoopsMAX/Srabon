#include "converter.hpp"

#include <algorithm>
#include <cwctype>
#include <unordered_set>
#include <utility>

namespace srabon {
namespace {
enum class Kind { Vowel, Consonant, Mark, Literal };
struct Option { std::wstring text, independent, sign; };
struct Token { std::wstring key; Kind kind; std::vector<Option> options; };

Option V(const wchar_t* independent, const wchar_t* sign) { return { L"", independent, sign }; }
Option C(const wchar_t* text) { return { text, L"", L"" }; }

const std::vector<std::pair<std::wstring, Option>>& Vowels() {
    static const std::vector<std::pair<std::wstring, Option>> values = {
        {L"aa", V(L"আ", L"া")}, {L"ee", V(L"ঈ", L"ী")}, {L"ii", V(L"ঈ", L"ী")},
        {L"oo", V(L"ঊ", L"ূ")}, {L"uu", V(L"ঊ", L"ূ")}, {L"oi", V(L"ঐ", L"ৈ")},
        {L"ou", V(L"ঔ", L"ৌ")}, {L"ri", V(L"ঋ", L"ৃ")},
        {L"a", V(L"আ", L"া")}, {L"i", V(L"ই", L"ি")}, {L"u", V(L"উ", L"ু")},
        {L"e", V(L"এ", L"ে")}, {L"o", V(L"ও", L"ো")}
    };
    return values;
}
const std::vector<std::pair<std::wstring, std::wstring>>& Consonants() {
    static const std::vector<std::pair<std::wstring, std::wstring>> values = {
        {L"chh",L"ছ"},{L"kh",L"খ"},{L"gh",L"ঘ"},{L"ch",L"চ"},{L"jh",L"ঝ"},
        {L"th",L"থ"},{L"dh",L"ধ"},{L"ph",L"ফ"},{L"bh",L"ভ"},{L"sh",L"শ"},{L"ny",L"ঞ"},
        {L"T",L"ট"},{L"Th",L"ঠ"},{L"D",L"ড"},{L"Dh",L"ঢ"},{L"N",L"ণ"},
        {L"k",L"ক"},{L"g",L"গ"},{L"c",L"চ"},{L"j",L"জ"},{L"t",L"ত"},{L"d",L"দ"},
        {L"n",L"ন"},{L"p",L"প"},{L"f",L"ফ"},{L"b",L"ব"},{L"m",L"ম"},{L"y",L"য"},
        {L"r",L"র"},{L"l",L"ল"},{L"s",L"স"},{L"h",L"হ"},{L"v",L"ভ"},{L"w",L"ও"},{L"z",L"জ"}
    };
    return values;
}

bool IsBoundary(const Token* token) {
    if (!token) return true;
    if (token->kind != Kind::Literal || token->key.empty()) return false;
    const wchar_t ch = token->key[0];
    return iswspace(ch) || std::wstring(L",.!?;:()[]{}\"'").find(ch) != std::wstring::npos;
}

std::vector<Token> Tokenize(const std::wstring& input) {
    std::vector<std::wstring> keys;
    for (const auto& item : Vowels()) keys.push_back(item.first);
    for (const auto& item : Consonants()) keys.push_back(item.first);
    keys.push_back(L"ng");
    std::stable_sort(keys.begin(), keys.end(), [](const auto& a, const auto& b) { return a.size() > b.size(); });

    std::vector<Token> tokens;
    for (std::size_t pos = 0; pos < input.size();) {
        bool matched = false;
        for (const auto& key : keys) {
            if (input.compare(pos, key.size(), key) != 0) continue;
            if (key == L"ng") tokens.push_back({key, Kind::Mark, {C(L"ং"), C(L"ঙ")}});
            else {
                const auto vowel = std::find_if(Vowels().begin(), Vowels().end(), [&](const auto& item) { return item.first == key; });
                if (vowel != Vowels().end()) tokens.push_back({key, Kind::Vowel, {vowel->second}});
                else {
                    const auto consonant = std::find_if(Consonants().begin(), Consonants().end(), [&](const auto& item) { return item.first == key; });
                    std::vector<Option> opts{C(consonant->second.c_str())};
                    if (key == L"s") opts = {C(L"স"), C(L"শ")};
                    if (key == L"j") opts = {C(L"জ"), C(L"য")};
                    tokens.push_back({key, Kind::Consonant, std::move(opts)});
                }
            }
            pos += key.size();
            matched = true;
            break;
        }
        if (!matched) {
            tokens.push_back({input.substr(pos, 1), Kind::Literal, {C(input.substr(pos, 1).c_str())}});
            ++pos;
        }
    }
    return tokens;
}

std::wstring Transliterate(const std::vector<Token>& tokens, const std::vector<Option>& choices) {
    std::wstring result;
    bool openConsonant = false;
    for (std::size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        const auto* previous = i ? &tokens[i - 1] : nullptr;
        const auto* next = i + 1 < tokens.size() ? &tokens[i + 1] : nullptr;
        const bool finalY = token.key == L"y" && previous && previous->kind == Kind::Vowel &&
            (previous->key == L"a" || previous->key == L"aa") && IsBoundary(next);
        if (finalY) { result += L"য়"; openConsonant = false; }
        else if (token.kind == Kind::Vowel) {
            result += openConsonant ? choices[i].sign : choices[i].independent;
            openConsonant = false;
        } else if (token.kind == Kind::Consonant) {
            if (openConsonant) result += L'্';
            result += choices[i].text;
            openConsonant = true;
        } else {
            result += choices[i].text;
            openConsonant = false;
        }
    }
    return result;
}
} // namespace

std::vector<std::wstring> Candidates(const std::wstring& input, std::size_t limit) {
    if (limit == 0) return {};
    const auto tokens = Tokenize(input);
    if (tokens.empty()) return {L""};
    std::vector<std::vector<Option>> variants(1);
    for (const auto& token : tokens) {
        std::vector<std::vector<Option>> expanded;
        for (const auto& prefix : variants) {
            for (const auto& option : token.options) {
                auto next = prefix;
                next.push_back(option);
                expanded.push_back(std::move(next));
                if (expanded.size() >= limit) break;
            }
            if (expanded.size() >= limit) break;
        }
        variants = std::move(expanded);
    }
    std::vector<std::wstring> result;
    std::unordered_set<std::wstring> seen;
    for (const auto& variant : variants) {
        auto value = Transliterate(tokens, variant);
        if (seen.insert(value).second) result.push_back(std::move(value));
    }
    return result.empty() ? std::vector<std::wstring>{input} : result;
}
} // namespace srabon
