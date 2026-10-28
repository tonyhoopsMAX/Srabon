using Srabon.Converter;

var tests = new (string Name, Action Run)[]
{
    ("reviewed regression example converts exactly", () => Equal("আমি বাংলায় লিখি", RomanBanglaConverter.Convert("ami banglay likhi"))),
    ("sample words convert independently", () =>
    {
        Equal("আমি", RomanBanglaConverter.Convert("ami"));
        Equal("বাংলায়", RomanBanglaConverter.Convert("banglay"));
        Equal("লিখি", RomanBanglaConverter.Convert("likhi"));
    }),
    ("longest digraph matching handles kh", () => Equal("লিখি", RomanBanglaConverter.Convert("likhi"))),
    ("capital retroflex notation is case-sensitive", () =>
    {
        Equal("টাকা", RomanBanglaConverter.Convert("Taka"));
        Equal("তাকা", RomanBanglaConverter.Convert("taka"));
    }),
    ("spaces and punctuation are preserved", () => Equal("আমি, বাংলায়!", RomanBanglaConverter.Convert("ami, banglay!"))),
    ("candidate alternatives come from the same Roman input", () =>
        SequenceEqual(["সালাম", "শালাম"], RomanBanglaConverter.Candidates("salam"))),
    ("candidate output is deterministic and bounded", () =>
    {
        var first = RomanBanglaConverter.Candidates("s j s", 6);
        SequenceEqual(first, RomanBanglaConverter.Candidates("s j s", 6));
        True(first.Count <= 6, "candidate count exceeded the requested limit");
    }),
    ("empty input yields an empty candidate", () =>
    {
        Equal(string.Empty, RomanBanglaConverter.Convert(string.Empty));
        SequenceEqual([string.Empty], RomanBanglaConverter.Candidates(string.Empty));
    }),
    ("unmapped punctuation and digits pass through", () => Equal("আমি 123?", RomanBanglaConverter.Convert("ami 123?"))),
    ("converter does not infer context", () => Equal("আমি সালাম", RomanBanglaConverter.Convert("ami salam"))),
    ("candidate selection updates the preview value", () =>
    {
        var selection = new CandidateSelection(RomanBanglaConverter.Candidates("salam"));
        Equal("সালাম", selection.Selected);
        selection.Select(1);
        Equal("শালাম", selection.Selected);
        Equal(1, selection.SelectedIndex);
    }),
    ("candidate selection rejects invalid indexes", () =>
    {
        var selection = new CandidateSelection(["প্রথম"]);
        Throws<ArgumentOutOfRangeException>(() => selection.Select(1));
        Throws<ArgumentOutOfRangeException>(() => selection.Select(-1));
    })
};

var failures = new List<string>();
foreach (var test in tests)
{
    try
    {
        test.Run();
        Console.WriteLine($"PASS {test.Name}");
    }
    catch (Exception exception)
    {
        failures.Add($"FAIL {test.Name}: {exception.Message}");
    }
}

foreach (var failure in failures) Console.Error.WriteLine(failure);
Console.WriteLine($"{tests.Length - failures.Count}/{tests.Length} tests passed.");
return failures.Count == 0 ? 0 : 1;

static void Equal<T>(T expected, T actual)
{
    if (!EqualityComparer<T>.Default.Equals(expected, actual))
        throw new InvalidOperationException($"Expected <{expected}> but got <{actual}>.");
}

static void SequenceEqual<T>(IReadOnlyList<T> expected, IReadOnlyList<T> actual)
{
    if (!expected.SequenceEqual(actual))
        throw new InvalidOperationException($"Expected [{string.Join(", ", expected)}] but got [{string.Join(", ", actual)}].");
}

static void True(bool value, string message)
{
    if (!value) throw new InvalidOperationException(message);
}

static void Throws<TException>(Action action) where TException : Exception
{
    try { action(); }
    catch (TException) { return; }
    throw new InvalidOperationException($"Expected {typeof(TException).Name}.");
}
