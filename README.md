# EtherCAT-Slave
基于STM32F407+LAN9252的EtherCAT从站，实现8路数字I\O,2路差分模拟量采集

#### STM32与LAN9252之间采用SPI通讯
#### 差分模拟量采集使用STM32的双重ADC模式
#### 支持串口数据回传以及OLED屏幕实时显示

*EtherCAT从站代码部分采用SSC（Slave Stack Code Tool）生成，STM32部分驱动代码参考正点原子开发板例程*
