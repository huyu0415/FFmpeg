#### 命令格式：
    ffmpeg -i [输入文件名] [参数选项] -f [格式] [输出文件]

#### 参数及解释

##### a) 通用选项
        
    -L                  license
    -h                  帮助
    -fromats            显示可用的格式，编解码的，协议的...
    -codecs             查看所有编解码器
    -filters            查看所有可用的filter
    -pix_fmts           查看所有支持的图片格式
    -sample_fmts        查看所有支持的像素格式
    -f fmt              强迫采用输出格式fmt
    -hq                 激活高质量设置
            
    -i filename         输入文件
    -i filename         filename 后面不再接其他参数，查看媒体文件 filename 的信息
    -y                  覆盖输出文件
    -t duration         设置纪录时间 hh:mm:ss[.xxx]格式的记录时间也支持
    -ss position        搜索到指定的时间 [-]hh:mm:ss[.xxx]的格式也支持
    -title string       设置标题
    -author string      设置作者
    -copyright string   设置版权
    -comment string     设置评论
    -target type        设置目标文件类型(vcd,svcd,dvd) 所有的格式选项（比特率，编解码以及缓冲区大小）自动设置，只需要输入如下的就可以了：ffmpeg -i myfile.avi -target vcd /tmp/vcd.mpg
    -itsoffset offset   设置以秒为基准的时间偏移，该选项影响所有后面的输入文件。该偏移被加到输入文件的时戳，定义一个正偏移意味着相应的流被延迟了 offset秒。 [-]hh:mm:ss[.xxx]的格式也支持

##### b) 视频选项
        
    -b bitrate          设置比特率，缺省200kb/s
    -r fps              设置帧频 缺省25
    -s size             设置帧大小 格式为WXH 缺省160X128.下面的简写也可以直接使用：
                            Sqcif 128X96 qcif 176X144 cif 252X288 4cif 704X576
						
    -aspect aspect      设置横纵比 4:3 16:9 或 1.3333 1.7777
    -croptop size       设置顶部切除带大小 像素单位
                            -cropbottom size -cropleft size -cropright size
                        
    -padtop size        设置顶部补齐的大小 像素单位
    -padbottom size
    -padleft size
    -padright size 
        
    -padcolor color     设置补齐条颜色(hex,6个16进制的数，红:绿:兰排列，比如 000000代表黑色)
    -vn                 去掉视频
    -bt tolerance       设置视频码率容忍度kbit/s
    -maxrate bitrate    设置最大视频码率容忍度
    -minrate bitreate   设置最小视频码率容忍度
    -bufsize size       设置码率控制缓冲区大小
    -vcodec codec       强制使用codec编解码方式。如果用copy表示原始编解码数据必须被拷贝。
    -sameq              使用同样视频质量作为源（VBR）
    -pass n             选择处理遍数（1或者2）。两遍编码非常有用。第一遍生成统计信息，第二遍生成精确的请求的码率
    -passlogfile file   选择两遍的纪录文件名为file

