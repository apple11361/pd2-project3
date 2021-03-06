#include "mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    srand(time(NULL));//初始化亂數.
    resize(800, 750);//調整MainWindow的視窗尺寸.
    scene = new QGraphicsScene;//建立1個場景scene.
    scene->setSceneRect(0, 0, 780, 740);//設定場景左上角座標和寬高.
    view = new QGraphicsView;//建立1個視圖view.
    view->setScene(scene);//將view的場景設為scene.
    setCentralWidget(view);//將view設為MainWindow的中央工作區.
    view->setStyleSheet("background-image: url(:/images/StoneCrush.png)");//view的背景.

    startGame();
}



MainWindow::~MainWindow()
{
    emit quit(starnum, score);
    delete timer;
    for(int i=0; i<Mheight; ++i){
        for(int j=0; j<Mwidth; ++j){
            delete stoneMatrix[j][i];
        }
    }
}



void MainWindow::startGame()//開始遊戲.
{
    initMatrix();//初始化石頭陣列.
    initCheck();//初始檢查石頭陣列(簡化式檢查).
    while(needFillMatrix){//如果需要補充石頭, 則補充, 再檢查, 直到不必再補充石頭.
        FillMatrix();
        initCheck();
    }

    sourceselected = false;//已選定來源石頭的flag設為false.
    sourceStone = new Stone;//新建來源石頭.
    destStone = new Stone;//新建目的石頭.

    tempscore = 0;//單次操作得分歸零.
    score = 0;//遊戲分數歸零.
    starnum = 0;//星星數歸零.
    step = 0;//交換次數歸零;

    enableClick = true;//允許滑鼠操作遊戲.

    font.setFamily(QString("Tunga"));//設定字體.
    font.setBold(true);//設定粗字體.



    font.setPointSize(16);//設定字體大小.

    //從scene獲得QGraphicsTextItem物件 Goal, Goal用來顯示過關條件的說明文字.
    Goal = scene->addText("在3分鐘內獲得2000分則過關");
    Goal->setFont(font);//設定Goal的字型.
    Goal->setPos(130, 80);//設定Goal的座標.
    Goal->setDefaultTextColor(QColor(200,70,190,255));//設定Goal顯示的文字顏色.



    font.setPointSize(20);

    //從scene獲得1個用來顯示常數文字"score"的QGraphicsTextItem物件 scoretext.
    scoretext = scene->addText("score");
    scoretext->setFont(font);//設定scoretext的字型.
    scoretext->setPos(130, 10);//設定scoretext的座標.
    scoretext->setDefaultTextColor(QColor(150,70,100,255));//設定scoretext顯示的文字顏色.

    //從scene獲得1個用來顯示分數的QGraphicsTextItem物件 scoreitem.
    scoreitem = scene->addText("0");//先顯示"0", 因為開始遊戲時分數為0.
    scoreitem->setFont(font);//設定scoreitem的字型.
    scoreitem->setPos(152, 35);//設定scoreitem的座標.
    scoreitem->setDefaultTextColor(QColor(150,70,100,255));//設定scoreitem顯示的文字顏色.



    //從scene獲得1個用來顯示常數文字"star"的QGraphicsTextItem物件 starnumtext.
    starnumtext = scene->addText("star");
    starnumtext->setFont(font);//設定starnumtext的字型.
    starnumtext->setPos(290, 10);//設定starnumtext的座標.
    starnumtext->setDefaultTextColor(QColor(150,70,100,255));//設定starnumtext顯示的文字顏色.

    //建立1個用來顯示星星的XorStar物件 starnumitem.
    //XorStar繼承QGraphicsItem, 所以starnumitem顯示的是圖形.
    starnumitem = new XorStar;
    starnumitem->setStarNum(0);//設0給starnumitem的star_num, 設定不顯示任何東西.
    starnumitem->setX(265);//設定x座標.
    starnumitem->setY(50);//設定y座標.
    scene->addItem(starnumitem);//將顯示星星圖片的starnumitem加入scene, 顯示星星數.



    //從scene獲得1個用來顯示常數文字"time"的QGraphicsTextItem物件 timetext.
    timetext = scene->addText("time");
    timetext->setFont(font);//設定timetext的字型.
    timetext->setPos(440,10);//設定timetext的座標.
    timetext->setDefaultTextColor(QColor(150,70,100,255));//設定timetext顯示的文字顏色.

    //從scene獲得1個用來顯示使用時間的QGraphicsTextItem物件 timeitem.
    timeitem = scene->addText("");//先顯示空字串.
    timeitem->setFont(font);//設定timeitem的字型.
    timeitem->setPos(440, 35);//設定timeitem的座標.
    timeitem->setDefaultTextColor(QColor(150,70,100,255));//設定timeitem顯示的文字顏色.



    //從scene獲得1個用來顯示常數文字"step"的QGraphicsTextItem物件 steptext.
    steptext = scene->addText("step");
    steptext->setFont(font);//設定steptext的字型.
    steptext->setPos(580,10);//設定steptext的座標.
    steptext->setDefaultTextColor(QColor(150,70,100,255));//設定steptext顯示的文字顏色.

    //從scene獲得1個用來顯示交換次數的QGraphicsTextItem物件 stepitem.
    stepitem = scene->addText("0");//先顯示"0".
    stepitem->setFont(font);//設定stepitem的字型.
    stepitem->setPos(590,35);//設定stepitem的座標.
    stepitem->setDefaultTextColor(QColor(150,70,100,255));//設定stepitem顯示的文字顏色.


    currTime = QTime::currentTime();//建立QTime物件currTime, 記錄開始遊戲的時間.


    timer = new QTimer(this);//建立計時器 timer.

    //建立信號槽關係, timer發出timeout() signal, 就執行this(MainWindow)
    //的slot function displayStatus().
    connect(timer, SIGNAL(timeout()), this, SLOT(displayStatus()));
    timer->start(1000);//timer開始計時, 並設定每秒發出timeout().
}



