# Libevent学习指导

## SOCKET
- ### 在通信有关编程领域，socket是人们群众喜闻乐见、耳熟能详的的接口函数，使用socket，简单敲敲几下键盘，Ctrl+C、Ctrl+V之后，一个简单的server/client模型的小程序就出现了。
- ### Client侧主要任务
  ```C
  client:

      //创建socket)
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
      //填写server地址(IP和PORT), 向server发起连接)
      sock_addr.sin_family = AF_INET;
      sock_addr.sin_port = htons(10086);
      sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
      connect(sockfd, (struct sockaddr *) &sock_addr, sizeof(sock_addr));
    
      //如果连接成功, 想发就write, 想收就read)
      write(sockfd, buf, size_of_buf);
      read(sockfd, buf, size_of_buf);
  ```

- ### Client侧还是很简单的，server侧会复杂些，毕竟一个server需要服务于多个client
  ```C
  server:
  
      // 创建socket
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      
      // 将socket绑定到需要监听的地址(IP和PORT)
      sock_addr.sin_family = AF_INET;
      sock_addr.sin_port = htons(10086);
      sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
      
      bind(sockfd, (struct sockaddr *) &sock_addr, sizeof(sock_addr));
      
      // 设置监听连接个数
      listen(sockfd, num_of_sockets);
      
      //  如果有client进行连接, accept函数返回新的socket, fd, 使用这个fd就可以与client通信
      a_new_client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
      
      // 想发就write, 想收就read
      write(a_new_client_fd, buf, size_of_buf);
      read(a_new_client_fd, buf, size_of_buf);
  ```


- ### 为了避免后续演示时候出现大面积重复代码，源码文件socket_helper.c对上述基本操作做了简单封装。
  ```C
  file: socket_helper.c
  
  #define SERVICE_SERVER 0
  #define SERVICE_CLIENT 1
  
  static int new_socket_fd(int service_type, int sock_type, const char *ip, int port)
  {
      int sockfd;
      struct sockaddr_in sock_addr;
  
      bzero(&sock_addr, sizeof(sock_addr));
  
      sock_addr.sin_family = AF_INET;
      sock_addr.sin_port = htons(port);
      sock_addr.sin_addr.s_addr = inet_addr(ip);
  
      sockfd = socket(AF_INET, sock_type, 0);
  
      // go die, gua le, gun du zi, wtf...
      assert(sockfd > 0);
  
      if (service_type == SERVICE_SERVER)
          bind(sockfd, (struct sockaddr *) &sock_addr, sizeof(sock_addr));
      else
          connect(sockfd, (struct sockaddr *) &sock_addr, sizeof(sock_addr));
  
      return sockfd;
  }
  
  int new_tcp_server_socket(const char *ip, int port)
  {
      return new_socket_fd(SERVICE_SERVER, SOCK_STREAM, ip, port);
  }
  
  int new_tcp_client_socket(const char *ip, int port)
  {
      return new_socket_fd(SERVICE_CLIENT, SOCK_STREAM, ip, port);
  }
  
  int new_udp_server_socket(const char *ip, int port)
  {
      return new_socket_fd(SERVICE_SERVER, SOCK_DGRAM, ip, port);
  }
  
  int new_udp_client_socket(const char *ip, int port)
  {
      return new_socket_fd(SERVICE_CLIENT, SOCK_DGRAM, ip, port);
  }
  ```

- ### 下面是一个基于TCP通信的，server和client代码如下：
  - #### file: tcp_server.c 
    ```C
    #include <stdio.h>
    #include <string.h>
    #include <arpa/inet.h>
    
    #include "socket_helper.h"
    
    int main(int argc, char **argv)
    {
        int client_fd;
        int server_fd;
    
        struct sockaddr_in sock_addr;
        socklen_t sock_addr_len = sizeof(sock_addr);
    
        char buff[128] = { 0 };
        int len;
    
        server_fd = new_tcp_server_socket("127.0.0.1", 20000);
        listen(server_fd, 1);
    
        client_fd = accept(server_fd, (struct sockaddr *) &sock_addr, &sock_addr_len);
        printf("tcp_server: new client, fd=%d\n", client_fd);
    
        len = read(client_fd, buff, 128);
        if (len > 0)
            printf("tcp_server: receive message: %s\n", buff);
        else
            printf("tcp_server: receive message ERROR!\n");
    
        close(client_fd);
        close(server_fd);
    
        return 0;
    }
    ```


  - #### file: tcp_client.c
    ```C
    #include <stdio.h>
    #include <string.h>
    
    #include "socket_helper.h"
    
    int main(int argc, char **argv)
    {
        const char *message = "this is a message from tcp client";
    
        int sockfd = new_tcp_client_socket("127.0.0.1", 20000);
    
        int ret = write(sockfd, message, strlen(message));
    
        if (ret > 0)
            printf("tcp_client: send message: %s\n", message);
        else
            fprintf(stderr, "tcp_client: failed to send message: %s\n", message);
    
        close(sockfd);
    
        return 0;
    }
    ```

