#ifndef BOARDDIALOG_H
#define BOARDDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QTimer>
#include <QTime>

#include "canvas.h"
#include "gamestate.h"
#include "aibotbase.h"
#include "randombot.h"
#include "mctsbot.h"

namespace Ui {
class BoardDialog;
}

class BoardDialog : public QDialog
{
    Q_OBJECT
    friend class AiBot;
public:
    explicit BoardDialog(QWidget *parent,
    unsigned int boardSize,
    double radius,
    double padding,
    QString mode,
    QString color,
    unsigned int time,
    QString bot,
    QString node,
    bool recycling,
    unsigned int budget);
    ~BoardDialog();

    BoardDialog(const BoardDialog&)=delete;
    BoardDialog& operator=(const BoardDialog&)=delete;

private slots:
    void on_quitButton_clicked();

    void on_startButton_clicked();

    void updateCountdown();

public slots:
    void updateGameState(unsigned int cellIdx, unsigned int pieceIdx);
    void updateFromAiBot();

private:
    // ---- timers ----
    void initTimers();
    void startTimer(bool restart);
    void stopTimer();
    void switchTimers();

    // ---- control panel ----
    void updateControlPanel();
    void freezeControlPanel(const Color winner);

    // ---- ui ----
    Ui::BoardDialog *ui;
    Canvas* canvas;

    // ---- game state ----
    Color playerColor;
    bool inGame;
    GameState gameState;
    AiBotBase* aiBot;

    // ---- timers ----
    unsigned int time;
    unsigned int rTimeWhite;
    unsigned int rTimeBlack;
    QTimer* whiteTimer;
    QTimer* blackTimer;
    unsigned int whiteCounter;
    unsigned int blackCounter;
    QTime timeWhite;
    QTime timeBlack;
    Color currPlayer;

signals:
    void back_to_main();
};

#endif // BOARDDIALOG_H
