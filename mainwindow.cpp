/* Tetris project: mainwindow.cpp
 *
 * Mainwindow file, contains all components of the window
 *
 * Program author/editor:
 * Name: Rasmus Kivinen
 * Student number: 285870
 * UserID: kivinenr
 * E-Mail: rasmus.kivinen@tuni.fi
 * */

#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QDebug>
#include <set>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // We need a graphics scene in which to draw tetrominos.
    scene_ = new QGraphicsScene(this);

    // The graphicsView object is placed on the window
    // at the following coordinates, but if you want
    // different placement, you can change their values.
    int left_margin = 100; // x coordinate
    int top_margin = 150; // y coordinate

    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(left_margin, top_margin,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);
    ui->graphicsView->setScene(scene_);

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    // each square of a tetromino is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    // Setting random engine ready for the first real call.
    int seed = time(0); // You can change seed value for testing purposes
    randomEng.seed(seed);
    distr = std::uniform_int_distribution<int>(0, NUMBER_OF_TETROMINOS - 1);
    distr(randomEng); // Wiping out the first random number (which is almost always 0)
    // After the above settings, you can use randomEng to draw the next falling
    // tetromino by calling: distr(randomEng) in a suitable method.

    // Add more initial settings and connect calls, when needed.

    // Setting the background's color and pattern
    QLinearGradient gradient(MIDDLE_X, 0, BORDER_DOWN, MIDDLE_X);
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::black);
    QBrush brush(gradient);
    scene_->setBackgroundBrush(brush);


    // Connect the two timers used to their respective functions
    connect(&gravity_timer, &QTimer::timeout, this, &MainWindow::drop_all);
    connect(&time_played_timer, &QTimer::timeout, this, &MainWindow::tick_time);

    //Start button is disabled until difficulty is selected
    ui->startPushButton->setEnabled(false);

    //Submit score button is disabled until game is over
    ui->submitscorePushButton->setEnabled(false);

    ui->gameoverLabel->hide();
    readhiscore();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QGraphicsRectItem* MainWindow::add_block(int x, int y, QColor color)
{
    //Create a brush with specified color for the block
    QBrush brush = QBrush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(color);

    QGraphicsRectItem* block = scene_->addRect(BORDER_LEFT, BORDER_UP, SQUARE_SIDE, SQUARE_SIDE);
    block->setPos(x, y);
    block->setBrush(brush);

    return block;
}

void MainWindow::move_block(std::vector<QGraphicsRectItem*> shape, std::string dir)
{
    if(not shape_can_move(shape, dir))
    {
        return;
    }

    //Move every block of the shape in the specified direction
    for(auto block_item: shape)
    {
        QPoint point = new_location(block_item, dir);
        block_item->setPos(point);
    }

}

void MainWindow::drop_all()
{
    //If the latest tetromino can't move any further down, create a new one
    if(not shape_can_move(tetrominos.back(), "DOWN"))
    {
        for(auto shape: tetrominos)
        {
            move_block(shape,"DOWN");
        }

        create_random_tetromino();
    }

    else
    {
        for(auto shape: tetrominos)
        {
            move_block(shape,"DOWN");
        }
    }

}

QPoint MainWindow::new_location(QGraphicsRectItem *block, std::string dir)
{
    int curr_x = block->x();
    int curr_y = block->y();

    QPoint new_location;

    if(dir == "DOWN")
    {
        new_location = QPoint(curr_x, curr_y + SQUARE_SIDE);
    }

    if(dir == "LEFT")
    {
        new_location = QPoint(curr_x - SQUARE_SIDE, curr_y);
    }

    if(dir == "RIGHT")
    {
        new_location = QPoint(curr_x + SQUARE_SIDE, curr_y);
    }

    return new_location;
}

