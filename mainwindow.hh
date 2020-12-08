/* Tetris project: mainwindow.hh
 *
 * Header file for the main window
 *
 * Program author/editor:
 * Name: Rasmus Kivinen
 * Student number: 285870
 * UserID: kivinenr
 * E-Mail: rasmus.kivinen@tuni.fi
 * */

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QKeyEvent>
#include <random>
#include <fstream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_downPushButton_clicked();

    void on_leftPushButton_clicked();

    void on_rightPushButton_clicked();

    void on_startPushButton_clicked();

    void on_downPushButton_pressed();

    void on_downPushButton_released();

    void on_submitscorePushButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_dropPushButton_clicked();

    void on_flipPushButton_clicked();

    void keyPressEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;

    QGraphicsScene* scene_;

    // Constants describing scene coordinates
    // Copied from moving circle example and modified a bit
    const int BORDER_UP = 0;
    const int BORDER_DOWN = 480; // 260; (in moving circle)
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = 240; // 680; (in moving circle)
    const int MIDDLE_X = 120;

    //Constants defining max speed of blocks and how fast it changes
    //as the game goes on
    const int MAXIMUM_SPEED = 100;
    const float SPEED_CHANGE_RATE = 0.95;

    // Size of a tetromino component
    const int SQUARE_SIDE = 20;
    // Number of horizontal cells (places for tetromino components)
    const int COLUMNS = BORDER_RIGHT / SQUARE_SIDE;
    // Number of vertical cells (places for tetromino components)
    const int ROWS = BORDER_DOWN / SQUARE_SIDE;

    // Constants for different tetrominos and the number of them
    enum Tetromino_kind {HORIZONTAL,
                         LEFT_CORNER,
                         RIGHT_CORNER,
                         SQUARE,
                         STEP_UP_RIGHT,
                         PYRAMID,
                         STEP_UP_LEFT,
                         NUMBER_OF_TETROMINOS};
    // From the enum values above, only the last one is needed in this template.
    // Recall from enum type that the value of the first enumerated value is 0,
    // the second is 1, and so on.
    // Therefore the value of the last one is 7 at the moment.
    // Remove those tetromino kinds above that you do not implement,
    // whereupon the value of NUMBER_OF_TETROMINOS changes, too.
    // You can also remove all the other values, if you do not need them,
    // but most probably you need a constant value for NUMBER_OF_TETROMINOS.


    // For randomly selecting the next dropping tetromino
    std::default_random_engine randomEng;
    std::uniform_int_distribution<int> distr;

    /**
     * @brief add_block Adds a single square to the coordinates
     * @param x coordinate
     * @param y coordinate
     * @param color of the bock
     * @return Returns pointer to created block
     */
    QGraphicsRectItem* add_block(int x, int y, QColor color);

    /**
     * @brief move_block Moves shape one SQUARE_SIZE in specified direction
     * @param shape pointer to shape that is wanted to move
     * @param dir direction to move the block in(string, e.g. "DOWN")
     */
    void move_block(std::vector<QGraphicsRectItem*> shape, std::string dir);

    /**
     * @brief drop_all Tries to move all shapes down one SQUARE_SIZE
     */
    void drop_all();

    /**
     * @brief new_location Creates a QPoint from given block and direction to move in
     * @param block whose cordinates this function inspects
     * @param dir direction which to create the point in
     * @return the created QPoint
     */
    QPoint new_location(QGraphicsRectItem* block, std::string dir);

    /**
     * @brief create_random_tetromino Creates a tetromino whose shape is based on a random value
     */
    void create_random_tetromino();

    /**
     * @brief block_can_move Checks if a block can move to the specified point
     *                       (point is within game bounds and not occupied)
     * @param point to check
     * @return bool of whether moving is possible or not
     */
    bool block_can_move(QPoint point);

    /**
     * @brief drop_current drops the active tetromino as far down as it can go
     */
    void drop_current();

    /**
     * @brief shape_can_move Checks if the whole shape can move in the specified direction
     * @param shape pointer to shape
     * @param dir direction to check as a string, e.g. "DOWN"
     * @return bool of whether the shape can move or not
     */
    bool shape_can_move(std::vector<QGraphicsRectItem*> shape, std::string dir);

    /**
     * @brief flip_shape mirrors the active tetromino vertically
     */
    void flip_shape();

    /**
     * @brief game_over Stops the timers, disables most of the UI,
     * allows player to enter hiscore
     */
    void game_over();

    /**
     * @brief readhiscore Reads hiscores from file
     */
    void readhiscore();

    /**
     * @brief writehiscore writes hiscores to file
     */
    void writehiscore();

    /**
     * @brief tick_time Advances the timeplayedLabel by 1 second, handles
     *        minute/second conversion
     */
    void tick_time();

    /**
     * @brief set_difficulty Sets how fast the blocks fall in the beginning of the game
     * @param diff difficulty level 1-4
     */
    void set_difficulty(int diff);

    // Vector containing all the tetrominos on the board
    std::vector<std::vector<QGraphicsRectItem*>> tetrominos;

    // Vector of colors for the tetrominos, used in create_random_tetromino
    std::vector<QColor> colors = {QColor("cyan"), QColor("magenta"), QColor("red"),
                          QColor("yellow"), QColor("darkCyan"), QColor("darkMagenta"),
                          QColor("green"), QColor("darkGreen"), QColor("red"),
                          QColor("blue")};


    // More constants, attibutes, and methods

    //Timers for making the blocks fall and recording time played
    QTimer gravity_timer;
    QTimer time_played_timer;

    int time_played_sec_ = 0;
    int time_played_min_ = 0;

    int score_ = 0;
    int timerinterval_ = 1000;

};

#endif // MAINWINDOW_HH