##### c)高级视频选项
        
    -g gop_size                 设置图像组大小
    -intra                      仅适用帧内编码
    -qscale q                   使用固定的视频量化标度(VBR)
    -qmin q                     最小视频量化标度(VBR)
    -qmax q                     最大视频量化标度(VBR)
    -qdiff q                    量化标度间最大偏差 (VBR)
    -qblur blur                 视频量化标度柔化(VBR)
    -qcomp compression          视频量化标度压缩(VBR)
    -rc_init_cplx complexity    一遍编码的初始复杂度
        
    -b_qfactor factor           在p和b帧间的qp因子
    -i_qfactor factor           在p和i帧间的qp因子
    -b_qoffset offset           在p和b帧间的qp偏差
    -i_qoffset offset 			在p和i帧间的qp偏差
        
    -rc_eq equation             设置码率控制方程 默认tex^qComp
    -rc_override override       特定间隔下的速率控制重载
    -me method                  设置运动估计的方法 可用方法有 zero phods log x1 epzs(缺省) full
    -dct_algo algo              设置dct的算法 可用的有 0 FF_DCT_AUTO 缺省的DCT 1 FF_DCT_FASTINT 2 FF_DCT_INT 3 FF_DCT_MMX 4 FF_DCT_MLIB 5 FF_DCT_ALTIVEC
    -idct_algo algo             设置idct算法。可用的有 0 FF_IDCT_AUTO 缺省的IDCT 1 FF_IDCT_INT 2 FF_IDCT_SIMPLE 3 FF_IDCT_SIMPLEMMX 4 FF_IDCT_LIBMPEG2MMX 5 FF_IDCT_PS2 6 FF_IDCT_MLIB 7 FF_IDCT_ARM 8 FF_IDCT_ALTIVEC 9 FF_IDCT_SH4 10 FF_IDCT_SIMPLEARM
    -er n                       设置错误残留为n 1 FF_ER_CAREFULL 缺省 2 FF_ER_COMPLIANT 3 FF_ER_AGGRESSIVE 4 FF_ER_VERY_AGGRESSIVE
    -ec bit_mask                设置错误掩蔽为bit_mask,该值为如下值的位掩码 1 FF_EC_GUESS_MVS (default=enabled) 2 FF_EC_DEBLOCK (default=enabled)
    -bf frames                  使用frames B 帧，支持mpeg1,mpeg2,mpeg4
    -mbd mode                   宏块决策 0 FF_MB_DECISION_SIMPLE 使用mb_cmp 1 FF_MB_DECISION_BITS 2 FF_MB_DECISION_RD
    -4mv                        使用4个运动矢量 仅用于mpeg4
    -part                       使用数据划分 仅用于mpeg4
    -bug param                  绕过没有被自动监测到编码器的问题
    -strict strictness          跟标准的严格性
    -aic                        使能高级帧内编码 h263+
    -umv                        使能无限运动矢量 h263+
    -deinterlace                不采用交织方法
    -interlace                  强迫交织法编码仅对mpeg2和mpeg4有效。当你的输入是交织的并且你想要保持交织以最小图像损失的时候采用该选项。可选的方法是不交织，但是损失更大
    -psnr                       计算压缩帧的psnr
    -vstats                     输出视频编码统计到vstats_hhmmss.log
    -vhook module               插入视频处理模块 module 包括了模块名和参数，用空格分开

##### d)音频选项
        
    -ab bitrate         设置音频码率
    -ar freq            设置音频采样率
    -ac channels        设置通道 缺省为1
    -an                 去掉音频
    -acodec codec       使用codec编解码，如果用copy表示原始编解码数据必须被拷贝。

##### e)音频/视频捕获选项
        
    -vd device          设置视频捕获设备。比如/dev/video0
    -vc channel         设置视频捕获通道 DV1394专用
    -tvstd standard     设置电视标准 NTSC PAL(SECAM)
    -dv1394             设置DV1394捕获
    -av device          设置音频设备 比如/dev/dsp

##### f)高级选项
        
    -map file:stream    设置输入流映射
    -debug              打印特定调试信息
    -benchmark          为基准测试加入时间
    -hex                倾倒每一个输入包
    -bitexact           仅使用位精确算法 用于编解码测试
    -ps size            设置包大小，以bits为单位
    -re                 以本地帧频读数据，主要用于模拟捕获设备
    -loop               循环输入流（只工作于图像流，用于ffserver测试）

##### g)格式：
        
    (1) h264:           表示输出的是h264的视频裸流，h264裸流是没有音频的
    (2) mp4:            表示输出的是mp4的视频
    (3) mpegts:         表示ts视频流


#### 第一组：yuv操作
1.  YUV序列播放
        
        ffplay -f rawvideo -video_size 1920x1080 input.yuv  //分辨率要与文件的分辨率一致，否则花屏

2.  播放封装格式
	        
	    ffplay -i test.mp4  
	    ffplay -i test.264	

3.  YUV序列转AVI
        
	    ffmpeg -s 1920*1080 -pix_fmt yuv420p -i input.yuv -vcodec mpeg4 output.avi
	    ffmpeg -s 1920*1080 -pix_fmt yuv420p -i input.yuv -vcodec h264 output.avi

4.  mp4 转 yuv (转换成YUV原始文件)
        
	    ffmpeg -i test.mp4 -vcodec rawvideo -an test.yuv 

5.  yuv格式转换
        
	    ffmpeg -pix_fmt yuv420p -s 176x144 -i carphone_qcif.yuv -pix_fmt nv12 carphone_qcif_nv12.yuv 	

