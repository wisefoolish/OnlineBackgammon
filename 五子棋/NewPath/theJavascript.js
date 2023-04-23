var isControlBlack=false;
var ChessArray=[];      // 棋盘数组
var isBlack=true;       // 是否是执黑的走
var isBlackWin=false;   // 黑方获胜否
var isWhiteWin=false;   // 白方获胜否
var isStart=false;
var ThreeHandExchange=0;
var isThreeHandExchange=false;

// 初始化棋盘
function InitBoard()
{
    isBlackWin=false;
    isWhiteWin=false;
    var listening=document.getElementById("listeningId");
    var winnerText=document.getElementsByClassName("winner_text");
    winnerText.innerText="";
    for(var i=0;i<15;i++)
    {
        ChessArray[i]=[];
        for(var j=0;j<15;j++)
        {
            ChessArray[i][j]=0;
            var childNode=document.createElement("div");
            if(i.valueOf()==0)
            {
                if(j.valueOf()==0)
                    childNode.className="gridBox_lt";
                else if(j.valueOf()==14)
                    childNode.className="gridBox_rt";
                else childNode.className="gridBox_nt";
            }
            else if(i.valueOf()==14)
            {
                if(j.valueOf()==0)
                    childNode.className="gridBox_lb";
                else if(j.valueOf()==14)
                    childNode.className="gridBox_rb";
                else childNode.className="gridBox_nd";
            }
            else if(j.valueOf()==0)
                childNode.className="gridBox_nl";
            else if(j.valueOf()==14)
                childNode.className="gridBox_nr";
            else childNode.className="gridBox";
            // 监听节点按下时的动作
            childNode.onclick=function(){
                if (isControlBlack ^ isBlack)return;
                if(isThreeHandExchange)return;
                SetChess(this);
            }
            childNode.id=i+" "+j;
            listening.appendChild(childNode);
        }
    }
}

// 判断是否胜利
function isWin(row,col,number)
{
    row=parseInt(row);
    col=parseInt(col);
    number=parseInt(number);
    // 先横后竖后斜
    var oriNum=0;
    for(var i=1;i<5;i++)
    {
        if(col+i<15&&ChessArray[row][col+i]==number)oriNum+=1;
        else break;
    }
    for(var i=1;i<5;i++)
    {
        if(col-i>=0&&ChessArray[row][col-i]==number)oriNum+=1;
        else break;
    }
    if(oriNum>=4)return true;
    oriNum=0;
    for(var i=1;i<5;i++)
    {
        if(row+i<15&&ChessArray[row+i][col]==number)oriNum+=1;
        else break;
    }
    for(var i=1;i<5;i++)
    {
        if(row-i>=0&&ChessArray[row-i][col]==number)oriNum+=1;
        else break;
    }
    if(oriNum>=4)return true;
    oriNum=0;
    for(var i=1;i<5;i++)
    {
        if(row+i<15&&col+i<15&&ChessArray[row+i][col+i]==number)oriNum+=1;
        else break;
    }
    for(var i=1;i<5;i++)
    {
        if(row-i>=0&&col-i>=0&&ChessArray[row-i][col-i]==number)oriNum+=1;
        else break;
    }
    if(oriNum>=4)return true;
    oriNum=0;
    for(var i=1;i<5;i++)
    {
        if(row+i<15&&col-i>=0&&ChessArray[row+i][col-i]==number)oriNum+=1;
        else break;
    }
    for(var i=1;i<5;i++)
    {
        if(row-i>=0&&col+i<15&&ChessArray[row-i][col+i]==number)oriNum+=1;
        else break;
    }
    if(oriNum>=4)return true;
    return false;
}

