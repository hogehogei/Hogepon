

#include "core/GameFieldState.hpp"

GameState::GameState()
    : 
	m_Point( 0 ),
    m_ChainCount( 0 ),
	m_SeriagariCount( 0 ),
	m_SeriagariWaitTimer( 0 ),
    m_GameOverWaitTimer( 0 ),
	m_Is_InChain( false ),
    m_FieldState( 0 )
{}

GameState::~GameState()
{}

void GameState::IncSeriagariCount(int count)
{
	m_SeriagariCount += count;
}

void GameState::SetSeriagariCount(int count)
{
	m_SeriagariCount = count;
}

int GameState::SeriagariCount() const
{
	return m_SeriagariCount;
}

void GameState::SetSeriagariWaitTimer(int time) 
{ 
	m_SeriagariWaitTimer = time;
}

int GameState::SeriagariWaitTimer() const 
{ 
	return m_SeriagariWaitTimer; 
}

void GameState::DecSeriagariWaitTimer() 
{ 
	--m_SeriagariWaitTimer; 
}

void GameState::SetGameOverWaitTimer(int time)
{
	m_GameOverWaitTimer = time;
}

int GameState::GameOverWaitTimer() const
{
	return m_GameOverWaitTimer;
}

void GameState::DecGameOverWaitTimer()
{
	--m_GameOverWaitTimer;
}

void GameState::SetState( unsigned state )
{
    m_FieldState |= state;
}

void GameState::ClearState( unsigned state )
{
    m_FieldState &= ~state;
}

bool GameState::IsGameOver() const
{
	if ((m_FieldState & STATE_GAME_OVER) == STATE_GAME_OVER) {
		return true;
	}

	return false;
}

bool GameState::IsDanger() const
{
	if ((m_FieldState & STATE_DANGER) == STATE_DANGER) {
		return true;
	}

	return false;
}

bool GameState::IsSeriagari() const
{
	if ((m_FieldState & STATE_SERIAGARI) == STATE_SERIAGARI) {
		return true;
	}
	
	return false;
}

bool GameState::IsAutoSeriagari() const
{
	if ((m_FieldState & STATE_AUTO_SERIAGARI) == STATE_AUTO_SERIAGARI) {
		return true;
	}

	return false;
}

bool GameState::IsDeletePanel() const
{
	if ((m_FieldState & STATE_DELETE_PANEL) == STATE_DELETE_PANEL) {
		return true;
	}

	return false;
}

bool GameState::IsUncompressOjyama() const
{
	if ((m_FieldState & STATE_UNCOMPRESS_OJYAMA) == STATE_UNCOMPRESS_OJYAMA) {
		return true;
	}

	return false;
}

void GameState::Doujikeshi(int doujikeshi_count)
{
	// 同時消しイベント送出
}

void GameState::IncChainCount()
{
	if (!m_Is_InChain) {
		m_Is_InChain = true;
		m_ChainCount = 2;
	}
	else {
		++m_ChainCount;
	}
}

void GameState::EndChain()
{
	m_Is_InChain = false;
	// 連鎖終了イベント送出
}