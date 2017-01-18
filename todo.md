# TODO
## Stuff that still has to be done

1. Doors that require keys to be opened
  - Item system so you can have keys
  - Doors that check for keys in the player's inventory
2. Item system
  - Typedef for items that has an enum which determines what kind of item it is
  - Further properties are taken from a list, depending on the enum
  - The player's inventory has a fixed size and is always full of items
  - Items can be marked as *empty*
  - Items can exist in the world by themselves
  - Items can be picked up by the player to their inventory
3. Fight system
  - Fighters have a limited amount of stamina, which regenerates slowly
  - Each attack costs stamina and have a random chance of missing their target
  - Successful attacks remove some of the victim's health
  - Fights continue until one of the participants is dead
  - Different types of attack are available
    - high risk / high reward
    - low risk / low reward
    - *etc.*
4. Enemies
  - Enemies exist in the world
  - Each has stats assigned from a list that depends on an enum
  - In areas of higher difficulty may roam the land, sometimes deciding to pursuit the player
    - Can only decide to pursuit the player if they're both in the same room
    - If the player is over-powered for the current area, enemies may run away from themselves
5. World
  - Levels are initialized from a const array that describes the rooms, doors, items, enemies and the player's starting position. The array is constructed as follows
  > number of rooms | room1 position | room1 size | number of doors in room1 | door1 position | door1 key level | door1 target | ... | number of items in room1 | item1 type | item1 position | ... | number of enemies in room1 | enemy1 type | enemy1 position | ... | ... |player starting position
  ```c
  const int level[] = {2,/* room1 */ 3, 3, 5, 5, 1, 4, 2, 0, 10, 10, 0, /* no items */, 0, /* no enemies */, /* room2 */ 9, 9, 5, 5, 1, 0, 0, 0, 7, 5, 0, /* no items */, 0, /* no enemies */, 4, 4 };
  ```
  - Rooms that are next to each other with no space between them can be traversed as if they were one
  - Levels are structured in a way that forces the player to go through a series of rooms to get a key that will allow them to begin again in a different series of rooms.
  - The last key opens the doors to the boss room where a big and tough enemy lives
  - Beating the boss gives you points depending on how many turns it took you to beat them from the start of the level
  - Scores from all levels cumulate, the lowest overall score is the best
6. Score
  - The lowest scores are saved to a file, which is filled out with dummy scores in the beginning
  - The score file has a message imploring the user to reconsider tampering with it