// 重置
function Reset()
{
    isBlack=true;
    isBlackWin=false;
    isWhiteWin=false;
    var winnerText=document.getElementsByClassName("winner_text")[0];
    winnerText.innerText="";
    winnerText.style="";
    var listener=document.getElementById("listeningId");
    for(var i=0;i<listener.childElementCount;i++)
    {
        childNode=listener.childNodes[i];
        row_col=childNode.id.split(" ");
        ChessArray[row_col[0]][row_col[1]]=0;
        if(row_col[0]==0)
        {
            if(row_col[1]==0)
                childNode.className="gridBox_lt";                
            else if(row_col[1]==14)
                childNode.className="gridBox_rt";
            else childNode.className="gridBox_nt";
        }
        else if(row_col[0]==14)
        {
            if(row_col[1]==0)
                childNode.className="gridBox_lb";
            else if(row_col[1]==14)
                childNode.className="gridBox_rb";
            else childNode.className="gridBox_nd";
        }
        else if(row_col[1]==0)
            childNode.className="gridBox_nl";
        else if(row_col[1]==14)
            childNode.className="gridBox_nr";
        else childNode.className="gridBox";
        childNode.style="";
    }
}

// 判断是否是禁手
function isForbiddenPoint(row,col,number)
{
    row=parseInt(row);
    col=parseInt(col);
    number=parseInt(number);
    var isAlive=1;
    var AliveNums=0;
    // 先横后竖后斜
    var oriNum=0;
    for(var i=1;i<5;i++)
    {
        if(col+i<15&&ChessArray[row][col+i]==number)oriNum++;
        else
        {
            if(col+i<15&&ChessArray[row][col+i]!=0) isAlive--;
            break;
        }
    }
    for(var i=1;i<5;i++)
    {
        if(col-i>=0&&ChessArray[row][col-i]==number)oriNum+=1;
        else
        {
            if(col-i>=0&&ChessArray[row][col-i]!=0) isAlive--;
            break;
        }
    }
    if(oriNum==2&&isAlive==1)AliveNums++;
    else if(oriNum==3&&isAlive>=0)AliveNums++;
    else if(oriNum>=5)return true;
    oriNum=0;
    isAlive=1;
    for(var i=1;i<5;i++)
    {
        if(row+i<15&&ChessArray[row+i][col]==number)oriNum+=1;
        else
        {
            if(row+i<15&&ChessArray[row+i][col]!=0) isAlive--;
            break;
        }
    }
    for(var i=1;i<5;i++)
    {
        if(row-i>=0&&ChessArray[row-i][col]==number)oriNum+=1;
        else
        {
            if(row-i>=0&&ChessArray[row-i][col]!=0) isAlive--;
            break;
        }
    }
    if(oriNum==2&&isAlive==1)AliveNums++;
    else if(oriNum==3&&isAlive>=0)AliveNums++;
    else if(oriNum>=5)return true;
    oriNum=0;
    isAlive=1;
    for(var i=1;i<5;i++)
    {
        if(row+i<15&&col+i<15&&ChessArray[row+i][col+i]==number)oriNum+=1;
        else
        {
            if(row+i<15&&col+i<15&&ChessArray[row+i][col+i]!=0) isAlive--;
            break;
        }
    }
    for(var i=1;i<5;i++)
    {
        if(row-i>=0&&col-i>=0&&ChessArray[row-i][col-i]==number)oriNum+=1;
        else
        {
            if(row-i>=0&&col-i>=0&&ChessArray[row-i][col-i]!=0) isAlive--;
            break;
        }
    }
    if(oriNum==2&&isAlive==1)AliveNums++;
    else if(oriNum==3&&isAlive>=0)AliveNums++;
    else if(oriNum>=5)return true;
    oriNum=0;
    isAlive=1;
    for(var i=1;i<5;i++)
    {
        if(row+i<15&&col-i>=0&&ChessArray[row+i][col-i]==number)oriNum+=1;
        else
        {
            if(row+i<15&&col-i>=0&&ChessArray[row+i][col-i]!=0) isAlive--;
            break;
        }
    }
    for(var i=1;i<5;i++)
    {
        if(row-i>=0&&col+i<15&&ChessArray[row-i][col+i]==number)oriNum+=1;
        else
        {
            if(row-i>=0&&col+i<15&&ChessArray[row-i][col+i]!=0) isAlive--;
            break;
        }
    }
    if(oriNum==2&&isAlive==1)AliveNums++;
    else if(oriNum==3&&isAlive>=0)AliveNums++;
    else if(oriNum>=5)return true;
    if(AliveNums>=2)return true;
    return false;
}

