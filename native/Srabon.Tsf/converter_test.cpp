#include "converter.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

int main() {
    const auto regression = srabon::Candidates(L"ami banglay likhi");
    if (regression.empty() || regression.front() != L"আমি বাংলায় লিখি") {
        std::wcerr << L"Regression fixture failed; received: "
                   << (regression.empty() ? L"<no candidates>" : regression.front()) << L"\n";
        return EXIT_FAILURE;
    }
    const auto alternatives = srabon::Candidates(L"s");
    if (alternatives.size() < 2 || alternatives[0] != L"স" || alternatives[1] != L"শ") {
        std::wcerr << L"Candidate alternatives or stable order failed.\n";
        return EXIT_FAILURE;
    }
    if (srabon::Candidates(L"").front() != L"") {
        std::wcerr << L"Empty input should yield one empty candidate.\n";
        return EXIT_FAILURE;
    }
    std::wcout << L"TSF converter regression passed: ami banglay likhi -> " << regression.front() << L"\n";
    return EXIT_SUCCESS;
}