- ### 在源码目录执行make命令分别编译出tcp_server和tcp_client。然后在第1个终端运行server，在第2个终端运行client，打印如下：
  ```bash
  # ./tcp_server 
  tcp_server: new client, fd=4
  tcp_server: receive message: this is a message from tcp client
      
  # ./tcp_client 
  tcp_client: send message: this is a message from tcp client
  ```


## LIBEVENT
### Libevent 介绍
  - #### 官方网址
    https://libevent.org

  - #### 源码阅读
    感兴趣的读者可以跟着这个博主一起看看源码
    https://blog.csdn.net/luotuo44/column/info/libevent-src

  - #### 一本很好的学习教程
    这本书很详细的介绍的Libevent的内容，细致到函数的定义、参数说明层面
    https://legacy.gitbook.com/download/pdf/book/aceld/libevent


### Libevent 安装
  - #### 很多发行版的Linux支持类似apt或yum等一键式安装，考虑到学习需求，我们直接通过编译源码方式安装，这样做可以：
    - 熟悉开源库编译、安装方式
    - 了解将开源库编入自己App的方法和运行方法
    - 必要时甚至修改libevent源码，调整感兴趣的部分
  - #### 选择一个工作目录(如：~/work)，后续使用 **$WORK_DIR** 表示该目录
  - #### 下载libevent源码包，本例使用libevent-2.1.8-stable
    ```bash
    # cd $WORK_DIR
    # wget https://github.com/libevent/libevent/releases/download/release-2.1.8-stable/libevent-2.1.8-stable.tar.gz
    ```
  - #### 创建一个目录，将libevent安装到这个目录下
    ```bash
    # cd $WORK_DIR
    # mkdir -p libevent-2.1.8-stable-libs
    ```
  - #### 编译配置，这里仅仅通过`--prefix`选项设置安装目录
    ```bash
    # cd $WORK_DIR
    # tar xvf libevent-2.1.8-stable.tar.gz
    # cd libevent-2.1.8-stable
    # ./configure --prefix=$WORK_DIR/libevent-2.1.8-stable-libs
    ```
  - ####  编译、安装
    ```bash
    # cd $WORK_DIR/libevent-2.1.8-stable
    # make
    # make install
    ```
  - #### 查看安装目录，已经存在一些头文件和.a/.so库文件
    ```bash
    # tree libevent-2.1.8-stable-libs
    libevent-2.1.8-stable-libs
    |---- bin
    |   \---- event_rpcgen.py
    |---- include
    |   |---- evdns.h
    |   |---- event2
    |   |   |---- buffer_compat.h
    |   |   |---- bufferevent_compat.h
    |   |   |---- bufferevent.h
    |   |   |---- bufferevent_ssl.h
    |   |   |---- bufferevent_struct.h
    ...
    |   |   |---- util.h
    |   |   \---- visibility.h
    |   |---- event.h
    |   |---- evhttp.h
    |   |---- evrpc.h
    |   \---- evutil.h
    \---- lib
        |---- libevent-2.1.so.6 -> libevent-2.1.so.6.0.2
        |---- libevent-2.1.so.6.0.2
        |---- libevent.a
    ...
        |---- libevent_extra.a
        |---- libevent_extra.la
        |---- libevent_extra.so -> libevent_extra-2.1.so.6.0.2
        |---- libevent.la
    ...
    ```

