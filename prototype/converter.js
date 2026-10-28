/*
 * Srabon converter harness: deterministic, local transliteration only.
 * This intentionally small rule table is a prototype, not an approved
 * Bangla romanization standard or a claim of broad conversion accuracy.
 */
(function (root) {
  "use strict";

  const VOWELS = {
    aa: { independent: "আ", sign: "া" },
    ee: { independent: "ঈ", sign: "ী" },
    ii: { independent: "ঈ", sign: "ী" },
    oo: { independent: "ঊ", sign: "ূ" },
    uu: { independent: "ঊ", sign: "ূ" },
    oi: { independent: "ঐ", sign: "ৈ" },
    ou: { independent: "ঔ", sign: "ৌ" },
    ri: { independent: "ঋ", sign: "ৃ" },
    a: { independent: "আ", sign: "া" },
    i: { independent: "ই", sign: "ি" },
    u: { independent: "উ", sign: "ু" },
    e: { independent: "এ", sign: "ে" },
    o: { independent: "ও", sign: "ো" }
  };

  const CONSONANTS = {
    chh: "ছ",
    kh: "খ",
    gh: "ঘ",
    ch: "চ",
    jh: "ঝ",
    th: "থ",
    dh: "ধ",
    ph: "ফ",
    bh: "ভ",
    sh: "শ",
    ny: "ঞ",
    T: "ট",
    Th: "ঠ",
    D: "ড",
    Dh: "ঢ",
    N: "ণ",
    k: "ক",
    g: "গ",
    c: "চ",
    j: "জ",
    t: "ত",
    d: "দ",
    n: "ন",
    p: "প",
    f: "ফ",
    b: "ব",
    m: "ম",
    y: "য",
    r: "র",
    l: "ল",
    s: "স",
    h: "হ",
    v: "ভ",
    w: "ও",
    z: "জ"
  };

  // Ambiguous one-input alternatives: these are transliteration candidates,
  // not suggestions inferred from surrounding words.
  const AMBIGUOUS = {
    s: ["স", "শ"],
    j: ["জ", "য"]
  };

  const TOKEN_KEYS = Array.from(new Set([
    ...Object.keys(VOWELS),
    ...Object.keys(CONSONANTS),
    "ng"
  ])).sort((a, b) => b.length - a.length);

  function tokenAt(text, index) {
    for (const key of TOKEN_KEYS) {
      if (text.startsWith(key, index)) {
        if (key === "ng") return { key, kind: "mark", value: "ং", alternatives: ["ং", "ঙ"] };
        if (Object.hasOwn(VOWELS, key)) return { key, kind: "vowel", value: VOWELS[key] };
        return {
          key,
          kind: "consonant",
          value: CONSONANTS[key],
          alternatives: AMBIGUOUS[key] || [CONSONANTS[key]]
        };
      }
    }
    return null;
  }

  function tokenize(input) {
    const tokens = [];
    for (let i = 0; i < input.length;) {
      const match = tokenAt(input, i);
      if (match) {
        tokens.push(match);
        i += match.key.length;
      } else {
        const char = input[i];
        tokens.push({ key: char, kind: "literal", value: char, alternatives: [char] });
        i += 1;
      }
    }
    return tokens;
  }

  function transliterateTokens(tokens, choices) {
    let result = "";
    let openConsonant = false;

    tokens.forEach((token, index) => {
      const value = choices && choices[index] !== undefined ? choices[index] : token.value;
      const previous = tokens[index - 1];
      const next = tokens[index + 1];
      const isWordFinalY = token.key === "y" && previous && previous.kind === "vowel" &&
        (previous.key === "a" || previous.key === "aa") &&
        (!next || (next.kind === "literal" &&
          (/\s/.test(next.value) || ",.!?;:()[]{}\"'".includes(next.value))));
      if (isWordFinalY) {
        result += "য়";
        openConsonant = false;
      } else if (token.kind === "vowel") {
        result += openConsonant ? value.sign : value.independent;
        openConsonant = false;
      } else if (token.kind === "consonant") {
        if (openConsonant) result += "্";
        result += value;
        openConsonant = true;
      } else {
        result += value;
        openConsonant = false;
      }
    });

    return result;
  }

  function candidates(input, limit = 8) {
    const tokens = tokenize(String(input));
    if (!tokens.length) return [""];

    let variants = [[]];
    for (const token of tokens) {
      const options = token.alternatives || [token.value];
      const expanded = [];
      for (const prefix of variants) {
        for (const option of options) {
          expanded.push(prefix.concat(option));
          if (expanded.length >= limit) break;
        }
        if (expanded.length >= limit) break;
      }
      variants = expanded;
    }

    const seen = new Set();
    const output = [];
    for (const choices of variants) {
      const candidate = transliterateTokens(tokens, choices);
      if (!seen.has(candidate)) {
        seen.add(candidate);
        output.push(candidate);
      }
    }
    return output.length ? output : [String(input)];
  }

  function convert(input) {
    return candidates(input, 1)[0];
  }

  const api = { convert, candidates, tokenize };
  if (typeof module !== "undefined" && module.exports) module.exports = api;
  root.SrabonConverter = api;
})(typeof globalThis !== "undefined" ? globalThis : this);
