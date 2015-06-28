#ifndef XORSTAR
#define XORSTAR

#include <QGraphicsItem>
#include <QPainter>
#include <QPoint>
#include <QImage>

class XorStar:public QGraphicsItem
{
public:
    explicit XorStar();

    ~XorStar();

    virtual QRectF boundingRect() const;

    virtual void paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget );

    void setStarNum(int starn);

    void setX(int xpos);

    void setY(int ypos);


private:
    int star_num;
    int x, y;

};


#endif // XORSTAR

