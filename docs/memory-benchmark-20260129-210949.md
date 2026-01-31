# Memory Benchmark Results

**Date:** 2026-01-29 21:09:49
**Runs:** 3 per test
**JS Engine:** Hermes 0.14.0

## Raw Results

### Native View (1000 views)
- Run 1: 104640 KB
- Run 2: 104720 KB
- Run 3: 105392 KB
- **Average: 104917 KB**

### Native Image (100 images)
- Run 1: 98096 KB
- Run 2: 99200 KB
- Run 3: 99392 KB
- **Average: 98896 KB**

### RN View (1000 views)
- Run 1: 140816 KB
- Run 2: 136416 KB
- Run 3: 137728 KB
- **Average: 138320 KB**

### RN Image (100 images)
- Run 1: 126640 KB
- Run 2: 127088 KB
- Run 3: 127088 KB
- **Average: 126938 KB**

## Summary

| Test | Native DALi | RN + DALi | Overhead |
|------|-------------|-----------|----------|
| 1000 Views | 102 MB | 135 MB | +32 MB (+31%) |
| 100 Images | 96 MB | 123 MB | +27 MB (+28%) |
