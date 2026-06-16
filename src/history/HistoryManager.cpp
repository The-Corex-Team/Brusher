#include "HistoryManager.h"

void HistoryManager::clear()
{
    m_states.clear();
    m_currentIndex = -1;
}

void HistoryManager::pushState(const DocumentSnapshot &state)
{
    if (m_currentIndex >= 0 && m_currentIndex < static_cast<int>(m_states.size()) - 1) {
        m_states.erase(m_states.begin() + m_currentIndex + 1, m_states.end());
    }

    m_states.push_back(state);

    while (static_cast<int>(m_states.size()) > MaxHistory) {
        m_states.erase(m_states.begin());
        m_currentIndex--;
    }

    m_currentIndex = static_cast<int>(m_states.size()) - 1;
}

const DocumentSnapshot &HistoryManager::currentState() const
{
    return m_states.at(m_currentIndex);
}

const DocumentSnapshot &HistoryManager::undo()
{
    if (canUndo()) {
        m_currentIndex--;
    }
    return currentState();
}

const DocumentSnapshot &HistoryManager::redo()
{
    if (canRedo()) {
        m_currentIndex++;
    }
    return currentState();
}

void HistoryManager::restoreStates(const std::vector<DocumentSnapshot> &states, int index)
{
    m_states = states;
    m_currentIndex = std::clamp(index, 0, static_cast<int>(m_states.size()) - 1);
}
