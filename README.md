# Teddy

*A simple tile grid editor*

## Acknowledgements

Teddy is programmed using the Python language and the Tkinter library. Many thanks to all the people behind these marvelous tools!

Teddy is a hommage to two great tools of the past:
- **TEd**, a tool whose origins date back to 1988, made by [John Romero](https://twitter.com/romero) and [used for more than 30 games](https://www.gamasutra.com/blogs/DavidLightbown/20170223/289955/Classic_Tools_Retrospective_John_Romero_talks_about_creating_TEd_the_tile_editor_that_shipped_over_30_games.php). It most famous version is TEd 5.0, dating from 1991 and used for Wolfenstein 3D from id Software.
- The map editor of the beloved **Warcraft II** (1995) from Blizzard.

## Description

Teddy is a program to edit tile grids, in order to make levels or maps for video games.

Teddy uses the concept of **mod**. A mod is set of **tiles** and **layers** which will be available to create the map.
- A map consists of one or more **layers**.
- A layer is just a matrix of integer.
- A tile is picture with an associated value, for example **1** is used for water, **2** for grass. **0** always means empty. 

For a real-time strategy game for example, you can have a layer *ground* for the ground tiles and a layer *unit* where you will store the units.

You can create your own mod, it is only a set of pictures for the tiles and a [JSON file](https://www.json.org/json-en.html "a lightweight data-interchange format") for configuring the mod.

Teddy is free and open source, licensed under the MIT licence. A binary is available for Microsoft Windows.

## Credits and history

Teddy is made by Damien Gouteux with ❤ in Occitania, 2019-2020.

It origins date back from 2012, a first incarnation using Pygame which was ditched in favor of this incarnation, which started in 2019.
