# xx11

blazing fast 2d game engine base on c++20 coroutine &amp; dx11

# tutorials

https://github.com/walbourn/directx-sdk-samples.git

https://github.com/denghe/xx.git

https://www.cnblogs.com/X-Jun/p/9069608.html

https://www.bilibili.com/video/BV1KC4y1Y7tc

https://directx11.tech/#/misc/Compile

# game input

https://learn.microsoft.com/zh-cn/gaming/gdk/_content/gc/input/overviews/input-overview

# dx11 enable debug ( win11 )

https://www.cnblogs.com/X-Jun/p/10189305.html#_lab2_0_2

# handle utf8 issue

1. system control panel -- language area -- manage -- non unicode program language -- choose "Beta : use Unicode UTF-8 ..... "

2. vs2022 install extension: Force Utf-8(BOM)

3. add_compile_options("/utf-8")

4. place following code into pch.h
```
#pragma execution_character_set("utf-8")
```

5. place following code into main()
```
SetConsoleOutputCP(CP_UTF8);
```
