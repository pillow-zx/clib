#!/usr/bin/bash

total=0
failed=0

for test in build/test/*; do
  total=$((total + 1))
  echo "=== Running $(basename "$test") ==="

  "$test"
  rc=$?
  if [ "$rc" -ne 0 ]; then
    failed=$((failed + 1))
  fi
done

echo "=== Test binaries: $total, failed: $failed ==="
[ "$failed" -eq 0 ]