### Libevent 案例: basic
- #### 这个例子应该算是最简单最基本的模型，目的是监听local的30000端口发来的UDP报文，收到后打印到控制台。大致分为三步，
  - 调用event_base_new创建event_base，MOSWA每一个app（对于多线程的，可能每个线程）基本都会创建有一个event_base
  - 调用event_new创建event，这个event可以用来监听socket事件，比如数据到达
  - 调用event_base_dispatch开始监听左右注册到event_base的事件，如果事件发生，调用对应回调函数。event_base_dispatch将一直运行, 直到没有已经注册的事件。
  创建并注册事件到event_base可以发生app任何位置、任何函数，只要存在需求，不限于在调用event_base_dispatch之前。
  ```C
  file: ev_basic.c
  
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  
  #include <event.h>
  #include <event2/util.h>
  
  #include "socket_helper.h"
  
  void socket_read_cb(evutil_socket_t fd, short what, void *arg)
  {
      char buff[128] = { 0 };
      int len = read(fd, buff, 128);
      if (len > 0)
          printf("%s: receive message: %s\n", __func__, buff);
      else
          printf("%s: receive message ERROR!\n", __func__);
  }
  
  int main(int argc, char *argv[])
  {
      int sockfd;
      struct event_base *base;
      struct event *ev_read;
  
      // #1
      base = event_base_new();
  
      // #2
      sockfd = new_udp_server_socket("127.0.0.1", 30000);
  
      // 关注socket READ事件(EV_READ), 如有数据到来, 调用socket_read_cb
      // 默认情况, 事件到来处理完毕, 该事件会被从event_base移除
      // 如需再次监控, 需要重新注册, EV_PERSIST标记标明该事件需要一直被监控, 就不用每次都重新注册了
      ev_read = event_new(base, sockfd, EV_READ | EV_PERSIST, socket_read_cb, NULL);
      event_add(ev_read, NULL);
  
      // #3
      event_base_dispatch(base);
  
      return 0;
  }
  ```

- #### 编译
  - 由于我们使用了自己编译的lievent库不在系统默认目录，因此在编译时候需要显示的指定libevent头文件位置和.a/.so位置，命令如下：
    ```bash
    # gcc -I$WORK_DIR/libevent-2.1.8-stable-libs/include \
          -L$WORK_DIR/libevent-2.1.8-stable-libs/lib  \
          socket_helper.c ev_basic.c -o ev_basic -levent
    
    # ls -l ev_basic
    -rwxr-xr-x 1 guolinp platform 13608 Nov 15 12:48 ev_basic
    ```
  - 在后面例子中，编译方法与该例子一致，不在重复。如果使用提供的源码，直接make即可，将编译全部例子。

- #### 运行
  - 同样需要指定libevent库位置，通过环境变量LD_LIBRARY_PATH告诉ev_basic在哪里寻找并加载库so文件，在第1个shell终端中执行：
    ```bash
    # LD_LIBRARY_PATH=$WORK_DIR/libevent-2.1.8-stable-libs/lib ./ev_basic
    ```
  - 在第2这个shell终端中运行udp_client，向30000端口发UDP报文，连续执行三次
    ```bash
    # ./udp_client 
    udp_client: send message: this is a message from udp client
    # ./udp_client 
    udp_client: send message: this is a message from udp client
    # ./udp_client 
    udp_client: send message: this is a message from udp client
    ```
  - 再次回到第1个shell，观察到三行来自udp_client消息，源自函数socket_read_cb打印。由于事件标记EV_PERSIST，所以event_base_dispatch不退出，按Ctrl+C退出ev_basic。
    ```bash
    socket_read_cb: receive message: this is a message from udp client
    socket_read_cb: receive message: this is a message from udp client
    socket_read_cb: receive message: this is a message from udp client
    ^C
    ```
  - 两个问题：
    - 在这例子中如果事件没有标记EV_PERSIST会是什么效果？
    - LD_LIBRARY_PATH 是什么含义？

