# Roomba Competition - Todo List

## Scoring System Refinement
- [ ] **Review Consistency Bonus Logic**:
    - **Issue**: Current consistent bonus rewards consistently *poor* behavior (e.g., `team14` which doesn't move creates perfect consistency 0 deviation).
    - **Goal**: Ensure bonus is only awarded to robots that are "Regular AND Good".
    - **Proposed Idea**: Add a minimum performance threshold (e.g., `total_score > X`) as a prerequisite to receive the consistency bonus.
    - **Context**: Observed in Dec 2025 competition testing where immobilized bots received bonuses.
