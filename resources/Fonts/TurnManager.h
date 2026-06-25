//#pragma once
//enum class TurnState
//{
//    ChoosingMonster,
//    ChoosingTargetTile,
//    MonsterSelected,
//    Attacking,
//    TurnEnded
//};
//
//class TurnManager
//{
//public:
//    TurnManager(Player& p1, Player& p2, Board& board);
//
//    void handleInput(sf::Vector2f mousePos);
//    
//    //void setState(TurnState newState);
//    //TurnState getState() const;
//
//private:
//    void endTurn();
//
//    Player& m_player1;
//    Player& m_player2;
//    Board& m_board;
//
//    Player* m_currentPlayer; // מצביע לשחקן שתורו עכשיו (יכול להשתנות, לכן זה מצביע ולא רפרנס)
//    TurnState m_state;
//};