void MainWindow::displayStatus()
{
    QString strM, strS, strTime;
    int tempSec;

    //設定顯示分數的座標.因為QGraphicsTextItem沒有置中對齊的功能, 只能自己計算座標.
    if(score>=10000){
        scoreitem->setX(125);
    }else if(score>=1000){
        scoreitem->setX(133);
    }else if(score>=100){
        scoreitem->setX(138);
    }else if(score>=10){
        scoreitem->setX(143);
    }else if(score<10){
        scoreitem->setX(145);
    }

    str.setNum(score);//將score轉為字串.
    scoreitem->setPlainText(str);//顯示分數(score).



    //判斷獲得的星星數.
    if(score>=4000){//超過4000分, 獲得3顆星.
        starnum = 3;
    }else if(score>=3000){//超過3000分, 獲得2顆星.
        starnum = 2;
    }else if(score>=2000){//超過2000分, 獲得1顆星.
        starnum = 1;
    }else if(score<2000){//少於2000分, 失敗, 獲得0顆星.
        starnum = 0;
    }

    starnumitem->setStarNum(starnum);//將starnum設給starnumitem的star_num, 顯示星星.
    view->repaint();//view重繪.



    elapsedMilliSec = currTime.elapsed();//設定currTime後經過的毫秒數.
    tempSec =  elapsedMilliSec/1000;//轉為秒.
    elapsedMin = tempSec/60;//計算經過的分.
    elapsedSec = tempSec%60;//計算經過的秒.

    strM.setNum(elapsedMin);//分轉為字串.
    strS.setNum(elapsedSec);//秒轉為字串.
    strTime = strM + ":" + strS;//結合為"分:秒"

    timeitem->setPlainText(strTime);//顯示使用的時間.



    str.setNum(step);//step轉為字串.
    stepitem->setPlainText(str);//顯示交換次數(step).



    //規定的遊戲時間到, 且星星數為0, 則沒過關, 顯示"X", 表示 Fail. emit signal, 結束.
    if(tempSec>9 && starnum==0){
        enableClick = false;//停止滑鼠點擊操作.

        Resultsitem = new XorStar;//建立1個用來顯示"X"的物件.
        Resultsitem->setX(345);//設定顯示"X"的x座標.
        Resultsitem->setY(680);//設定顯示"X"的y座標.
        Resultsitem->setStarNum(-1);//讓Resultsitem畫出"X".

        scene->addItem(Resultsitem);//將Resultsitem加入場景scene中, 讓Resultsitem顯示出來.

        view->repaint();//view重繪.

        //送出void quit(int starnum, int score)的signal, 傳遞星星數和分數.
        emit quit(starnum, score);
        this->close();//結束程式.

    //規定的遊戲時間到, 且星星數>0, 則過關, 顯示星星, 表示 Pass. emit signal, 結束.
    }else if(tempSec>9 && starnum>0){
        enableClick = false;//停止滑鼠點擊操作.

        Resultsitem = new XorStar;//建立1個用來顯示星星的物件.
        Resultsitem->setX(345);//設定顯示星星的x座標.
        Resultsitem->setY(680);//設定顯示星星的y座標.
        Resultsitem->setStarNum(starnum);//讓Resultsitem畫出星星.

        scene->addItem(Resultsitem);//將Resultsitem加入場景scene中, 讓Resultsitem顯示出來.

        view->repaint();//view重繪.

        //送出void quit(int starnum, int score)的signal, 傳遞星星數和分數.
        emit quit(starnum, score);
        this->close();//結束程式.
    }
}




void MainWindow::initMatrix()//初始化儲存石頭的2維陣列stoneMatrix.
{
    stoneMatrix.resize(10);// 讓stoneMatrix成為10x10的2維陣列.
    for (int i = 0; i < 10; ++i)
        stoneMatrix[i].resize(10);

    for(int row=0; row<Mheight; ++row){
        for(int col=0; col<Mwidth; ++col){
            createStone(row, col);//隨機建立1個普通石頭.
        }
    }
}



void MainWindow::createStone(int row, int col)//隨機建立1個普通石頭.
{
    int stonecolor;
    stonecolor = rand()%StoneColor;//隨機選取石頭顏色.
    Stone *s = new Stone(NORMAL);//建立普通的石頭.
    s->setcolor(stonecolor);//color為stonecolor.
    s->setrow(row);//記住自己的row.
    s->setcol(col);//記住自己的col.
    scene->addItem(s);//將s加入場景scene中.
    stoneMatrix[col][row] = s;//將s存入stoneMatrix[col][row].
}



void MainWindow::initCheck()//初始盤面檢查.
{
    QString str;
    Stone *s;
    hasStoneRemove = false;//有石頭被消除的flag設為false.
    needFillMatrix = false;//需要補充石頭的flag設為false.

    for(int row=0; row<Mheight; ++row){
        for(int col=0; col<Mwidth; ++col){
            s = stoneMatrix[col][row];

            //如果正在檢查的石頭已被標記需消除, 則跳過, 換下一個.
            if(!s || s->getneedremove())continue;

            getLeftRightList(s, 1);//獲得s左右邊的檢查結果List, 結果放在sourceList.
            getUpDownList(s, 1);//獲得s上下邊的檢查結果List, 結果放在sourceList.

            //取較長的List.
            slongerList = sRightList.size()>sDownList.size()?sRightList:sDownList;

            if(slongerList.size()<2)continue;//如果List的長度<2, 不構成型態, 跳過.

            hasStoneRemove = true;//到這裡表示有石頭需要消除, hasStoneRemove設為true.

            s->setneedremove(true);//s的needremove設為true, 表示s需要消除.

            for(int i=0; i<slongerList.size(); ++i){//標記List中的石頭需要消除.
                s = slongerList.at(i);
                s->setneedremove(true);
            }
        }
    }

    //如果hasStoneRemove為true, 表示有石頭需要消除, 則needFillMatrix設為true, 表示需要補充石頭.
    if(hasStoneRemove){
        needFillMatrix = true;//needFillMatrix設為true, 表示需要補充石頭.
        removeStone();//消除所有needremove==true的石頭.
    }
}



