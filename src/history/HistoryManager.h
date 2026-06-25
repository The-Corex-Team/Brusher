#pragma once

#include "DocumentSnapshot.h"
#include <vector>

class HistoryManager {
public:
    static constexpr int MaxHistory = 20;

    void clear();
    void pushState(const DocumentSnapshot &state);

    bool canUndo() const { return m_currentIndex > 0; }
    bool canRedo() const { return m_currentIndex >= 0 && m_currentIndex < static_cast<int>(m_states.size()) - 1; }

    const DocumentSnapshot &currentState() const;
    const DocumentSnapshot &undo();
    const DocumentSnapshot &redo();

    int currentIndex() const { return m_currentIndex; }
    const std::vector<DocumentSnapshot> &states() const { return m_states; }
    std::vector<DocumentSnapshot> &mutableStates() { return m_states; }
    void restoreStates(const std::vector<DocumentSnapshot> &states, int index);

private:
    std::vector<DocumentSnapshot> m_states;
    int m_currentIndex = -1;
};