function SetChess(event)
{
    row_col=event.id.split(" ");
    if (!isStart)
    {
        alert("游戏未开始");
        return;
    }
    if (isBlackWin || isWhiteWin || ChessArray[row_col[0]][row_col[1]] != 0)return;
    if (isBlack && isForbiddenPoint(row_col[0], row_col[1], 1))
    {
        alert("这是禁手");
        return;
    }
    event.className = "DrawChess";
    if (ChessArray[row_col[0]][row_col[1]] == 0 && isBlack)
    {
        ChessArray[row_col[0]][row_col[1]] = 1;
        event.style = "background-color:#000000;";
    }
    else if (ChessArray[row_col[0]][row_col[1]] == 0 && !isBlack)
    {
        ChessArray[row_col[0]][row_col[1]] = 2;
        event.style = "background-color:#ffffff;";
    }
    PutChess(row_col[1], row_col[0]);
    if (isWin(row_col[0], row_col[1], ChessArray[row_col[0]][row_col[1]]))
    {
        isBlackWin = isBlack;
        isWhiteWin = !isBlack;
        var winnerText = document.getElementsByClassName("winner_text")[0];
        if (isBlack)
        {
            winnerText.innerText = "黑胜";
            winnerText.style = "color:#000000;background-color:#888888";
        }
        else
        {
            winnerText.innerText = "白胜";
            winnerText.style = "color:#ffffff;background-color:#888888";
        }
    }
    isBlack=!isBlack;
    ThreeHandExchange++;
}

const socket = new WebSocket('ws://localhost:8888');
socket.addEventListener('message', function (event)
{
    if(event.data=="Successful Enter")
    {
        console.log("recv successful");
        isStart=true;
        var winnerText=document.getElementsByClassName("winner_text")[0];
        if(isControlBlack)
        winnerText.innerText="执黑";
        else
        winnerText.innerText="执白";
    }
    else
    {
        var recvMsg=event.data.split(":");
        if(recvMsg[0]=="PutChess")
        {
            if(recvMsg[1]=="WHITE"&&isControlBlack)
            {
                SetChess(document.getElementById(recvMsg[3]+" "+recvMsg[2]));
            }
            else if(recvMsg[1]=="BLACK"&&!isControlBlack)
            {
                SetChess(document.getElementById(recvMsg[3]+" "+recvMsg[2]));
            }
            if(ThreeHandExchange==3)
            {
                isThreeHandExchange=true;
                if(!isControlBlack)
                document.getElementsByClassName("AskThreeHand")[0].style="display:block;";
            }
        }
        else if(recvMsg[0]=="ThreeHand")
        {
            console.log("接收到消息:"+recvMsg[0]+":"+recvMsg[1]);
            if(recvMsg[1]=="true")
            {
                isControlBlack=!isControlBlack;
                isThreeHandExchange=false;
                var winnerText=document.getElementsByClassName("winner_text")[0];
                if(isControlBlack)
                winnerText.innerText="执黑";
                else
                winnerText.innerText="执白";
            }
            else if(recvMsg[1]=="false")
            {
                isThreeHandExchange=false;
            }
            document.getElementsByClassName("AskThreeHand")[0].style="display:none;";
        }
    }
});
socket.addEventListener('open',function(event)
{
    socket.send("Game:"+document.cookie);
});
function SendMsg(msg)
{
    socket.send(msg);
}

function PutChess(x,y)
{
    var camp="";
    if(isControlBlack)camp="BLACK";
    else camp="WHITE";
    SendMsg("PutChess:"+camp+":"+document.cookie+":"+x+":"+y);
}