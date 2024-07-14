**Custom loading screens and loading bars for SDK2013 SP/MP**

**I. Overview**

This project started as an improvement of an <a href="https://developer.valvesoftware.com/wiki/Custom_loading_screen">existing tutorial</a> on Valve Developer Community.

The main goal was to replace the loading bars and add fully-controllable secondary elements such as the % readout, gameplay tips, or a rotating loading wheel.

Both are now achieved, although there's always room for improvement.

The project's been tested on SDK 2013 SP (main focus) and MP. Works in both, but MP lacks some elements such as the secondary loading bar for downloading external files. 

*Pictured: the loading in progress, with all elements active at once.*

![hl2 2024-03-28 20-07-57-148](https://github.com/Cvoxalury/sdk2013-custom-loading-screens/assets/5245025/aea20925-5e4d-4b09-9065-63eeee5b6d7c)

**II. Functionality**

The loading screen works as a VGUI element that replaces the basic loading screen and its tied elements (such as the progress bar), done in clientmode_shared.cpp. 

The image is drawn based on what map is being loaded; however, there's no known way to know it immediately as it begins loading.

It is only at about 70% progress that the engine can be queried for the map name, and the loading background can be updated. 
*Update 14.07.2024. The new map name lookup code now allows for minimal delay when loading saved games or starting new game/chapter, at least in Singleplayer.*

If there's a .VMT present that matches the map name, at materials/vgui/loading, then that image will be used. If there isn't one, or the map name isn't yet known,
one of the default screens will be picked. There's 8 default sample screens provided, all recycling HL2/Episodes blurry menu backgrounds, and 1 named after a map,
ep2_outland_12a. So, when loading any map that isn't ep2_outland_12a, one of the defaults will be picked randomly; if loading that map, it will first show one of the
defaults, then update to a black texture (as specified in ep2_outland_12a.VMT).

The gameplay tips shown during loading are picked from the mod-specific txt file in resource/ folder. There are sample strings provided. The code scans all of the mod's loaded strings, then filters out the ones relevant to the loading screen. If there's no strings defined at all, the tip
label remains empty, so as not to show an error message.

**III. Credits**

<a href="https://github.com/MaestroFenix">Maestra Fénix</a> - the original tutorial, code revision.

<a href="https://developer.valvesoftware.com/wiki/User:Psycommando/VGUI_OverrideTest">Psycommando</a> - the example code of how to find inaccessible (engine-locked) VGUI elements,
used for querying the base progress bar about its progress %.

Cvoxalury - the new code.

<a href="https://www.obsidianconflict.net/">Obsidian Conflict Team</a> - additional code for map name lookup and picking map-specific background textures.

<a href="https://github.com/sylveonsylvia">sylveonsylvia</a> - contributor, fix for compiling the code under Linux

The round lambda logo used as the loading wheel is based on <a href="https://combineoverwiki.net/wiki/File:Lambda_logo.svg">the SVG</a> from Combine OverWiki.

Valve - Source & Source SDK.
