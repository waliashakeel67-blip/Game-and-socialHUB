#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace sf;
using namespace std;


//settings

const float CLICK_VOLUME = 60.f;      // 0 = silent, 100 = loudest
const float HOVER_VOLUME = 25.f;      // set to 0 to turn the hover sound off
const float SPARKLE_VOLUME = 8.f;     // wand sparkle sound, set to 0 to turn it off



// helper functions

// Turns a number into text 
string intToString(int n)
{
    if (n == 0)
        return "0";

    bool negative = false;

    if (n < 0)
    {
        negative = true;
        n = -n;
    }

    string result = "";

    while (n > 0)
    {
        result = (char)('0' + n % 10) + result;
        n = n / 10;
    }

    if (negative)
        result = "-" + result;

    return result;
}


// true if c is a letter 
bool isLetter(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}


// makes a capital letter small 
char toLowerChar(char c)
{
    if (c >= 'A' && c <= 'Z')
        return (char)(c + ('a' - 'A'));

    return c;
}


// true if the letter c is somewhere inside s 
bool hasChar(const string& s, char c)
{
    for (int i = 0; i < (int)s.length(); i++)
    {
        if (s[i] == c)
            return true;
    }

    return false;
}


// deletes the last character of s 
void removeLast(string& s)
{
    string result = "";

    for (int i = 0; i < (int)s.length() - 1; i++)
        result += s[i];

    s = result;
}


void setButton(RectangleShape& box, Text& text, string words, float x, float y)
{
    box.setSize(Vector2f(380, 85));
    box.setPosition(x, y);

    text.setString(words);
    text.setFillColor(Color::White);

    if (words.length() > 15)
        text.setCharacterSize(18);
    else
        text.setCharacterSize(22);

    FloatRect t = text.getLocalBounds();

    text.setPosition(
        x + (380 - t.width) / 2 - t.left,
        y + (85 - t.height) / 2 - t.top
    );
}


// Same as setButton but with any size (used for small buttons)
void setSmallButton(RectangleShape& box, Text& text, string words,
    float x, float y, float w, float h, int size)
{
    box.setSize(Vector2f(w, h));
    box.setPosition(x, y);

    text.setString(words);
    text.setFillColor(Color::White);
    text.setCharacterSize(size);

    FloatRect t = text.getLocalBounds();

    text.setPosition(
        x + (w - t.width) / 2 - t.left,
        y + (h - t.height) / 2 - t.top
    );
}


// A small button placed with 5 numbers: x, y, width, height, text size
void setButtonAt(RectangleShape& box, Text& text, string words, const float p[5])
{
    setSmallButton(box, text, words, p[0], p[1], p[2], p[3], (int)p[4]);
}


void setHeading(Text& headingText, string title, int size)
{
    headingText.setString(title);
    headingText.setCharacterSize(size);
    headingText.setFillColor(Color(246, 238, 215));
    headingText.setOutlineColor(Color(55, 45, 80));
    headingText.setOutlineThickness(5);
    headingText.setOrigin(0.0f, 0.0f);

    FloatRect bounds = headingText.getLocalBounds();
    float centeredX = 490.0f - bounds.width / 2.0f - bounds.left;

    headingText.setPosition(centeredX, 70.0f);
}


void setBackground(Sprite& sprite, Texture& tex)
{
    sprite.setTexture(tex, true);
    sprite.setScale(1000.f / tex.getSize().x, 600.f / tex.getSize().y);
}


bool isClicked(RectangleShape& box, Vector2f mouse)
{
    return box.getGlobalBounds().contains(mouse);
}


// Puts a text in the horizontal middle of the screen at height y
void centerText(Text& text, float y)
{
    FloatRect b = text.getLocalBounds();
    text.setPosition(490.0f - b.width / 2.0f - b.left, y);
}


// Centers a text around cx and puts its top edge at y
void centerAt(Text& text, float cx, float y)
{
    FloatRect b = text.getLocalBounds();
    text.setPosition(cx - b.width / 2.0f - b.left, y - b.top);
}


// Puts the top-left of a text at (x, y) exactly
void placeText(Text& text, float x, float y)
{
    FloatRect b = text.getLocalBounds();
    text.setPosition(x - b.left, y - b.top);
}


// Makes a text smaller until it fits in maxW
void fitText(Text& t, float maxW, int startSize, int minSize)
{
    t.setCharacterSize(startSize);

    while (t.getLocalBounds().width > maxW && (int)t.getCharacterSize() > minSize)
        t.setCharacterSize(t.getCharacterSize() - 1);
}


// Breaks a long text into lines that fit in maxW 
string wrapText(const string& s, const Font& font, int size, float maxW)
{
    Text test("", font, size);

    string result = "";
    string line = "";
    string word = "";

    for (int i = 0; i <= (int)s.length(); i++)
    {
        if (i == (int)s.length() || s[i] == ' ')
        {
            string tryLine = line.empty() ? word : line + " " + word;
            test.setString(tryLine);

            if (test.getLocalBounds().width > maxW && !line.empty())
            {
                result += line + "\n";
                line = word;
            }
            else
            {
                line = tryLine;
            }

            word = "";
        }
        else
        {
            word += s[i];
        }
    }

    result += line;
    return result;
}


// Fits an answer inside its row: one line if it fits, otherwise two smaller lines
void fitAnswer(Text& t, const string& s, const Font& font, float maxW, float maxH)
{
    for (int size = 24; size >= 21; size--)
    {
        t.setCharacterSize(size);
        t.setString(s);

        if (t.getLocalBounds().width <= maxW)
            return;
    }

    for (int size = 21; size >= 16; size--)
    {
        t.setCharacterSize(size);
        t.setString(wrapText(s, font, size, maxW));

        if (t.getLocalBounds().height <= maxH)
            return;
    }
}

// glowing of button


const int GLOW_REACH = 22;            // room around the button for the glow (pixels)
const int GLOW_BLUR = 7;              // how far the light spreads 
const float GLOW_STRENGTH = 150.f;    // brightness of the glow, 0 to 255 

// Blurs a grid of numbers between 0 and 1 (used once to build each glow picture)
void blurGrid(float* g, int W, int H, int r)
{
    float* tmp = new float[W * H];

    for (int i = 0; i < W * H; i++)
        tmp[i] = 0.f;
    // horizontal pass
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            float sum = 0.f;

            for (int k = -r; k <= r; k++)
            {
                int xx = x + k;

                if (xx >= 0 && xx < W)
                    sum += g[y * W + xx];
            }

            tmp[y * W + x] = sum / (2 * r + 1);
        }
    }

    // vertical pass
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            float sum = 0.f;

            for (int k = -r; k <= r; k++)
            {
                int yy = y + k;

                if (yy >= 0 && yy < H)
                    sum += tmp[yy * W + x];
            }

            g[y * W + x] = sum / (2 * r + 1);
        }
    }
 delete[] tmp;
}


// Builds a thin glow that follows the real shape of the button picture
// (so it is the same width on the top, the bottom and the sides)
// It is made once for each button size and then reused.
const int GLOW_CACHE_SIZE = 10;       // how many different button sizes can be remembered

