// Create WebSocket connection.
const socket = new WebSocket('ws://localhost:8888');
var state=0;
// Listen for messages
socket.addEventListener('message', function (event)
{
    if(event.data=="Successful Exit Match")
    {
        var inMac=document.getElementsByClassName("InMatching")[0];
        console.log("成功退出匹配");
        inMac.innerText="";
    }
    else
    {
        var recvMsg=event.data.split(":");
        if(recvMsg[0]=="Successful Match")
        {
            document.cookie=recvMsg[1];
            if(recvMsg[2]=="WHITE")
            window.location.href="./NewPath/whitePlayer.html";
            else if(recvMsg[2]=="BLACK")
            window.location.href="./NewPath/blackPlayer.html";
        }
    }
});

socket.addEventListener('open',function(event)
{
    socket.send("Enter Connect");
});

function sendMessage(msgStr)
{
    socket.send(msgStr);
}
function matchingOperate()
{
    sendMessage("Matching");
    var inMac=document.getElementsByClassName("InMatching")[0];
    inMac.innerText="匹配中";
}
function ExitMatchingOperate()
{
    sendMessage("Exit Matching");
}
