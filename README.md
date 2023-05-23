# Start Menu Icons
1. If you want more icons, add them to graphics/interface/start_menu_icons.png in the right order. Grayscale in the first batch and then coloured in the latter part.

2. You'll need to adjust some entries in start_menu.c if you want your icon to show such as sStartMenuIconFrames, HandleStartMenuInput, DynamicallyLoadStartMenuIcon, etc. You'll need to read through it to know exactly what you need.

3. DNS may darken the icons when late. Its up to you to make an exception for that.. This line might be of use "oam.paletteNum = LoadSpritePalette(&palSheet);"

4. The spritesheet is divided into 2 portions(Grayscale and Coloured) and only the index of the icon in the first portion is needed. It will be appropriately adjusted when needed.

5. Remember to check the defines with comments in start_menu.c and start_menu.h.

6. The start menu's in places like the Battle Pyramid, Safari Zone, etc may look wonky. Changing the placement of options should help.


# Pokémon Emerald

This is a decompilation of Pokémon Emerald.

It builds the following ROM:

* [**pokeemerald.gba**](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=1961) `sha1: f3ae088181bf583e55daf962a92bb46f4f1d07b7`

To set up the repository, see [INSTALL.md](INSTALL.md).


## See also

Other disassembly and/or decompilation projects:
* [**Pokémon Red and Blue**](https://github.com/pret/pokered)
* [**Pokémon Gold and Silver (Space World '97 demo)**](https://github.com/pret/pokegold-spaceworld)
* [**Pokémon Yellow**](https://github.com/pret/pokeyellow)
* [**Pokémon Trading Card Game**](https://github.com/pret/poketcg)
* [**Pokémon Pinball**](https://github.com/pret/pokepinball)
* [**Pokémon Stadium**](https://github.com/pret/pokestadium)
* [**Pokémon Gold and Silver**](https://github.com/pret/pokegold)
* [**Pokémon Crystal**](https://github.com/pret/pokecrystal)
* [**Pokémon Ruby and Sapphire**](https://github.com/pret/pokeruby)
* [**Pokémon Pinball: Ruby & Sapphire**](https://github.com/pret/pokepinballrs)
* [**Pokémon FireRed and LeafGreen**](https://github.com/pret/pokefirered)
* [**Pokémon Mystery Dungeon: Red Rescue Team**](https://github.com/pret/pmd-red)


## Contacts

You can find us on [Discord](https://discord.gg/d5dubZ3) and [IRC](https://web.libera.chat/?#pret).
