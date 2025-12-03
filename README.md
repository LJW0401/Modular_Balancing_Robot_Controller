# Modular Balancing Robot Controller (Base on STM32F4)

<div align=center>

![Logo](./doc/pic/Logo6.png)

![language](https://img.shields.io/badge/language-C-blue.svg)
![license](https://img.shields.io/badge/license-MIT-green.svg)

</div>

## 简介

基于 深圳北理莫斯科大学 北极熊战队 的统一嵌入式代码 `StandardRobot++` 修改的`模块化轮腿控制器`。旨在基于一个轮腿底盘，模块化的更换其他组件实现不同任务模块的并网，一机多用。

## 配套套件

- [StandardRobot++ Tool](https://gitee.com/SMBU-POLARBEAR/Serial_Port_Assistant) `(后期开新仓库精简功能，更加适配本项目)`上位机调试工具，可用于可视化查看机器人状态及相关参数，便于调试。
- [ROS2_StandardRobot++](https://gitee.com/SMBU-POLARBEAR/ROS2_StandardRobotpp) `(后期开新仓库优化功能，更加适配本项目，实现下位机和上位机的高效协同)`ROS2驱动包，可用于实现上位机对机器人的控制。
- [IMU_Controller](https://github.com/LJW0401/IMU_Controller) 一个简易的体感控制器，通过头追和姿态追踪实现对本机器人的控制。

## 模块支持

### 主要模块

- **Robot cmd**\
  机器人控制模块：
  - [x] 大疆电机
  - [x] 达妙电机
  - [x] 瓴控电机
  - [x] 小米电机
  - [x] 超级电容控制板
  - [x] 板间通信(CAN)
  - [x] PWM 控制舵机
  - [x] PWM 控制气泵
- **Chassis**\
  - [ ] 平衡底盘
- **Gimbal**\
  云台模块：
  - [x] yaw-pitch 直连云台
- **Shoot**\
  发射机构模块：
  - [ ] 摩擦轮+拨弹盘
- **Mechanical arm**\
  机械臂模块：
  - [ ] 六自由度机械臂
- **Communication**\
  通信模块：
  - [x] 板间通信(4pin uart2)
  - [x] 上下位机通信
- **Music**\
  音乐模块：
  - [x] 开机声
  - [ ] 报错音
  - [ ] 裁判系统接入提示音
  - [x] 《卡农》
  - [x] 《天空之城》
  - [x] 《恭喜发财》
  - [x] 《好运来》
  - [x] 《See You Again》
  - [x] 《Unity》
  - [x] 《你》

## 开发工具

使用VSCode作为代码编辑器，Keil5作为IDE。
> 虽然keil5老，但在嵌入式调试以及环境配置方面是非常简单易上手的。**千万不要用keil5来编辑代码！！！**
\
\
> VSCode作为现代化代码编辑器 ，通过各种插件即可实现各种提高效率的开发方式，**尤其是Github Copilot**

`.vscode` 文件夹已经提供了很多非常好用的插件，在`扩展`页面输入 `@recommended` 即可获取这些插件，点击安装即可。

## 开发日志

[LOG.md](./doc/LOG.md) 记录了大家在开发过程中的各种奇思妙想和经验。多翻一翻或许能发现惊喜哦。大家有什么想法也都可以往里面写。

## 致谢

感谢各个战队的代码开源，所有本框架涉及到的主要的参考资料位于[reference.md](./doc/reference.md)

## 附录

本文的详细补充内容写在 [附录](./doc/appendix.md) 中。

## 通讯信息

- 作者 `小企鹅` 微信：18968748997 QQ：1357482552
- 交流群
  > ![qrcode](./doc/pic/qrcode_1743387096263.jpg)
