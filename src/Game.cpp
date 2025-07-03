#include "Game.hpp"
#include <iostream>
#include <curses.h>
#include <fstream>//библиотека для работы с файлами(нам нужна для загрузки уровня из файла)
Game::Game() : isRunning(true) {}

Game::~Game(){}

void Game::initialize(){// готовит терминал к работе с игрой
    //инициализация Curses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    refresh();
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK); //игрок
        init_pair(2, COLOR_BLUE, COLOR_BLACK);   //итены
        init_pair(3, COLOR_WHITE, COLOR_BLACK);  //точки
    }
    refresh();//Обновление экрана
    loadLevel("C:/study/summer_practice/Pac-Man/assets/levels/level_one.txt");;//в инициализацию добавил загрузку уровня 



    //поиск начальной позиции плеера
    for (int y = 0; y < levelData.size(); y++) {
        for (int x = 0; x < levelData[y].size(); x++) {
            if (levelData[y][x] == 'P') {
                playerX = x;
                playerY = y;
            }
        }
    }

}

void Game::tryMovePlayer(int dx, int dy) {
    int newX = playerX + dx;
    int newY = playerY + dy;
    if (levelData[newY][newX] != '#') {//проверка на стену
        if (levelData[newY][newX] == '.') {
            score += 10;
        }
        //обновление позиции плеера
        levelData[playerY][playerX] = ' ';
        playerX = newX;
        playerY = newY;
        levelData[playerY][playerX] = 'P';
    }
}

void Game::processInput() {
    int ch = getch();
    switch(ch) {
        case 'q'://выход из консольного приложения
        case 'Q':
            isRunning = false;
            break;
        case KEY_UP: case 'w':
            tryMovePlayer(0, -1);
            break;
        case KEY_DOWN: case 's':
            tryMovePlayer(0, 1);
            break;
        case KEY_LEFT: case 'a':
            tryMovePlayer(-1, 0);
            break;
        case KEY_RIGHT: case 'd':
            tryMovePlayer(1, 0);
            break;
    }
        //дальше будет расширяться все о клавишах:ходьба,пауза
}

void Game::update() {
    //будут добавляться основная логика: движение, коллизии, ИИ призраков
}

void Game::render() {
    clear(); //очистка экрана
    
    attron(A_REVERSE); // Инверсный цвет для выделения
    mvprintw(0, 0, "Score: %d", score);
    clrtoeol(); // Очистить остаток строки
    attroff(A_REVERSE);

            //отрисовка уровня из файла
    for (int y = 0; y < levelData.size(); y++) {
        for (int x = 0; x < levelData[y].size(); x++) {

            char cell = levelData[y][x];

            
            switch(cell) {
                case '#'://стена из синих #
                    attron(COLOR_PAIR(2));
                    mvaddch(y + 1, x, cell);
                    attroff(COLOR_PAIR(2));
                    break;
                    
                case '.'://точка белая
                    attron(COLOR_PAIR(3));
                    mvaddch(y + 1, x, cell);
                    attroff(COLOR_PAIR(3));
                    break;
                    
                case 'P'://игрок желтое P
                    attron(COLOR_PAIR(1));
                    mvaddch(y + 1, x, cell);
                    attroff(COLOR_PAIR(1));
                    break;
                    
                default://все остальное
                    mvaddch(y + 1, x, cell);
            }
        }
    }


    refresh();//обновление экрана
}

void Game::cleanup() {
    endwin();//завершение работы с curses
}

void Game::run()
{   initialize();
    while(isRunning) {//цикл работы программы
        processInput();
        update();
        render();
    }
    cleanup();

    std::cout << "Game running" << std::endl;
}

//войд функция для загрузки уровня 
void Game::loadLevel(const std::string& filename) {
    levelData.clear();
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {//удаление символа возврата каретки в Windows символы перевода строки состоят из двух символов: \r\n поэтмоу надо удалить \r
        if (!line.empty() && line[line.size()-1] == '\r') {
            line.erase(line.size()-1);
        }
        levelData.push_back(line);
    }
    file.close();
}