Texture& getGlowTexture(Texture& tex, float w, float h)
{
    static Texture cacheTex[GLOW_CACHE_SIZE];
    static long cacheKey[GLOW_CACHE_SIZE];
    static int cacheCount = 0;

    long key = (long)w * 10000L + (long)h;

    for (int i = 0; i < cacheCount; i++)
    {
        if (cacheKey[i] == key)
            return cacheTex[i];
    }

    // place for the new glow (if the list is full the last place is reused)
    int slot = cacheCount;

    if (slot >= GLOW_CACHE_SIZE)
        slot = GLOW_CACHE_SIZE - 1;

    // the button is drawn 6 percent bigger while the mouse is over it
    float bw = w * 1.06f;
    float bh = h * 1.06f;

    int W = (int)bw + 2 * GLOW_REACH;
    int H = (int)bh + 2 * GLOW_REACH;

    Image src = tex.copyToImage();
    float sw = (float)src.getSize().x;
    float sh = (float)src.getSize().y;

      float* g = new float[W * H];

    for (int i = 0; i < W * H; i++)
        g[i] = 0.f;

    // copy the shape of the button (where it is solid and where it is see through)
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            int px = x - GLOW_REACH;
            int py = y - GLOW_REACH;

            if (px >= 0 && px < (int)bw && py >= 0 && py < (int)bh)
            {
                int sx = (int)(px * sw / bw);
                int sy = (int)(py * sh / bh);

                g[y * W + x] = src.getPixel(sx, sy).a / 255.f;
            }
        }
    }

    // two blurs in a row = a smooth soft edge
    blurGrid(g, W, H, GLOW_BLUR);
    blurGrid(g, W, H, GLOW_BLUR);

    Image img;
    img.create(W, H, Color(0, 0, 0, 0));

    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            float a = g[y * W + x] * 1.7f;

            if (a > 1.f)
                a = 1.f;

            img.setPixel(x, y, Color(255, 236, 185, (Uint8)(a * GLOW_STRENGTH)));
        }
    }

  delete[] g;

    cacheTex[slot].loadFromImage(img);
    cacheTex[slot].setSmooth(true);
    cacheKey[slot] = key;

    if (cacheCount < GLOW_CACHE_SIZE)
        cacheCount++;

    return cacheTex[slot];


// Buttons grow a little and glow when the mouse is over them
void drawButton(RenderWindow& window, RectangleShape& box, Texture& tex, Vector2f mouse)
{
    Sprite s(tex);

    float w = box.getSize().x;
    float h = box.getSize().y;

    float sx = w / tex.getSize().x;
    float sy = h / tex.getSize().y;

    if (box.getGlobalBounds().contains(mouse))
    {
        // thin light around the edges only added on top of the background
        Sprite glow(getGlowTexture(tex, w, h));
        glow.setPosition(box.getPosition().x - w * 0.03f - GLOW_REACH,
            box.getPosition().y - h * 0.03f - GLOW_REACH);
        window.draw(glow, RenderStates(BlendAdd));

        s.setScale(sx * 1.06f, sy * 1.06f);
        s.setPosition(box.getPosition().x - w * 0.03f,
            box.getPosition().y - h * 0.03f);
    }
    else
    {
        s.setScale(sx, sy);
        s.setPosition(box.getPosition());
    }

    window.draw(s);
}


// Turns "dobby" into "d o b b y"
string spaced(string s)
{
    string result = "";

    for (int i = 0; i < (int)s.length(); i++)
    {
        result += s[i];
        if (i != (int)s.length() - 1)
            result += ' ';
    }

    return result;
}


// Turns "12345678" into "12 34 56 78"
string makePairs(string s)
{
    string result = "";

    for (int i = 0; i < (int)s.length(); i++)
    {
        result += s[i];

        if (i % 2 == 1 && i != (int)s.length() - 1)
            result += ' ';
    }

    return result;
}


//sparkling of wand


struct Spark
{
    Vector2f pos;
    Vector2f vel;
    float life;
    float maxLife;
    float size;
    float rot;
};


// Draws a small 4-point star
void drawStar(RenderWindow& window, Vector2f p, float r, Color c, float rot)
{
    ConvexShape s(8);

    for (int i = 0; i < 8; i++)
    {
        float a = rot + i * 3.14159265f / 4.0f;
        float rad = (i % 2 == 0) ? r : r * 0.28f;
        s.setPoint(i, Vector2f(cos(a) * rad, sin(a) * rad));
    }

    s.setPosition(p);
    s.setFillColor(c);
    window.draw(s, RenderStates(BlendAdd));
}

// DRAWING RECREATION PICTURES

const int GRID = 8;   // number of boxes per row and per column

// The 3 emoji pictures. '#' = colored square, '.' = empty
const char* pictures[3][GRID] = {
    { // Smiley
      "..####..",
      ".#....#.",
      "#.#..#.#",
      "#......#",
      "#.#..#.#",
      "#..##..#",
      ".#....#.",
      "..####.." },
    { // Sad face
      "..####..",
      ".#....#.",
      "#.#..#.#",
      "#......#",
      "#..##..#",
      "#.#..#.#",
      ".#....#.",
      "..####.." },
    { // Heart
      ".##..##.",
      "########",
      "########",
      "########",
      ".######.",
      "..####..",
      "...##...",
      "........" }
};

// Copies picture number n into the target grid and clears the player grid
void loadPicture(int n, int target[GRID][GRID], int player[GRID][GRID])
{
    for (int r = 0; r < GRID; r++)
    {
        for (int c = 0; c < GRID; c++)
        {
            target[r][c] = (pictures[n][r][c] == '#') ? 1 : 0;
            player[r][c] = 0;
        }
    }
}

// MAIN

