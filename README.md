# Superium (Prototype)

This is the prototype for a FPS game where you play as a superhero (or supervillain) and go around stopping (or starting) crime.

This is also a testbed for eventual mod support, with a goal of creating a modular game relying on Unreal Engine's DLC system.

## How the game works

Not everything listed below is implemented. However, this is a roadmap for what I hope to achieve with this prototype. 

### Game Loop

During the game, there are two teams: The heroes and the villains. Heroes want to tilt the city towards a utopia. Villains want to do the opposite, to force the city towards dystopia. You can affect the city's ideology by completing missions for your team *or* by stopping the enemy team from completing their missions.

A character's health is measured by 2 separate values: "Defense" and "Health."

* Defense can be thought of as a "shield." As long as your defense is positive, you don't lose any health. Defense also recovers over time, based on your stats (more below).

* Your Health does not regenerate under normal circumstances. Once a character is out of health, it's game over for them. In the future, a version of the "rival" system from *Shadow of Mordor* would be a cool addition, as would a way to bring dead characters back to life in true comic book fashion (see "Stretch Goals").

### Create a Hero

* You start off by selecting a hero mesh. Eventually, there will be a bunch of hero meshes to choose from. Part of the modular approach involves dynamically loading in extra models at runtime. At the moment, this isn't implemented, but it's on the to-do list.

* Next, you give the hero stats and powers. Each of these pulls from the same budget -- if you have a lot of stats, you have fewer powers.

#### Powers

* Powers are abilities given to the player. They can either be active (laser beams from the eyes) or passive (regeneration).

* Some powers can be actively bad (think like Marvel's Hulk where he can't control his power, or Spider-Man where he has to protect his identity to save his family). Negative powers give you some budget back.

* There is a hard cap on the max number of powers you can have, to cut down on min-maxing.

#### Stats

There are 8 different "categories" of stats. Giving different numbers to these stats causes different in-game effects (similar to Fallout's S.P.E.C.I.A.L. system):

**Perseverance:**

* Impacts how fast a character regenerates their defense.

* Also impacts how much health you have when a character's defense is completely broken.

**Offense:**

* This affects overall "damage per second." It lets characters shoot faster, reload faster, punch faster, etc.

* It also affects intimidate rolls when talking to NPCs during missions. A successful intimidate roll may cause the NPC to be more likely to give information.

* This increases your base defense stat.

**Willpower:**

* This causes characters to do more damage with mental attacks.

* This also causes characters to take less damage *from* mental attacks.

* This also gives you extra base health for when a character's defense is broken.

**Strength:**

* This makes melee attacks (punch, sword, etc.) do more damage.

* This also can allow you to lift up heavier objects. At high values, you can lift cars, tanks, etc. Low values may struggle to even hold a sword.

* This gives base defense and better jump height (never skip leg day).

**Intuition:**

* This lets you pick up on subtle things easier. NPCs give you more info during conversations, and you get more/rarer missions.

* This also lets you see how close an enemy's defense is to breaking and how much they have left (both only giving info within a certain range; better intuition narrows the range).

* Intuition also increases base experience gain.

**Education:**

* Education lets you hack into things faster, allowing you to turn technology to your side.

* A high education stat also lets you be more technical when talking to NPCs, letting them give you more info.

* Furthermore, characters with a better education get better weapons and technology.

* Education modifies how much experience you get by completing missions.

**Estate:**

* Estate is how wealthy your character is. It also modifies how much money your character has at the start of the game.

* A high estate score means your character gets more money when completing missions.

* A better estate score also modifies your experience gain.

**Dexterity:**

* This causes characters to move faster.

* It also lets you move more quietly, being less likely to trigger alarms or alert enemies.

* It also increases base jump height and gives a better defense stat.

### Leveling Up

* Leveling up can grant you extra powers and abilities. You get experience points by completing missions successfully.

* Your base stats are set in stone; however, you can pick up a passive power which gives you a bonus to a stat under certain conditions.

* Leveling up also gives you more missions to select from in the overworld.

### Missions

Missions are part of the core gameplay loop, and the main way your team can win the game. Successfully completing a mission tilts the city more towards your ideology as well as gives players a cash reward.

Initial prototypes simply give a single mission; the eventual goal is a plethora of missions over a persistent world.

## Stretch Goals

These are things that may not make it into the prototype, but will probably make it into the final game.

* Much like any good comic book story, unless you're Batman's parents or Uncle Ben your character has a shot at coming back when you "die." The ultimate goal is a persistent world that lasts between saves -- like the difference between a town in the Sims and one particular household. Future characters in the same city should be able to undertake special missions to revive dead characters, making their save files accessible again.

## License

This project is released under the MIT License.

It contains Unreal Engine 4 Starter Content in `Content/StarterContent`, which is [free to distribute, according to Epic Games' staff.](https://answers.unrealengine.com/questions/24336/open-repository-and-startercontent.html)