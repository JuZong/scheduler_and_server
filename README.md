# 负载均衡器
基于linux平台下，c++开发的高性能应用层负载均衡器
基于STLmap和md5实现一致性哈希算法，且实现了服务器的容灾处理，采用STL内存池高效管理内存
采用半同步半异步多线程处理客户端请求，半同步半反应堆线程池处理服务器请求，使用json的合成与解析，实现对配置文件的读取以及数据的交互。
利用同步信号实现负载均衡器端服务器的热加载。
采用半同步半反应堆多进程模型，实现http服务器端，实现了http报头的简单封装和解析。