void MainWindow::create_random_tetromino()
{
    //If the spawning area for the new block is occupied, the game ends
    if(not block_can_move(QPoint(MIDDLE_X, BORDER_UP)) ||
       not block_can_move(QPoint(MIDDLE_X, BORDER_UP + SQUARE_SIDE)))
    {
        game_over();
        return;
    }

    //Create vector to contain all individual squares of the tetromino
    std::vector<QGraphicsRectItem*> shape;

    //Get random number for tetromino creation, assign the color associated
    //with that number
    int tetromino_number = distr(randomEng);
    QColor color = colors.at(tetromino_number);

    int length;


    //Create blocks based on tetromino_number and add them to the vector
    //If the shape is simple, this is done using for loops, otherwise
    //create the new blocks manually
    if(tetromino_number == 0)
    {
        length = SQUARE_SIDE * 4;
        for(int x = MIDDLE_X - length/2;
            x < MIDDLE_X + length/2; x += SQUARE_SIDE)
        {
            QGraphicsRectItem* block = add_block(x, BORDER_UP, color);

            shape.push_back(block);
        }
    }

    if(tetromino_number == 1)
    {
        QGraphicsRectItem* block = add_block(MIDDLE_X - SQUARE_SIDE, BORDER_UP, color);

        shape.push_back(block);

        for(int x = MIDDLE_X - SQUARE_SIDE;
            x < MIDDLE_X + 2 * SQUARE_SIDE; x += SQUARE_SIDE)
        {
            QGraphicsRectItem* block = add_block(x, BORDER_UP + SQUARE_SIDE, color);

            shape.push_back(block);
        }
    }

    if(tetromino_number == 2)
    {
        QGraphicsRectItem* block = add_block(MIDDLE_X + SQUARE_SIDE, BORDER_UP, color);

        shape.push_back(block);

        for(int x = MIDDLE_X - SQUARE_SIDE;
            x < MIDDLE_X + 2 * SQUARE_SIDE; x += SQUARE_SIDE)
        {
            QGraphicsRectItem* block = add_block(x, BORDER_UP + SQUARE_SIDE, color);

            shape.push_back(block);
        }
    }

    if(tetromino_number == 3)
    {
        for(int x = MIDDLE_X - SQUARE_SIDE; x
            < MIDDLE_X +  SQUARE_SIDE; x += SQUARE_SIDE)
        {
            for(int y = BORDER_UP;
                y < BORDER_UP + 2* SQUARE_SIDE; y+= SQUARE_SIDE)
            {
                QGraphicsRectItem* block = add_block(x, y, color);

                shape.push_back(block);
            }
        }
    }

    if(tetromino_number == 4)
    {
        QGraphicsRectItem* block1 = add_block(MIDDLE_X, BORDER_UP, color);

        shape.push_back(block1);

        QGraphicsRectItem* block2 = add_block(MIDDLE_X + SQUARE_SIDE,
                                              BORDER_UP, color);

        shape.push_back(block2);

        QGraphicsRectItem* block3 = add_block(MIDDLE_X,
                                              BORDER_UP + SQUARE_SIDE, color);

        shape.push_back(block3);

        QGraphicsRectItem* block4 = add_block(MIDDLE_X - SQUARE_SIDE,
                                             BORDER_UP + SQUARE_SIDE, color);

        shape.push_back(block4);
    }

    if(tetromino_number == 5)
    {
        QGraphicsRectItem* block = add_block(MIDDLE_X, BORDER_UP, color);

        shape.push_back(block);

        for(int x = MIDDLE_X - SQUARE_SIDE;
            x < MIDDLE_X + 2 * SQUARE_SIDE; x += SQUARE_SIDE)
        {
            QGraphicsRectItem* block =
                    add_block(x, BORDER_UP + SQUARE_SIDE, color);

            shape.push_back(block);
        }
    }

    if(tetromino_number == 6)
    {
        QGraphicsRectItem* block1 =
                add_block(MIDDLE_X, BORDER_UP, color);

        shape.push_back(block1);

        QGraphicsRectItem* block2 =
                add_block(MIDDLE_X - SQUARE_SIDE, BORDER_UP, color);

        shape.push_back(block2);

        QGraphicsRectItem* block3 =
                add_block(MIDDLE_X, BORDER_UP + SQUARE_SIDE, color);

        shape.push_back(block3);

        QGraphicsRectItem* block4 =
                add_block(MIDDLE_X + SQUARE_SIDE,
                                             BORDER_UP + SQUARE_SIDE, color);

        shape.push_back(block4);
    }

    //Add the created vector (tetromino) to vector of all tetrominos
    tetrominos.push_back(shape);

    //Update the score
    score_ += 4;
    ui->blocksnumberLabel->setText(QString::number(score_));

    //Speeds up the falling rate of the blocks (up to specified point)
    if(gravity_timer.interval() > MAXIMUM_SPEED)
    {
        gravity_timer.setInterval(gravity_timer.interval() * SPEED_CHANGE_RATE);
    }





}