### Libevent 案例: bufferevent
- #### bufferevent的使用方法与上面socket例子很像，使用bufferevent对socket进行包装，当socket有数据到达时仍会调用提供的回调函数。同时bufferevent内部提供了buffer，如果本次回调函数不读出收到的数据，这些数据不会丢失，下次数据到达时候，回调函数再次被调用，这次读取数据的话，会将上次数据一并读出。
  - 下面演示代码中，在回调函数中跳过前两次数据读取操作，第三次才读数据。
    ```C
    file: ev_bufferevent.c
    
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    #include <event.h>
    #include <event2/util.h>
    
    #include "socket_helper.h"
    
    void buffer_event_event_cb(struct bufferevent *bev, short what, void *ctx)
    {
        printf("%s: buffer event is received\n", __func__);
    }
    
    void buffer_event_read_cb(struct bufferevent *bev, void *ctx)
    {
        int ret = 0;
        char buff[128] = { 0 };
    
        static int n = 0;
        if (n < 2) {
            // 这里跳过前两次数据读取操作
            printf("%s: do not read message from the buffer, %d time\n", __func__, n);
            n++;
            return;
        }
    
        ret = bufferevent_read(bev, buff, 128);
        if (ret > 0)
            printf("%s: bufferevent receive message: %s\n", __func__, buff);
        else
            printf("%s: bufferevent receive messag ERROR!\n", __func__);
    }
    
    int main(int argc, char *argv[])
    {
        int sockfd;
        struct event_base *base;
        struct bufferevent *bev;
    
        base = event_base_new();
        sockfd = new_udp_server_socket("127.0.0.1", 30000);
        bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
    
        bufferevent_setcb(bev, buffer_event_read_cb, NULL, buffer_event_event_cb, NULL);
        bufferevent_enable(bev, EV_READ | EV_PERSIST);
    
        event_base_dispatch(base);
    
        return 0;
    }
    ```
  - 在第1个shell终端中执行：
    ```bash
    # LD_LIBRARY_PATH=$WORK_DIR/libevent-2.1.8-stable-libs/lib ./ev_bufferevent
    ```
  - 在第2这个shell终端中运行udp_client，向30000端口发UDP报文，连续执行三次
    ```bash
    # ./udp_client 
    udp_client: send message: this is a message from udp client
    # ./udp_client 
    udp_client: send message: this is a message from udp client
    # ./udp_client 
    udp_client: send message: this is a message from udp client
    ```
  - 再次回到第1个shell，观察到出现两次打印：
    ```
    buffer_event_read_cb: do not read message from the buffer, 0 time
    ```
  - 第三次读数据操作将包括前面2个消息一并读出来。
    ```bash
    buffer_event_read_cb: do not read message from the buffer, 0 time
    buffer_event_read_cb: do not read message from the buffer, 1 time
    buffer_event_read_cb: bufferevent receive message: this is a message from udp clientthis is a message from udp clientthis is a message from udp client
    ^C
    ```

### Libevent 案例: bufferevent-pair
- #### Libevent提供一个API: bufferevent_pair_new， 它可以将两个bufferevent关联的起来，通过合适的设置，能够实现从一个bufferevent写数据之后，另一个bufferevent能够感知，这个和Linux中的管道行为很像。
  https://www.gnu.org/software/libc/manual/html_node/Creating-a-Pipe.html#Creating-a-Pipe
  ```C
  file: ev_bufferevent_pair.c
  
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  
  #include <event.h>
  #include <event2/util.h>
  #include <event2/bufferevent.h>
  
  #include "socket_helper.h"
  
  // 两个bufferevent, 分别用于读写
  struct bufferevent *bev[2] = { NULL, NULL };
  
  void buffer_event_event_cb(struct bufferevent *bev, short what, void *ctx)
  {
      printf("%s: bufferevent event is received\n", __func__);
  }
  
  void buffer_event_read_cb(struct bufferevent *bev, void *ctx)
  {
      int ret = 0;
      char buff[128] = { 0 };
  
      ret = bufferevent_read(bev, buff, 128);
      if (ret > 0)
          printf("%s: bufferevent receive message: %s\n", __func__, buff);
      else
          printf("%s: bufferevent receive message ERROR!\n", __func__);
  }
  
  void socket_read_cb(evutil_socket_t fd, short what, void *arg)
  {
      char buff[128] = { 0 };
      int len = read(fd, buff, 128);
      if (len > 0) {
          // 读取socket数据, 然后写入bev[0]
          // 这个操作之后event_base会检测到, 然后调用bev[1]注册的回调函数
          printf("%s: receive message: %s\n", __func__, buff);
          bufferevent_write(bev[0], buff, strlen(buff));
      } else {
          printf("%s: receive message ERROR!\n", __func__);
          bufferevent_write(bev[0], "ERROR!", strlen("ERROR!"));
      }
  }
  
  int main(int argc, char *argv[])
  {
      int sockfd;
      struct event_base *base;
      struct event *ev_read;
  
      base = event_base_new();
      sockfd = new_udp_server_socket("127.0.0.1", 30000);
      ev_read = event_new(base, sockfd, EV_READ | EV_PERSIST, socket_read_cb, NULL);
      event_add(ev_read, NULL);
  
      // 关联两个bufferevent到event_base, 之后会受到监控
      bufferevent_pair_new(base, BEV_OPT_CLOSE_ON_FREE, bev);
      // bev[1]作为读的一端, 设置回调函数
      bufferevent_setcb(bev[1], buffer_event_read_cb, NULL, buffer_event_event_cb, NULL);
      bufferevent_enable(bev[1], EV_READ | EV_PERSIST);
  
      event_base_dispatch(base);
  
      return 0;
  }
  ```

