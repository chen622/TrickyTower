# Tricky Tower

## 1.  游戏内容

《Tricky Towers》是Weird Beard制作并发行的一款全新的益智游戏，游戏采用经典的“俄罗斯方块”玩法，以创意的方式带玩家重温经典游戏，而本次大实验作品创意便来自这款游戏。

![image-20201203160804219](https://img.ccm.ink/image-20201203160804219.png)

本游戏为一款网页游戏，通过调用相关游戏引擎实现了重力，碰撞等效果，同时本游戏为一款多人游戏，每局最多可容纳3名玩家。游戏胜利的条件为在掉落砖块不超过规定限制的情况下，将自己的砖块塔搭建到一定高度。



## 2.  具体实现

本游戏为Web应用，采用前后端分离的设计模式，前端为符合MVVM(Model-View-ViewMode)设计模式的Vue.js应用，后端采用C/C++编写的非阻塞服务器。前后端之间通过基于WebSocket协议的消息通信。

### 2.1. 前端

前端总体采用了近期比较热门的Vue渐进式框架，同时为给游戏中的物体带来真实世界的感觉运用了Matter.js 物理引擎，这种真实世界的感觉也是本游戏相较普通俄罗斯方块最大的区别。

本游戏的首页为游戏的相关内容的介绍。

![image-20201203160906679](https://img.ccm.ink/image-20201203160906679.png)

点击TRY后进入游戏的房间系统。玩家在其中可以自由创建或加入房间，使用该系统玩家既可以与朋友们一同畅快玩耍，也可以在网上结交新的朋友。

![image-20201203160914152](https://img.ccm.ink/image-20201203160914152.png)

当一个房间内有足够的人数后，它就会变为红色，并且等候房主的开始。当房主开始后，游戏便进入对战阶段。

![image-20201203160920220](https://img.ccm.ink/image-20201203160920220.png)



### 2.2. 后端

后端使用C/C++编写，并使用epoll相关函数实现了一个非阻塞式的WebSocket服务器。



## 3.  亮点

### 3.1. 异步I/O

使用C语言提供的epoll相关函数便可实现一个非阻塞式的服务器。

```C
if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event) == -1){    //注册新的listen_fd到epoll_fd中  
    perror("epll_ctl:servaddr register failed\n");  
    exit(EXIT_FAILURE);  
}  
printf("listen:%d\n", listen_fd);  
int nfds;  
while (true) {  
    nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 10);  
    if (nfds != 0)  
        printf("\nnfds:%d\n", nfds);  
    if (nfds == -1) {  
        perror("start epoll_wait failed\n");  
        continue;  
    }  
    for (int i = 0; i < nfds; ++i) {  
        if (events[i].data.fd == listen_fd) {  
            //建立连接  
        } else if (events[i].events & EPOLLIN) {  
            //读取请求  
        } else {  
            //写入请求  
        }  

    }  
} 

```


首先异步I/O相较于普通的多进程解决方案，他能打开的socket描述符的数量可能是几百倍的。

同时epoll这种函数相较其他事件处理机制具有更高的并发行，因为select和poll事件处理机制都是采用轮询IO的方式，而采用轮询IO是最耗时的操作之一。

### 3.2. WebSocket

为照顾游戏实时性高的特点，采用了真正的全双工协议WebSocket。

### 3.3. Matter.js

采用了一个2D物理引擎Matter.js进行世界的渲染。

### 3.4. 房间机制

后端使用STL提供的map数据结构来保存房间。

### 3.5. “心跳”同步

采用类似“心跳”机制的方式进行多个玩家间的信息传递，每一个玩家都会以固定的间隔时间将自己的游戏数据传递给同一句的其他玩家。