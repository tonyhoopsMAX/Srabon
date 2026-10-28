namespace Srabon.Converter;

/// <summary>
/// Small, deterministic Roman-to-Bangla transliterator for the Stage 1 harness.
/// Its rule table is provisional and is not a broad-accuracy claim.
/// </summary>
public static class RomanBanglaConverter
{
    private enum TokenKind { Vowel, Consonant, Mark, Literal }

    private sealed record Option(string Text, string? Independent = null, string? Sign = null);
    private sealed record Token(string Key, TokenKind Kind, IReadOnlyList<Option> Options);

    private static Option V(string independent, string sign) => new("", independent, sign);
    private static Option C(string value) => new(value);
    private static readonly Dictionary<string, Option> Vowels = new(StringComparer.Ordinal)
    {
        ["aa"] = V("আ", "া"), ["ee"] = V("ঈ", "ী"), ["ii"] = V("ঈ", "ী"),
        ["oo"] = V("ঊ", "ূ"), ["uu"] = V("ঊ", "ূ"), ["oi"] = V("ঐ", "ৈ"),
        ["ou"] = V("ঔ", "ৌ"), ["ri"] = V("ঋ", "ৃ"),
        ["a"] = V("আ", "া"), ["i"] = V("ই", "ি"), ["u"] = V("উ", "ু"),
        ["e"] = V("এ", "ে"), ["o"] = V("ও", "ো")
    };

    private static readonly Dictionary<string, string> Consonants = new(StringComparer.Ordinal)
    {
        ["chh"] = "ছ", ["kh"] = "খ", ["gh"] = "ঘ", ["ch"] = "চ", ["jh"] = "ঝ",
        ["th"] = "থ", ["dh"] = "ধ", ["ph"] = "ফ", ["bh"] = "ভ", ["sh"] = "শ", ["ny"] = "ঞ",
        ["T"] = "ট", ["Th"] = "ঠ", ["D"] = "ড", ["Dh"] = "ঢ", ["N"] = "ণ",
        ["k"] = "ক", ["g"] = "গ", ["c"] = "চ", ["j"] = "জ", ["t"] = "ত", ["d"] = "দ",
        ["n"] = "ন", ["p"] = "প", ["f"] = "ফ", ["b"] = "ব", ["m"] = "ম", ["y"] = "য",
        ["r"] = "র", ["l"] = "ল", ["s"] = "স", ["h"] = "হ", ["v"] = "ভ", ["w"] = "ও", ["z"] = "জ"
    };

    private static readonly Dictionary<string, string[]> Ambiguous = new(StringComparer.Ordinal)
    {
        ["s"] = ["স", "শ"],
        ["j"] = ["জ", "য"]
    };

    private static readonly string[] Keys = Vowels.Keys
        .Concat(Consonants.Keys)
        .Append("ng")
        .Distinct(StringComparer.Ordinal)
        .OrderByDescending(key => key.Length)
        .ToArray();

    /// <summary>Returns the first deterministic transliteration candidate.</summary>
    public static string Convert(string? input) => Candidates(input, 1)[0];

    /// <summary>
    /// Returns bounded alternatives for the same Roman input. These are not
    /// context-predicted suggestions.
    /// </summary>
    public static IReadOnlyList<string> Candidates(string? input, int maxCandidates = 8)
    {
        input ??= string.Empty;
        if (maxCandidates < 1) throw new ArgumentOutOfRangeException(nameof(maxCandidates));

        var tokens = Tokenize(input);
        if (tokens.Count == 0) return [string.Empty];

        var variants = new List<Option[]> { Array.Empty<Option>() };
        foreach (var token in tokens)
        {
            var expanded = new List<Option[]>(Math.Min(maxCandidates, variants.Count * token.Options.Count));
            foreach (var prefix in variants)
            {
                foreach (var option in token.Options)
                {
                    var next = new Option[prefix.Length + 1];
                    prefix.CopyTo(next, 0);
                    next[^1] = option;
                    expanded.Add(next);
                    if (expanded.Count >= maxCandidates) break;
                }
                if (expanded.Count >= maxCandidates) break;
            }
            variants = expanded;
        }

        var output = new List<string>(variants.Count);
        var seen = new HashSet<string>(StringComparer.Ordinal);
        foreach (var choices in variants)
        {
            var value = Transliterate(tokens, choices);
            if (seen.Add(value)) output.Add(value);
        }
        return output.Count == 0 ? [input] : output;
    }

    private static List<Token> Tokenize(string input)
    {
        var tokens = new List<Token>();
        for (var index = 0; index < input.Length;)
        {
            var found = false;
            foreach (var key in Keys)
            {
                if (!input.AsSpan(index).StartsWith(key.AsSpan(), StringComparison.Ordinal)) continue;

                if (key == "ng")
                    tokens.Add(new Token(key, TokenKind.Mark, [C("ং"), C("ঙ")]));
                else if (Vowels.TryGetValue(key, out var vowel))
                    tokens.Add(new Token(key, TokenKind.Vowel, [vowel]));
                else
                {
                    var value = Consonants[key];
                    var alternatives = Ambiguous.TryGetValue(key, out var choices)
                        ? choices.Select(C).ToArray()
                        : [C(value)];
                    tokens.Add(new Token(key, TokenKind.Consonant, alternatives));
                }

                index += key.Length;
                found = true;
                break;
            }

            if (found) continue;
            var literal = input[index].ToString();
            tokens.Add(new Token(literal, TokenKind.Literal, [C(literal)]));
            index++;
        }
        return tokens;
    }

    private static string Transliterate(IReadOnlyList<Token> tokens, IReadOnlyList<Option> choices)
    {
        var result = new System.Text.StringBuilder();
        var openConsonant = false;
        for (var index = 0; index < tokens.Count; index++)
        {
            var token = tokens[index];
            var choice = choices[index];
            var previous = index > 0 ? tokens[index - 1] : null;
            var next = index + 1 < tokens.Count ? tokens[index + 1] : null;
            var isFinalY = token.Key == "y" && previous?.Kind == TokenKind.Vowel &&
                previous.Key is "a" or "aa" && IsWordBoundary(next);

            if (isFinalY)
            {
                result.Append("য়");
                openConsonant = false;
            }
            else if (token.Kind == TokenKind.Vowel)
            {
                result.Append(openConsonant ? choice.Sign : choice.Independent);
                openConsonant = false;
            }
            else if (token.Kind == TokenKind.Consonant)
            {
                if (openConsonant) result.Append('্');
                result.Append(choice.Text);
                openConsonant = true;
            }
            else
            {
                result.Append(choice.Text);
                openConsonant = false;
            }
        }
        return result.ToString();
    }

    private static bool IsWordBoundary(Token? token)
    {
        if (token is null) return true;
        if (token.Kind != TokenKind.Literal || token.Key.Length == 0) return false;
        return char.IsWhiteSpace(token.Key[0]) || ",.!?;:()[]{}\"'".Contains(token.Key[0]);
    }
}

/// <summary>Small selection model shared by the UI and automated tests.</summary>
public sealed class CandidateSelection
{
    public CandidateSelection(IEnumerable<string> candidates)
    {
        Items = candidates.ToArray();
        if (Items.Count == 0) throw new ArgumentException("At least one candidate is required.", nameof(candidates));
    }

    public IReadOnlyList<string> Items { get; }
    public int SelectedIndex { get; private set; }
    public string Selected => Items[SelectedIndex];

    public void Select(int index)
    {
        if ((uint)index >= (uint)Items.Count) throw new ArgumentOutOfRangeException(nameof(index));
        SelectedIndex = index;
    }
}
