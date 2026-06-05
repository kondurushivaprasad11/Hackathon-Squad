# Hackathon Squad

## Problem Statement

Given N coders each with a skill rating, and M conflict pairs where two coders
refuse to work together, select a conflict-free subset of coders that maximizes
the total skill rating.

This is a variant of the **Maximum Weight Independent Set** problem, which is
NP-Hard in general. The solution uses a multi-phase heuristic approach designed
to run within a 5-minute time limit.

---

## Algorithm — 4 Phase Approach

### Phase 1 : Greedy Initial Team
Rank every coder by the priority formula:

```
priority = skill / sqrt(degree + 1)
```

Walk the ranked list top to bottom. If a coder is not blocked by a previously
selected rival, add them to the team and instantly block all their enemies.

This gives a strong valid starting team fast in O(N log N).

---

### Phase 2 : Single Replacement Optimization
For each team member, scan the top 600 non-selected coders by skill.
If a candidate has higher skill and no conflict with the remaining team,
swap them in. Repeat passes until no improvement is found (convergence).

---

### Phase 3 : 1-for-2 Augmentation
For each team member, try replacing them with two non-members whose combined
skill exceeds the removed member, and who are mutually compatible with each
other and with the rest of the team. Uses a pool of top 400 non-selected coders.

---

### Phase 4 : Free Insertions
After Phase 3 removals, some previously blocked coders may now have zero
conflicts with the current team. A single linear scan picks them all up at
zero cost — pure gain.

---

## Complexity

| Phase | Time Complexity |
|-------|----------------|
| Phase 1 | O(N log N) |
| Phase 2 | O(passes × team × SWAP_POOL) |
| Phase 3 | O(team × POOL_SIZE²) |
| Phase 4 | O(N × max_degree) |

---

## Constraints Handled

- N up to 200,000 coders
- M up to N*(N-1)/2 conflict pairs
- Skill ratings up to 1,000,000,000
- Execution within 5 minutes

---

## Input Format

```
N M
S1 S2 ... SN
u1 v1
u2 v2
...
```

## Output Format

```
<total skill score>
<space-separated coder indices in ascending order>
```

---

## How to Compile and Run

```bash
g++ -O2 -std=c++17 -o solution solution.cpp
./solution < test1_input.txt
```

---

## Test Cases

| File | Description |
|------|-------------|
| test1_input.txt | Small mixed conflict graph |
| test2_input.txt | No conflicts — entire pool selected |
| test3_input.txt | Complete conflict graph — single best coder |
| test4_input.txt | Single coder edge case |
| test5_input.txt | Chain conflict — alternating picks |