6.  yuv缩放
        
	    ffmpeg -s:v 1920x1080 -r 25 -i input.yuv -vf scale=960:540 -c:v rawvideo -pix_fmt yuv420p out.yuv
	
7.  解码码流 264解码出yuv
        
	    ffmpeg -i 720P.264 -s 1280x720 -pix_fmt yuv422p 720P-out.yuv 	

8.  抽取某一帧YUV
        
	1.先抽出jpeg图片（先抽取10帧图片）
	        
        ffmpeg -i input.mp4 -ss 00:00:20 -t 10 -r 1 -q:v 2 -f image2 pic-%03d.jpeg
        
	2.把jpeg转为YUV	（挑一张，转为YUV）
	        
		ffmpeg -i pic-001.jpeg -s 1440x1440 -pix_fmt yuv420p xxx3.yuv  //如果-s参数不写，则输出大小与输入一样,YUV还有yuv422p啥的，你在 -pix_fmt 换成yuv422p就行

9. H264编码profile & level控制
        
	[详见](http://www.cnblogs.com/frost-yen/p/5848781.html)http://www.cnblogs.com/frost-yen/p/5848781.html

		
		
#### 第二组：格式转换
1.  封装格式转换，比如一个avi文件，想转为mp4，或者一个mp4想转为ts。 

    	ffmpeg -i input.avi output.mp4 
	    ffmpeg -i input.mp4 output.ts
	
2.  H264视频转ts视频流：

	    ffmpeg -i test.h264 -vcodec copy -f mpegts test.ts  

3.  H264视频转mp4:
        
	    ffmpeg -i test.h264 -vcodec copy -f mp4 test.mp4  
      
4.  ts视频转mp4:

	    ffmpeg -i test.ts -acodec copy -vcodec copy -f mp4 test.mp4 

5.  视频转码
        
        ffmpeg -i test.mp4 -vcodec h264 -s 352*278 -an -f m4v test.264              //转码为h264码流原始文件
        ffmpeg -i test.mp4 -vcodec h264 -bf 0 -g 25 -s 352*278 -an -f m4v test.264  //转码为码流原始文件
        ffmpeg -i test.avi -vcodec mpeg4 -vtag xvid -qsame test_xvid.avi            //转码为封装文件
        -bf B帧数目控制
        -g 关键帧间隔控制
        -s 分辨率控制	
	
6.  编码格式转换

	    //视频的编码是MPEG4，转成H264/H265编码 
	    ffmpeg -i input.mp4 -vcodec h264 -s 1280x720 output.mp4 //设置输出视频的分辨率
	    ffmpeg -i input.mp4 -vcodec hevc output.mp4 
	    //相反 
	    ffmpeg -i input.mp4 -vcodec mpeg4 output.mp4
	        
    当然了，如果ffmpeg当时编译时，添加了外部的x265或者X264，那也可以用外部的编码器来编码。 
    
	    ffmpeg -i input.mp4 -c:v libx265 output.mp4 
	    ffmpeg -i input.mp4 -c:v libx264 output.mp4
	    ffmpeg -i input.mp4 -c:v libx264 -crf 24 output.flv
	    // -crf 控制转码后视频的质量，质量越高，文件也就越大。
	    // 此值的范围是 0 到 51：0 表示高清无损；23 是默认值（如果没有指定此参数）；51 虽然文件最小，但效果是最差的。
	    // 建议的值范围是 18 到 28。而值 18 是视觉上看起来无损或接近无损的，当然不代表是数据（技术上）的转码无损。
		
7.  将 .avi 视频转成 .mpg
        
    	ffmpeg -i input.avi output.mpg

8.  将 .mpg 转成 .avi
        
    	ffmpeg -i input.mpg output.avi
	
9.  将 .avi 转成 gif动画（未压缩）
        
    	ffmpeg -i input.avi gif_anime.gif	
	
10. 将 .avi 转成 .flv
        
    	ffmpeg -i video_origine.avi -ab 56 -ar 44100 -b 200 -r 15 -s 320x240 -f flv video_finale.flv

11. 将 .avi 转成 dv
        
	    ffmpeg -i video_origine.avi -s pal -r pal -aspect 4:3 -ar 48000 -ac 2 video_finale.dv
	或者：
	
	    ffmpeg -i video_origine.avi -target pal-dv video_finale.dv
    

	
	
#### 第三组：音视频操作
1.  获取视频的信息
        
	    ffmpeg -i video.avi

2.  将图片序列 + 声音文件 合成视频
        
	    ffmpeg -f image2 -i image%d.jpg video.mpg
	上面的命令会把当前目录下的图片（名字如：image1.jpg. image2.jpg. 等...）合并成video.mpg
	    
	    ffmpeg -i 001.mp3 -i darkdoor.%3d.jpg -s 1024x768 -author fy -vcodec mpeg4 darkdoor.avi
		// 把darkdoor.[001-100].jpg序列帧和001.mp3音频文件利用mpeg4编码方式合成视频文件darkdoor.avi

3.  将视频分解成图片序列
        
	    ffmpeg -i video.mpg image%d.jpg
	上面的命令会生成image1.jpg. image2.jpg. ...支持的图片格式有：PGM. PPM. PAM. PGMYUV. JPEG. GIF. PNG. TIFF. SGI

4.  为视频重新编码以适合在 iPod/iPhone 上播放
        
	    ffmpeg -i input.avi input -acodec aac -ab 128kb -vcodec mpeg4 -b 1200kb -mbd 2 -flags +4mv+trell -aic 2 -cmp 2 -subcmp 2 -s 320x180 -title X final_video.mp4

5.  为视频重新编码以适合在 PSP 上播放
        
	    ffmpeg -i input.avi -b 300 -s 320x240 -vcodec xvid -ab 32 -ar 24000 -acodec aac final_video.mp4

6.  从视频抽出声音并存为 Mp3
        
	    ffmpeg -i input.avi -vn -ar 44100 -ac 2 -ab 192 -f mp3 sound.mp3
	
7.  从 flv 提取 mp3

	    ffmpeg -i input.flv -ab 128k dest.mp3

8.  提取视频中的音频,并保存为 mp3 然后输出
        
	    ffmpeg -i input.avi -b:a 128k output.mp3
		
9.  将 wav 文件转成 Mp3
        
	    ffmpeg -i input.avi -vn -ar 44100 -ac 2 -ab 192 -f mp3 son_final.mp3

10. 分离视频音频流
        
        ffmpeg -i input_file -vcodec copy -an output_file_video　　//分离视频流
        ffmpeg -i input_file -acodec copy -vn output_file_audio　　//分离音频流
        ffmpeg -i input_file -vn -acodec pcm_s16le output_file_audio.wav 
    
11. 视频解复用，提取视频ES数据

        ffmpeg -i test.mp4 -vcodec copy -an -f m4v test.264
        ffmpeg -i test.avi -vcodec copy -an -f m4v test.264
    
12. 音频 + 视频合并
        
        ffmpeg -i video_file -i audio_file -vcodec copy -acodec copy output_file
	    ffmpeg -i son.wav -i video_origine.avi video_finale.mpg
    
13. 视频剪切
        
        ffmpeg -ss 0:1:30 -t 0:0:20 -i input.avi -vcodec copy -acodec copy output.avi    //剪切视频
	    ffmpeg -ss 90 -t 20 -i input.avi -vcodec copy -acodec copy output.avi
        -r 提取图像的频率
        -ss 开始时间
        -t 持续时间, 上面的两种方式等价，时间格式支持 HH:MM:SS或者秒数
	
14. 截屏
        
	    ffmpeg -i test.avi -r 1 -q:v 2 -f image2 image-%3d.jpeg        
	    //提取图片，每隔一秒，存一张图片下来
	     
	    ffmpeg -i input.mp4 -ss 00:00:20 -t 10 -r 1 -q:v 2 -f image2 pic-%03d.jpeg 	
	    //可以设置开始的时间，和你想要截取的时间。从第20s时间开始，往下10s，即20~30s这10秒钟之间，每隔1s就抓一帧，总共会抓10帧
	    -ss 	表示开始时间 
	    -t 		表示共要多少时间。 
	    -r 		表示提取图像的频率，表示每一秒几帧 ；
	    -q:v	表示存储jpeg的图像质量，一般2是高质量
	    -f 		表示输出格式， %3d 表示文件命名方式（也即生成的图片名为 image-001.jpeg, image-002.jpeg, ...., image-999.jpeg）
	
15. 视频录制 rtsp 网络数据流,将 rtsp 的网络视频流文件保存为 out.avi 本地文件
        
        ffmpeg -i rtsp://192.168.3.205:5555/test -vcodec copy out.avi

16. 视频拼接：
        
    对于视频拼接，个人感觉最好是从视频的裸码流开始拼接，后面再进行视频包装，即格式转换。  
            
	    ffmpeg -i "concat:test1.h264|test2.h264" -vcodec copy -f h264 out12.h264
	    
	或者创建一个需要拼接的文件，例如 concat.txt，内容如下：
	        
	    file 'orig_20160616113303_0.avi'
	    file 'orig_20160616113303_1.avi'
	执行
	        
		ffmpeg -f concat -i concat.txt -c copy orig_20160616113303_merge.avi 

17. 利用ffmpeg视频切片

	主要把视频源切成若干个 .ts 格式的视频片段然后生成一个 .m3u8 的切片文件索引提供给 html5 的 video 做 hls 直播源
            
	    ffmpeg -i 视频源地址 -strict -2 -c:v libx264 -c:a aac -f hls m3u8文件输出地址

18. 将 .avi 压缩成 divx
        
	    ffmpeg -i video_origine.avi -s 320x240 -vcodec msmpeg4v2 video_finale.avi

19. 将 Ogg Theora 压缩成 Mpeg dvd
        
	    ffmpeg -i film_sortie_cinelerra.ogm -s 720x576 -vcodec mpeg2video -acodec mp3 film_terminate.mpg

20. 将 .avi 压缩成 SVCD mpeg2
	NTSC格式：
	    
		ffmpeg -i video_origine.avi -target ntsc-svcd video_finale.mpg
		
	PAL格式：
	        
		ffmpeg -i video_origine.avi -target pal-dvcd video_finale.mpg

21. 将 .avi 压缩成 VCD mpeg2
	NTSC格式：
	        
		ffmpeg -i video_origine.avi -target ntsc-vcd video_finale.mpg
		    
	PAL格式：
	        
		ffmpeg -i video_origine.avi -target pal-vcd video_finale.mpg

22. 多通道编码
        
	    ffmpeg -i fichierentree -pass 2 -passlogfile ffmpeg2pass fichiersortie-2

23. 控制关键帧间隔和B帧
        
	    ffmpeg -i input_file  -vcodec h264 -bf 0 -g 25 -s 854x480 -an -f m4v output_file  	
	    -bf 控制B帧数目
	    -g 控制关键帧间隔
	    -f 控制文件格式（format，注意与codec的区别）
	
24. 码率控制
	码率控制对于在线视频比较重要。因为在线视频需要考虑其能提供的带宽，那么，什么是码率？
	很简单： bitrate = file size / duration 
	比如一个文件20.8M，时长1分钟，那么，码率就是： biterate = 20.8M bit/60s = 20.8*1024*1024*8 bit/60s= 2831Kbps 
	一般音频的码率只有固定几种，比如是128Kbps， 那么，video的就是 video biterate = 2831Kbps -128Kbps = 2703Kbps。

	ffmpg控制码率有3种选择，-minrate -b:v -maxrate 
		-b:v主要是控制平均码率。 比如一个视频源的码率太高了，有10Mbps，文件太大，想把文件弄小一点，但是又不破坏分辨率。 
	        
        ffmpeg -i input.mp4 -b:v 2000k output.mp4 
        
	上面把码率从原码率转成2Mbps码率，这样其实也间接让文件变小了。目测接近一半。 
	不过，ffmpeg官方wiki比较建议，设置b:v时，同时加上 -bufsize 
	-bufsize 用于设置码率控制缓冲器的大小，设置的好处是，让整体的码率更趋近于希望的值，减少波动。（简单来说，比如1 2的平均值是1.5， 1.49 1.51 也是1.5, 当然是第二种比较好） 
		    
		ffmpeg -i input.mp4 -b:v 2000k -bufsize 2000k output.mp4
		
	-minrate 在线视频有时候，希望码率波动，不要超过一个阈值，可以设置maxrate。 
	    
		ffmpeg -i input.mp4 -b:v 2000k -bufsize 2000k -maxrate 2500k output.mp4
	
25. 在多音轨文件中提取某音轨文件
    
    第一步，查看源文件，找到对应的音频序号；
	        
		ffmpeg -i demo.mkv
	第二步，将对应的音频序号流提取出来(我们只提取第二个音轨流)
	        
		ffmpeg -i demo.mkv -map 0:2 audio_2.wav  

26. 过滤器的使用
        
	1.将输入的1080p（即 1920×1080 px，16:9）缩小到 480p 输出:
	              
		ffmpeg -i input.mp4 -vf scale=853:480 -acodec aac -vcodec h264 output.mp4
		-i a.mov 指定待处理视频的文件名
		-vf scale=853:480 vf 参数用于指定视频滤镜，其中 scale 表示缩放，后面的数字表示缩放至 853×480 px，
		    其中的 853px 是计算而得，因为原始视频的宽高比为 16:9，所以为了让目标视频的高度为 480px，则宽度 = 480 x 9 / 16 = 853
		    scale=853:480 当中的宽度和高度实际应用场景中通常只需指定一个，比如指定高度为 480 或者 720，至于宽度则可以传入 “-1” 表示由原始视频的宽高比自动计算而得。
		    即参数可以写为：scale=-1:480，当然也可以 scale=480:-1
		-acodec aac 指定音频使用 aac 编码。注：因为 ffmpeg 的内置 aac 编码目前还是试验阶段，故会提示添加参数 “-strict -2” 才能继续，尽管添加即可。又或者使用外部的 libfaac（需要重新编译 ffmpeg）。
		-vcodec h264 指定视频使用 h264 编码。注：手机一般视频拍摄的格式（封装格式、文件格式）为 mov 或者 mp4，这两者的音频编码都是 aac，视频都是 h264。
		out.mp4 指定输出文件名上面的参数 

	2.为视频添加logo
	        
		ffmpeg -i input.mp4 -i logo.png -filter_complex overlay output.mp4   		左上角
		ffmpeg -i input.mp4 -i logo.png -filter_complex overlay=W-w output.mp4 		右上角
		ffmpeg -i input.mp4 -i logo.png -filter_complex overlay=0:H-h output.mp4 	左下角
		ffmpeg -i input.mp4 -i logo.png -filter_complex overlay=W-w:H-h output.mp4	右下角
		ffmpeg -i input.mp4 -i logo.png -filter_complex [1:v]scale=100：200[s];[0:v][s]overlay=0:0 output.mp4   		调整水印大小
		ffmpeg -i input.mp4 -i logo1.png -i logo2.png -filter_complex [1:v]scale=100:100[img1];[2:v]scale=1280:720[img2];[0:v][img1]overlay=(main_w-overlay_w)/2:(main_h-overlay_h)/2[bkg];[bkg][img2]overlay=0:0 output.mp4   		2张水印图片
		ffmpeg -i input.mp4 -ignore_loop 0 -i giflogo.gif -i logo2.png -filter_complex [1:v]scale=100:100[img1];[2:v]scale=1280:720[img2];[0:v][img1]overlay=(main_w-overlay_w)/2:(main_h-overlay_h)/2[bkg];[bkg][img2]overlay=0:0 output.mp4   		2张水印图片
		// ignore_loop参数。这个参数的值为1则忽略gif文件本身的循环设置，为0的话则使用文件本身的设置
		
	3.去掉视频的logo
	
		ffmpeg -i input.mp4 -vf delogo=0:0:220:90:100:1 output.mp4 
		-vf delogo=x:y:w:h[:t[:show]]
		    x:y     离左上角的坐标 
		    w:h     logo的宽和高 
		    t:      矩形边缘的厚度默认值4 
		    show：   若设置为1有一个绿色的矩形，默认值0。
	
	4.文字水印
	    ffmpeg -i input.flv -vf "drawtext=fontfile=/home/huyu/font/simhei.ttf:text='huyu':x=100:y=10:fontsize=24:fontcolor=yellow:shadowy=2" output.mp4
	     	 	
	
27. ffmpeg裁剪
    有时可能只需要视频的正中一块，而两头的内容不需要，这时可以对视频进行裁剪（crop），
    比如有一个竖向的视频 1080 x 1920，如果指向保留中间 1080×1080 部分命令如下：
                
        ffmpeg -i 视频源地址 -strict -2 -vf crop=1080:1080:0:420 视频输出地址（如：out.mp4）
        crop=1080:1080:0:420 裁剪参数
	    crop=width:height:x:y，其中 width 和 height 表示裁剪后的尺寸，x:y 表示裁剪区域的左上角坐标。
	
    比如当前这个示例，我们只需要保留竖向视频的中间部分，所以 x 不用偏移，故传入0，而 y 则需要向下偏移：(1920 - 1080) / 2 = 420
	
	
#### 第四组,直播相关

1.  将文件当做直播送至live (RTMP + FLV)
        
	    ffmpeg -re -i demo.mp4 -c copy -f flv rtmp://server/live/streamName

2.  将直播媒体保存至本地文件
        
	    ffmpeg -i rtmp://server/live/streamName -c copy dump.flv

3.  将其中一个直播流，视频改用h264压缩，音频不变，送至另外一个直播服务流
        
	    ffmpeg -i rtmp://server/live/originalStream -c:a copy -c:v libx264 -vpre slow -f flv rtmp://server/live/h264Stream

4.  将其中一个直播流，视频改用h264压缩，音频改用faac压缩，送至另外一个直播服务流
        
	    ffmpeg -i rtmp://server/live/originalStream -c:a libfaac -ar 44100 -ab 48k -c:v libx264 -vpre slow -vpre baseline -f flv rtmp://server/live/h264Stream

5.  将其中一个直播流，视频不变，音频改用faac压缩，送至另外一个直播服务流
        
	    ffmpeg -i rtmp://server/live/originalStream -acodec libfaac -ar 44100 -ab 48k -vcodec copy -f flv rtmp://server/live/h264_AAC_Stream

6.  将一个高清流，复制为几个不同视频清晰度的流重新发布，其中音频不变
        
	    ffmpeg -re -i rtmp://server/live/high_FMLE_stream -acodec copy -vcodec x264lib -s 640×360 -b 500k -vpre medium -vpre baseline rtmp://server/live/baseline_500k -acodec copy -vcodec x264lib -s 480×272 -b 300k -vpre medium -vpre baseline rtmp://server/live/baseline_300k -acodec copy -vcodec x264lib -s 320×200 -b 150k -vpre medium -vpre baseline rtmp://server/live/baseline_150k -acodec libfaac -vn -ab 48k rtmp://server/live/audio_only_AAC_48k

7.  功能一样，只是采用-x264 opts选项
        
	    ffmpeg -re -i rtmp://server/live/high_FMLE_stream -c:a copy -c:v x264lib -s 640×360 -x264opts bitrate=500:profile=baseline:preset=slow rtmp://server/live/baseline_500k -c:a copy -c:v x264lib -s 480×272 -x264opts bitrate=300:profile=baseline:preset=slow rtmp://server/live/baseline_300k -c:a copy -c:v x264lib -s 320×200 -x264opts bitrate=150:profile=baseline:preset=slow rtmp://server/live/baseline_150k -c:a libfaac -vn -b:a 48k rtmp://server/live/audio_only_AAC_48k

8.  将当前摄像头及音频通过 DSSHOW 采集，视频 h264、音频 faac 压缩后发布
        
	    ffmpeg -r 25 -f dshow -s 640×480 -i video="source name":audio="audio source name" -vcodec libx264 -b 600k -vpre slow -acodec libfaac -ab 128k -f flv rtmp://server/application/stream_name

9.  将一个JPG图片经过 h264 压缩循环输出为 mp4 视频
        
	    ffmpeg -i INPUT.jpg -an -vcodec libx264 -coder 1 -flags +loop -cmp +chroma -subq 10 -qcomp 0.6 -qmin 10 -qmax 51 -qdiff 4 -flags2 +dct8x8 -trellis 2 -partitions +parti8x8+parti4x4 -crf 24 -threads 0 -r 25 -g 25 -y OUTPUT.mp4

10. 将普通流视频改用 h264 压缩，音频不变，送至高清流服务(新版本 FMS live=1)
        
	    ffmpeg -i rtmp://server/live/originalStream -c:a copy -c:v libx264 -vpre slow -f flv “rtmp://server/live/h264Stream live=1〃


	
	
	
	
	
	
	
	
	
	