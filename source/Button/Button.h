/*
MIT License
Copyright (c) 2018 fuzun
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef BUTTON_H
#define BUTTON_H

#include "common.h"

#include <QGraphicsPixmapItem>
#include <QPixmap>

class Game;

class ButtonFuncs
{
public:
    static void about(Game *game_instance);
    static void play(Game *game_instance);
    static void soundEnable(Game *game_instance);
    static void soundDisable(Game *game_instance);
    static void aiEnable(Game *game_instance);
    static void aiPlay(Game *game_instance);
    // static void function(Game *game_instance);

private:
    ButtonFuncs() {}
};


class Button : public QGraphicsPixmapItem
{
    typedef void (*Function)(Game *game_instance);

private:
    QPixmap Pixmap;
    QPixmap Pixmap2;
    bool toggle;
    enum { isPixmap, isPixmap2 } whichPixmap;

public:
    Button(Game *parent_game, const QPixmap& pixmap, Function function, bool toggleMode = false, const QPixmap& pixmap2 = QPixmap(), Function function2 = nullptr)
        : QGraphicsPixmapItem(pixmap), Pixmap(pixmap), Pixmap2(pixmap2), toggle(toggleMode), whichPixmap(isPixmap), buttonFunction(function), buttonFunction2(function2), game(parent_game)  { setPixmap(pixmap); }

    virtual void invoke(Game *parent_game = nullptr);

protected:
    Function buttonFunction;
    Function buttonFunction2;
    Game *game;
};

#endif // BUTTON_H
