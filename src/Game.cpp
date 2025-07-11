#include "Game.hpp"
#include <iostream>
#include <curses.h>
#include <fstream> //библиотека для работы с файлами(нам нужна для загрузки уровня из файла)
Game::Game() : isRunning(true), score(0), lives(3) {}

Game::~Game() {}

void Game::initialize()
{ // готовит терминал к работе с игрой
    // инициализация Curses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    refresh();
    start_color();
    if (has_colors())
    {
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);  // игрок
        init_pair(2, COLOR_BLUE, COLOR_BLACK);    // итены
        init_pair(3, COLOR_WHITE, COLOR_BLACK);   // точки
        init_pair(4, COLOR_MAGENTA, COLOR_BLACK); // Призрак 1
        init_pair(5, COLOR_CYAN, COLOR_BLACK);    // Призрак 2
        init_pair(6, COLOR_RED, COLOR_BLACK);     // Призрак 3
    }
    if (!has_colors())
    {
        endwin();
        std::cerr << "Терминал не поддерживает цвета!\n";
        exit(1);
    }

    refresh(); // Обновление экрана
    loadLevel("../assets/levels/level_one.txt");
    ; // в инициализацию добавил загрузку уровня

    // поиск начальной позиции плеера
    for (int y = 0; y < levelData.size(); y++)
    {
        for (int x = 0; x < levelData[y].size(); x++)
        {
            if (levelData[y][x] == 'S')
            { // 'S' - стартовая позиция
                playerX = x;
                playerY = y;
                levelData[y][x] = ' '; // Очищаем стартовую позицию
                startY = y;
                startX = x;
            }
        }
    }

    auto ghostSpawns = findFreePositions('G');

    std::vector<int> ghostColors = {4, 5, 6}; // MAGENTA, CYAN, RED

    for (int i = 0; i < ghostSpawns.size() && i < ghostColors.size(); i++)
    {
        auto [x, y] = ghostSpawns[i];
        ghosts.emplace_back(x, y, ghostColors[i]);
        levelData[y][x] = ' '; // очищаем позицию призрака
    }

    // Помещаем игрока на стартовую позицию
    levelData[playerY][playerX] = 'P';
}

bool Game::tryMovePlayer(int dx, int dy)
{
    int newX = playerX + dx;
    int newY = playerY + dy;

    // проверка телепортации по краям карты
    if (newX < 0)
        newX = levelData[newY].size() - 1;
    if (newX >= levelData[newY].size())
        newX = 0;
    if (newY < 0)
        newY = levelData.size() - 1;
    if (newY >= levelData.size())
        newY = 0;

    if (levelData[newY][newX] != '#')
    { // проверка на стену
        if (levelData[newY][newX] == '.')
        {
            score += 10;
        }
        else if (levelData[newY][newX] == 'o')
        {
            score += 50;
            // активируем режим "силы" (пока просто флаг) ТУТ МЫ ДОЛЖНЫ ДОБАВИТЬ СЪЕДАНИЕ ПРИЗРАКОВ ЗА НЕКОТОРЫЙ ПРОМЕЖУТОК
            // в некст update можно добавить возможность есть призраков
        }
        // обновление позиции плеера
        levelData[playerY][playerX] = ' ';
        playerX = newX;
        playerY = newY;
        levelData[playerY][playerX] = 'P';
        return true;
    }
    return false;
}

