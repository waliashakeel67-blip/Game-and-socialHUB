# ✨ Game & Social Hub ✨

> Pick up your wand and step into a magical world made with C++ and SFML.

Your mouse is a **glowing wand** that leaves a trail of sparkles wherever you go. Win three magical mini games, let the **Sorting Hat** decide your Hogwarts house, and write a **secret letter** locked with a 4-digit PIN that only your friend can open.

🪄 Sparkling wand cursor | 🎩 Sorting Hat house quiz | 🔮 3 mini games | 💌 Secret letters | 🎶 Magical music

## Contents
- [Features](#features)
- [How to play](#how-to-play)
- [Secret Letters](#secret-letters)
- [Requirements](#requirements)
- [How to run](#how-to-run)
- [Project structure](#project-structure)
- [Known limitations](#known-limitations)
- [Made by](#made-by)
- [Credits](#credits)

## Features
- Main Menu, Game Hub, House Quiz and Secret Letters
- 3 mini games: Guess the Word, Memory Number and Drawing Recreation
- Hogwarts house quiz with 10 questions and a result page with the house crest
- Secret Letters: write a letter, lock it with a 4-digit PIN, and share the PIN with a friend
- Magic wand cursor with sparkles, glowing buttons, background music, click and hover sounds

## How to play

| Screen | What to do |
|---|---|
| Main Menu | Click Game Hub, House Quiz or Secret Letters |
| Guess the Word | Type letters on the keyboard. You have 5 chances. |
| Memory Number | Remember the 8 digits for 5 seconds, then type them. Backspace deletes. |
| Drawing Recreation | Click squares to copy the picture, then press Check. You need 90% correct. |
| House Quiz | Click the Sorting Hat, then answer 10 questions. |
| Secret Letters | Win all 3 games to write a letter. Anyone can read a letter with its PIN. |

Press Enter to start a new round after finishing a game.

## Secret Letters
1. Win Guess the Word, Memory Number and Drawing Recreation.
2. Open Secret Letters and click Next: Write Letter.
3. Write your letter (up to 300 characters), then choose a 4-digit PIN.
4. Tell the PIN to your friend.
5. Your friend opens Secret Letters, clicks Read a Friend's Letter, and types the PIN.

Letters are saved in `secret_letters.txt`. Each PIN can be used for only one letter.

## Requirements
- Windows 10 or 11
- Visual Studio 2022 with the C++ desktop development workload
- SFML 2.x (Graphics, Window, System and Audio)

## How to run
1. Clone or download this repository.
2. Open the `.sln` file in Visual Studio.
3. Make sure SFML is linked in the project settings.
4. Keep the SFML `.dll` files next to the program.
5. Keep the fonts, pictures and sounds in the project folder.
6. Press Ctrl+F5 to build and run.

## Project structure
```
Game-and-socialHUB
├── README.md
├── .gitignore
└── pf project
    ├── FileName.cpp          main game code
    ├── *.png / *.jpeg        pictures and backgrounds
    ├── *.ttf                 fonts
    ├── click.wav, hover.wav  sound effects
    ├── music.ogg             background music
    └── sfml-*.dll            SFML libraries
```

## Known limitations
- Letters are saved on one computer. To share a letter with a friend on another computer, copy `secret_letters.txt` into their game folder.
- The game window is 1000 x 600 pixels.

## Made by
Walia, Ayesha and Sarmad

## Credits
- UI sounds: Micro Sound UI Vol. 1 by Effe Tei (itch.io)
- Music: Pixabay (pixabay.com/music)
- Title font: Harry P (free fan-made font)
- Button font: Cinzel Decorative Bold by Natanael Gama (Google Fonts, SIL Open Font License)
- Body font: Cormorant Garamond Bold by Christian Thalmann (Google Fonts, SIL Open Font License)
- Backgrounds and images: designed in Canva, using images found on [Pinterest](https://www.pinterest.com/) (edited and cropped). All images belong to their original owners.
- Made with [SFML](https://www.sfml-dev.org/)
- Harry Potter is owned by its respective owners. This is a fan-made student project, not for sale.
-