- #### 在第1个shell终端中执行：
  ```bash
  # LD_LIBRARY_PATH=$WORK_DIR/libevent-2.1.8-stable-libs/lib ./ev_bufferevent_pair
  ```
- #### 在第2这个shell终端中运行udp_client，向30000端口发UDP报文
  ```bash
  # ./udp_client 
  udp_client: send message: this is a message from udp client
  ```
- #### 再次回到第1个shell，观察到两个回调函数被预期调用：
  ```bash
  socket_read_cb: receive message: this is a message from udp client
  buffer_event_read_cb: bufferevent receive message: this is a message from udp client
  ```

### Libevent 案例: bufferevent-socket
- #### 在前面的tcp_server例子中可以看到，建立一个典型的server至少需要如下几个步骤：
  - 创建socket
  - 绑定socket到需要监听的地址端口
  - 开始侦听client
  - 收到client连接之后，accept得到与该client有关的socket fd以备使用
 
- #### 在libevent中提供了一个API: evconnlistener_new_bind，帮助我们完成上述所有操作，请看代码：
    ```C
    file: ev_bufferevent_socket.c
    
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    #include <event.h>
    #include <event2/util.h>
    #include <event2/bufferevent.h>
    #include <event2/listener.h>
    
    void buffer_event_event_cb(struct bufferevent *bev, short what, void *ctx)
    {
        printf("%s: buffer event is received\n", __func__);
    }
    
    void buffer_event_read_cb(struct bufferevent *bev, void *ctx)
    {
        int ret = 0;
        char buff[128] = { 0 };
    
        ret = bufferevent_read(bev, buff, 128);
        if (ret > 0)
            printf("%s: bufferevent receive message: %s\n", __func__, buff);
        else
            printf("%s: bufferevent receive messag ERROR!\n", __func__);
    }
    
    //这个函数在调用evconnlistener_new_bind被提供作为回调函数
    //调用时机是当client连接被接受而且新的与client关联的socket, fd被创建后
    //函数入参
    //fd为client, socket
    //sock_addr/len, 为client地址信息
    //ctx, 为注册callback时提供的context的回传, 本例传递是event_base实例
    void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock_addr, int len, void *ctx)
    {
        struct bufferevent *bev;
        struct event_base *base = (struct event_base *) ctx;
    
        printf("%s: new client, fd=%d\n", __func__, fd);
    
        // 我们得到client的socket, 并将其加入event_base, 这样一旦client有数据到达可以检测到
        bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    
        bufferevent_setcb(bev, buffer_event_read_cb, NULL, buffer_event_event_cb, NULL);
        bufferevent_enable(bev, EV_READ | EV_PERSIST);
    }
    
    int main(int argc, char *argv[])
    {
        struct event_base *base;
        struct event *ev_read;
        struct sockaddr_in sock_addr;
        struct evconnlistener *listener;
    
        const char *ip = "127.0.0.1";
        int port = 20000;
    
        bzero(&sock_addr, sizeof(sock_addr));
        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = htons(port);
        sock_addr.sin_addr.s_addr = inet_addr(ip);
    
        base = event_base_new();
    
        // 我们没有自己创建server socket, 仅提供需要被监听的地址信息和listener_cb
        listener = evconnlistener_new_bind(base,
                           listener_cb,
                           base,
                           LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
                           4, (struct sockaddr *) &sock_addr,
                           sizeof(sock_addr));
    
        event_base_dispatch(base);
    
        return 0;
    }
    ```

- #### 在第1个shell终端中执行：
  ```bash
  # LD_LIBRARY_PATH=$WORK_DIR/libevent-2.1.8-stable-libs/lib ./ev_bufferevent_socket
  ```

- #### 在第2这个shell终端中运行tcp_client，向20000端口发TCP报文
  ```bash
  # ./tcp_client 
  tcp_client: send message: this is a message from tcp client
  ```