void Game::processInput()
{
    int ch = getch();
    switch (ch)
    {
    case 'q': // выход из консольного приложения
    case 'Q':
        isRunning = false;
        break;
    case KEY_UP:
    case 'w': // работают и стрелочки так как (KEY_UP KEY_DOWN KEY_LEFT KEY_RIGHT)
        nextDirX = 0;
        nextDirY = -1;
        break;
    case KEY_DOWN:
    case 's':
        nextDirX = 0;
        nextDirY = 1;
        break;
    case KEY_LEFT:
    case 'a':
        nextDirX = -1;
        nextDirY = 0;
        break;
    case KEY_RIGHT:
    case 'd':
        nextDirX = 1;
        nextDirY = 0;
        break;
    }
    // паузу МОЖНО ДОБАВИТЬ (В НЕКСТ РАЗ)
}
void Game::update()
{
    // Попытка изменить направление, если возможно
    int testX = playerX + nextDirX;
    int testY = playerY + nextDirY;

    // Телепорт по краям
    if (testX < 0)
        testX = levelData[testY].size() - 1;
    if (testX >= levelData[testY].size())
        testX = 0;
    if (testY < 0)
        testY = levelData.size() - 1;
    if (testY >= levelData.size())
        testY = 0;

    if (levelData[testY][testX] != '#')
    {
        dirX = nextDirX;
        dirY = nextDirY;
    }

    tryMovePlayer(dirX, dirY);

    for (auto &ghost : ghosts)
    {
        ghost.update(levelData);
    }

    // проверка стычьки с призраками
    for (const auto &ghost : ghosts)
    {
        if (playerX == ghost.getX() && playerY == ghost.getY())
        {
            lives--;
            if (lives <= 0)
            {
                isRunning = false;
            }
            else
            {
                // Возвращаем игрока на стартовую позицию
                levelData[playerY][playerX] = ' ';
                playerX = startX;
                playerY = startY;
                mvprintw(10, 10, "You died!");
                refresh();
                napms(3000);
            }
            break;
        }
    }

    bool pointsLeft = false;
    for (const auto &row : levelData)
    {
        for (char cell : row)
        {
            if (cell == '.' || cell == 'o')
            {
                pointsLeft = true;
                break;
            }
        }
        if (pointsLeft)
            break;
    }

    if (!pointsLeft)
    {
        // Уровень пройден!
        mvprintw(10, 10, "LEVEL COMPLETE!");
        refresh();
        napms(2000); // Задержка 2 секунды
        isRunning = false;
    }
}

void Game::render()
{
    clear(); // очистка экрана

    attron(A_REVERSE); // инверсный цвет для выделения(приятность для глаз)
    mvprintw(0, 0, "Score: %d", score);
    mvprintw(0, 20, "Lives: %d", lives);
    clrtoeol(); // Очистить остаток строки
    attroff(A_REVERSE);

    // отрисовка уровня из файла
    for (int y = 0; y < levelData.size(); y++)
    {
        for (int x = 0; x < levelData[y].size(); x++)
        {

            char cell = levelData[y][x];

            switch (cell)
            {
            case '#': // стена из синих #
                attron(COLOR_PAIR(2));
                mvaddch(y + 1, x, cell);
                attroff(COLOR_PAIR(2));
                break;

            case '.': // точка белая
                attron(COLOR_PAIR(3));
                mvaddch(y + 1, x, cell);
                attroff(COLOR_PAIR(3));
                break;

            case 'P': // игрок желтое P
                attron(COLOR_PAIR(1));
                mvaddch(y + 1, x, cell);
                attroff(COLOR_PAIR(1));
                break;

            default: // все остальное
                mvaddch(y + 1, x, cell);
            }
        }
    }
    // отрисовываем призраков
    for (const auto &ghost : ghosts)
    {
        ghost.render();
    }

    refresh(); // обновление экрана
}

void Game::cleanup()
{
    endwin(); // завершение работы с curses
}

void Game::run()
{
    initialize();

    timeout(100);

    while (isRunning)
    { // цикл работы программы
        processInput();
        update();
        render();
    }
    cleanup();

    std::cout << "Game running" << std::endl;
}

// войд функция для загрузки уровня
void Game::loadLevel(const std::string &filename)
{
    levelData.clear();
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open level file");
    }

    std::string line;
    size_t maxWidth = 0;

    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (!line.empty())
        {
            maxWidth = std::max(maxWidth, line.size());
            levelData.push_back(line);
        }
    }
    file.close();

    // Выравнивание всех строк по максимальной длине
    for (auto &row : levelData)
    {
        if (row.size() < maxWidth)
        {
            row.append(maxWidth - row.size(), ' ');
        }
    }

    if (levelData.empty())
    {
        throw std::runtime_error("Level file is empty");
    }
}
// функция для нахождения свободной позиции(ИСПОЛЬЗУЕТСЯ ТУТ: Поиск точек для еды: Поиск спавн-точек для призраков: Поиск любой свободной клетки:)
std::vector<std::pair<int, int>> Game::findFreePositions(char allowed)
{
    std::vector<std::pair<int, int>> positions;
    for (int y = 0; y < levelData.size(); y++)
    {
        for (int x = 0; x < levelData[y].size(); x++)
        {
            char cell = levelData[y][x];
            if (cell != '#' && (allowed == '\0' || cell == allowed))
            {
                positions.push_back({x, y});
            }
        }
    }
    return positions;
}