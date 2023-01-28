#ifndef _WIN64
#pragma GCC optimize("O3")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#endif

#include <cmath>
#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <float.h>
#include <iostream>
#include <limits.h>
#include <memory.h>
#include <algorithm>

using namespace std;
using namespace std::chrono;

static const int MAX_MANA = 12;
static const int MAX_CREATURES_IN_PLAY = 6;

// Card ranking based on match results
const float draftScores[] = {
    0.4625,0.176471,0.622222,0.397849,0.695652,0.590909,1.00962,0.394495,0.713043,0.13,0.553957,0.357895,0.277228,0.075,0.410526,0.0833333,0.405063,0.768421,0.45098,0.0614035,0.413793,0.0515464,0.625,0.0689655,0.435294,0.432099,0.216495,0.602564,0.822917,0.302752,0.0506329,0.821053,0.680412,0.247525,0.0506329,0.106195,0.4321,0.32967,0.253333,0.170732,0.418605,0.0465116,0.0111111,0.58,0.0666667,0.0309278,0.2875,1.00952,0.865854,0.786408,0.903614,0.858407,0.930556,0.802469,0.0,0.223529,0.0361446,0.038835,0.0909091,0.0186916,0.205882,0.163043,0.0227273,0.830189,1.01053,0.857143,0.727273,0.963855,0.964286,0.5375,0.291139,0.256637,0.177083,0.0588235,0.410714,0.0140845,0.0746269,0.0,0.126126,0.746835,0.247863,0.460674,0.577982,0.942308,0.739837,0.717172,0.674797,0.655556,0.0594059,0.111111,0.418803,0.0,0.346154,0.22449,0.493827,0.597403,0.578512,0.227273,0.619565,0.189474,0.197674,0.0731707,0.534653,0.32,0.195876,0.217742,0.0114943,0.04,0.324561,0.0121951,0.46,0.0540541,0.0123457,0.0705882,0.317647,0.425,
    0.00925926,0.03125,0.0,0.00970874,0.308511,0.107143,0.00980392,0.0245902,0.0462963,0.032967,0.133333,0.0470588,0.2,0.0246914,0.025641,0.00917431,0.323529,0.11215,0.114583,0.0263158,0.235955,0.025,0.861702,0.0106383,
    0.382609,0.0344828,0.5,0.496504,0.54023,0.284091,0.696429,0.764045,0.333333,0.444444,0.981651,0.627451,
    0.01,0.01,0.01,0.01,0.01,0.01,0.09,0.01
};

int opponentBoardCount = 0;
int t_opponentBoardCount = 0;
int highCount = 0;
int itemCount = 0;

struct Random {
    uint32_t state;

    Random() {
        state = (uint32_t) time(0);
    }

    uint32_t getRandom() {
        uint32_t x = state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        state = x;
        return x;
    }

    int getRandomInt(int bound) {
        return getRandom() % bound;
    }
};

struct Timeout {
    void start();
    bool isElapsed(double maxTimeSeconds);

    high_resolution_clock::time_point startTime;
};

void Timeout::start() {
    startTime = high_resolution_clock::now();
}

bool Timeout::isElapsed(double maxTimeSeconds) {
    duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() - startTime);
    return time_span.count() >= maxTimeSeconds;
}

struct Player {
    int health;
    int mana;
    int cardsRemaining;
    int rune;

    int cardsDrawn = 0;
};

enum class CardLocation {
    Opponent = -1,
    InHand = 0,
    Mine = 1,
    OutOfPlay = 2
};

enum class CardType {
    Creature,
    GreenItem,
    RedItem,
    BlueItem
};

struct Card {
    int cardName;
    int idx;
    int id;
    CardLocation location;
    CardType cardType;
    int cost;
    int attack;
    int defense;
    int abilities;
    int hpChange;
    int hpChangeEnemy;
    int cardDraw;

    bool canAttack;
    int abilitiesCount;
};

enum Ability {
    None = 0,
    Breakthrough = 1 << 0,
    Charge       = 1 << 1,
    Drain        = 1 << 2,
    Guard        = 1 << 3,
    Lethal       = 1 << 4,
    Ward         = 1 << 5
};

enum class ActionType {
    Pass,
    Summon,
    Attack,
    Use,
    Pick
};

struct State;