//獲得checkedStone左右邊的檢查結果List, check==1表示檢查結果放在sourceList,
//check==2表示檢查結果放在destList.
void MainWindow::getLeftRightList(Stone *checkedStone, int check)
{
    Stone *LStone, *RStone;
    int LCol, RCol;

    if(check==1){//檢查結果放在sourceList.
        sLeftList.clear();//清空List.
        sRightList.clear();//清空List.
        LCol = checkedStone->getcol() - 1;//LCol:被檢查的石頭的左邊.
        while(LCol>=0){
            LStone = stoneMatrix[LCol][checkedStone->getrow()];

            //如果LStone存在, 沒標記需消除, 沒標記忽略, 兩個石頭顏色相同, 則將LStone加入sLeftList中.
            if(LStone && !LStone->getneedremove() && !LStone->getignorecheck() &&
               *LStone==*checkedStone){//*LStone==*checkedStone, ==運算子多載.
                sLeftList.append(LStone);
                --LCol;//檢查更左邊.
            }else{
                break;//有不相同時, 跳出檢查迴圈.
            }
        }

        RCol = checkedStone->getcol() + 1;//RCol:被檢查的stone的右邊.
        while(RCol<Mwidth){
            RStone = stoneMatrix[RCol][checkedStone->getrow()];

            //如果RStone存在, 沒標記需消除, 沒標記忽略, 兩個石頭顏色相同, 則將RStone加入sRightList中.
            if(RStone && !RStone->getneedremove() && !RStone->getignorecheck() &&
               *RStone==*checkedStone){//*RStone==*checkedStone, ==運算子多載.
                sRightList.append(RStone);
                ++RCol;//檢查更右邊.
            }else{
                break;//有不相同時, 跳出檢查迴圈.
            }
        }

    //check==2, 檢查結果放在destList.
    }else{
        dLeftList.clear();//清空List.
        dRightList.clear();//清空List.
        LCol = checkedStone->getcol() - 1;//LCol:被檢查的stone的左邊.
        while(LCol>=0){
            LStone = stoneMatrix[LCol][checkedStone->getrow()];

            //如果LStone存在, 沒標記需消除, 沒標記忽略, 兩個石頭顏色相同, 則將LStone加入dLeftList中.
            if(LStone && !LStone->getneedremove() && !LStone->getignorecheck() &&
               *LStone==*checkedStone){//*LStone==*checkedStone, ==運算子多載.
                dLeftList.append(LStone);
                --LCol;//檢查更左邊.
            }else{
                break;//有不相同時, 跳出檢查迴圈.
            }
        }

        RCol = checkedStone->getcol() + 1;//RCol:被檢查的stone的右邊.
        while(RCol<Mwidth){
            RStone = stoneMatrix[RCol][checkedStone->getrow()];

            //如果RStone存在, 沒標記需消除, 沒標記忽略, 兩個石頭顏色相同, 則將RStone加入dRightList中.
            if(RStone && !RStone->getneedremove() && !RStone->getignorecheck() &&
               *RStone==*checkedStone){//*RStone==*checkedStone, ==運算子多載.
                dRightList.append(RStone);
                ++RCol;
            }else{
                break;
            }
        }
    }
}



void MainWindow::getUpDownList(Stone *checkedStone, int check)
{
    Stone *UStone, *DStone;
    int URow, DRow;

    if(check==1){
        sUpList.clear();//清空List.
        sDownList.clear();//清空List.
        URow = checkedStone->getrow() - 1;//URow:被檢查的stone的上邊.
        while(URow>=0){
            UStone = stoneMatrix[checkedStone->getcol()][URow];

            //如果UStone存在, 沒標記需消除, 沒標記忽略, 兩個石頭顏色相同, 則將UStone加入sUpList中.
            if(UStone && !UStone->getneedremove() && !UStone->getignorecheck() &&
               *UStone==*checkedStone){//*UStone==*checkedStone, ==運算子多載.
                sUpList.append(UStone);
                --URow;//檢查更上邊.
            }else{
                break;//有不相同時, 跳出檢查迴圈.
            }
        }

        DRow = checkedStone->getrow() + 1;//DRow:被檢查的stone的下邊.
        while(DRow<Mheight){
            DStone = stoneMatrix[checkedStone->getcol()][DRow];

            //如果DStone存在, 沒標記需消除, 沒標記忽略, 兩個石頭顏色相同, 則將DStone加入sDownList中.
            if(DStone && !DStone->getneedremove() && !DStone->getignorecheck() &&
               *DStone==*checkedStone){//*DStone==*checkedStone, ==運算子多載.
                sDownList.append(DStone);
                ++DRow;//檢查更下邊.
            }else{
                break;//有不相同時, 跳出檢查迴圈.
            }
        }

    //check==2, 檢查結果放在destList.
    }else{
        dUpList.clear();//清空List.
        dDownList.clear();//清空List.
        URow = checkedStone->getrow() - 1;//URow:被檢查的stone的上邊.
        while(URow>=0){
            UStone = stoneMatrix[checkedStone->getcol()][URow];

            //如果UStone存在, 沒標記需消除, 沒標記忽略, 兩個石頭顏色相同, 則將UStone加入dUpList中.
            if(UStone && !UStone->getneedremove() && !UStone->getignorecheck() &&
               *UStone==*checkedStone){//*UStone==*checkedStone, ==運算子多載.
                dUpList.append(UStone);
                --URow;//檢查更上邊.
            }else{
                break;//有不相同時, 跳出檢查迴圈.
            }
        }

        DRow = checkedStone->getrow() + 1;//DRow:被檢查的stone的下邊.
        while(DRow<Mheight){
            DStone = stoneMatrix[checkedStone->getcol()][DRow];

            //如果DStone存在, 沒標記需消除, 沒標記忽略, 兩個石頭顏色相同, 則將DStone加入dDownList中.
            if(DStone && !DStone->getneedremove() && !DStone->getignorecheck() &&
               *DStone==*checkedStone){//*DStone==*checkedStone, ==運算子多載.
                dDownList.append(DStone);
                ++DRow;//檢查更下邊.
            }else{
                break;//有不相同時, 跳出檢查迴圈.
            }
        }
    }
}



void MainWindow::removeStone()//消除所有needremove==true的石頭.
{
    Stone *s;
    for(int row=0; row<Mheight; ++row){
        for(int col=0; col<Mwidth; ++col){
            s = stoneMatrix[col][row];
            if(!s)continue;//如果s==NULL則跳過.

            if(s->getneedremove()){
                scene->removeItem(s);//從場景scene中刪除s.
                stoneMatrix[col][row] = NULL;//原來s存放的位置設為NULL.
                tempscore +=10;//消除1個石頭獲得10分.
            }
        }
    }
}



