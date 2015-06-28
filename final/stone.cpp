#include "stone.h"


static const char *img[50] = {
    ":/images/red.png",
    ":/images/green.png",
    ":/images/blue.png",
    ":/images/orange.png",
    ":/images/purple.png",
    ":/images/redHR.png",
    ":/images/greenHR.png",
    ":/images/blueHR.png",
    ":/images/orangeHR.png",
    ":/images/purpleHR.png",
    ":/images/redVR.png",
    ":/images/greenVR.png",
    ":/images/blueVR.png",
    ":/images/orangeVR.png",
    ":/images/purpleVR.png",
    ":/images/redBomb.png",
    ":/images/greenBomb.png",
    ":/images/blueBomb.png",
    ":/images/orangeBomb.png",
    ":/images/purpleBomb.png",
    ":/images/star.png",
    ":/images/redSelected.png",
    ":/images/greenSelected.png",
    ":/images/blueSelected.png",
    ":/images/orangeSelected.png",
    ":/images/purpleSelected.png",
    ":/images/redHRSelected.png",
    ":/images/greenHRSelected.png",
    ":/images/blueHRSelected.png",
    ":/images/orangeHRSelected.png",
    ":/images/purpleHRSelected.png",
    ":/images/redVRSelected.png",
    ":/images/greenVRSelected.png",
    ":/images/blueVRSelected.png",
    ":/images/orangeVRSelected.png",
    ":/images/purpleVRSelected.png",
    ":/images/redBombSelected.png",
    ":/images/greenBombSelected.png",
    ":/images/blueBombSelected.png",
    ":/images/orangeBombSelected.png",
    ":/images/purpleBombSelected.png",
    ":/images/starSelected.png"
};



Stone::Stone(int k)
{
    kind = k;
    select = false;
    ignorecheck = false;
    needremove = false;
}



Stone::~Stone()
{

}



bool Stone::operator ==(Stone &s)
{
    if(color==s.getcolor()){
        return true;
    }else{
        return false;
    }

}



int Stone::getcolor()
{
    return color;
}



void Stone::setcolor(int c)
{
    color = c;
}



int Stone::getrow()
{
    return row;
}



int Stone::getcol()
{
    return col;
}



void Stone::setrow(int r)
{
    row = r;
}



void Stone::setcol(int c)
{
    col = c;
}



int Stone::getkind()
{
    return kind;
}



void Stone::setkind(int k)
{
    kind = k;
}


bool Stone::getignorecheck()
{
    return ignorecheck;
}



void Stone::setignorecheck(bool ignore)
{
    ignorecheck = ignore;
}



bool Stone::getneedremove()
{
    return needremove;
}


void Stone::setneedremove(bool needrv)
{
    needremove = needrv;
}



bool Stone::getselect()
{
    return select;
}



void Stone::setselect(bool s)
{
    select = s;
}



QRectF Stone::boundingRect() const
{
    return QRectF((col*Swidth)+col+lefttop_x, (row*Sheight)+row+lefttop_y, 50, 50);
}



void Stone::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *option,
                  QWidget *widget)
{
    int imgpos;//圖片在img陣列的位置.
    QPoint point;
    point.setX((col*Swidth)+col+lefttop_x);
    point.setY((row*Sheight)+row+lefttop_y);



    if(select){
        if(kind==STAR){
            imgpos = 41;
        }else{
            imgpos = (kind*5)+color+21;
        }

        painter->drawImage(point, QImage(img[imgpos]));
    }else{
        if(kind==STAR){
            imgpos = 20;
        }else{
            imgpos = (kind*5)+color;
        }

        painter->drawImage(point, QImage(img[imgpos]));
    }

}