struct Action {
    ActionType type = ActionType::Pass;
    int idx = -1;
    int idxTarget = -1;

    void pass();
    void summon(int idx);
    void attack(int idx, int idxTarget = -1);
    void use(int idx, int idxTarget = -1);
    void pick(int idx);
    void print(ostream& os, State& state);
};

struct Turn {
    vector<Action> actions;

    void clear() { actions.clear(); }

    Action& newAction();

    bool isCardPlayed(int id);

    void print(ostream& os, State& state);
};

struct State {
    Player players[2];
    int opponentHand;
    vector<Card> cards;
    vector<int> creatureIdxs[2];

    bool isInDraft();

    void removeCard(Card& card);
    void dealDamage(Card& card, int amount);
    void dealLethal(Card& card);
    void doAttack(Card& card, Card& cardTarget);

    void generateActions(vector<Action>& actions, int playerIdx);

    void applyGlobalEffects(Card& card, int playerIdx);
    template <bool testValidity> void summon(Action& action, int playerIdx);
    template <bool testValidity> void use(Action& action, int playerIdx);
    template <bool testValidity> void attack(Action& action, int playerIdx);

    template <bool testValidity> void update(Action& action, int playerIdx);
    template <bool testValidity> void update(Turn& turn, int playerIdx);
};

bool State::isInDraft() {
    return players[0].mana == 0;
}

void State::removeCard(Card& card) {
    int playerIdx = card.location == CardLocation::Mine ? 0 : 1;
    auto& creatures = creatureIdxs[playerIdx];
    auto it = find(creatures.begin(), creatures.end(), card.idx);
    
    if (it != creatures.end())
        creatures.erase(it);
        
    card.location = CardLocation::OutOfPlay;
}

void State::dealDamage(Card& card, int amount) {
    if (amount <= 0)
        return;

    if (card.abilities & Ward) {
        card.abilities &= ~Ward;
        return;
    }

    card.defense -= amount;
    
    if (card.defense <= 0)
        removeCard(card);
        
    t_opponentBoardCount--;
}

void State::dealLethal(Card& card) {
    if (card.abilities & Ward)
        card.abilities &= ~Ward;
    else
        removeCard(card);
        t_opponentBoardCount--;
}

void State::doAttack(Card& card, Card& cardTarget) {
    if (card.abilities & Lethal)
        dealLethal(cardTarget);
    else
        dealDamage(cardTarget, card.attack);
}

void State::generateActions(vector<Action>& actions, int playerIdx) {
    Player& myPlayer = players[playerIdx];
    Player& opponentPlayer = players[1 - playerIdx];
    CardLocation myLocation = playerIdx == 0 ? CardLocation::Mine : CardLocation::Opponent;
    
    for (Card& card: cards) {
        if (card.location == CardLocation::InHand) {
            if (card.cost > myPlayer.mana)
                continue;
                
            if (card.cardType == CardType::Creature) {
                if (creatureIdxs[playerIdx].size() >= MAX_CREATURES_IN_PLAY)
                    continue;
                    
                actions.emplace_back();
                Action& action = actions.back();
                action.summon(card.idx);
            } else {
                if (card.cardType == CardType::BlueItem && card.defense == 0) {
                    actions.emplace_back();
                    Action& action = actions.back();
                    action.use(card.idx);
                } else {
                    auto& creatures = creatureIdxs[card.cardType == CardType::GreenItem ? playerIdx : (1 - playerIdx)];
                    
                    for (int creatureIdx: creatures) {
                        Card& creature = cards[creatureIdx];
                        
                        if ((card.cardType == CardType::GreenItem) && (creature.abilities & Guard) && ((creature.attack + creature.defense) < 9))
                            break;
                            
                        if ((card.cardType == CardType::GreenItem) && (card.abilities & Lethal) && ((creature.attack + creature.defense) < 9) && (!(creature.abilities & Charge)))
                            break;
                            
                        if (card.cardName == 142 ||
                            card.cardName == 148 ||
                            card.cardName == 149 ||
                            card.cardName == 151) {
                                if ((creature.attack + creature.defense) < 6)
                                    continue;
                                    
                                actions.emplace_back();
                                Action& action = actions.back();
                                action.use(card.idx, creatureIdx);
                        } else {
                            actions.emplace_back();
                            Action& action = actions.back();
                            action.use(card.idx, creatureIdx);
                        }
                    }
                }
            }
        } else if (card.location == myLocation && card.canAttack && card.attack > 0) {
            bool foundGuard = false;
            
            for (int creatureIdx: creatureIdxs[1 - playerIdx]) {
                Card& creature = cards[creatureIdx];
                
                if (!(creature.abilities & Guard))
                    continue;
                
                foundGuard = true;
                
                if ((creature.cardName == 49) && (card.attack < 2))
                    continue;

                actions.emplace_back();
                Action& action = actions.back();
                action.attack(card.idx, creatureIdx);
            }
            
            if (!foundGuard) {
                for (int creatureIdx: creatureIdxs[1 - playerIdx]) {
                    Card& creature = cards[creatureIdx];
                    actions.emplace_back();
                    Action& action = actions.back();
                    action.attack(card.idx, creatureIdx);
                }
                               
                if ((t_opponentBoardCount < 1) || (card.attack >= opponentPlayer.health) || (players[0].cardsRemaining < 5)) {
                    actions.emplace_back();
                    Action& action = actions.back();
                    action.attack(card.idx);
                }                
            }
        }
    }
}