//標記某顆石頭需要消除, 該顆石頭如果是特殊石頭, 則再做相應的處理.
void MainWindow::markRemove(Stone *s)
{
    int LCol, RCol, URow, DRow;
    Stone *tmpstone;
    if(s->getneedremove())return;//如果s已標記需消除則結束函數.
    if(s->getignorecheck())return;//如果s標記忽略則結束函數.

    //如果s是星星, 但color==-1, 表示s並非和其他石頭交換, 而是被其它特殊石頭處理到. 所以不處理s.
    if(s->getkind()==STAR && s->getcolor()==-1)return;

    s->setneedremove(true);//標記s需消除.

    //檢查s是否是特殊的石頭(星星,炸彈,水平消除,垂直消除), 如果是, 需再做處理.
    if(s->getkind()==STAR){
        //s是星星, 且data member color 存有要消除的石頭的顏色.

        for(int row=0; row<Mheight; ++row){
            for(int col=0; col<Mwidth; ++col){
                tmpstone = stoneMatrix[col][row];

                //如果tmpstone是空的, 或tmpstone就是s本身, 則跳過.
                if(NULL==tmpstone || tmpstone==s)continue;

                //如果tmpstone是普通石頭, 且是s星星要消除的顏色, 則標記需消除.
                if(tmpstone->getkind()==NORMAL && *tmpstone==*s){//*tmpstone==s, ==運算子多載.
                    tmpstone->setneedremove(true);

                //tmpstone是特殊石頭, 且是s星星要消除的顏色, 再呼叫markRemove(), 再做處理.
                }else if(tmpstone->getkind()!=NORMAL && *tmpstone==*s){//*tmpstone==*s, ==運算子多載.
                    markRemove(tmpstone);
                }
            }
        }

    }else if(s->getkind()==BOMB){//s如果是炸彈, 則標記s周圍的石頭需消除.
        LCol = s->getcol() - 1;
        RCol = s->getcol() + 1;
        URow = s->getrow() - 1;
        DRow = s->getrow() + 1;

        if(LCol<0){//如果超越左邊界, 則設LCol為0.
            LCol = 0;
        }

        if(RCol==Mwidth){//如果超越右邊界, 則設RCol為Mwidth-1.
            RCol = Mwidth - 1;
        }

        if(URow<0){//如果超越上邊界, 則設URow為0.
            URow = 0;
        }

        if(DRow==Mheight){//如果超越下邊界, 則設URow為Mheight-1.
            DRow = Mheight - 1;
        }

        for(int row=URow; row<=DRow; ++row){//檢查s的周圍.
            for(int col=LCol; col<=RCol; ++col){
                tmpstone = stoneMatrix[col][row];

                //如果tmpstone是空的, 或tmpstone就是s本身, 則跳過.
                if(NULL==tmpstone || tmpstone==s)continue;

                if(tmpstone->getkind()==NORMAL){//如果tmpstone是普通石頭, 則標記需消除.
                    tmpstone->setneedremove(true);
                }else{//tmpstone是特殊石頭, 再呼叫markRemove(), 再做處理.
                    markRemove(tmpstone);
                }
            }
        }

    }else if(s->getkind()==HR){//s如果是水平消除, 則標記整列石頭需消除.
        for(int col=0; col<Mwidth; ++col){
            tmpstone = stoneMatrix[col][s->getrow()];//檢查s所在列的所有行.

            if(NULL==tmpstone || tmpstone==s)continue;//如果tmpstone是空的, 或tmpstone就是s本身, 則跳過.

            if(tmpstone->getkind()==NORMAL){//如果tmpstone是普通石頭, 則標記需消除.
                tmpstone->setneedremove(true);
            }else{//tmpstone是特殊石頭, 再呼叫markRemove(), 再做處理.
                markRemove(tmpstone);
            }
        }

    }else if(s->getkind()==VR){//s如果是垂直消除, 則標記整行石頭需消除.
        for(int row=0; row<Mheight; ++row){
            tmpstone = stoneMatrix[s->getcol()][row];//檢查s所在行的所有列.

            if(NULL==tmpstone || tmpstone==s)continue;//如果tmpstone是空的, 或tmpstone就是s本身, 則跳過.

            if(tmpstone->getkind()==NORMAL){//如果tmpstone是普通石頭, 則標記需消除.
                tmpstone->setneedremove(true);
            }else{//tmpstone是特殊石頭, 再呼叫markRemove(), 再做處理.
                markRemove(tmpstone);
            }
        }
    }
}



void MainWindow::FillMatrix()//補充石頭.
{
    Stone *s;
    int removedStoneofCol;//在某行裡被消除的stone數.
    int colEmpty[10];//紀錄每行空缺數.

    for(int col=0; col<Mwidth; ++col){
        removedStoneofCol = 0;
        for(int row=Mheight-1; row>=0; --row){
            s = stoneMatrix[col][row];
            if(NULL==s){
                ++removedStoneofCol;//找到空缺, ++removedStoneofCol.
            }else{
                //如果removedStoneofCol>0, 表示s需往下移removedStoneofCol列.
                if(removedStoneofCol>0){
                    s->setrow(row+removedStoneofCol);//s被移到新列位置.
                    stoneMatrix[col][row+removedStoneofCol] = s;//s轉存新位置.
                    stoneMatrix[col][row] = NULL;//原位置設為NULL.
                }
            }
        }

        colEmpty[col] = removedStoneofCol;//紀錄本行的空缺數.
    }

    for(int col=0; col<Mwidth; ++col){//補充每一行空缺的石頭.
        for(int row=0; row<colEmpty[col]; ++row){
            createStone(row, col);
        }
    }

    view->repaint();//view重繪, 即時顯示新結果.
}



void MainWindow::mousePressEvent(QMouseEvent * event)//滑鼠點擊處理函數.
{
    if(!enableClick)return;//如果enableClick為false, 表示上次操作還在處理, 暫停操作遊戲.

    bool stoneSelected;
    Stone *s = new Stone;

    for(int row=0; row<Mheight; ++row){//檢查哪一個石頭被點擊到.
        for(int col=0; col<Mwidth; ++col){
            s = stoneMatrix[col][row];
            stoneSelected = s->contains(QPointF(event->x()-10, event->y()-5));
            if(stoneSelected)break;//已經找到被點擊的石頭, 跳出迴圈.
        }
        if(stoneSelected)break;//已經找到被點擊的石頭, 跳出迴圈.
    }

    if(stoneSelected){//有點擊到石頭.
        if(!sourceselected){//如果還沒選定來源石頭.
            sourceStone = s;//將s設為來源石頭.

            //將來源石頭的select設為true, 讓來源石頭的paint()畫出石頭被選定的圖片.
            sourceStone->setselect(true);
            view->repaint();//view重繪, 即時顯示新結果.
            sourceselected = true;
            return;//離開函數.
        }

        //又選到自己, 離開函數.
        if(s->getrow()==sourceStone->getrow() && s->getcol()==sourceStone->getcol())return;

        //選到來源石頭的旁邊.
        if((s->getrow()==sourceStone->getrow()-1 && s->getcol()==sourceStone->getcol()) ||
           (s->getrow()==sourceStone->getrow()+1 && s->getcol()==sourceStone->getcol()) ||
           (s->getcol()==sourceStone->getcol()-1 && s->getrow()==sourceStone->getrow()) ||
           (s->getcol()==sourceStone->getcol()+1 && s->getrow()==sourceStone->getrow())){
            destStone = s;//將s設為目的石頭.

            sourceStone->setselect(false);

            destStone->setselect(false);

            enableClick = false;//暫停操作遊戲.
            changeStone();//交換石頭.

            FillMatrix();//能交換石頭, 表示必有石頭被消除, 所以一定需要補充石頭.

            afterFillCheck();//補充石頭後的檢查, 看有無可得分型態, 有的話, 再得分, 再消除石頭.

            while(needFillMatrix){//補充石頭, 補充石頭後檢查的迴圈.
                FillMatrix();
                afterFillCheck();
            }

            //累計得分.
            score +=tempscore;

            //準備下一次操作.
            sourceStone = new Stone;
            destStone = new Stone;
            sourceselected = false;

            enableClick = true;//允許操作遊戲.

        }else{//選到的石頭不在來源石頭的旁邊, 將這次選到的石頭設為來源石頭.
            sourceStone->setselect(false);//將原來源石頭的select設為false.
            sourceStone = s;//將s設為新的來源石頭.
            sourceStone->setselect(true);//將新的來源石頭的select設為true.
            view->repaint();//view重繪, 即時顯示新結果.

        }
    }
}