bool MainWindow::block_can_move(QPoint point)
{
    //Checks if a block can move to the specified point
    //(no other blocks are there and it is inside the game boundaries)
    if(not scene_->sceneRect().contains(point))
    {
        return false;
    }


    for(auto shape: tetrominos)
    {
        for(auto rect: shape)
        {
            if(rect->x() == point.x() && rect->y() == point.y())
            {
                return false;
            }
        }
    }
    return true;

}

void MainWindow::drop_current()
{
    //Command the block to move down as many times as there are rows
    //on the board
    for(int i = 0; i <= BORDER_DOWN / SQUARE_SIDE; i++)
    {
        move_block(tetrominos.back(), "DOWN");
    }
}

bool MainWindow::shape_can_move(std::vector<QGraphicsRectItem*> shape, std::string dir)
{
    for(auto block:shape)
    {
        QPoint point = new_location(block, dir);
        if(not block_can_move(point))
        {
            //If there is a block taking up the space, check if
            //the block belongs to the same tetromino
            //(the block will naturally move in the same direction)
            bool occupied_by_buddy = false;

            for(auto testblock: shape)
            {
                if(QPoint(testblock->x(), testblock->y()) == point)
                {
                    occupied_by_buddy = true;
                }
            }
            if(occupied_by_buddy == false)
            {
                return false;
            }
        }
    }
    return true;
}

void MainWindow::flip_shape()
{
    //Note: only works with shapes that are 2 blocks high since it mirrors
    //them vertically

    //Define higher_y as the lowest point on the board
    qreal higher_y = BORDER_DOWN;

    //Find the highest point of the shape
    for(auto block: tetrominos.back())
    {
        if(block->y() <= higher_y)
        {
            higher_y = block->y();
            QPoint newpoint = QPoint(block->x(), block->y() + 2 * SQUARE_SIDE);
            if(not block_can_move(newpoint))
            {
                return;
            }
        }
    }

    for(auto block:tetrominos.back())
    {
        //If the block is on the higher row, move it below the lower row
        if(block->y() == higher_y)
        {
            block->setY(block->y() + 2 * SQUARE_SIDE);
        }

        //Move the whole shape up to avoid it falling faster
        //because of the flip
        block->setY(block->y() - SQUARE_SIDE);
    }
}

void MainWindow::game_over()
{
    //Color all blocks gray
    QBrush brush = QBrush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::gray);
    for(auto shape: tetrominos)
    {
        for(auto block: shape)
        {
            block->setBrush(brush);
        }
    }

    gravity_timer.stop();
    time_played_timer.stop();

    ui->gameoverLabel->show();

    ui->playernameLineEdit->setEnabled(true);
    ui->submitscorePushButton->setEnabled(true);

    releaseKeyboard();


    ui->leftPushButton->setDisabled(true);
    ui->rightPushButton->setDisabled(true);
    ui->downPushButton->setDisabled(true);
    ui->dropPushButton->setDisabled(true);
    ui->startPushButton->setDisabled(true);
    ui->comboBox->setDisabled(true);
    ui->flipPushButton->setDisabled(true);

}

