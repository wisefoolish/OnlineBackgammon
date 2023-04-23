# 联机五子棋
"sha1.h" "sha1.cpp" "base64.h" "base64.cpp" 是别人的代码，实现了网络加密解密，我实现了main.cpp里的内容，实现了解析请求头和服务器端状态机。
五子棋文件夹里是服务器端可执行文件"server.exe"和html代码，StartUI.html是开始界面，需要在vscode里用插件liveserver打开，不然网页跳转时cookie会被清空，而这个项目是在cookie里保存房间号的。在一台电脑上可以打开两个StartUI.html界面测试，当两个人同时匹配就能匹配上，进入五子棋游戏界面，五子棋有双三，长连和三手交换的规则。注意在同一个浏览器打开，一个页面的cookie被更新另一个页面的cookie也会被更新，所以不能打开四个StartUI.html进行匹配，会四个人进入同一个房间而出错。
