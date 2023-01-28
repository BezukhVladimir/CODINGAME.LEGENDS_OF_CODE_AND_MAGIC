# LEGENDS OF CODE AND MAGIC
CodinGame's global programming contest for 2018.

## GOAL
Draft a deck of cards, battle an opponent with those cards and reduce their Health Points (HP) from 30 to 0.

## RULES
This game is a two-player card game which is played in two phases: the Draft phase and the Battle phase. <br>
During the Draft phase, both players must create a deck of 30 cards. <br>
Once the Draft phase is over, both decks are shuffled. <br>
During the Battle, the board is divided in two parts: each player plays cards from their hand on their side of the board. <br>
Each player starts with 30 HP. Some cards can increase this number. <br>
To reduce the health points of an opponent, the player must make use of cards to deal damage.

## DRAFT PHASE
For 30 turns, both players are given a choice between 3 different cards. Players select the card they want to add to their deck by using the PICK command followed by 0, 1 or 2.  <br>
By default, the PASS command will pick the first card. <br>
Both players can select the same card, they will each receive a copy.

## BATTLE PHASE
Card Draw <br>
First player starts with 4 cards in hand whereas the second player starts with 5. <br>
Each turn, the active player draws one additional card from their deck. <br>
Some cards can make players draw additional cards at the beginning of the next turn when played. <br>
Both players possess runes which have an impact on card draw. More details in Advanced Details section.

Mana <br>
Mana is necessary to play cards. <br>
Both players start with 1 max mana. <br>
Each turn, the active player is granted one additional max mana, unless they already have 12 max mana. <br>
Each player can spend as much mana per turn as they have max mana. <br>

## CARD TYPES
There are two different types of cards: Creatures and Items.

### Creatures
Placing a creature card from the hand to the board is called summoning. <br>
A player summons creatures to their side of the board by paying their cost in mana. They are used to attack the opponent and also serve as a defense against the creatures of the opposing player. <br>
Creatures have a cost in mana, attack points and defense points. Some creatures start with certain abilities. <br>
By default, creatures can't attack the turn they are summoned. They can attack once per turn only. <br>
When a creature attacks another one, they both deal damage equals to their attack to the defense of the other creature. When a creature attacks the opponent, it deals damage equals to its attack to the HP of the opponent. <br>
Creatures are removed from play when their defense reaches 0 or less. <br>
Creatures can have an effect on the player's health, the opponent's health or the card draw of the player when played. <br>
Creatures can have different abilities: <br>
Breakthrough: Creatures with Breakthrough can deal extra damage to the opponent when they attack enemy creatures. If their attack damage is greater than the defending creature's defense, the excess damage is dealt to the opponent. <br>
Charge: Creatures with Charge can attack the turn they are summoned. <br>
Drain: Creatures with Drain heal the player of the amount of the damage they deal (when attacking only). <br>
Guard: Enemy creatures must attack creatures with Guard first. <br>
Lethal: Creatures with Lethal kill the creatures they deal damage to. <br>
Ward: Creatures with Ward ignore once the next damage they would receive from any source. The "shield" given by the Ward ability is then lost. <br>

### Items

When played, items have an immediate and permanent effect on the board or on the players. They are then removed from play. <br>
Items have a cost in mana and one or multiple effects out of the following: <br>
Permanent modifier of a creature's attack and/or defense characteristics. Example: +0/+2 or -1/-1. <br>
The addition or removal of one or more abilities to one creature. <br>
Additional card draw the next turn they're played. <br>
Health gain for the player or health loss for the opponent. <br>
There are three types of items: <br>
Green items should target the active player's creatures. They have a positive effect on them. <br>
Red items should target the opponent's creatures. They have a negative effect on them. <br>
Blue items can be played with the "no creature" target identifier (-1) to give the active player a positive effect or cause damage to the opponent, depending on the card. Blue items with negative defense points can also target enemy creatures.

## GAMEPLAY
Possible Actions <br>
SUMMON id to summon the creature id from your hand. <br>
ATTACK id1 id2 to attack creature id2 with creature id1. <br>
ATTACK id -1 to attack the opponent directly with creature id. <br>
USE id1 id2 to use item id1 on creature id2. <br>
USE id -1 to use item id. <br>
PASS to do nothing this turn. <br>
A player can do any number of valid actions during one turn. Actions must be separated by a semi-colon ;.

### Game End
The game is over once any player reaches 0 or less HP.

### Victory Conditions
Reduce your opponent Health Points (HP) from 30 to 0 or less.

### Loss Conditions
Your HP gets reduced to 0 or less. <br>
You do not respond in time or output an unrecognized command.

## ADVANCED DETAILS
You can see the game's source code on https://github.com/CodinGame/LegendsOfCodeAndMagic.

