# nfc-errrrror-v2  

一个基于 NT3H1101 的 NFC Tag  
支持接到 OpenWrt 路由器的 USB 上，路由器每次启动时生成随机的访客网络密码，并自动同步配置到 NT3H1101 的 EEPROM 中，使用支持 NFC 的手机接近即可自动联网  

普通地填了个坑，看上去填完了，但还没填完  

![blink](https://github.com/libc0607/nfc-errrrror-v2/blob/main/pic/blink.gif)  

## 硬件 
尺寸为 54mm * 54mm，包含一个 USB Type-C 接口，支持正反插，带两个 5.1k 下拉电阻；  
NFC 板载天线；  
背面带指示灯，在 USB 供电时可以通过代码控制，掉电时通过 NFC 取电。  

上图中的硬件设计，包含原理图和 PCB，见 [OSHWHub: libc0607/ch552-nfc-v1_copy](https://oshwhub.com/libc0607/ch552-nfc-v1_copy)  

![sch](https://github.com/libc0607/nfc-errrrror-v2/blob/main/pic/schematic01.png)  

只要原理图相同，软件即可通用。板子你也可以自己画一个，比如画个六龙盘dio带跑马灯之类的。。不过请注意 OSHWHub 上的开源协议。  
更多的硬件设计细节见 OSHWHub。  

![pcb](https://github.com/libc0607/nfc-errrrror-v2/blob/main/pic/pcblayout01.png)  


## 软件
使用 CH552 模拟 U 盘进行配置同步（基于 [DeqingSun/ch55xduino](https://github.com/DeqingSun/ch55xduino) 的示例修改），  
需先安装 Arduino 与 ch55xduino 后，自行编译烧录 [mcu-src/nfc-errrrror-v2](https://github.com/libc0607/nfc-errrrror-v2/tree/main/mcu-src/nfc-errrrror-v2) 目录下的源码。

## OpenWrt 自动配置 
该部分基于 luci-app-guest-wifi 修改，将 [luci-app-guest-wifi-ntagmod](https://github.com/libc0607/nfc-errrrror-v2/tree/main/luci-app-guest-wifi-ntagmod) 目录下的源码放进 OpenWrt 的编译环境中，并在 make 时选中，即可得到 ipk。  
该软件包理论上在各硬件平台上通用，但编译时需要选中 USB 存储相关软件包，以及需要支持 fdisk 命令，才能正常工作。不支持自动挂载 U 盘的话还需要手动挂载一下。  

路由器安装该软件包后，进行访客网络配置时，可以勾选【启用 NFC Tag】，保存启动后，路由器会自动寻找 NFC Tag，并将访客网络的配置写入该 NFC Tag 中。  
为了增强安全性，也可以勾选在每次启动后生成随机的访客网络密码（下图中没有，代码里后面加上了。。）。  

![op-web](https://github.com/libc0607/nfc-errrrror-v2/blob/main/pic/op-screenshot.png)  


## To-Do
写完这个 todo 和[一些细节](https://libc0607.wordpress.com/2021/07/06/%e3%80%90%e5%a1%ab%e5%9d%91%e3%80%91%e8%b7%9f%e9%a3%8e%e5%81%9a%e4%b8%aa-nfc-%e6%a0%87%e7%ad%be%ef%bc%8c%e5%9f%ba%e4%ba%8e-nt3h1101/)

