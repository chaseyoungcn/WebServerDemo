# WebServerDemo
一个win下的web服务器 

只是为了熟悉流程, https://www.bilibili.com/video/BV16t411N7aQ?p=7&t=1720
按照本地址中,web服务器视频实现
###存在的BUG:
1. 接受baidu服务器的信息, 无法识别. 
  :提示无法解析的外部符号
   课程中使用 ```#pragma	comment(lib, "ws2_32.lib")```, 在vs2019中已失效
   
2. 在本机充当服务器的时候, 发送静态网页源码 貌似没有渲染成功,只输出字符串 ,视频中既如此

### 用到的socketAPI:



### 代码详解:
1. 封装socket操作