- #### 再次回到第1个shell，观察到client连接时listener_cb被调用，当client数据到来时buffer_event_read_cb被调用。
  ```bash
  listener_cb: new client, fd=7
  buffer_event_read_cb: bufferevent receive message: this is a message from tcp client
  buffer_event_event_cb: buffer event is received
  ```

- #### 一个问题：
  - 例子中buffer_event_event_cb也被调用了，这个在之前以UDP作为例子时候没有见到过。buffer_event_event_cb被执行时因为收到了什么事件呢？
  - 提示：事件用what入参表示，定义如下：
    ```C
    #define BEV_EVENT_READING    0x01  // error encountered while reading
    #define BEV_EVENT_WRITING    0x02  // error encountered while writing
    #define BEV_EVENT_EOF    0x10  // eof file reached           
    #define BEV_EVENT_ERROR      0x20  // unrecoverable error encountered
    #define BEV_EVENT_TIMEOUT    0x40  // user-specified timeout reached 
    #define BEV_EVENT_CONNECTED  0x80  // connect operation finished    
    ```

### Llibevent 案例: evbuffer
- #### Libevent的evbuffer实现了为向后面添加数据和从前面移除数据而优化的字节队列。
  ```C
  file: ev_evbuffer.c
  
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  
  #include <event.h>
  #include <event2/util.h>
  
  #include "socket_helper.h"
  
  void socket_read_cb(evutil_socket_t fd, short what, void *arg)
  {
      int len;
      char *str;
      struct evbuffer *evbuf = (struct evbuffer *) arg;
      char data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  
      // client 数据到达, 将数据读入evbuffer
      // 使用API：int evbuffer_add(struct evbuffer *buf, const void *data, size_t datlen
      //也可以将data保存到evbuffer, 此时需要先使用read读取socket数据到data,然后evbuffer_add到evbuffer
      len = evbuffer_read(evbuf, fd, 128);
      printf("%s: read data to evbuffer, %d\n", __func__, len);
  
      //API:, evbuffer_get_length, 查看读到数据大小)
      len = evbuffer_get_length(evbuf);
      printf("%s: data length in evbuffer, %d\n", __func__, len);
  
      //API:, evbuffer_pullup, 提取前N个字节, 注意在调用之后数据仍旧保存在evbuffer不会丢失
      str = evbuffer_pullup(evbuf, 4);
      printf("%s: pullup the first 4 bytes, %c,%c,%c,%c\n", __func__, str[0], str[1], str[2], str[3]);
      //为了验证不会丢失, 再次查看一下
      str = evbuffer_pullup(evbuf, 4);
      printf("%s: pullup the first 4 bytes, %c,%c,%c,%c\n", __func__, str[0], str[1], str[2], str[3]);
  
      //删除前4个字节
      printf("%s: drain evbuffer, 4 bytes\n", __func__);
      evbuffer_drain(evbuf, 4);
  
      //查看读到数据大小, 应该减少4
      len = evbuffer_get_length(evbuf);
      printf("%s: data length in evbuffer, %d\n", __func__, len);
  
      //打印一下前4个字节
      str = evbuffer_pullup(evbuf, 4);
      printf("%s: pullup the first 4 bytes, %c,%c,%c,%c\n", __func__, str[0], str[1], str[2], str[3]);
  
      //拷贝前7个字节之后并从evbuffer内删除
      evbuffer_remove(evbuf, (void *) data, 7);
      printf("%s: remove data, %s\n", __func__, data);
  
      //查看读到数据大小, 应该减少7
      len = evbuffer_get_length(evbuf);
      printf("%s: data length in evbuffer, %d\n", __func__, len);
  }
  
  int main(int argc, char *argv[])
  {
      int sockfd;
      struct event_base *base;
      struct event *ev_read;
      struct evbuffer *evbuf;
  
      // 新建一个evbuffer
      evbuf = evbuffer_new();
  
      base = event_base_new();
      sockfd = new_udp_server_socket("127.0.0.1", 30000);
      // 新建socket读事件, 将evbuf作为回调函数socket_read_cb参数
      ev_read = event_new(base, sockfd, EV_READ | EV_PERSIST, socket_read_cb, evbuf);
  
      event_add(ev_read, NULL);
  
      event_base_dispatch(base);
  
      return 0;
  }
  ```

- #### 在第1个shell终端中执行：
  ```bash
  # LD_LIBRARY_PATH=$WORK_DIR/libevent-2.1.8-stable-libs/lib ./ev_evbuffer
  ```

