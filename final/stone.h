#ifndef STONE
#define STONE

#include <QGraphicsItem>
#include <QPainter>
#include <QImage>
#include <QVector>
#include <QPoint>
#include <QString>
#include "constant.h"

class Stone : public QGraphicsItem
{
public:
    explicit Stone(int k=0);

    ~Stone();

    bool operator ==(Stone &s);//==運算子多載.

    virtual QRectF boundingRect() const;

    virtual void paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget );

    void setcolor(int c);

    int getcolor();

    int getrow();

    int getcol();

    void setrow(int r);

    void setcol(int c);

    bool getignorecheck();

    void setignorecheck(bool ignore);

    bool getneedremove();

    void setneedremove(bool needrv);

    bool getselect();

    void setselect(bool s);

    int getkind();

    void setkind(int k);


protected:
    int kind;//石頭的種類.
    int color;//石頭的顏色.
    int row, col;//石頭的位置, 在row列, col行.
    bool select;//是否點選的flag.
    bool ignorecheck;//是否忽略檢查本石頭的flag.
    bool needremove;//是否需要消除本石頭的flag.

};


#endif // STONE

