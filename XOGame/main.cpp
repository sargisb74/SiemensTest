#include <QCoreApplication>
#include <QTimer>
#include <iostream>
#include <QThread>
#include <QMap>
#include <QPoint>
#include <QRandomGenerator>
#include <QDateTime>

int GetDimension(int argc, char* argv[])
{
    int dimension = -1;
    if (argc > 1)
    {
        if (QString(argv[1]) == "XO3")
        {
            dimension = 3;
        }
        else
        {
            if (QString(argv[1]) == "XO4")
            {
                dimension = 4;
            }
            else
            {
                if (QString(argv[1]) == "XO5")
                {
                    dimension = 5;
                }
            }
        }
    }

    if (dimension == -1)
    {
        qDebug() << "Incorrect argument, the first should be XO3 or XO4 or XO5!";
        dimension = 3;
    }

    return dimension;
}

void wait(int mSec)
{
    QElapsedTimer timer;
    timer.start();

    while (!timer.hasExpired(mSec))
        QCoreApplication::processEvents();
}

// Function to check if a player has won
char checkWinner(const QVector<QVector<char>>& field, QPoint coordinates, int dimension)
{
    char current = field[coordinates.x()][coordinates.y()];
    //Check the left diagonal
    if (coordinates.x() == coordinates.y())
    {
        int x = 0;
        int y = 0;
        char win = '-';
        while (y < dimension)
        {
            char ch = field[x][y];
            if (current != ch)
            {
                win = ' ';
                break;
            }

            x++;
            y++;
            win = current;
        }

        if (win == current)
        {
            return win;
        }
    }

    //Check the right diagonal
    if (coordinates.x() + coordinates.y() == dimension - 1)
    {
        int x = 0;
        int y = dimension - 1;
        char win = '-';
        while (y >= 0)
        {
            char ch = field[x][y];
            if (current != ch)
            {
                win = ' ';
                break;
            }
            x++;
            y--;
            win = current;
        }

        if (win == current)
        {
            return win;
        }
    }

    //Check the vertical
    int x = coordinates.x();
    int y = dimension - 1;
    char win = '-';
    while (y >= 0)
    {
        char ch = field[x][y];
        if (current != ch)
        {
            win = ' ';
            break;
        }
        y--;
        win = current;
    }

    if (win == current)
    {
        return win;
    }

    //Check the horizontal
    x = dimension - 1;
    y = coordinates.y();
    win = '-';
    while (x >= 0)
    {
        char ch = field[x][y];
        if (current != ch)
        {
            win = ' ';
            break;
        }
        x--;
        win = current;
    }

    if (win == current)
    {
        return win;
    }

    return win;
}

char DoStep(QList<int>& list,
    QMap<int, QPoint>& fieldIndexes,
    QVector<QVector<char>>& field,
    int dimension,
    bool X = true)
{
    // Use QRandomGenerator to generate a random index
    long long randomIndex = QRandomGenerator::global()->bounded(list.size());

    // Retrieve the random number from the list
    int randomNumber = list[randomIndex];
    QPoint point = fieldIndexes[randomNumber];
    fieldIndexes.remove(randomNumber);
    list.remove(randomIndex);

    int timeout = QRandomGenerator::global()->
        bounded(500 / (dimension * dimension), 2000 / (dimension * dimension));

    wait(timeout);
    if (X)
        field[point.x()][point.y()] = 'X';
    else
        field[point.x()][point.y()] = 'O';

    return checkWinner(field, point, dimension);
}

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    int dimension = GetDimension(argc, argv);

    QVector<QVector<char>> field(dimension);
    QMap<int, QPoint> fieldIndexes;
    int k = 1;
    for (int i = 0; i < dimension; ++i)
    {
        field[i] = QVector<char>(dimension);
        field[i].fill(' ');
        for (int j = 0; j < dimension; ++j)
        {
            fieldIndexes[k++] = QPoint(i, j);
        }
    }

    QList<int> list = fieldIndexes.keys();
    char result = '-';

    while (!list.isEmpty())
    {
        result = DoStep(list, fieldIndexes, field, dimension);
        if (result != ' ' && result != '-')
        {
            break;
        }
        if ((!list.isEmpty()))
        {
            result = DoStep(list, fieldIndexes, field, dimension, false);
            if (result != ' ' && result != '-')
            {
                break;
            }
        }
    }

    QString ret = result == 'X' ? "1" : "2";

    if (result != ' ' && result != '-')
    {
        std::cout << ret.toStdString() << std::endl;
    }
    else
    {
        std::cout << "0" << result << std::endl;
    }

    QTimer::singleShot(0, [&a, &result]()
    {
      QCoreApplication::exit();
    });

    return QCoreApplication::exec();
}