void State::applyGlobalEffects(Card& card, int playerIdx) {
    Player& myPlayer = players[playerIdx];
    Player& opponentPlayer = players[1 - playerIdx];
    myPlayer.mana -= card.cost;
    myPlayer.health += card.hpChange;
    opponentPlayer.health += card.hpChangeEnemy;
    myPlayer.cardsDrawn += card.cardDraw;
}

template <bool testValidity>
void State::summon(Action& action, int playerIdx) {
    Player& myPlayer = players[playerIdx];

    Card& card = cards[action.idx];

    applyGlobalEffects(card, playerIdx);

    card.location = playerIdx == 0 ? CardLocation::Mine : CardLocation::Opponent;
    card.canAttack = card.abilities & Charge;
    myPlayer.mana -= card.cost;
    creatureIdxs[playerIdx].push_back(card.idx);
}

template <bool testValidity>
void State::use(Action& action, int playerIdx) {
    Player& myPlayer = players[playerIdx];

    Card& card = cards[action.idx];
  
    card.location = CardLocation::OutOfPlay;
    applyGlobalEffects(card, playerIdx);

    if (action.idxTarget != -1) {
        Card& cardTarget = cards[action.idxTarget];

        if (card.cardType == CardType::GreenItem)
            cardTarget.abilities |= card.abilities;
        else if (card.cardType == CardType::RedItem)
            cardTarget.abilities &= ~card.abilities;

        cardTarget.attack += card.attack;
        
        if (cardTarget.attack < 0)
            cardTarget.attack = 0;
        
        if (card.defense > 0)
            cardTarget.defense += card.defense;
        else if (card.defense < 0)
            dealDamage(cardTarget, -card.defense);
    }
}

template <bool testValidity>
void State::attack(Action& action, int playerIdx) {
    Player& myPlayer = players[playerIdx];
    Player& opponentPlayer = players[1 - playerIdx];
    Card& card = cards[action.idx];

    if (action.idxTarget == -1) {
        opponentPlayer.health -= card.attack;
    } else {
        Card& cardTarget = cards[action.idxTarget];

        if ((card.abilities & Breakthrough) && !(cardTarget.abilities & Ward)) {
            int remainder = card.attack - cardTarget.defense;
            if (remainder > 0)
                opponentPlayer.health -= remainder;
        }

        if ((card.abilities & Drain) && !(cardTarget.abilities & Ward)) {
            myPlayer.health += card.attack;
        }
        
        doAttack(card, cardTarget);
        doAttack(cardTarget, card);
    }
    
    card.canAttack = false;
}

template  <bool testValidity>
void State::update(Action& action, int playerIdx) {
    if (action.type == ActionType::Summon)
        summon<testValidity>(action, playerIdx);
    else if (action.type == ActionType::Use)
        use<testValidity>(action, playerIdx);
    else if (action.type == ActionType::Attack)
        attack<testValidity>(action, playerIdx);
}

template <bool testValidity>
void State::update(Turn& turn, int playerIdx) {
    for (Action& action: turn.actions) {
        update<testValidity>(action, playerIdx);
    }
}

void Action::pass() {
    type = ActionType::Pass;
}

