# 说明
Fenice is a rtsp media server,base on ver 1.12 and add support to H.264 file.

原项目来自一个开源项目，由于找不到1.12的开源项目地址，找了另一个旧版的开源的地址，可以参考下
由于比较麻烦，就不在原项目fork修改了。

Fenice-1.11项目地址：
[https://github.com/zhuqingcode/fenice-1.11](https://github.com/zhuqingcode/fenice-1.11 "")

#修改内容
由于我是基于原来的Fenice开源项目的结构来修改，原项目已经实现了RTSP部分以及MPEG-1和2的流媒体传输，所以我参考MPV的读取、加载释放文件进行修改，主要修改了下列文件：

- **avroot/myh264.sd** …… 用于记录H.264视频文件的流格式信息，包括264文件的帧率、payload_type、编码方式等等
- **include/fenice/itu_h264.h** …… 对H.264格式的声明，例如包含FU_HEADER结构体，以及read/load/free三个方法的声明
- **include/fenice/mediainfo.h** ……主要添加了_media_entry的成员H.264_time，给read_H.264使用
- **include/fenice/types.h** …… 为了方便，添加了TRUE和FALSE的宏定义
- **mediainfo/Makefile.am** …… 修改makefile添加新的c文件
- **mediainfo/free_h264.c** …… 释放H.264视频
- **mediainfo/load_h264.c** …… 加载H.264视频，直接照搬load_MPV.c
- **mediainfo/read_h264.c** …… 读取H.264视频并打包RTP包
- **mediainfo/parse_SD_file.c** …… 解析SD文件，添加支持H.264编码
- **mediainfo/register_media.c** …… 声明H.264的load_media、read_media和free_media的三个对应方法
- **src/is_supported_url.c** …… 判断是否支持地址类型，添加264文件支持

- **avroot/yourfile.264** ……H.264视频文件，**需要你自己提供**，可以参考下面提供的博文连接

#H.264文件
如何转换出H.264文件,可以参考我的博文
[http://blog.csdn.net/maxwell_nc/article/details/50315675](http://blog.csdn.net/maxwell_nc/article/details/50315675 "http://blog.csdn.net/maxwell_nc/article/details/50315675")

另外整个封包过程也可以参考我的博文:
[http://blog.csdn.net/maxwell_nc/article/details/50267593](http://blog.csdn.net/maxwell_nc/article/details/50267593 "http://blog.csdn.net/maxwell_nc/article/details/50267593")

#使用方法

1. 配置文件：/etc/fenice.conf
```
	root=/srv/fenice/avroot
	log_file=/var/log/fenice.log
	tcp_port=554
	sctp_port=-1
	max_session=100
```
	默认端口554，root就是avroot目录，可以自行修改。

2. 日志文件：/var/log/fenice.log

3. 编译：
```
	./auto
	./configure
	make
	sudo make install
```

4. 运行：

	默认生成在/usr/local/bin
```
	./fenice -c /etc/fenice.conf
```

5. 使用VLC访问：
```
rtsp://localhost:554/myh264.sd
```