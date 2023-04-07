# Teddy

*A simple tile grid editor*

## Acknowledgements

Teddy is programmed using the Python language and the Tkinter and Pillow libraries. Many thanks to all the people behind these marvelous tools!

Teddy is an homage to two great tools of the past:
- **TEd**, a tool whose origins date back to 1988, made by [John Romero](https://twitter.com/romero) and [used for more than 30 games](https://www.gamasutra.com/blogs/DavidLightbown/20170223/289955/Classic_Tools_Retrospective_John_Romero_talks_about_creating_TEd_the_tile_editor_that_shipped_over_30_games.php). It most famous version is TEd 5.0, dating from 1991 and used for Wolfenstein 3D from id Software.
- The map editor of the beloved **Warcraft II** (1995) from Blizzard Entertainment.

## Description

Teddy is a program to edit tile grids, in order to make levels or maps for video games.

Teddy uses the concept of **mod**. A mod is set of **tiles** and **layers** which will be available to create the map.
- A map consists of one or more **layers**.
- A layer can be a **texture layer** or an **object layer** both are a matrix of integer. 
- In a **texture layer**, an integer at x,y designates a tile.
- A tile is picture with an associated integer value, for example **1** is used for water, **2** for grass. **0** always means empty.
- In an **object layer**, an integer at x,y designates a element in a list of **objects** with **properties**.

For a real-time strategy game for example, you can have a texture layer *ground* for the ground tiles and an object layer *unit* where you will store the units. A unit can have a *player* property, refering to the player it belongs.

You can create your own mod, it is only a set of pictures for the tiles and a [JSON file](https://www.json.org/json-en.html "a lightweight data-interchange format") for configuring the mod.

## License and distribution

Teddy is free and open source, licensed under the MIT licence.

A binary is available for Microsoft Windows.

To run the source code, you will need to install a [Python distribution](https://www.python.org/) (version >= 3.6) and only the third-party [Pillow](https://pypi.org/project/Pillow/) library. Tkinter is already included in the standard Python distribution.

## Credits and history

Teddy is made by Damien Gouteux with ❤ in Occitania, 2019-2020.

It origins date back from 2012, a first incarnation using Pygame which was ditched in favor of this incarnation, which started in 2019.