### Runes
Each player has 5 runes corresponding to the 25, 20, 15, 10 and 5 HP thresholds. <br>
The first time a player's HP go below one of these thresholds, that player loses a rune and will draw an additional card at the beginning of the next turn. A maximum of 5 cards can thus be drawn this way during the entire game. <br>
When players have no more cards in their decks and must draw a card, they lose a rune and reach the corresponding HP threshold. <br>
Example: a player has 23 HP, 4 runes remaining and no more cards in the deck. If that player must draw a card, the player loses a rune (the 20 HP rune) and 3 HP to reach 20. <br>
If a player has no more runes, no more cards in the deck and must draw a card, that player's HP reaches 0.

### Constraints
If a player already has the maximum number of 8 cards in hand and must draw, the draw is cancelled. <br>
If a player already has the maximum number of 6 creatures on board and tries summoning a new one, the summoning action is cancelled. <br>
If a player tries to attack an untargetable target (wrong instance id or presence of other defensive creatures with Guard) with one of his creatures, the attack action is cancelled. <br>
Once a player has played over 50 turns, their deck is considered empty. <br>

### Abilities special cases
Giving an ability to a creature with that same ability has no effect. <br>
Attacking a creature with Ward with a creature with Lethal does not kill the creature (since no damage is dealt to the creature). <br>
Attacking a creature with Ward with a creature with Breakthrough never deals excess damage to the opponent (since no damage is dealt to the creature). <br>
Attacking a creature with Ward with a creature with Drain does no heal the player (since no damage is dealt to the creature).

## GAME INPUT
### Input for one game turn
First 2 lines: for each player, playerHealth, playerMana, playerDeck and playerRune: <br>
Integer playerHealth: the remaining HP of the player. <br>
Integer playerMana: the current maximum mana of the player. <br>
Integer playerDeck: the number of cards in the player's deck. <br>
Integer playerRune: the next remaining rune of a player. <br>
The player's input comes first, the opponent's input comes second. <br>
During the Draft phase, playerMana is always 0. <br>
Next line: Integer opponentHand, the total number of cards in the opponent's hand. These cards are hidden until they're played. <br>
Next line: Integer cardCount: during the Battle phase, the total number of cards on the board and in the player's hand. During the Draft phase, always 3. <br>
Next cardCount lines: for each card, cardNumber, instanceId, location, cardType, cost, attack, defense, abilities, myhealthChange, opponentHealthChange and cardDraw: <br>
Integer cardNumber: the identifier of a card (see complete list). <br>
Integer instanceId: the identifier representing the instance of the card (there can be multiple instances of the same card in a game). <br>
Integer location, during the Battle phase: <br>
0: in the player's hand <br>
1: on the player's side of the board <br>
-1: on the opponent's side of the board <br>
Always 0 during the Draft phase. <br>
Integer cardType: <br>
0: Creature <br>
1: Green item <br>
2: Red item <br>
3: Blue item <br>
Integer cost: the mana cost of the card, <br>
Integer attack: <br>
Creature: its attack points <br>
Item: its attack modifier <br>
Integer defense: <br>
Creature: its defense points <br>
Item: its defense modifier. Negative values mean this causes damage. <br>
String abilities of size 6: the abilities of a card. Each letter representing an ability (B for Breakthrough, C for Charge and G for Guard, D for Drain, L for Lethal and W for Ward). <br>
Integer myHealthChange: the health change for the player. <br>
Integer opponentHealthChange: the health change for the opponent. <br>
Integer cardDraw: the additional number of cards drawn next turn for the player. <br>

### Output for one game turn of the Draft
PICK nb where nb equals 0, 1 or 2 to choose one of the three proposed cards to add to your deck. <br>
PASS to do nothing (picks the 1st card by default).

### Output for one game turn of the Card Battle
The available actions are: <br>
SUMMON id to summon the creature of instanceId id from the player's hand. <br>
ATTACK idAttacker idTarget to attack an opposing creature or opposing player of instanceId idTarget with a creature on the board of instanceId idAttacker. <br>
idTarget can be the "no-creature" identifier -1. It is used to attack the opponent directly. <br>
USE idCard idTarget to use an item of instanceId idCard on a creature of instanceId idTarget or without a target with the "no-creature" identifier -1. <br>
PASS to do nothing. <br>
Players may use several actions by using a semi-colon ;. <br>
Players may append text to each of their actions, it will be displayed in the viewer. <br>
Example: SUMMON 3;ATTACK 4 5 yolo; ATTACK 8 -1 no fear.

### Constraints
0 ≤ cost ≤ 12 <br>
0 ≤ creatures on one side of the board ≤ 6 <br>
0 ≤ cards in hand ≤ 8 <br>

Response time for the first draft turn ≤ 1000ms <br>
Response time for the first battle turn ≤ 1000ms <br>
Response time per turn ≤ 100ms
