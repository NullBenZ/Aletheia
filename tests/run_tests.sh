#!/usr/bin/env bash
set -u
root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root" || exit 1
failures=0
pass() { echo "PASS: $1"; }
fail() { echo "FAIL: $1"; failures=$((failures + 1)); }
g++ -std=c++17 -O2 -Wall -Wextra -Iinclude src/*.cpp -o aletheia || exit 1
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
for size in 0 1 3 55 56 63 64 65 119 120 1000 1000000; do
  head -c "$size" /dev/zero > "$tmp/$size"
  actual=$(./aletheia hash "$tmp/$size" | cut -d' ' -f1)
  expected=$(sha256sum "$tmp/$size" | cut -d' ' -f1)
  if [[ "$actual" == "$expected" ]]; then pass "SHA-256 $size bytes"; else fail "SHA-256 $size bytes"; fi
done
if [[ "$(./aletheia hash "$tmp/0" | cut -d' ' -f1)" == e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855 ]]; then pass "known empty hash"; else fail "known empty hash"; fi
printf abc > "$tmp/abc"
if [[ "$(./aletheia hash "$tmp/abc" | cut -d' ' -f1)" == ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad ]]; then pass "known abc hash"; else fail "known abc hash"; fi
printf '\211PNG\r\n\032\n' > "$tmp/fake.pdf"
printf '\211PNG\r\n\032\n' > "$tmp/good.png"
if ./aletheia analyze "$tmp/fake.pdf" | rg -q 'PNG image' && ./aletheia analyze "$tmp/fake.pdf" | rg -q 'Extension mismatch'; then pass "signature mismatch"; else fail "signature mismatch"; fi
if ! ./aletheia analyze "$tmp/good.png" | rg -q 'Extension mismatch'; then pass "correct extension"; else fail "correct extension"; fi
mkdir "$tmp/dups"; printf same > "$tmp/dups/a"; cp "$tmp/dups/a" "$tmp/dups/b"; printf other > "$tmp/dups/c"
if [[ "$(./aletheia duplicates "$tmp/dups" | rg -c '^Duplicate group')" == 1 ]]; then pass "duplicate grouping"; else fail "duplicate grouping"; fi
mkdir "$tmp/empty"; : > "$tmp/empty/a"; : > "$tmp/empty/b"
if ./aletheia duplicates "$tmp/empty" | rg -q 'No duplicates found'; then pass "empty files excluded"; else fail "empty files excluded"; fi
printf 'http://example.com 10.0.0.5 a@b.com powershell' > "$tmp/patterns.txt"
out=$(./aletheia analyze "$tmp/patterns.txt")
if [[ "$out" == *'http://example.com'* && "$out" == *'10.0.0.5'* && "$out" == *'a@b.com'* && "$out" == *'powershell'* ]]; then pass "strings and patterns"; else fail "strings and patterns"; fi
if ./aletheia hash "$tmp/missing" >/dev/null 2>&1; then fail "missing path exit code"; else [[ $? == 1 ]] && pass "missing path exit code" || fail "missing path exit code"; fi
if ./aletheia unknown >/dev/null 2>&1; then fail "unknown command exit code"; else [[ $? == 1 ]] && pass "unknown command exit code" || fail "unknown command exit code"; fi
before=$(find samples -type f -print0 | sort -z | xargs -0 sha256sum)
./aletheia scan samples >/dev/null
after=$(find samples -type f -print0 | sort -z | xargs -0 sha256sum)
if [[ "$before" == "$after" ]]; then pass "scan leaves samples unchanged"; else fail "scan leaves samples unchanged"; fi
if [[ $failures -ne 0 ]]; then echo "$failures test(s) failed"; exit 1; fi
echo "All tests passed."