void Action::summon(int _id) {
    type = ActionType::Summon;
    idx = _id;
}

void Action::attack(int _id, int _idTarget) {
    type = ActionType::Attack;
    idx = _id;
    idxTarget = _idTarget;
}

void Action::use(int _id, int _idTarget) {
    type = ActionType::Use;
    idx = _id;
    idxTarget = _idTarget;
}

void Action::pick(int _id) {
    type = ActionType ::Pick;
    idx = _id;
}

void Action::print(ostream& os, State& state) {
    if (type == ActionType::Pass) {
        os << "PASS";
    } else if (type == ActionType::Summon) {
        Card& card = state.cards[idx];
        os << "SUMMON " << card.id;
    } else if (type == ActionType::Attack) {
        Card& card = state.cards[idx];
        
        if (idxTarget == -1) {
            os << "ATTACK " << card.id << " -1";
        } else {
            Card& cardTarget = state.cards[idxTarget];
            os << "ATTACK " << card.id << " " << cardTarget.id;
        }
    } else if (type == ActionType::Use) {
        Card& card = state.cards[idx];
        
        if (idxTarget == -1) {
            os << "USE " << card.id << " -1";
        } else {
            Card &cardTarget = state.cards[idxTarget];
            os << "USE " << card.id << " " << cardTarget.id;
        }
    } else if (type == ActionType::Pick) {
        os << "PICK " << idx;
    }
}

Action& Turn::newAction() {
    actions.emplace_back();
    return actions.back();
}

bool Turn::isCardPlayed(int id) {
    for (Action& action: actions) {
        if (!(action.type == ActionType::Summon || action.type == ActionType::Use))
            continue;
            
        if (action.idx == id)
            return true;
    }
    
    return false;
}

void Turn::print(ostream& os, State& state) {
    if (actions.size() == 0) {
        os << "PASS";
        return;
    }

    bool first = true;
    for (Action& action: actions)
    {
        if (!first)
            os << ";";
            
        first = false;
        action.print(os, state);
    }
}

struct Agent {
    Random rnd;
    Timeout timeout;
    State state;
    Turn bestTurn;
    vector<Card> draftedCards;

    bool getRandomAction(State& state, Action& action, int playerIdx);

    void read();
    float evalScore(State& state);
    void think();
    void print();
};

bool Agent::getRandomAction(State& state, Action& action, int playerIdx) {
    vector<Action> actions;
    state.generateActions(actions, playerIdx);
    
    if (actions.empty())
        return false;
        
    int actionIdx = rnd.getRandomInt((int)actions.size());
    action = actions[actionIdx];
    
    return true;
}

void Agent::read() {
    for (int i = 0; i < 2; i++) {
        Player& player = state.players[i];

        int playerHealth, playerMana, playerDeck, playerRune;
        cin >> playerHealth >> playerMana >> playerDeck >> playerRune; cin.ignore();
        player.health = playerHealth;
        player.mana = playerMana;
        player.cardsRemaining = playerDeck;
        player.rune = playerRune;
		
        state.creatureIdxs[i].clear();
    }
    
    int opponentHand;
    cin >> opponentHand; cin.ignore();
    state.opponentHand = opponentHand;

    int cardCount;
    cin >> cardCount; cin.ignore();
    state.cards.clear();

    for (int i = 0; i < cardCount; i++) {
        state.cards.emplace_back();
        Card& card = state.cards.back();
        card.idx = i;

        int cardNumber;
        int instanceId;
        int location;
        int cardType;
        int cost;
        int attack;
        int defense;
        string abilities;
        int myHealthChange;
        int opponentHealthChange;
        int cardDraw;
        cin >> cardNumber >> instanceId >> location >> cardType >> cost >> attack >> defense >> abilities >> myHealthChange >> opponentHealthChange >> cardDraw; cin.ignore();
        card.cardName = cardNumber;
        card.id = instanceId;
        card.location = (CardLocation)location;
        card.cardType = (CardType)cardType;
        card.cost = cost;
        card.attack = attack;
        card.defense = defense;
        card.hpChange = myHealthChange;
        card.hpChangeEnemy = opponentHealthChange;
        card.cardDraw = cardDraw;
        card.canAttack = !(card.location == CardLocation::InHand);

        card.abilities = 0;
        card.abilitiesCount = 0;
        if (card.location == CardLocation::Opponent) opponentBoardCount++;
        
        for (char c: abilities) {
            if (c == 'B') {card.abilities |= Ability::Breakthrough; card.abilitiesCount++;}
            if (c == 'C') {card.abilities |= Ability::Charge; card.abilitiesCount++;}
            if (c == 'D') {card.abilities |= Ability::Drain; card.abilitiesCount++;}
            if (c == 'G') {card.abilities |= Ability::Guard; card.abilitiesCount++;}
            if (c == 'L') {card.abilities |= Ability::Lethal; card.abilitiesCount++;}
            if (c == 'W') {card.abilities |= Ability::Ward; card.abilitiesCount++;}
        }
        
        if (card.cardType == CardType::Creature
        && (card.location == CardLocation::Mine
            || card.location == CardLocation::Opponent))
            state.creatureIdxs[card.location == CardLocation::Mine ? 0 : 1].push_back(card.idx);
    }

    timeout.start();
}

