#include "xorstar.h"

XorStar::XorStar()
{

}



XorStar::~XorStar()
{

}



void XorStar::setStarNum(int starn)
{
    star_num = starn;
}



void XorStar::setX(int xpos)
{
    x = xpos;
}



void XorStar::setY(int ypos)
{
    y = ypos;
}




QRectF XorStar::boundingRect() const
{
    return QRectF(265, 50, 90, 25);
}



void XorStar::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *option,
                  QWidget *widget)
{
    //顯示星星數.
    if(star_num==-1){
        painter->drawImage(QPoint(x, y), QImage(":/images/fail.png"));
    }

    if(star_num==1){
        painter->drawImage(QPoint(x, y), QImage(":/images/star1.png"));
    }

    if(star_num==2){
        painter->drawImage(QPoint(x, y), QImage(":/images/star2.png"));
    }

    if(star_num==3){
        painter->drawImage(QPoint(x, y), QImage(":/images/star3.png"));
    }
}