void MainWindow::readhiscore()
{
    ui->hiscoreTextBrowser->clear();

    std::ifstream infile("tetrishiscore.txt");
    std::string line;
    std::string name;
    std::string score;

    bool get_name = false;
    std::map<int, std::vector<std::string>> hiscoremap;

    if(infile.is_open())
    {
        while(std::getline(infile, line))
        {
            for(auto c: line)
            {
                if(c != ';')
                {
                    if(get_name)
                    {
                        name.push_back(c);
                    }

                    else
                    {
                        score.push_back(c);
                    }
                }

                else
                {
                    get_name = true;
                }
            }
            if(hiscoremap.find(std::stoi(score)) == hiscoremap.end())
            {
                hiscoremap[std::stoi(score)] = {name};
            }
            else
            {
                hiscoremap.at(std::stoi(score)).push_back(name);
            }

            name = "";
            score = "";
            get_name = false;
        }
    }

    infile.close();

    int i = 1;
    std::string hiscoretext;
    for(auto it = hiscoremap.rbegin(); it != hiscoremap.rend(); it++)
    {
        for(auto a: it->second)
        {
            hiscoretext = std::to_string(i) + ". " + a + " : " +
                    std::to_string(it->first) + " blocks";
            ui->hiscoreTextBrowser->append(hiscoretext.c_str());
        }
        i++;
    }
}

void MainWindow::writehiscore()
{
    if(ui->playernameLineEdit->text() == "")
    {
        return;
    }

    std::string name = ui->playernameLineEdit->text().toStdString();
    std::string score = std::to_string(score_);

    std::ofstream outfile("tetrishiscore.txt", std::ofstream::app);
    outfile << score << ";" << name << std::endl;
    outfile.close();

    readhiscore();
}

void MainWindow::tick_time()
{
    time_played_sec_ += 1;
    if(time_played_sec_ > 60)
    {
        time_played_sec_ = 0;
        time_played_min_ += 1;
    }
    QString sec_text = QString::number(time_played_sec_);
    QString min_text = QString::number(time_played_min_);

    ui->timeLabel->setText(min_text + " min " + sec_text + " sec");

}

void MainWindow::set_difficulty(int diff)
{
    //Difficulty level sets the interval with which the blocks
    //fall 1 step. The interval is difficulty² * 50
    //where 1 <= difficulty <= 4
    timerinterval_ = timerinterval_ - (diff*diff*50);
}


void MainWindow::on_downPushButton_clicked()
{
    move_block(tetrominos.back(), "DOWN");

    //Restart timer so it wont tick immediately after pressing down
    gravity_timer.start();
}

void MainWindow::on_leftPushButton_clicked()
{
    move_block(tetrominos.back(), "LEFT");
}

void MainWindow::on_rightPushButton_clicked()
{
    move_block(tetrominos.back(), "RIGHT");
}

void MainWindow::on_startPushButton_clicked()
{
    create_random_tetromino();

    gravity_timer.start(timerinterval_);
    time_played_timer.start(1000);

    ui->startPushButton->setDisabled(true);
    ui->playernameLineEdit->setDisabled(true);

    grabKeyboard();
}

void MainWindow::on_downPushButton_pressed()
{
    //Speeds up the falling of the blocks as long as
    //the down pushbutton is held
    timerinterval_ = gravity_timer.interval();
    gravity_timer.setInterval(50);
}

void MainWindow::on_downPushButton_released()
{
    gravity_timer.setInterval(timerinterval_);
}



void MainWindow::on_submitscorePushButton_clicked()
{
    writehiscore();

    //Can only submit score once
    ui->submitscorePushButton->setDisabled(true);
}


void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    //Can only start game if difficulty is set
    if(index == 0)
    {
        ui->startPushButton->setEnabled(false);
    }

    else
    {
        set_difficulty(index);
        ui->startPushButton->setEnabled(true);
    }
}

void MainWindow::on_dropPushButton_clicked()
{
    drop_current();
}

void MainWindow::on_flipPushButton_clicked()
{
    flip_shape();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left)
    {
        on_leftPushButton_clicked();
    }

    if(event->key() == Qt::Key_Right)
    {
        on_rightPushButton_clicked();
    }

    //Works differently to holding the downPushButton, because
    //the autorepeat function of keyboards was causing issues
    //with blocks not falling as they should.
    if(event->key() == Qt::Key_Down)
    {
        move_block(tetrominos.back(), "DOWN");
    }

    if(event->key() == Qt::Key_Space)
    {
        on_flipPushButton_clicked();
    }

    if(event->key() == Qt::Key_Control)
    {
        on_dropPushButton_clicked();
    }


}