- #### 在第2这个shell终端中运行tcp_client，向30000端口发UDP报文
  ```bash
  # LD_LIBRARY_PATH=../libevent-2.1.8-stable-libs/lib ./udp_client
  udp_client: send message: this is a message from udp client
  ```

- #### 再次回到第1个shell，观察打印信息。
  ```bash
  socket_read_cb: read data to evbuffer, 33
  socket_read_cb: data length in evbuffer, 33
  socket_read_cb: pullup the first 4 bytes, t,h,i,s
  socket_read_cb: pullup the first 4 bytes, t,h,i,s
  socket_read_cb: drain evbuffer, 4 bytes
  socket_read_cb: data length in evbuffer, 29
  socket_read_cb: pullup the first 4 bytes,  ,i,s, 
  socket_read_cb: remove data,  is a m
  socket_read_cb: data length in evbuffer, 22
  ```

### Libevent 案例: signal
- #### 当进程接收到信号时候，会采取默认行为处理：
  http://man7.org/linux/man-pages/man7/signal.7.html
 
- #### C库也提供了手段使得用户可以接管信号处理：
  http://man7.org/linux/man-pages/man2/sigaction.2.html
- #### 这个例子我们看一下libevent对于信号处理的封装接口。
  ```C
  file: ev_signal.c
  
  #include <stdio.h>
  #include <stdlib.h>
  #include <signal.h>
  
  #include <event2/event.h>
  #include <event2/event_struct.h>
  
  void signal_handler(int signo, short events, void *arg)
  {
      printf("signal_handler: receive signal %d\n", signo);
  }
  
  int main(int argc, char *argv[])
  {
      int signo = SIGINT;
      struct event_base *base;
      struct event *sig_event;
  
      base = event_base_new();
  
      // 创建并注册信号事件, 信号处理函数signal_handler
      // 接管信号是 SIGINT, 按Ctrl+C即可向进程发送该信号, 便于测试
      sig_event = evsignal_new(base, signo, signal_handler, NULL);
      evsignal_add(sig_event, NULL);
  
      event_base_dispatch(base);
  
      return 0;
  }
  ```

- #### 执行如下命令，按Ctrl+C发送信号。按Ctrl+\退出
  ```bash
  # LD_LIBRARY_PATH=../libevent-2.1.8-stable-libs/lib ./ev_signal
    
  ^Csignal_handler: receive signal 2
  ^Csignal_handler: receive signal 2
  ^Csignal_handler: receive signal 2
  ^Csignal_handler: receive signal 2
  ^Csignal_handler: receive signal 2
  ^\[1]    9907 quit       LD_LIBRARY_PATH=../libevent-2.1.8-stable-libs/lib ./ev_signal
  ```

### Libevent 案例: timer
- #### 使用libevent提供的timer十分容易，请直接看代码
  ```C
  fiel: ev_timer.c
  
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  
  #include <event.h>
  #include <event2/util.h>
  
  struct context {
      struct timeval tv;
      struct event *t_event;
  };
  
  void timeout_cb(int fd, short event, void *context)
  {
      struct context *ctx = (struct context *) context;
      printf("%s: 1 second passed\n", __func__);
      // 设定时间到达, 若再次开启需再次加入event_base
      evtimer_add(ctx->t_event, &ctx->tv);
  }
  
  int main(int argc, char *argv[])
  {
      struct context ctx = { {1, 0}, NULL };
      struct event_base *base = event_base_new();
  
      // 创建timer事件, 超时回调函数timeout_cb
      ctx.t_event = evtimer_new(base, timeout_cb, &ctx);
      // 将timer事件加入event_base, 此时timer已经启动
      evtimer_add(ctx.t_event, &ctx.tv);
  
      event_base_dispatch(base);
  
      return 0;
  }
  ```
- #### 执行如下命令，观察到每秒打印一行，timer工作！
  ```bash
  # LD_LIBRARY_PATH=$WORK_DIR/libevent-2.1.8-stable-libs/lib ./ev_timer
  
  
  timeout_cb: 1 second passed
  timeout_cb: 1 second passed
  timeout_cb: 1 second passed
  timeout_cb: 1 second passed
  timeout_cb: 1 second passed
  timeout_cb: 1 second passed
  timeout_cb: 1 second passed
  timeout_cb: 1 second passed
  ^C
  ```