void MainWindow::changeStone()//交換石頭.
{
    Stone *s;
    int tmprow, tmpcol, sHPattern, sVPattern, sLPattern_LU, sLPattern_RU, sLPattern_LD, sLPattern_RD,
        dHPattern, dVPattern, dLPattern_LU, dLPattern_RU, dLPattern_LD, dLPattern_RD;

    bool hasSpecialStone;

    for(int row=0; row<Mheight; ++row){//先將所有石頭的ignorecheck設為false.
        for(int col=0; col<Mwidth; ++col){
            s = stoneMatrix[col][row];
            s->setignorecheck(false);
        }
    }

    //兩個石頭交換位置.
    stoneMatrix[sourceStone->getcol()][sourceStone->getrow()] = destStone;
    stoneMatrix[destStone->getcol()][destStone->getrow()] = sourceStone;
    tmprow = sourceStone->getrow();
    tmpcol = sourceStone->getcol();
    sourceStone->setrow(destStone->getrow());
    sourceStone->setcol(destStone->getcol());
    destStone->setrow(tmprow);
    destStone->setcol(tmpcol);

    tempscore = 0;//單次操作得分歸零.

    //如果sourceStone是星星, destStone不是星星, 可交換, 標示sourceStone需消除.
    if(sourceStone->getkind()==STAR && destStone->getkind()!=STAR){
        ++step;

        //和destStone的colr相同的石頭都要被消除, 將此color紀錄在sourceStone的color裡.
        sourceStone->setcolor(destStone->getcolor());
        markRemove(sourceStone);//只需標記sourceStone,  destStone也會被處理到.
        removeStone();
        return;

    //如果destStone是星星, sourceStone不是星星, 可交換, 標示destStone需消除.
    }else if(destStone->getkind()==STAR && sourceStone->getkind()!=STAR){
        ++step;

        //和sourceStone的colr相同的石頭都要被消除, 將此color紀錄在destStone的color裡.
        destStone->setcolor(sourceStone->getcolor());
        markRemove(destStone);//只需標記destStone,  sourceStone也會被處理到.
        removeStone();
        return;
    }

    getLeftRightList(sourceStone, 1);//獲得sourceStone左右邊的檢查結果List, 結果放在sourceList.
    getUpDownList(sourceStone, 1);//獲得sourceStone上下邊的檢查結果List, 結果放在sourceList.

    getLeftRightList(destStone, 2);//獲得destStone左右邊的檢查結果List, 結果放在destList.
    getUpDownList(destStone, 2);//獲得destStone上下邊的檢查結果List, 結果放在destList.

    //下面的條件如果成立, 表示兩個stone無法交換, 需要再換回來.
    if((sLeftList.size()+sRightList.size())<2 && (sUpList.size()+sDownList.size())<2 &&
       (dLeftList.size()+dRightList.size())<2 && (dUpList.size()+dDownList.size())<2){
        stoneMatrix[sourceStone->getcol()][sourceStone->getrow()] = destStone;
        stoneMatrix[destStone->getcol()][destStone->getrow()] = sourceStone;
        tmprow = sourceStone->getrow();
        tmpcol = sourceStone->getcol();
        sourceStone->setrow(destStone->getrow());
        sourceStone->setcol(destStone->getcol());
        destStone->setrow(tmprow);
        destStone->setcol(tmpcol);
    }else{//可以交換.
        ++step;//交換次數加1.

        sHPattern = sLeftList.size()+sRightList.size();//計算sourceStone左右邊List長度.
        sVPattern = sUpList.size()+sDownList.size();//計算sourceStone上下邊List長度.
        sLPattern_LU = sLeftList.size()+sUpList.size();//計算sourceStone左上邊List長度.
        sLPattern_RU = sRightList.size()+sUpList.size();//計算sourceStone右上邊List長度.
        sLPattern_LD = sLeftList.size()+sDownList.size();//計算sourceStone左下邊List長度.
        sLPattern_RD = sRightList.size()+sDownList.size();//計算sourceStone右下邊List長度.

        dHPattern = dLeftList.size()+dRightList.size();//計算destStone左右邊List長度.
        dVPattern = dUpList.size()+dDownList.size();//計算destStone上下邊List長度.
        dLPattern_LU = dLeftList.size()+dUpList.size();//計算destStone左上邊List長度.
        dLPattern_RU = dRightList.size()+dUpList.size();//計算destStone右上邊List長度.
        dLPattern_LD = dLeftList.size()+dDownList.size();//計算destStone左下邊List長度.
        dLPattern_RD = dRightList.size()+dDownList.size();//計算destStone右下邊List長度.


        //開始消除sourceStone構成的Pattern之所有石頭.=========================
        if(sHPattern==4){
            //水平5連, 如果全部都是普通石頭, sourceStone成為星星.

            hasSpecialStone = false;

            for(int i=0; i<sLeftList.size(); ++i){//標記sourceStone左邊List中的石頭需消除.
                s = sLeftList.at(i);

                //檢查是否有特殊石頭, 如果有, hasSpecialStone設為true.
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<sRightList.size(); ++i){//標記sourceStone右邊List中的石頭需消除.
                s = sRightList.at(i);

                //檢查是否有特殊石頭, 如果有, hasSpecialStone設為true.
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            //如果List中有特殊石頭, 或是sourceStone本身就是特殊石頭, 則標記sourceStone需消除.
            if(hasSpecialStone || sourceStone->getkind()!=NORMAL){
                markRemove(sourceStone);
            }else{//型態中, 全部都是普通石頭, 才能生成新的特殊石頭.
                //List中都是普通石頭, sourceStone成為星星.
                sourceStone->setkind(STAR);
                sourceStone->setcolor(-1);

                //ignorecheck設為true, 避免sourceStone在本循環中被消除.
                sourceStone->setignorecheck(true);
            }

        }else if(sVPattern==4){
            //垂直5連, 如果全部都是普通石頭, sourceStone成為星星.

            hasSpecialStone = false;

            for(int i=0; i<sUpList.size(); ++i){


                s = sUpList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<sDownList.size(); ++i){
                s = sDownList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || sourceStone->getkind()!=NORMAL){
                markRemove(sourceStone);
            }else{
                sourceStone->setkind(STAR);
                sourceStone->setcolor(-1);
                sourceStone->setignorecheck(true);
            }

        }else if(sLPattern_LU==4){
            //左上L型, 如果全部都是普通石頭, sourceStone成為炸彈.

            hasSpecialStone = false;

            for(int i=0; i<sLeftList.size(); ++i){
                s = sLeftList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<sUpList.size(); ++i){
                s = sUpList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || sourceStone->getkind()!=NORMAL){
                markRemove(sourceStone);
            }else{
                sourceStone->setkind(BOMB);
                sourceStone->setignorecheck(true);
            }

        }else if(sLPattern_RU==4){
            //右上L型, 如果全部都是普通石頭, sourceStone成為炸彈.

            hasSpecialStone = false;

            for(int i=0; i<sRightList.size(); ++i){
                s = sRightList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<sUpList.size(); ++i){
                s = sUpList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || sourceStone->getkind()!=NORMAL){
                markRemove(sourceStone);
            }else{
                sourceStone->setkind(BOMB);
                sourceStone->setignorecheck(true);
            }

        }else if(sLPattern_LD==4){
            //左下L型, 如果全部都是普通石頭, sourceStone成為炸彈.

            hasSpecialStone = false;

            for(int i=0; i<sLeftList.size(); ++i){
                s = sLeftList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<sDownList.size(); ++i){
                s = sDownList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || sourceStone->getkind()!=NORMAL){
                markRemove(sourceStone);
            }else{
                sourceStone->setkind(BOMB);
                sourceStone->setignorecheck(true);
            }

        }else if(sLPattern_RD==4){
            //右下L型, 如果全部都是普通石頭, sourceStone成為炸彈.

            hasSpecialStone = false;

            for(int i=0; i<sRightList.size(); ++i){
                s = sRightList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<sDownList.size(); ++i){
                s = sDownList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || sourceStone->getkind()!=NORMAL){
                markRemove(sourceStone);
            }else{
                sourceStone->setkind(BOMB);
                sourceStone->setignorecheck(true);
            }

        }else if(sHPattern==3){
            //水平4連, 如果全部都是普通石頭, sourceStone成為水平消除.

            hasSpecialStone = false;

            for(int i=0; i<sLeftList.size(); ++i){
                s = sLeftList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<sRightList.size(); ++i){
                s = sRightList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || sourceStone->getkind()!=NORMAL){
                markRemove(sourceStone);
            }else{
                sourceStone->setkind(HR);
                sourceStone->setignorecheck(true);
            }

        }else if(sVPattern==3){
            //垂直4連, 如果全部都是普通石頭, sourceStone成為垂直消除.

            hasSpecialStone = false;

            for(int i=0; i<sUpList.size(); ++i){
                s = sUpList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<sDownList.size(); ++i){
                s = sDownList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || sourceStone->getkind()!=NORMAL){
                markRemove(sourceStone);
            }else{
                sourceStone->setkind(VR);
                sourceStone->setignorecheck(true);
            }

        }else if(sHPattern==2){
            //水平3連.

            markRemove(sourceStone);

            for(int i=0; i<sLeftList.size(); ++i){
                s = sLeftList.at(i);
                markRemove(s);
            }

            for(int i=0; i<sRightList.size(); ++i){
                s = sRightList.at(i);
                markRemove(s);
            }

        }else if(sVPattern==2){
            //垂直3連.

            markRemove(sourceStone);

            for(int i=0; i<sUpList.size(); ++i){
                s = sUpList.at(i);
                markRemove(s);
            }

            for(int i=0; i<sDownList.size(); ++i){
                s = sDownList.at(i);
                markRemove(s);
            }
        }



        //開始消除目的stone構成的Pattern之所有stone.=======================

        if(dHPattern==4){
            //水平5連, 如果全部都是普通石頭, destStone成為星星.

            hasSpecialStone = false;

            for(int i=0; i<dLeftList.size(); ++i){
                s = dLeftList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<dRightList.size(); ++i){
                s = dRightList.at(i);
                if(s->getkind()!=NORMAL){
                    hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || destStone->getkind()!=NORMAL){
                markRemove(destStone);
            }else{
                destStone->setkind(STAR);
                destStone->setcolor(-1);
                destStone->setignorecheck(true);
            }

        }else if(dVPattern==4){
            //垂直5連, 如果全部都是普通石頭, destStone成為星星.

            hasSpecialStone = false;

            for(int i=0; i<dUpList.size(); ++i){
                s = dUpList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<dDownList.size(); ++i){
                s = dDownList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || destStone->getkind()!=NORMAL){
                markRemove(destStone);
            }else{
                destStone->setkind(STAR);
                destStone->setcolor(-1);
                destStone->setignorecheck(true);
            }

        }else if(dLPattern_LU==4){
            //左上L型, 如果全部都是普通石頭, destStone成為炸彈.

            hasSpecialStone = false;

            for(int i=0; i<dLeftList.size(); ++i){
                s = dLeftList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<dUpList.size(); ++i){
                s = dUpList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || destStone->getkind()!=NORMAL){
                markRemove(destStone);
            }else{
                destStone->setkind(BOMB);
                destStone->setignorecheck(true);
            }

        }else if(dLPattern_RU==4){
            //右上L型, 如果全部都是普通石頭, destStone成為炸彈.

            hasSpecialStone = false;

            for(int i=0; i<dRightList.size(); ++i){
                s = dRightList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<dUpList.size(); ++i){
                s = dUpList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || destStone->getkind()!=NORMAL){
                markRemove(destStone);
            }else{
                destStone->setkind(BOMB);
                destStone->setignorecheck(true);
            }

        }else if(dLPattern_LD==4){
            //左下L型, 如果全部都是普通石頭, destStone成為炸彈.

            hasSpecialStone = false;

            for(int i=0; i<dLeftList.size(); ++i){
                s = dLeftList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<dDownList.size(); ++i){
                s = dDownList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || destStone->getkind()!=NORMAL){
                markRemove(destStone);
            }else{
                destStone->setkind(BOMB);
                destStone->setignorecheck(true);
            }

        }else if(dLPattern_RD==4){
            //右下L型, 如果全部都是普通石頭, destStone成為炸彈.

            hasSpecialStone = false;

            for(int i=0; i<dRightList.size(); ++i){
                s = dRightList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<dDownList.size(); ++i){
                s = dDownList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || destStone->getkind()!=NORMAL){
                markRemove(destStone);
            }else{
                destStone->setkind(BOMB);
                destStone->setignorecheck(true);
            }

        }else if(dHPattern==3){
            //水平4連, 如果全部都是普通石頭, destStone 成為水平消除.

            hasSpecialStone = false;

            for(int i=0; i<dLeftList.size(); ++i){
                s = dLeftList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<dRightList.size(); ++i){
                s = dRightList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || destStone->getkind()!=NORMAL){
                markRemove(destStone);
            }else{
                destStone->setkind(HR);
                destStone->setignorecheck(true);
            }

        }else if(dVPattern==3){
            //垂直4連, 如果全部都是普通石頭, destStone 成為垂直消除.

            hasSpecialStone = false;

            for(int i=0; i<dUpList.size(); ++i){
                s = dUpList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            for(int i=0; i<dDownList.size(); ++i){
                s = dDownList.at(i);
                if(s->getkind()!=NORMAL){
                     hasSpecialStone = true;
                }
                markRemove(s);
            }

            if(hasSpecialStone || destStone->getkind()!=NORMAL){
                markRemove(destStone);
            }else{
                destStone->setkind(VR);
                destStone->setignorecheck(true);
            }

        }else if(dHPattern==2){
            //水平3連.

            markRemove(destStone);

            for(int i=0; i<dLeftList.size(); ++i){
                s = dLeftList.at(i);
                markRemove(s);
            }

            for(int i=0; i<dRightList.size(); ++i){
                s = dRightList.at(i);
                markRemove(s);
            }

        }else if(dVPattern==2){
            //垂直3連.

            markRemove(destStone);

            for(int i=0; i<dUpList.size(); ++i){
                s = dUpList.at(i);
            markRemove(s);
            }

            for(int i=0; i<dDownList.size(); ++i){
                s = dDownList.at(i);
                markRemove(s);
            }
        }


        removeStone();//消除所有needremove==true的石頭.
        sourceStone->setignorecheck(false);//sourceStone的ignorecheck設為false, 下次循環可被處裡.
        destStone->setignorecheck(false);//destStone的ignorecheck設為false, 下次循環可被處裡.
    }
}



void MainWindow::afterFillCheck()//補充石頭後的檢查.
{
    Stone *checkedStone, *s;
    int HPattern, VPattern, LPattern_LU, LPattern_RU, LPattern_LD, LPattern_RD;
    bool hasSpecialStone;

    ignoreCheckList.clear();//清空忽略檢查的List.

    needFillMatrix = false;//需要補充石頭的flag設為false.

    for(int row=0; row<Mheight; ++row){//先檢查全部的Matrix是否有5連, 有的話標記消除.
        for(int col=0; col<Mwidth; ++col){
            checkedStone = stoneMatrix[col][row];

            //如果已在之前的檢查中被標記為'需消除'或'忽略檢查', 則跳過.
            if(checkedStone->getneedremove() || checkedStone->getignorecheck())continue;

            getLeftRightList(checkedStone, 1);//獲得checkedStone左右邊的檢查結果List, 結果放在sourceList.
            getUpDownList(checkedStone, 1);//獲得checkedStone上下邊的檢查結果List, 結果放在sourceList.

            HPattern = sLeftList.size()+sRightList.size();
            VPattern = sUpList.size()+sDownList.size();

            if(HPattern==4){//水平5連,
                hasSpecialStone = false;

                for(int i=0; i<sLeftList.size(); ++i){//標記checkedStone左邊List中的石頭需消除.
                    s = sLeftList.at(i);

                    //檢查是否有特殊石頭, 如果有, hasSpecialStone設為true.
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                for(int i=0; i<sRightList.size(); ++i){//標記checkedStone右邊List中的石頭需消除.
                    s = sRightList.at(i);

                    //檢查是否有特殊石頭, 如果有, hasSpecialStone設為true.
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                //如果List中有特殊石頭, 或是checkedStone本身就是特殊石頭, 則標記checkedStone需消除.
                if(hasSpecialStone || checkedStone->getkind()!=NORMAL){
                    markRemove(checkedStone);
                }else{//型態中, 全部都是普通石頭, 才能生成新的特殊石頭.
                    //List中都是普通石頭, checkedStone成為星星.
                    checkedStone->setkind(STAR);
                    checkedStone->setcolor(-1);

                    //ignorecheck設為true, 避免checkedStone在本循環中被消除.
                    checkedStone->setignorecheck(true);
                    ignoreCheckList.append(checkedStone);//checkedStone加入ignoreCheckList.
                }

                needFillMatrix = true;//需要補充石頭的flag設為true.

            }else if(VPattern==4){//垂直5連.
                hasSpecialStone = false;

                for(int i=0; i<sUpList.size(); ++i){
                    s = sUpList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                for(int i=0; i<sDownList.size(); ++i){
                    s = sDownList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                if(hasSpecialStone || checkedStone->getkind()!=NORMAL){
                    markRemove(checkedStone);
                }else{
                    checkedStone->setkind(STAR);
                    checkedStone->setcolor(STAR);
                    checkedStone->setignorecheck(true);
                    ignoreCheckList.append(checkedStone);
                }

                needFillMatrix = true;
            }
        }
    }


    for(int row=0; row<Mheight; ++row){//再檢查全部的Matrix是否有L Pattern, 有的話標記消除.
        for(int col=0; col<Mwidth; ++col){
            checkedStone = stoneMatrix[col][row];

            //如果已在之前的檢查中被標記為'需消除'或'忽略檢查', 則跳過.
            if(checkedStone->getneedremove() || checkedStone->getignorecheck())continue;

            getLeftRightList(checkedStone, 1);
            getUpDownList(checkedStone, 1);

            LPattern_LU = sLeftList.size()+sUpList.size();
            LPattern_RU = sRightList.size()+sUpList.size();
            LPattern_LD = sLeftList.size()+sDownList.size();
            LPattern_RD = sRightList.size()+sDownList.size();

            if(LPattern_LU==4){//左上L型, 如果全部都是普通石頭, sourceStone成為炸彈.
                hasSpecialStone = false;

                for(int i=0; i<sLeftList.size(); ++i){
                    s = sLeftList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                for(int i=0; i<sUpList.size(); ++i){
                    s = sUpList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                if(hasSpecialStone || checkedStone->getkind()!=NORMAL){
                    markRemove(checkedStone);
                }else{
                    checkedStone->setkind(BOMB);
                    checkedStone->setignorecheck(true);
                    ignoreCheckList.append(checkedStone);
                }

                needFillMatrix = true;
            }else if(LPattern_RU==4){//右上L型, 如果全部都是普通石頭, sourceStone成為炸彈.
                hasSpecialStone = false;

                for(int i=0; i<sRightList.size(); ++i){
                    s = sRightList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                for(int i=0; i<sUpList.size(); ++i){
                    s = sUpList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                if(hasSpecialStone || checkedStone->getkind()!=NORMAL){
                    markRemove(checkedStone);
                }else{
                    checkedStone->setkind(BOMB);
                    checkedStone->setignorecheck(true);
                    ignoreCheckList.append(checkedStone);
                }

                needFillMatrix = true;
            }else if(LPattern_LD==4){//左下L型, 如果全部都是普通石頭, sourceStone成為炸彈.
                hasSpecialStone = false;

                for(int i=0; i<sLeftList.size(); ++i){
                    s = sLeftList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                for(int i=0; i<sDownList.size(); ++i){
                    s = sDownList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                if(hasSpecialStone || checkedStone->getkind()!=NORMAL){
                    markRemove(checkedStone);
                }else{
                    checkedStone->setkind(BOMB);
                    checkedStone->setignorecheck(true);
                    ignoreCheckList.append(checkedStone);
                }

                needFillMatrix = true;
            }else if(LPattern_RD==4){//右下L型, 如果全部都是普通石頭, sourceStone成為炸彈.
                hasSpecialStone = false;

                for(int i=0; i<sRightList.size(); ++i){
                    s = sRightList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                for(int i=0; i<sDownList.size(); ++i){
                    s = sDownList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                if(hasSpecialStone || checkedStone->getkind()!=NORMAL){
                    markRemove(checkedStone);
                }else{
                    checkedStone->setkind(BOMB);
                    checkedStone->setignorecheck(true);
                    ignoreCheckList.append(checkedStone);
                }



                needFillMatrix = true;
            }
        }
    }


    for(int row=0; row<Mheight; ++row){//再檢查全部的Matrix是否有4連, 有的話標記消除.
        for(int col=0; col<Mwidth; ++col){
            checkedStone = stoneMatrix[col][row];

            //如果已在之前的檢查中被標記為'需消除'或'忽略檢查', 則跳過.
            if(checkedStone->getneedremove() || checkedStone->getignorecheck())continue;

            getLeftRightList(checkedStone, 1);
            getUpDownList(checkedStone, 1);

            HPattern = sLeftList.size()+sRightList.size();
            VPattern = sUpList.size()+sDownList.size();

            if(HPattern==3){//水平4連.
                hasSpecialStone = false;

                for(int i=0; i<sLeftList.size(); ++i){
                    s = sLeftList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                for(int i=0; i<sRightList.size(); ++i){
                    s = sRightList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                if(hasSpecialStone || checkedStone->getkind()!=NORMAL){
                    markRemove(checkedStone);
                }else{
                    checkedStone->setkind(HR);
                    checkedStone->setignorecheck(true);
                    ignoreCheckList.append(checkedStone);
                }

                needFillMatrix = true;
            }else if(VPattern==3){//垂直4連.
                hasSpecialStone = false;

                for(int i=0; i<sUpList.size(); ++i){
                    s = sUpList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                for(int i=0; i<sDownList.size(); ++i){
                    s = sDownList.at(i);
                    if(s->getkind()!=NORMAL){
                        hasSpecialStone = true;
                    }
                    markRemove(s);
                }

                if(hasSpecialStone || checkedStone->getkind()!=NORMAL){
                    markRemove(checkedStone);
                }else{
                    checkedStone->setkind(VR);
                    checkedStone->setignorecheck(true);
                    ignoreCheckList.append(checkedStone);
                }

                needFillMatrix = true;
            }
        }
    }

    for(int row=0; row<Mheight; ++row){//再檢查全部的Matrix是否有3連, 有的話標記消除.
        for(int col=0; col<Mwidth; ++col){
            checkedStone = stoneMatrix[col][row];

            //如果已在之前的檢查中被標記為'需消除'或'忽略檢查', 則跳過.
            if(checkedStone->getneedremove() || checkedStone->getignorecheck())continue;

            getLeftRightList(checkedStone, 1);
            getUpDownList(checkedStone, 1);

            HPattern = sLeftList.size()+sRightList.size();
            VPattern = sUpList.size()+sDownList.size();

            if(HPattern==2){//水平3連.
                markRemove(checkedStone);

                for(int i=0; i<sLeftList.size(); ++i){
                    s = sLeftList.at(i);
                    markRemove(s);
                }

                for(int i=0; i<sRightList.size(); ++i){
                    s = sRightList.at(i);
                    markRemove(s);
                }

                needFillMatrix = true;
            }else if(VPattern==2){//垂直3連.
                markRemove(checkedStone);

                for(int i=0; i<sUpList.size(); ++i){
                    s = sUpList.at(i);
                    markRemove(s);
                }

                for(int i=0; i<sDownList.size(); ++i){
                    s = sDownList.at(i);
                    markRemove(s);
                }

                needFillMatrix = true;
            }
        }
    }

    if(needFillMatrix){//needFillMatrix==true, 表示有石頭被標記需消除, 所以需要消除石頭.
        removeStone();
    }

    //將ignoreCheckList中所有石頭的ignorecheck設為false, 下次循環可被處裡.
    for(int i=0; i<ignoreCheckList.size(); ++i){
        ignoreCheckList.at(i)->setignorecheck(false);
    }

}

