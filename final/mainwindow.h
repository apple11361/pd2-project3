#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <time.h>
#include <QMainWindow>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QStyleOptionGraphicsItem>
#include <QString>
#include <QVector>
#include <QList>
#include <QPointF>
#include <QPainter>
#include <QMouseEvent>
#include <QTime>
#include <QTimer>
#include "stone.h"
#include "xorstar.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //void init();

    void initMatrix();

    void createStone(int row, int col);

    void initCheck();

    void startGame();//開始遊戲.

    void afterFillCheck();

    void getLeftRightList(Stone *checkedStone, int check);//check==1檢查來源stone, check==2檢查目的stone.

    void getUpDownList(Stone *checkedStone, int check);//check==1檢查來源stone, check==2檢查目的stone.

    //void initRemove(Stone *s);

    void markRemove(Stone *s);

    void removeStone();

    void FillMatrix();

    void mousePressEvent(QMouseEvent * event);

    void changeStone();


signals:
    void quit(int starnum, int score);


public slots:
    void displayStatus();



private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsTextItem *scoretext, *scoreitem, *starnumtext;//, *starnumitem;
    QGraphicsTextItem *steptext, *stepitem;
    QGraphicsTextItem *timetext, *timeitem;
    QGraphicsTextItem *Goal;


    XorStar *starnumitem, *Resultsitem;


    QVector< QVector<Stone*> > stoneMatrix;// stone的2維陣列.


    QList<Stone*> sLeftList, sRightList, sUpList, sDownList, slongerList,
                  dLeftList, dRightList, dUpList, dDownList, dlongerList, ignoreCheckList;
    bool hasStoneRemove;//是否有stone被消除的flag.
    bool needFillMatrix;//Matrix是否要補充stone的flag.
    bool enableClick, sourceselected;//enableClick:true 可操作遊戲 ,false 暫不允許操作遊戲.
    bool enableSTAR;//控制星星是否作用的flag.
    Stone *sourceStone, *destStone;//sourceStone:先點選的來源石頭, destStone:想交換的目的石頭.
    int tempscore;//累計一次交換所得分數.
    int score;//遊戲得分.
    int starnum;//遊戲分數獲得的星星數;
    int step;//交換石頭的次數.

    QFont font;
    QString str;//顯示分數和星星數的QGraphicsTextItem scoreitem, starnumitem要使用的.
    QTimer *timer;
    QTime currTime;//目前時間.
    int startHour, startMin, startSec;//遊戲開始的時, 分, 秒.
    int elapsedMin, elapsedSec, elapsedMilliSec;//遊戲經過的分, 秒, 毫秒.



};

#endif // MAINWINDOW_H
