#include "Game.hpp"
#include <iostream>
#include <pdcurses.h>
#include <fstream> //библиотека для работы с файлами(нам нужна для загрузки уровня из файла)
Game::Game() : isRunning(true), score(0), lives(3) {}

Game::~Game() {}

void Game::initialize()
{ // готовит терминал к работе с игрой
    // инициализация Curses
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
        init_pair(4, COLOR_RED, COLOR_BLACK);     // Blinky (красный)
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // Pinky (розовый)
        init_pair(6, COLOR_CYAN, COLOR_BLACK);    // Inky (голубой)
        init_pair(8, 202, COLOR_BLACK);           // Clyde (оранжевый)
        init_pair(7, COLOR_WHITE, COLOR_BLUE);    // Режим призраков при съедении усиления
    }
    if (!has_colors())
    {
        endwin();
        std::cerr << "Терминал не поддерживает цвета!\n";
        exit(1);
    }

    refresh(); // Обновление экрана
    loadLevel(levelFiles[currentLevel]);
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
    // Поиск начальной позции второго игрока
    for (int y = 0; y < levelData.size(); y++)
    {
        for (int x = 0; x < levelData[y].size(); x++)
        {
            if (levelData[y][x] == 'T' && twoPlayers)
            {
                player2X = x;
                player2Y = y;
                levelData[y][x] = ' '; // убираем метку стартовой позиции
                start2X = x;
                start2Y = y;
            }
            else if (levelData[y][x] == 'T')
            {
                levelData[y][x] = '.';
            }
        }
    }

    std::vector<GhostType> ghostTypes = {
        GhostType::BLINKY, GhostType::PINKY,
        GhostType::INKY, GhostType::CLYDE};
    auto spawns = findFreePositions('G');
    for (int i = 0; i < spawns.size() && i < ghostTypes.size(); ++i)
    {
        auto [gx, gy] = spawns[i];
        ghosts.emplace_back(gx, gy, 4 + i, ghostTypes[i], levelData);
        levelData[gy][gx] = ' ';
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
            dotsEaten++;
        }
        else if (levelData[newY][newX] == 'o')
        {
            score += 50;
            powerUpActive = true;
            powerUpTicks = maxPowerUpTicks;

            for (auto &ghost : ghosts)
            {
                ghost.setMode(GhostMode::FRIGHTENED);
            }
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

bool Game::tryMovePlayer2(int d2x, int d2y)
{
    int new2X = player2X + d2x;
    int new2Y = player2Y + d2y;

    // проверка телепортации по краям карты
    if (new2X < 0)
        new2X = levelData[new2Y].size() - 1;
    if (new2X >= levelData[new2Y].size())
        new2X = 0;
    if (new2Y < 0)
        new2Y = levelData.size() - 1;
    if (new2Y >= levelData.size())
        new2Y = 0;

    if (levelData[new2Y][new2X] != '#')
    { // проверка на стену
        if (levelData[new2Y][new2X] == '.')
        {
            score += 10;
        }
        else if (levelData[new2Y][new2X] == 'o')
        {
            score += 50;
            powerUpActive = true;
            powerUpTicks = maxPowerUpTicks;

            for (auto &ghost : ghosts)
            {
                ghost.setMode(GhostMode::FRIGHTENED);
            }
        }
        // обновление позиции плеера
        levelData[player2Y][player2X] = ' ';
        player2X = new2X;
        player2Y = new2Y;
        levelData[player2Y][player2X] = 'p';
        return true;
    }
    return false;
}

void Game::processInput()
{
    wint_t ch;
    get_wch(&ch);

    switch (ch)
    {
    case L'q':
    case L'Q': // выход из консольного приложения
    case L'й':
    case L'Й':
        isRunning = false;
        break;
    // Управление первого игрока на WASD
    case L'w':
    case L'W':
    case L'ц':
    case L'Ц':
        nextDirX = 0;
        nextDirY = -1;
        break;
    case L's':
    case L'S':
    case L'ы':
    case L'Ы':
        nextDirX = 0;
        nextDirY = 1;
        break;
    case L'a':
    case L'A':
    case L'ф':
    case L'Ф':
        nextDirX = -1;
        nextDirY = 0;
        break;
    case L'd':
    case L'D':
    case L'в':
    case L'В':
        nextDirX = 1;
        nextDirY = 0;
        break;
        // Управление второго игрока на стрелочках
        if (twoPlayers)
        {
        case KEY_UP:
            nextDir2X = 0;
            nextDir2Y = -1;
            break;
        case KEY_DOWN:
            nextDir2X = 0;
            nextDir2Y = 1;
            break;
        case KEY_LEFT:
            nextDir2X = -1;
            nextDir2Y = 0;
            break;
        case KEY_RIGHT:
            nextDir2X = 1;
            nextDir2Y = 0;
            break;
        }
    // Пауза на Esc
    case 27:
        isPaused = !isPaused;
    case L'1':
    case L'2':
    case L'3':
    case L'4':
    case L'5':
    case L'6':
    case L'7':
    case L'8':
    case L'9':
    case L'0':
    {
        if (ch == L'0')
        {
            int selected = 9;
            currentLevel = selected;
            ghosts.clear();
            initialize();
            break;
        }

        else
        {
            int selected = ch - L'1';
            if (selected < levelFiles.size())
            {
                currentLevel = selected;
                ghosts.clear(); // очистить старых призраков
                initialize();   // перезапустить на новом уровне
            }
            break;
        }
    }
    }
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

    // Попытка поменять направление 2 игрока
    if (twoPlayers)
    {
        int test2X = player2X + nextDir2X;
        int test2Y = player2Y + nextDir2Y;

        if (test2X < 0)
            test2X = levelData[test2Y].size() - 1;
        if (test2X >= levelData[test2Y].size())
            test2X = 0;
        if (test2Y < 0)
            test2Y = levelData.size() - 1;
        if (test2Y >= levelData.size())
            test2Y = 0;

        if (levelData[test2Y][test2X] != '#')
        {
            dir2X = nextDir2X;
            dir2Y = nextDir2Y;
        }
    }
    // Глобальное переключение режимов призраков
    ghostModeGlobalTicks++;
    if (ghostModeGlobalTicks > modeDurations[currentModeIndex])
    {
        currentModeIndex = (currentModeIndex + 1) % modeDurations.size();
        ghostModeGlobalTicks = 0;
        if (ghostGlobalMode == GhostMode::SCATTER)
            ghostGlobalMode = GhostMode::CHASE;
        else
            ghostGlobalMode = GhostMode::SCATTER;

        for (auto &ghost : ghosts)
        {
            if (ghost.getMode() != GhostMode::FRIGHTENED && ghost.getMode() != GhostMode::EATEN)
            {
                ghost.setMode(ghostGlobalMode);
            }
        }
    }
    if (powerUpActive)
    {
        powerUpTicks--;
        if (powerUpTicks <= 0)
        {
            powerUpActive = false;

            for (auto &ghost : ghosts)
            {
                ghost.setMode(ghost.getPreviousMode());
            }
        }
    }

    tryMovePlayer(dirX, dirY);
    if (twoPlayers)
    {
        tryMovePlayer2(dir2X, dir2Y);
    }

    Ghost *blinky = nullptr;
    for (auto &g : ghosts)
        if (g.getType() == GhostType::BLINKY)
            blinky = &g;

    for (auto &g : ghosts)
    {
        g.calculateTarget(playerX, playerY, dirX, dirY, blinky, levelData, dotsEaten);
        g.update(levelData);
    }
    // проверка стычьки с призраками
    bool collided = false;
    for (auto &ghost : ghosts)
    {
        if ((playerX == ghost.getX() && playerY == ghost.getY()) ||
            (playerX == ghost.getPrevX() && playerY == ghost.getPrevY()) ||
            (twoPlayers && ((player2X == ghost.getX() && player2Y == ghost.getY()) ||
                            (player2X == ghost.getPrevX() && player2Y == ghost.getPrevY()))))
        {
            if (ghost.getMode() == GhostMode::FRIGHTENED)
            {
                score += 200;

                // Очистка старой позиции призрака, если она в пределах карты
                if (ghost.getY() >= 0 && ghost.getY() < (int)levelData.size() &&
                    ghost.getX() >= 0 && ghost.getX() < (int)levelData[ghost.getY()].size())
                {
                    levelData[ghost.getY()][ghost.getX()] = ' ';
                }

                ghost.respawn();

                ghost.forceMode(ghostGlobalMode);
                render();

                break; // Выходим из цикла, чтобы не работать с тем же призраком
            }
            else
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
                    levelData[playerY][playerX] = 'P';

                    if (twoPlayers)
                    {
                        levelData[player2Y][player2X] = ' ';
                        player2X = start2X;
                        player2Y = start2Y;
                        levelData[player2Y][player2X] = 'p';
                    }
                    // Респавн призраков
                    for (auto &ghost : ghosts)
                    {
                        if (ghost.getY() >= 0 && ghost.getY() < (int)levelData.size() &&
                            ghost.getX() >= 0 && ghost.getX() < (int)levelData[ghost.getY()].size())
                        {
                            levelData[ghost.getY()][ghost.getX()] = ' ';
                        }
                        ghost.respawn();
                        ghost.forceMode(ghostGlobalMode);
                    }

                    render();

                    std::string deathText = "== YOU DIED ==";
                    int textY = levelData.size() / 2;
                    int textX = (levelData[0].size() - deathText.length()) / 2;

                    attron(A_BOLD | A_REVERSE);
                    mvprintw(textY, textX, "%s", deathText.c_str());
                    attroff(A_BOLD | A_REVERSE);

                    refresh();
                    napms(3000);
                }
                break; // И здесь тоже прерываем цикл после столкновения
            }
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
        // Очищаю экран перед надписью
        clear();

        std::string message = "== LEVEL COMPLETE! ==";
        std::string border(message.size(), '=');

        int winHeight, winWidth;
        getmaxyx(stdscr, winHeight, winWidth);
        int msgY = winHeight / 2;
        int msgX = (winWidth - message.size()) / 2;

        attron(A_BOLD | A_REVERSE | COLOR_PAIR(3));
        mvprintw(msgY - 1, msgX, "%s", border.c_str());
        mvprintw(msgY, msgX, "%s", message.c_str());
        mvprintw(msgY + 1, msgX, "%s", border.c_str());
        attroff(A_BOLD | A_REVERSE | COLOR_PAIR(3));

        refresh();
        napms(3000);

        currentLevel++;
        if (currentLevel < levelFiles.size())
        {
            ghosts.clear(); // очистить старых призраков
            initialize();   // перезапускаем игру на новом уровне
        }
        else
        {
            // Победа во всей игре
            clear();
            std::string finalMsg = "== YOU WIN THE GAME! ==";
            int finalX = (winWidth - finalMsg.length()) / 2;
            int finalY = winHeight / 2;

            attron(A_BOLD | A_REVERSE | COLOR_PAIR(1));
            mvprintw(finalY, finalX, "%s", finalMsg.c_str());
            attroff(A_BOLD | A_REVERSE | COLOR_PAIR(1));

            refresh();
            napms(5000);
            isRunning = false;
        }
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
            case 'p':
                if (twoPlayers)
                {
                    attron(COLOR_PAIR(5));
                    mvaddch(player2Y + 1, player2X, 'p');
                    attroff(COLOR_PAIR(5));
                }
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

    // Отрисовать сразу уровень
    render();

    // Показываем сообщение "Готовься!" с обратным отсчетом
    int waitSeconds = 3; // задержка в секундах

    for (int i = waitSeconds; i > 0; --i)
    {
        std::string msg = "Get ready! Starting in " + std::to_string(i) + "...";
        int msgY = levelData.size() / 2;
        int msgX = (levelData[0].size() - msg.length()) / 2;

        attron(A_BOLD | A_REVERSE | COLOR_PAIR(3));
        mvprintw(msgY, msgX, "%s", msg.c_str());
        attroff(A_BOLD | A_REVERSE | COLOR_PAIR(3));

        refresh();

        napms(1000); // задержка 1 секунда

        // Стираем сообщение перед следующей итерацией
        move(msgY, msgX);
        for (size_t j = 0; j < msg.length(); j++)
            addch(' ');
    }

    timeout(100);

    while (isRunning)
    {
        processInput();
        if (!isPaused)
        {
            update();
            render();
        }
        else
        {
            std::string pauseText = "== PAUSED ==";
            int pauseY = levelData.size() / 2;
            int pauseX = (levelData[0].size() - pauseText.length()) / 2;

            attron(A_BOLD | A_REVERSE);
            mvprintw(pauseY, pauseX, "%s", pauseText.c_str());
            attroff(A_BOLD | A_REVERSE);

            refresh();
        }
    }
    cleanup();

    std::cout << "Game finished" << std::endl;
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