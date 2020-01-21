<h1 class="contract">create</h1>

---
title: Create A Game
summary: Adds a row to the games table
icon: ICON
---

Parameter set contains host & challenger names to define the game. Host always plays X and goes first.
Authority for creating a game is the action key for the "host" account.


<h1 class="contract">erase</h1>

---
title: Destroy A Game
summary: Erases a row from the games table
icon: ICON
---

Parameter set contains host & challenger names and a "by" name identifying who is erasing the game.
Authority for the game is the action key for the "by" account


<h1 class="contract">move</h1>

---
title: Take a turn
summary: Marks a cell as containing an x or an o
icon: ICON
---

Parameter set contains host & challenger names, identifying the game in the games table, the 0-indexed row and column of the move and a "by" name identifying who is moving.
Authority for the game is the action key for the "by" account.
The turn is validated to confirm the player making the turn is the next up, and that the cell moving into is empty. 


<h1 class="contract">restart</h1>

---
title: Start a new game
summary: Clears the game state in the games table
icon: ICON
---

Parameter set contains host & challenger names and a "by" name identifying who is restarting.
Authority for the game is the action key for the "by" account. The "by" player must match either the host or the challenger.