float Agent::evalScore(State& state) {
    if (state.players[0].health <= 0)
        return -FLT_MAX;
        
    if (state.players[1].health <= 0)
        return FLT_MAX;

    float score = 0;
            
    for (int i = 0; i < 2; i++)
    {
        auto& creatures = state.creatureIdxs[i];
        for (int creatureIdxs: creatures) {
            if (i == 1)
            {
                Card& creature = state.cards[creatureIdxs];
                
                if (creature.abilities & Breakthrough)
                    score -= creature.attack * 0.2f;
                if (creature.abilities & Drain)
                    score -= creature.attack * 0.4f;
                if (creature.abilities & Guard)
                    score -= creature.defense * 0.2f;
                if (creature.abilities & Lethal)
                    score -= creature.defense * 0.4f;
                if (creature.abilities & Ward) {
                    score -= creature.attack * 0.1f;
                    score -= creature.defense * 0.1f;
                }
                
                score -= creature.attack * 1.005f;
                score -= creature.defense * 1.005f;
            }
            else
            {
                Card& creature = state.cards[creatureIdxs];
                
                if (creature.abilities & Breakthrough)
                    score += creature.attack * 0.2f;
                if (creature.abilities & Drain)
                    score += creature.attack * 0.4f;
                if (creature.abilities & Guard)
                    score += creature.defense * 0.2f;
                if (creature.abilities & Lethal)
                    score += creature.defense * 0.4f;
                if (creature.abilities & Ward) {
                    score += creature.attack * 0.1f;
                    score += creature.defense * 0.1f;
                }
                    
                score += creature.attack;
                score += creature.defense;
            }
        }
    }

    return score;
}

void Agent::think() {
    bestTurn.clear();

    if (state.isInDraft()) {
        float bestScore = -FLT_MAX;
        int bestPick = -1;
        CardType bestType;
        
        for (int i = 0; i < 3; ++i) {
            Card& card = state.cards[i];
            float score = 0;
            
            score = draftScores[card.cardName - 1];
            if (card.cardType != CardType::Creature)
                score -= 0.0001 * itemCount;
            
            if (card.cost > 8)
                score -= 0.001 * highCount * highCount;
                                                
            if (score > bestScore)
            {
                bestScore = score;
                bestPick = i;
                bestType = card.cardType;
            }
        }
        
        if (bestScore > 8)
            highCount++;
            
        if (bestType != CardType::Creature)
            itemCount++;
            
        auto& action = bestTurn.newAction();
        action.pick(bestPick);
        draftedCards.push_back(state.cards[bestPick]);
        return;
    }

    float bestScore = -FLT_MAX;
    bestTurn.clear();
    while (!timeout.isElapsed(0.096)) {
        t_opponentBoardCount = opponentBoardCount;
        State newState = state;
        Turn turn;
        
        while (true) {
            Action action;
            
            if (!getRandomAction(newState, action, 0))
                break;
                
            turn.actions.push_back(action);
            newState.update<true>(action, 0);
        }
        
        float score = evalScore(newState);
        
        if (score > bestScore) {
            bestScore = score;
            bestTurn = turn;
        }
    }
}

void Agent::print() {
    bestTurn.print(cout, state);
    cout << endl;
}

int main()
{
    Agent agent;
    
    while (true) {
        opponentBoardCount = 0;
        agent.read();
        agent.think();
        agent.print();
    }
}