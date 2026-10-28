const test = require('node:test');
const assert = require('node:assert/strict');
const { convert, candidates } = require('../converter.js');

test('reviewed regression example converts exactly', () => {
  assert.equal(convert('ami banglay likhi'), 'আমি বাংলায় লিখি');
});

test('sample parts convert independently', () => {
  assert.equal(convert('ami'), 'আমি');
  assert.equal(convert('banglay'), 'বাংলায়');
  assert.equal(convert('likhi'), 'লিখি');
});

test('longest matching digraph handles kh as one consonant', () => {
  assert.equal(convert('likhi'), 'লিখি');
});

test('case-sensitive retroflex notation remains deterministic', () => {
  assert.equal(convert('Taka'), 'টাকা');
  assert.equal(convert('taka'), 'তাকা');
});

test('spaces and punctuation pass through without changing word boundaries', () => {
  assert.equal(convert('ami, banglay!'), 'আমি, বাংলায়!');
});

test('candidate alternatives are outputs for the same input', () => {
  const values = candidates('salam');
  assert.deepEqual(values, ['সালাম', 'শালাম']);
  assert.equal(values.length, 2);
});

test('candidate generation is deterministic and bounded', () => {
  assert.deepEqual(candidates('s j s'), candidates('s j s'));
  assert.ok(candidates('s j s').length <= 8);
});

test('empty input yields an empty candidate', () => {
  assert.deepEqual(candidates(''), ['']);
  assert.equal(convert(''), '');
});

test('unmapped symbols are preserved literally', () => {
  assert.equal(convert('ami 123?'), 'আমি 123?');
});

test('conversion does not infer context or alter an unrelated word', () => {
  assert.equal(convert('salam'), 'সালাম');
  assert.equal(convert('ami salam'), 'আমি সালাম');
});