int main()
{
    RenderWindow window(VideoMode(1000, 600), "Game & Social Hub");
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);      // the wand is the cursor

    srand((unsigned)time(0));


    // The game always thinks in 1000x600. When the window is maximized,
    // the view keeps the same shape (black bars if needed), and
    // mapPixelToCoords turns real mouse positions into game positions
    View view(FloatRect(0.f, 0.f, 1000.f, 600.f));
    window.setView(view);

    auto fixView = [&](unsigned winW, unsigned winH)
        {
            float windowRatio = (float)winW / (float)winH;
            float viewRatio = 1000.f / 600.f;

            float sizeX = 1.f, sizeY = 1.f, posX = 0.f, posY = 0.f;

            if (windowRatio >= viewRatio)
            {
                sizeX = viewRatio / windowRatio;
                posX = (1.f - sizeX) / 2.f;
            }
            else
            {
                sizeY = windowRatio / viewRatio;
                posY = (1.f - sizeY) / 2.f;
            }

            view.setViewport(FloatRect(posX, posY, sizeX, sizeY));
            window.setView(view);
        };


    // FONTS
    Font titleFont, buttonFont, bodyFont;

    if (!titleFont.loadFromFile("titlefont.ttf"))
        return -1;

    if (!buttonFont.loadFromFile("buttonfont.ttf"))
        return -1;

    if (!bodyFont.loadFromFile("bodyfont.ttf"))      // Cormorant Garamond Bold
        return -1;


    // TEXTURES
    Texture menuTex, hubTex, quizTex, letterTex, btnTex,guessTex,memTex,drawTex;

    if (!menuTex.loadFromFile("menu.png"))
        return -1;

    if (!hubTex.loadFromFile("hub.png"))
        return -1;

    // the quiz design (used for the start page, the questions and the result)
    if (!quizTex.loadFromFile("design7.png"))
        return -1;

    if (!letterTex.loadFromFile("letters.png"))
        return -1;

    if (!btnTex.loadFromFile("button.png"))
        return -1;
     if (!guessTex.loadFromFile("guess.png"))
        return -1;

    if (!memTex.loadFromFile("mem.png"))
        return -1;

    if (!drawTex.loadFromFile("draw.png"))
        return -1;

    btnTex.setSmooth(true);

    // Quiz pictures
    Texture hatTex, hatGlowTex, wandTex;

    if (!hatTex.loadFromFile("hat.png")) return -1;
    if (!hatGlowTex.loadFromFile("hat_glow.png")) return -1;
    if (!wandTex.loadFromFile("wand.png")) return -1;

    hatTex.setSmooth(true);
    hatGlowTex.setSmooth(true);
    wandTex.setSmooth(true);

    // houses: 0 = Gryffindor, 1 = Slytherin, 2 = Ravenclaw, 3 = Hufflepuff
    string houseFile[4] = { "gryffindor", "slytherin", "ravenclaw", "hufflepuff" };

    Texture crestBig[4];

    for (int i = 0; i < 4; i++)
    {
        if (!crestBig[i].loadFromFile("crest_" + houseFile[i] + ".png")) return -1;

        crestBig[i].setSmooth(true);
    }

    Sprite background;

    Text heading("", titleFont, 60);


    // BUTTON SOUNDS // have not ye downloaded /seleced button sounds will complete later
    SoundBuffer clickBuf, hoverBuf;
    Sound clickSound, hoverSound;

    bool soundsOK = clickBuf.loadFromFile("click.wav") && hoverBuf.loadFromFile("hover.wav");

    if (soundsOK)
    {
        clickSound.setBuffer(clickBuf);
        hoverSound.setBuffer(hoverBuf);

        clickSound.setVolume(CLICK_VOLUME);
        hoverSound.setVolume(HOVER_VOLUME);
    }

    int lastHover = 0;      // which button the mouse was over in the last frame


    // wand sparkle sound
    SoundBuffer sparkleBuf;
    Sound sparkleSound[4];

    bool sparkleOK = sparkleBuf.loadFromFile("sparkle.wav");

    if (sparkleOK)
    {
        for (int i = 0; i < 4; i++)
            sparkleSound[i].setBuffer(sparkleBuf);
    }

    int nextSparkleSound = 0;
    float sparkleSoundTimer = 0.f;
    Vector2f prevMouse(0.f, 0.f);


    // BUTTONS (b1, b2, b3 are used differently on each screen)
    RectangleShape b1, b2, b3, back;
    Text t1("", buttonFont, 22);
    Text t2("", buttonFont, 22);
    Text t3("", buttonFont, 22);
    Text tBack("", buttonFont, 22);


    // scree numbers
    // 0 = Main Menu
    // 1 = Game Hub
    // 2 = House Quiz
    // 3 = Secret Letters
    // 4 = Guess the Word
    // 5 = Memory Number
    // 6 = Drawing Recreation
    int screen = 0;
    int lastScreen = -1;


    // WINS (Secret Letters unlock after all 3 games are won)
    bool wonGuess = false;
    bool wonMemory = false;
    bool wonDrawing = false;

    Text lockMsg("", buttonFont, 28);
    Text lockCount("", buttonFont, 28);

    lockMsg.setFillColor(Color::White);
    lockCount.setFillColor(Color::White);

    Text drawMsg("", buttonFont, 30);
    drawMsg.setFillColor(Color(255,240,190);



    // wand cursor plus sparkles


    Sprite wand(wandTex);
    wand.setOrigin(297.f, 1.f);        // the tip of the wand (top right of the picture)
    wand.setScale(-0.5f, 0.5f);         // 300px picture shown at 150px

    vector<Spark> sparks;
    Clock frameClock;
    float sparkTimer = 0.f;
    float totalTime = 0.f;
    bool mouseInside = false;



    // harry potter house personality quiz variables


    const int TOTAL_Q = 10;

    string houseNames[4] = { "Gryffindor", "Slytherin", "Ravenclaw", "Hufflepuff" };

    string houseDesc[4] =
    {
        "You act first and worry later, and you stand up for what is right even when your knees shake. People follow you because you walk into the storm with a smile.",
        "You see a door where others see a wall. Driven and sharp, you plan carefully, aim high and never settle for being ordinary.",
        "Your mind is never still. You love a good puzzle, a strange idea, and the thrill of understanding something nobody ever explained to you.",
        "You are the heart of any group. Kind, steady and generous, you work hard without needing applause and you never leave a friend behind."
    };

    string houseTraits[4] =
    {
        "Courage  -  Daring  -  Chivalry",
        "Ambition  -  Cunning  -  Resourcefulness",
        "Wisdom  -  Curiosity  -  Creativity",
        "Loyalty  -  Patience  -  Fair Play"
    };

    string houseFacts[4] =
    {
        "Scarlet and Gold   |   Element: Fire",
        "Green and Silver   |   Element: Water",
        "Blue and Bronze   |   Element: Air",
        "Yellow and Black   |   Element: Earth"
    };

    string houseMotto[4] =
    {
        "\"Brave at heart\"",
        "\"Aim higher\"",
        "\"Think beyond\"",
        "\"Loyal above all\""
    };

    // dark colors, used for the see-through panel behind the description
    Color houseColors[4] =
    {
        Color(140, 25, 25),
        Color(20, 95, 50),
        Color(25, 55, 135),
        Color(170, 115, 10)
    };

    // the house name two dark colors taken from each crest
    Color houseMain[4] =
    {
        Color(125, 30, 36),         // Gryffindor the dark red of the crest
        Color(28, 78, 52),          // Slytherin dark green
        Color(28, 42, 105),         // Ravenclaw dark blue
        Color(150, 105, 18)         // Hufflepuff dark gold
    };

    Color houseOutline[4] =
    {
        Color(40, 30, 28),          // near black
        Color(15, 18, 14),          // near black
        Color(16, 14, 22),          // near black
        Color(57, 51, 43)           // dark bronze
    };

    Color inkColor(85, 48, 20);       // dark brown text on the parchment

    string questions[TOTAL_Q] =
    {
        "You find a locked door in an old castle corridor. What do you do?",
        "Your team loses because of a mistake you made. What comes first?",
        "Which of these would be the best prize?",
        "Someone nearby is being treated unfairly. You...",
        "How do you like to spend a rainy afternoon?",
        "Which of these would bother you the most?",
        "At a great feast, where would you like to sit?",
        "A small creature is stuck in a trap. What do you do?",
        "Which strength would you most like to have?",
        "Years from now, how would you like to be remembered?"
    };

    string options[TOTAL_Q][4] =
    {
        { "Open it at once and see what is inside", "Look for a hidden key or a clue", "Wait for my friends before going in", "Find out who wants it kept shut" },
        { "Understand exactly what went wrong", "Hope my team is not upset with me", "Make it right, whatever it takes", "Make sure I win next time" },
        { "A medal for a daring deed", "A feast shared with all my friends", "A title that gives me real influence", "A rare book nobody has read" },
        { "Quietly comfort them afterwards", "Work out who holds the power here", "Speak up, even if it costs me", "Find a clever way to fix it" },
        { "Plan my next big project", "Dash outside in the rain anyway", "Read something strange and new", "Bake something for the people I love" },
        { "Being called foolish", "Being ordinary and unnoticed", "Being called a coward", "Letting a friend down" },
        { "Anywhere my friends are sitting", "Right in the middle of the action", "Near the most important guests", "Somewhere quiet with good talk" },
        { "Study the trap before touching it", "Think about what I could gain", "Free it quickly before it is hurt", "Stay with it gently until help comes" },
        { "A brilliant mind", "Unshakable courage", "Unstoppable determination", "Endless loyalty" },
        { "As someone who was always there for others", "As someone who never backed down", "As someone who changed how people think", "As someone who achieved great things" }
    };

    // which house each option gives a point to
    // 0 = Gryffindor, 1 = Slytherin, 2 = Ravenclaw, 3 = Hufflepuff
    int optHouse[TOTAL_Q][4] =
    {
        { 0, 2, 3, 1 },
        { 2, 3, 0, 1 },
        { 0, 3, 1, 2 },
        { 3, 1, 0, 2 },
        { 1, 0, 2, 3 },
        { 2, 1, 0, 3 },
        { 3, 0, 1, 2 },
        { 2, 1, 0, 3 },
        { 2, 0, 1, 3 },
        { 3, 0, 2, 1 }
    };

    int houseScore[4] = { 0, 0, 0, 0 };
    int resultHouse = 0;

    int qIndex = 0;
    bool quizStarted = false;
    bool quizDone = false;

    RectangleShape opt[4];
    Text optText[4];

    for (int i = 0; i < 4; i++)
    {
        optText[i].setFont(bodyFont);
        optText[i].setFillColor(inkColor);

    }

    // Start page texts
    Text quizTitle("Discover Your True", titleFont, 40);
    Text quizTitle2("Hogwarts House", titleFont, 40);
    Text quizIntro("", bodyFont, 22);
    Text quizPrompt("Click the Sorting Hat to start your quiz", bodyFont, 24);

    quizTitle.setFillColor(Color(60, 30, 12));
    quizTitle2.setFillColor(Color(60, 30, 12));
    quizIntro.setFillColor(Color(70, 38, 15));
    quizPrompt.setFillColor(Color(45, 22, 6));          // dark brown

    // Question page texts
    Text quizInfo("", bodyFont, 25);
    Text quizPercent("", bodyFont, 25);
    Text quizQuestion("", bodyFont, 26);
    Text letterText("", bodyFont, 21);

    quizInfo.setFillColor(inkColor);
    quizPercent.setFillColor(inkColor);
    quizQuestion.setFillColor(Color(90, 50, 20));
    letterText.setFillColor(Color(120, 65, 25));


    float barFill = 0.f;      // 0.0 to 1.0

    // Result page texts
    Text resSmall("You belong to...", bodyFont, 26);
    Text resName("", titleFont, 50);
    Text resDesc("", bodyFont, 22);
    Text resTraits("", bodyFont, 21);
    Text resFacts("", bodyFont, 20);
    Text resMotto("", bodyFont, 24);

    resSmall.setFillColor(Color(55, 28, 10));
    resDesc.setFillColor(Color(55, 28, 10));
    resTraits.setFillColor(Color(50, 25, 8));
    resFacts.setFillColor(Color(55, 28, 10));
    resMotto.setFillColor(Color(80, 40, 15));

    // see through panel in the house color, behind the description
    RectangleShape descPanel;

    // Sorting Hat on the start page
    Sprite hat(hatTex);
    Sprite hatGlow(hatGlowTex);

    hat.setOrigin(hatTex.getSize().x / 2.f, hatTex.getSize().y / 2.f);
    hatGlow.setOrigin(hatGlowTex.getSize().x / 2.f, hatGlowTex.getSize().y / 2.f);
    //its size and place are set in layoutQuizPage

    // Large crest on the result page
    Sprite crestResult;



    // where thihgs go on this design of harry potter quiz (the picture is 1000 x 600 pixels)


    //start page 
    const float START_CX = 520.f;       // middle of the start page text and the hat
    const float TITLE_Y = 90.f;         // top of the title
    const float TITLE_MAXW = 480.f;   // the title gets smaller if it is wider than this
    const int TITLE_SIZE = 50;       // biggest size of the title
    const float TITLE_GAP = 20.f;     // space under the title
    const int INTRO_SIZE = 22;       // size of the intro text
    const float INTRO_W = 480.f;         // width of the intro text
    const float INTRO_GAP = 18.f;     // space under the intro text
    const float HAT_H = 115.f;             // height of the Sorting Hat
    const float HAT_GAP = 14.f;         // space under the hat
    const int PROMPT_SIZE = 24;     // size of "Click the Sorting Hat..."
    const string INTRO_TEXT = "Take our magical house quiz and let the Sorting Hat decide where you truly belong. Are you brave, wise, loyal or ambitious?";

    // question page
    const float INFO_X = 285.f;                 // "Question 1 of 10"
    const float INFO_Y = 100.f;
    const int INFO_SIZE = 22;
    const float PERCENT_RIGHT = 810.f;   // right edge of "10% Complete"
    const float BAR_X = 285.f;                   // progress bar
    const float BAR_Y = 132.f;
    const float BAR_W = 525.f;
    const float Q_X = 285.f;                       // the question
    const float Q_Y = 158.f;
    const float Q_W = 520.f;
    const int Q_SIZE = 28;
    const int COLS = 1;                         // answers in 1 or 2 columns
    const float ROW_X0 = 285.f;                 // left edge of column 1
    const float ROW_X1 = 285.f;                 // left edge of column 2
    const float ROW_Y = 252.f;                   // top of the first answer
    const float ROW_W = 400.f;                   // size of one answer
    const float ROW_H = 54.f;
    const float ROW_STEP = 62.f;             // distance between answers

    //buttons: x, y, width, height, text size
    const float BACK_START[5] = { 142, 510, 112, 40, 18 };
    const float BACK_Q[5] = { 142, 510, 112, 40, 18 };
    const float BACK_R[5] = { 142, 510, 112, 40, 18 };
    const float PLAY_R[5] = { 142, 462, 112, 40, 17 };


    // Places the texts,the hat and the buttons for the quiz page that is showing
    // the start page, a question page, or the result page
    auto layoutQuizPage = [&]()
        {
            if (!quizStarted)
            {
                // start page

                // title in two lines, so it can be big
                quizTitle.setString("Discover Your True");
                quizTitle2.setString("Hogwarts House");
                fitText(quizTitle, TITLE_MAXW, TITLE_SIZE, 22);
                quizTitle2.setCharacterSize(quizTitle.getCharacterSize());

                float lineH = quizTitle.getCharacterSize() * 1.12f;
                centerAt(quizTitle, START_CX, TITLE_Y);
                centerAt(quizTitle2, START_CX, TITLE_Y + lineH);

                float y = TITLE_Y + lineH + quizTitle2.getLocalBounds().height + TITLE_GAP;

                quizIntro.setCharacterSize(INTRO_SIZE);
                quizIntro.setString(wrapText(INTRO_TEXT, bodyFont, INTRO_SIZE, INTRO_W));
                centerAt(quizIntro, START_CX, y);
                y += quizIntro.getLocalBounds().height + INTRO_GAP;

                // the Sorting Hat (also the start button)
                float hs = HAT_H / hatTex.getSize().y;

                hat.setScale(hs, hs);
                hatGlow.setScale(hs, hs);
                hat.setPosition(START_CX, y + HAT_H / 2.f);
                hatGlow.setPosition(START_CX, y + HAT_H / 2.f);
                y += HAT_H + HAT_GAP;

                quizPrompt.setCharacterSize(PROMPT_SIZE);
                centerAt(quizPrompt, START_CX, y);

                setButtonAt(back, tBack, "Back", BACK_START);
            }
            else if (!quizDone)
            {
                setButtonAt(back, tBack, "Back", BACK_Q);
            }
            else
            {
                setButtonAt(back, tBack, "Back", BACK_R);
                setButtonAt(b3, t3, "Play Again", PLAY_R);
            }
        };


    // Puts the current question and answers on the screen
    auto loadQuestion = [&]()
        {
            quizInfo.setCharacterSize(INFO_SIZE);
            quizInfo.setString("Question " + intToString(qIndex + 1) + " of " +
                intToString(TOTAL_Q));
            placeText(quizInfo, INFO_X, INFO_Y);

            int percent = (qIndex + 1) * 100 / TOTAL_Q;
            quizPercent.setCharacterSize(INFO_SIZE);
            quizPercent.setString(intToString(percent) + "% Complete");
            FloatRect pb = quizPercent.getLocalBounds();
            quizPercent.setPosition(PERCENT_RIGHT - pb.width - pb.left, INFO_Y - pb.top);

            barFill = (float)(qIndex + 1) / (float)TOTAL_Q;

            quizQuestion.setCharacterSize(Q_SIZE);
            quizQuestion.setString(wrapText(questions[qIndex], bodyFont, Q_SIZE, Q_W));
            placeText(quizQuestion, Q_X, Q_Y);

            for (int i = 0; i < 4; i++)
            {
                int col = (COLS == 2) ? (i % 2) : 0;
                int row = (COLS == 2) ? (i / 2) : i;

                float rowX = (col == 0) ? ROW_X0 : ROW_X1;
                float rowY = ROW_Y + row * ROW_STEP;

                opt[i].setSize(Vector2f(ROW_W, ROW_H));
                opt[i].setPosition(rowX, rowY);

                fitAnswer(optText[i], options[qIndex][i], bodyFont, ROW_W - 66.f, ROW_H - 8.f);

                FloatRect tb = optText[i].getLocalBounds();
                optText[i].setPosition(rowX + 58.f - tb.left, rowY + (ROW_H - tb.height) / 2.f - tb.top);
            }
        };


    // Starts the quiz questions
    auto startQuiz = [&]()
        {
            qIndex = 0;

            for (int i = 0; i < 4; i++)
                houseScore[i] = 0;

            quizStarted = true;
            quizDone = false;
            loadQuestion();
            layoutQuizPage();
        };


    // Finds the house with the most points (random choice if there is a tie)
    // and prepares everything the result page needs
    auto findHouse = [&]()
        {
            int best = houseScore[0];

            for (int i = 1; i < 4; i++)
            {
                if (houseScore[i] > best)
                    best = houseScore[i];
            }

            int tied[4];
            int count = 0;

            for (int i = 0; i < 4; i++)
            {
                if (houseScore[i] == best)
                {
                    tied[count] = i;
                    count++;
                }
            }

            resultHouse = tied[rand() % count];

            // big crest, top left of the sheet
            crestResult.setTexture(crestBig[resultHouse], true);
            float crestH = 195.f;
            float sc = crestH / crestBig[resultHouse].getSize().y;
            crestResult.setScale(sc, sc);
            crestResult.setOrigin(crestBig[resultHouse].getSize().x / 2.f,
                crestBig[resultHouse].getSize().y / 2.f);
            crestResult.setPosition(372.f, 172.f);

            // "You belong to..." and the house name, to the right of the crest
            const float NX = 470.f;

            resSmall.setCharacterSize(24);
            placeText(resSmall, NX, 106.f);

            resName.setString(houseNames[resultHouse]);
            resName.setCharacterSize(54);
            fitText(resName, 340.f, 54, 30);
            resName.setFillColor(houseMain[resultHouse]);
            resName.setOutlineColor(houseOutline[resultHouse]);
            resName.setOutlineThickness(2.f);
            placeText(resName, NX, 136.f);

            //see through panel under them=description, traits, colors / element, motto
            const float TX = 285.f;// left edge of the panel
            const float TW = 395.f;// width of the panel
            const float panelTop = 282.f;// where the panel starts

            float y = panelTop + 9.f;

            resDesc.setCharacterSize(20);
            resDesc.setString(wrapText(houseDesc[resultHouse], bodyFont, 20, TW - 24.f));
            placeText(resDesc, TX + 12.f, y);
            y += resDesc.getLocalBounds().height + 11.f;

            resTraits.setString(houseTraits[resultHouse]);
            fitText(resTraits, TW - 24.f, 19, 14);
            placeText(resTraits, TX + 12.f, y);
            y += resTraits.getLocalBounds().height + 9.f;

            resFacts.setString(houseFacts[resultHouse]);
            fitText(resFacts, TW - 24.f, 19, 14);
            placeText(resFacts, TX + 12.f, y);
            y += resFacts.getLocalBounds().height + 9.f;

            resMotto.setCharacterSize(21);
            resMotto.setString(houseMotto[resultHouse]);
            placeText(resMotto, TX + 12.f, y);
            y += resMotto.getLocalBounds().height + 10.f;

            // as tall as everything inside it
            Color pc = houseMain[resultHouse];

            descPanel.setSize(Vector2f(TW, y - panelTop));
            descPanel.setPosition(TX, panelTop);
            descPanel.setFillColor(Color(pc.r, pc.g, pc.b, 60));
            descPanel.setOutlineThickness(1.5f);
            descPanel.setOutlineColor(Color(pc.r, pc.g, pc.b, 150));
        };

    // game guess the word vriables

    string word = "";
    string hidden = "";
    string used = "";
    string guessMessage = "";

    int chances = 5;
    bool won = false;
    bool lost = false;

    Text gameWord("", buttonFont, 50);
    Text gameChances("", buttonFont, 25);
    Text gameUsed("", buttonFont, 25);
    Text gameMessage("", buttonFont, 28);

    gameWord.setFillColor(Color(255,255,255);  // blanks white ivory
    gameChances.setFillColor(Color(255,255,255);
    gameUsed.setFillColor(Color(255,255,255);
    gameMessage.setFillColor(Color(255,255,255); // type one letter and msgs


    // The words, and which letters are shown at the start
    // Every copy of a shown letter is revealed, so no letter is half-shown
    string guessWords[10] =
    {
        "hogwarts", "voldemort", "halfblood", "mudblood", "azkaban",
        "dumbledore", "bellatrix", "horcrux", "hagrid", "dobby"
    };

    string guessReveal[10] =
    {
        "hw", "ler", "hfo", "ubd", "zn",
        "ul", "bax", "hc", "hg", "oy"
    };


    // Places the Guess the Word buttons depending on the state of the game
auto layoutGuessButtons = [&]()
{
    if (won || lost)
    {
        bool allWon = wonGuess && wonMemory && wonDrawing;

        setSmallButton(b1, t1, allWon ? "Secret Letters" : "Next Game", 190, 400, 280, 62, 20);
        setSmallButton(b2, t2, won ? "New Word" : "Try Again", 510, 400, 280, 62, 20);
        setSmallButton(back, tBack, "Back", 350, 490, 280, 62, 20);
    }
    else
    {
        setSmallButton(b2, t2, "New Word", 190, 505, 280, 62, 20);
        setSmallButton(back, tBack, "Back", 510, 505, 280, 62, 20);
    }
};



    // Starts a new round of Guess the Word
    auto startGuessGame = [&]()
        {
            int r = rand() % 10;

            word = guessWords[r];
            used = guessReveal[r];// revealed letters count as used
            hidden = "";

            for (int i = 0; i < (int)word.length(); i++)
            {
                if (used.find(word[i]) != string::npos)
                    hidden += word[i];
                else
                    hidden += '_';
            }

            chances = 5;
            won = false;
            lost = false;
            guessMessage = "Type one letter";

            layoutGuessButtons();
        };
    // game mempry umber variabels

    string memSequence = "";// the 8 digits to remember (4 pairs)
    string memTyped = "";// what the player has typed
    string memMessage = "";

    // memPhase: 0 = showing numbers, 1 = typing, 2 = won, 3 = lost
    int memPhase = 0;

    Clock memClock;

    Text memNumber("", buttonFont, 50);
    Text memInfo("", buttonFont, 28);
    Text memMsg("", buttonFont, 28);

    memNumber.setFillColor(Color(255, 222, 110));
    memInfo.setFillColor(Color(255, 222, 110));
    memMsg.setFillColor(Color(255, 222, 110));


    // Places the Memory Number buttons depending on the state of the game
auto layoutMemoryButtons = [&]()
{
    if (memPhase >= 2)
    {
        bool allWon = wonGuess && wonMemory && wonDrawing;
        
        setSmallButton(b1, t1, allWon ? "Secret Letters" : "Next Game", 190, 400, 280, 62, 20);
        setSmallButton(b2, t2, memPhase == 2 ? "New Round" : "Try Again", 510, 400, 280, 62, 20);
        setSmallButton(back, tBack, "Back", 350, 490, 280, 62, 20);
    }
    else
    {
        setSmallButton(b2, t2, "New Round", 190, 505, 280, 62, 20);
        setSmallButton(back, tBack, "Back", 510, 505, 280, 62, 20);
    }
};


    // Starts a new round of Memory Number
    auto startMemoryGame = [&]()
        {
            memSequence = "";

            for (int i = 0; i < 4; i++)
            {
                int n = rand() % 100;

                if (n < 10)
                    memSequence += '0';

                memSequence += to_string(n);
            }

            memTyped = "";
            memMessage = "";
            memPhase = 0;
            memClock.restart();

            layoutMemoryButtons();
        };



    // drawing recreation variables


    const int DRAW_N = GRID; // grid is 8 x 8
    const float DRAW_CELL = 35.f;// size of one square (8 x 35 = 280 px)
    const float DRAW_Y = 160.f; // top of both grids
    const float DRAW_X_TARGET = 150.f; // left grid (the picture to copy)
    const float DRAW_X_PLAYER = 550.f;// right grid (the player draws here)
    const int DRAW_NEEDED = GRID * GRID * 9 / 10;// 90% must match (57 out of 64)

    int drawTarget[GRID][GRID];
    int drawPlayer[GRID][GRID];
    int drawCurrent = -1;// which emoji is showing (0, 1 or 2)
    string drawMessage = "";

    // drawPhase: 0 = drawing, 1 = won, 2 = lost
    int drawPhase = 0;

    Text drawLabel("", buttonFont, 22);
    drawLabel.setFillColor(Color(255,232,170);


    // draw the buttons based on curent state of the game
auto layoutDrawingButtons = [&]()
{
    bool allWon = wonGuess && wonMemory && wonDrawing;
 
    if (drawPhase != 0)
    {
        setSmallButton(b1, t1, allWon ? "Secret Letters" : "Next Game", 150, 508, 200, 54, 20);
        setSmallButton(b2, t2, drawPhase == 1 ? "Play Again" : "Try Again", 390, 508, 200, 54, 20);
    }
    else
    {
        setSmallButton(b1, t1, "Check", 150, 508, 200, 54, 20);
        setSmallButton(b2, t2, "Clear", 390, 508, 200, 54, 20);
    }
 
    setSmallButton(back, tBack, "Back", 630, 508, 200, 54, 20);
};

    // Starts a round of Drawing Recreation.
    // newPicture = true     a different random emoji
    // newPicture = false    same emoji, just a clean grid
    auto startDrawingGame = [&](bool newPicture)
        {
            if (newPicture || drawCurrent < 0)
            {
                int next;

                do
                {
                    next = rand() % 3;
                } while (next == drawCurrent);

                drawCurrent = next;
            }

            loadPicture(drawCurrent, drawTarget, drawPlayer);

            drawPhase = 0;
            drawMessage = "Copy the picture on the left, then press Check";

            layoutDrawingButtons();
        };


    // Compares the two grids (Enter or the Check button)
    auto checkDrawing = [&]()
        {
            int matches = 0;

            for (int r = 0; r < DRAW_N; r++)
            {
                for (int c = 0; c < DRAW_N; c++)
                {
                    if (drawPlayer[r][c] == drawTarget[r][c])
                        matches++;
                }
            }

            if (matches >= DRAW_NEEDED)
            {
                drawPhase = 1;
                wonDrawing = true;

                drawMessage = (wonGuess && wonMemory && wonDrawing)
                    ? "You won! Secret Letters unlocked!"
                    : "You won! Choose what to play next";
            }
            else
            {
                drawPhase = 2;
                 drawMessage = "You lost Drawing Recreation! You matched " + intToString(matches) + " / " + intToString(GRID * GRID);
            }

            layoutDrawingButtons();
        };


    // Decides where (Next Game) goes=
    // all 3 games won              Secret Letters
    // another game not won         one of those (random if there are two)
    // nothing else to play         Game Hub
    auto pickNextScreen = [&](int current) -> int
        {
            if (wonGuess && wonMemory && wonDrawing)
                return 3;

            int choices[3];
            int count = 0;

            if (current != 4 && !wonGuess) { choices[count] = 4; count++; }
            if (current != 5 && !wonMemory) { choices[count] = 5; count++; }
            if (current != 6 && !wonDrawing) { choices[count] = 6; count++; }

            if (count == 0)
                return 1;

            return choices[rand() % count];
        };


    // Tells which button (if any) is under the point p used for the button sounds
    // 0 = nothing, 1 = b1, 2 = b2, 3 = b3, 4 = back, 10 = Sorting Hat, 20-23 = quiz answers
    auto hoveredButton = [&](Vector2f p) -> int
        {
            if (screen == 0)
            {
                if (isClicked(b1, p)) return 1;
                if (isClicked(b2, p)) return 2;
                if (isClicked(b3, p)) return 3;
            }
            else if (screen == 1)
            {
                if (isClicked(b1, p)) return 1;
                if (isClicked(b2, p)) return 2;
                if (isClicked(b3, p)) return 3;
                if (isClicked(back, p)) return 4;
            }
            else if (screen == 2)
            {
                if (isClicked(back, p)) return 4;

                if (!quizStarted)
                {
                    if (hat.getGlobalBounds().contains(p)) return 10;
                }
                else if (!quizDone)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        if (isClicked(opt[i], p)) return 20 + i;
                    }
                }
                else
                {
                    if (isClicked(b3, p)) return 3;
                }
            }
            else if (screen == 3)
            {
                if (isClicked(back, p)) return 4;
            }
            else if (screen == 6)
            {
                if (isClicked(back, p)) return 4;
                if (isClicked(b1, p)) return 1;
                if (isClicked(b2, p)) return 2;
            }
            else if (screen == 4)
            {
                if (isClicked(back, p)) return 4;
                if ((won || lost) && isClicked(b1, p)) return 1;
                if (isClicked(b2, p)) return 2;
            }
            else if (screen == 5)
            {
                if (isClicked(back, p)) return 4;
                if (memPhase >= 2 && isClicked(b1, p)) return 1;
                if (isClicked(b2, p)) return 2;
            }

            return 0;
        };

    // main gaem loop

    while (window.isOpen())
    {
        float dt = frameClock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;
        totalTime += dt;



        //set up screen (runs once when the screen changes)


        if (screen != lastScreen)
        {
            lastScreen = screen;

            if (screen == 0)
            {
                setBackground(background, menuTex);
                setHeading(heading, "Main Menu", 100);

                setButton(b1, t1, "Game Hub", 300, 220);
                setButton(b2, t2, "House Quiz", 300, 330);
                setButton(b3, t3, "Secret Letters", 300, 440);
            }
            else if (screen == 1)
            {
                setBackground(background, hubTex);
                setHeading(heading, "Game Hub", 100);

                setButton(b1, t1, "Guess the Word", 300, 200);
                setButton(b2, t2, "Drawing Recreation", 300, 295);
                setButton(b3, t3, "Memory Number", 300, 390);
                setButton(back, tBack, "Back", 300, 485);
            }
            else if (screen == 2)
            {
                setBackground(background, quizTex);

                quizStarted = false;// show the start page (with the hat) first
                quizDone = false;

                layoutQuizPage();
            }
            else if (screen == 3)
            {
                setBackground(background, letterTex);
                setHeading(heading, "Secret Letters", 80);

                setButton(back, tBack, "Back", 310, 495);
            }
            else if (screen == 4)
            {
               setBackground(background, guessTex);
               setHeading(heading, "Guess the Word", 70);
               heading.setFillColor(Color(232, 238, 232));
               heading.setOutlineColor(Color(15, 55, 38));
                
               startGuessGame();
            }
            else if (screen == 5)
            {
                setBackground(background, memTex);
                setHeading(heading, "Memory Number", 75);
                heading.setFillColor(Color(255, 236, 170));
                heading.setOutlineColor(Color(60, 10, 15));

                startMemoryGame();
            }
            else if (screen == 6)
            {
               setBackground(background, drawTex);
               setHeading(heading, "Drawing Recreation", 50);
               heading.setPosition(heading.getPosition().x, 30.f); // making heading drawing recreation more towards top


                startDrawingGame(true);
            }
        }

        // EVENTS

        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::Resized)
                fixView(event.size.width, event.size.height);

            if (event.type == Event::MouseEntered || event.type == Event::MouseMoved)
                mouseInside = true;

            if (event.type == Event::MouseLeft)
                mouseInside = false;


            //mouseclicks
            if (event.type == Event::MouseButtonPressed &&
                event.mouseButton.button == Mouse::Left)
            {
                // convert the click the same way hover is converted
                Vector2f m = window.mapPixelToCoords(
                    Vector2i(event.mouseButton.x, event.mouseButton.y));

                // click sound, only when a button was really clicked
                if (soundsOK && hoveredButton(m) != 0)
                    clickSound.play();

                if (screen == 0)
                {
                    if (isClicked(b1, m))
                        screen = 1;
                    else if (isClicked(b2, m))
                        screen = 2;
                    else if (isClicked(b3, m))
                        screen = 3;
                }
                else if (screen == 1)
                {
                    if (isClicked(b1, m))
                        screen = 4;
                    else if (isClicked(b2, m))
                        screen = 6;
                    else if (isClicked(b3, m))
                        screen = 5;
                    else if (isClicked(back, m))
                        screen = 0;
                }
                else if (screen == 2)
                {
                    if (isClicked(back, m))
                    {
                        screen = 0;
                    }
                    else if (!quizStarted)
                    {
                        // the Sorting Hat is the start button
                        if (hat.getGlobalBounds().contains(m))
                            startQuiz();
                    }
                    else if (!quizDone)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            if (isClicked(opt[i], m))
                            {
                                // give a point to the house this answer belongs to
                                houseScore[optHouse[qIndex][i]]++;

                                qIndex++;

                                if (qIndex >= TOTAL_Q)
                                {
                                    quizDone = true;
                                    findHouse();
                                    layoutQuizPage();
                                }
                                else
                                {
                                    loadQuestion();
                                }

                                break;
                            }
                        }
                    }
                    else
                    {
                        if (isClicked(b3, m))
                        {
                            quizStarted = false;// back to the start page
                            quizDone = false;
                            layoutQuizPage();
                        }
                    }
                }
                else if (screen == 3)
                {
                    if (isClicked(back, m))
                        screen = 0;
                }
                else if (screen == 4)
                {
                    if (isClicked(back, m))
                        screen = 1;
                    else if ((won || lost) && isClicked(b1, m))// Next Game
                        screen = pickNextScreen(4);
                    else if (isClicked(b2, m))
                        startGuessGame();
                }
                else if (screen == 5)
                {
                    if (isClicked(back, m))
                        screen = 1;
                    else if (memPhase >= 2 && isClicked(b1, m))// Next Game
                        screen = pickNextScreen(5);
                    else if (isClicked(b2, m))
                        startMemoryGame();
                }
                else if (screen == 6)
                {
                    if (isClicked(back, m))
                    {
                        screen = 1;
                    }
                    else if (isClicked(b1, m))
                    {
                        if (drawPhase == 0)
                            checkDrawing();// Check
                        else
                            screen = pickNextScreen(6);// Next Game / Secret Letters
                    }
                    else if (isClicked(b2, m))
                    {
                        // Clear / Try Again keep the same picture,Play Again gives a new one
                        startDrawingGame(drawPhase == 1);
                    }
                    else if (drawPhase == 0)
                    {
                        //click on the player's grid: flip the square
                        float gx = m.x - DRAW_X_PLAYER;
                        float gy = m.y - DRAW_Y;

                        if (gx >= 0 && gy >= 0)
                        {
                            int col = (int)(gx / DRAW_CELL);
                            int row = (int)(gy / DRAW_CELL);

                            if (col < DRAW_N && row < DRAW_N)
                                drawPlayer[row][col] = 1 - drawPlayer[row][col];
                        }
                    }
                }
            }


            //keybord part guess the word
            if (screen == 4 && event.type == Event::TextEntered)
            {
                int c = event.text.unicode;

                if ((c == 13 || c == 10) && (won || lost))// Enter a new word
                {
                    startGuessGame();
                }
                else if (!won && !lost && c < 128 && isalpha(c))
                {
                    char letter = (char)tolower(c);

                    if (used.find(letter) != string::npos)
                    {
                        guessMessage = "That letter was already tried or shown";
                    }
                    else
                    {
                        used += letter;

                        if (word.find(letter) != string::npos)
                        {
                            for (int i = 0; i < (int)word.length(); i++)
                            {
                                if (word[i] == letter)
                                    hidden[i] = letter;
                            }

                            guessMessage = "Good guess!";

                            if (hidden == word)
                            {
                                won = true;
                                wonGuess = true;
                                guessMessage = (wonGuess && wonMemory && wonDrawing) ? "You won! Secret Letters unlocked!" : "You won! Choose what to play next";
                                layoutGuessButtons();
                            }
                        }
                        else
                        {
                            chances--;
                            guessMessage = "Wrong letter!";

                            if (chances <= 0)
                            {
                                lost = true;
                                guessMessage = "You lost Guess the Word! The word was " + word;
                                layoutGuessButtons();
                            }
                        }
                    }
                }
            }


            // keybaord part of memeory umber
            if (screen == 5 && event.type == Event::TextEntered)
            {
                int c = event.text.unicode;

                if ((c == 13 || c == 10) && memPhase >= 2)//Enter a new round
                {
                    startMemoryGame();
                }
                else if (memPhase == 1)
                {
                    if (c == 8 && !memTyped.empty())// Backspace
                    {
                        memTyped.erase(memTyped.length() - 1);
                    }
                    else if (c >= '0' && c <= '9' && memTyped.length() < 8)
                    {
                        memTyped += (char)c;

                        if (memTyped.length() == 8)
                        {
                            if (memTyped == memSequence)
                            {
                                memPhase = 2;
                                wonMemory = true;
                                memMessage = (wonGuess && wonMemory && wonDrawing) ? "Correct! Secret Letters unlocked!" : "Correct! Choose what to play next";
                                layoutMemoryButtons();
                            }
                            else
                            {
                                memPhase = 3;
                                memMessage = "You lost Memory Number! It was " + makePairs(memSequence);
                                layoutMemoryButtons();
                            }
                        }
                    }
                }
            }


            //keyboard part of drawing recreation 
            if (screen == 6 && event.type == Event::TextEntered)
            {
                int c = event.text.unicode;

                if (c == 13 || c == 10)// Enter
                {
                    if (drawPhase == 0)
                        checkDrawing();
                    else
                        startDrawingGame(drawPhase == 1);
                }
            }
        }
        // updation (Memory Number timer + hover sound + wand sparkles)


        if (screen == 5 && memPhase == 0 && memClock.getElapsedTime().asSeconds() >= 5.0f)
            memPhase = 1;

        Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));

        // soft sound when the mouse moves onto a button
        int hoverNow = mouseInside ? hoveredButton(mouse) : 0;

        if (soundsOK && hoverNow != 0 && hoverNow != lastHover)
            hoverSound.play();

        lastHover = hoverNow;

        // soft sparkle sound while the wand is moving
        sparkleSoundTimer += dt;

        float moveX = mouse.x - prevMouse.x;
        float moveY = mouse.y - prevMouse.y;

        if (sparkleOK && mouseInside && sparkleSoundTimer > 0.35f &&
            (moveX * moveX + moveY * moveY) > 16.f)
        {
            sparkleSound[nextSparkleSound].setVolume(SPARKLE_VOLUME);
            sparkleSound[nextSparkleSound].setPitch(0.85f + (rand() % 50) / 100.f);   // a little different each time
            sparkleSound[nextSparkleSound].play();

            nextSparkleSound = (nextSparkleSound + 1) % 4;
            sparkleSoundTimer = 0.f;
        }

        prevMouse = mouse;

        if (mouseInside)
        {
            sparkTimer += dt;

            while (sparkTimer > 0.025f)
            {
                sparkTimer -= 0.025f;

                Spark s;
                s.pos = mouse + Vector2f((float)(rand() % 9 - 4), (float)(rand() % 9 - 4));
                s.vel = Vector2f((float)(rand() % 50 - 25), (float)(rand() % 40 - 10));
                s.maxLife = 0.45f + (rand() % 20) / 100.f;
                s.life = s.maxLife;
                s.size = 3.f + (rand() % 40) / 10.f;
                s.rot = (rand() % 100) / 50.f;

                sparks.push_back(s);
            }
        }

        for (int i = 0; i < (int)sparks.size(); i++)
        {
            sparks[i].life -= dt;
            sparks[i].pos += sparks[i].vel * dt;
            sparks[i].vel.y += 40.f * dt;// drifts down slowly
        }

        for (int i = (int)sparks.size() - 1; i >= 0; i--)
        {
            if (sparks[i].life <= 0.f)
                sparks.erase(sparks.begin() + i);
        }

        // DRAWING

        window.clear();
        window.draw(background);

        if (screen == 0)
        {
            window.draw(heading);

            drawButton(window, b1, btnTex, mouse);
            window.draw(t1);

            drawButton(window, b2, btnTex, mouse);
            window.draw(t2);

            drawButton(window, b3, btnTex, mouse);
            window.draw(t3);
        }
        else if (screen == 1)
        {
            window.draw(heading);

            drawButton(window, b1, btnTex, mouse);
            window.draw(t1);

            drawButton(window, b2, btnTex, mouse);
            window.draw(t2);

            drawButton(window, b3, btnTex, mouse);
            window.draw(t3);

            drawButton(window, back, btnTex, mouse);
            window.draw(tBack);
        }
        else if (screen == 2)
        {
            if (!quizStarted)
            {
                // star page
                window.draw(quizTitle);
                window.draw(quizTitle2);
                window.draw(quizIntro);

                // the hat glows when the mouse is over it
                if (hat.getGlobalBounds().contains(mouse))
                {
                    float pulse = 0.5f + 0.5f * sin(totalTime * 5.f);
                    hatGlow.setColor(Color(255, 255, 255, (Uint8)(170 + 85 * pulse)));
                    window.draw(hatGlow, RenderStates(BlendAdd));
                }

                window.draw(hat);
                window.draw(quizPrompt);
            }
            else if (!quizDone)
            {
                //question page
                window.draw(quizInfo);
                window.draw(quizPercent);

                // progress bar
                RectangleShape barBack(Vector2f(BAR_W, 9.f));
                barBack.setPosition(BAR_X, BAR_Y);
                barBack.setFillColor(Color(150, 115, 70, 90));
                window.draw(barBack);

                RectangleShape barFront(Vector2f(BAR_W * barFill, 9.f));
                barFront.setPosition(BAR_X, BAR_Y);
                barFront.setFillColor(Color(200, 140, 20));
                window.draw(barFront);

                window.draw(quizQuestion);

                for (int i = 0; i < 4; i++)
                {
                    Vector2f rowPos = opt[i].getPosition();
                    bool hover = opt[i].getGlobalBounds().contains(mouse);

                    // faint dark band behind every answer
                    RectangleShape band(opt[i].getSize());
                    band.setPosition(rowPos);
                    band.setFillColor(Color(70, 40, 15, 14));
                    band.setOutlineThickness(1.f);
                    band.setOutlineColor(Color(100, 60, 25, 60));
                    window.draw(band);

                    // soft golden highlight on the row under the mouse
                    if (hover)
                    {
                        RectangleShape hl(opt[i].getSize());
                        hl.setPosition(rowPos);
                        hl.setFillColor(Color(235, 175, 70, 70));
                        window.draw(hl);
                    }

                    // letter badge
                    CircleShape badge(17.f);
                    badge.setPosition(rowPos.x + 12.f, rowPos.y + opt[i].getSize().y / 2.f - 17.f);
                    badge.setFillColor(hover ? Color(240, 195, 120) : Color(225, 185, 140));
                    badge.setOutlineThickness(1.5f);
                    badge.setOutlineColor(Color(160, 100, 55));
                    window.draw(badge);

                     string letterStr = "";
                    letterStr += (char)('A' + i);
                    letterText.setString(letterStr);
                    letterText.setCharacterSize(20);
                    FloatRect lb = letterText.getLocalBounds();
                    letterText.setPosition(rowPos.x + 29.f - lb.width / 2.f - lb.left,
                        rowPos.y + opt[i].getSize().y / 2.f - lb.height / 2.f - lb.top);
                    window.draw(letterText);

                    window.draw(optText[i]);

                    // dotted line under each answer
                    for (float x = rowPos.x; x < rowPos.x + opt[i].getSize().x; x += 7.f)
                    {
                        RectangleShape dot(Vector2f(3.f, 1.5f));
                        dot.setPosition(x, rowPos.y + opt[i].getSize().y + 3.f);
                        dot.setFillColor(Color(120, 85, 50, 150));
                        window.draw(dot);
                    }
                }
            }
            else
            {
                // ----- RESULT PAGE -----
                window.draw(crestResult);

                window.draw(resSmall);
                window.draw(resName);
                window.draw(descPanel);
                window.draw(resDesc);
                window.draw(resTraits);
                window.draw(resFacts);
                window.draw(resMotto);

                drawButton(window, b3, btnTex, mouse);
                window.draw(t3);
            }

            drawButton(window, back, btnTex, mouse);
            window.draw(tBack);
        }
        else if (screen == 3)
        {
            window.draw(heading);

            int winsCount = 0;
            if (wonGuess)   winsCount++;
            if (wonMemory)  winsCount++;
            if (wonDrawing) winsCount++;

            if (winsCount == 3)
            {
                lockMsg.setString("You can now write a secret letter.");
            }
            else
            {
                lockMsg.setString("Locked! Win all 3 games to unlock.");
            }

            centerText(lockMsg, 230);
            window.draw(lockMsg);  
            lockCount.setString("Games won: " + intToString(winsCount) + " / 3");
            centerText(lockCount, 290);
            window.draw(lockCount);

            drawButton(window, back, btnTex, mouse);
            window.draw(tBack);
        }
        else if (screen == 4)
        {
            window.draw(heading);

            gameWord.setString(spaced(hidden));
            centerText(gameWord, 160);
            window.draw(gameWord);

            gameChances.setString("Chances left: " + intToString(chances));
            centerText(gameChances, 250);
            window.draw(gameChances);

            gameUsed.setString("Used letters: " + spaced(used));
            centerText(gameUsed, 300);
            window.draw(gameUsed);

            gameMessage.setString(guessMessage);
            centerText(gameMessage, (won || lost) ? 340.0f : 380.0f);
            window.draw(gameMessage);

            if (won || lost)
            {
                drawButton(window, b1, btnTex, mouse);
                window.draw(t1);
            }

            drawButton(window, b2, btnTex, mouse);
            window.draw(t2);

            drawButton(window, back, btnTex, mouse);
            window.draw(tBack);
        }
        else if (screen == 5)
        {
            window.draw(heading);

            if (memPhase == 0)
            {
                int left = 5 - (int)memClock.getElapsedTime().asSeconds();

                memInfo.setString("Remember this number! (" + intToString(left) + ")");
                centerText(memInfo, 170);
                window.draw(memInfo);

                memNumber.setString(makePairs(memSequence));
                centerText(memNumber, 250);
                window.draw(memNumber);
            }
            else
            {
                memInfo.setString("Type the 8 digits (Backspace to delete)");
                centerText(memInfo, 170);
                window.draw(memInfo);

                memNumber.setString(makePairs(memTyped) + (memTyped.length() < 8 ? "_" : ""));
                centerText(memNumber, 250);
                window.draw(memNumber);

                memMsg.setString(memMessage);
                centerText(memMsg, memPhase >= 2 ? 340.0f : 380.0f);
                window.draw(memMsg);
            }

            if (memPhase >= 2)
            {
                drawButton(window, b1, btnTex, mouse);
                window.draw(t1);
            }

            drawButton(window, b2, btnTex, mouse);
            window.draw(t2);

            drawButton(window, back, btnTex, mouse);
            window.draw(tBack);
        }
        else if (screen == 6)
        {
            window.draw(heading);
            // labels above the grids
            drawLabel.setString("Copy this");
            centerAt(drawLabel, DRAW_X_TARGET + DRAW_N * DRAW_CELL / 2.f, 128.f);
            window.draw(drawLabel);
            drawLabel.setString("Your drawing");
            centerAt(drawLabel, DRAW_X_PLAYER + DRAW_N * DRAW_CELL / 2.f, 128.f);
            window.draw(drawLabel);
            // two grids
            for (int r = 0; r < DRAW_N; r++)
{
    for (int c = 0; c < DRAW_N; c++)
    {
        RectangleShape cell(Vector2f(DRAW_CELL - 2.f, DRAW_CELL - 2.f));
        cell.setOutlineThickness(-1.5f);
        cell.setOutlineColor(Color(120, 85, 50, 220));

        // left grid 
        cell.setPosition(DRAW_X_TARGET + c * DRAW_CELL, DRAW_Y + r * DRAW_CELL);
        cell.setFillColor(drawTarget[r][c] ? Color(105, 65, 160) : Color(236, 222, 190, 190));
        window.draw(cell);

        // right grid
        cell.setPosition(DRAW_X_PLAYER + c * DRAW_CELL, DRAW_Y + r * DRAW_CELL);
        cell.setFillColor(drawPlayer[r][c] ? Color(105, 65, 160) : Color(236, 222, 190, 190));
        window.draw(cell);
    }
}

            drawMsg.setString(drawMessage);
            fitText(drawMsg, 680.f, 23, 14); // sentennce b/w teo grids
            centerText(drawMsg, 452);
            window.draw(drawMsg);

            drawButton(window, b1, btnTex, mouse);
            window.draw(t1);

            drawButton(window, b2, btnTex, mouse);
            window.draw(t2);

            drawButton(window, back, btnTex, mouse);
            window.draw(tBack);
        }
        // WAND CURSOR 
        if (mouseInside)
        {
            for (int i = 0; i < (int)sparks.size(); i++)
            {
                float k = sparks[i].life / sparks[i].maxLife;     // 1 -> 0
                Color c(255, 225, 140, (Uint8)(255 * k));

                drawStar(window, sparks[i].pos, sparks[i].size * (0.4f + 0.6f * k),
                    c, sparks[i].rot + totalTime * 2.f);
            }
            wand.setPosition(mouse);
            window.draw(wand);
        }
        window.display();
    }
    return 0;
}
