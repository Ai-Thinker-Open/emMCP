[![中文](https://img.shields.io/badge/Language-简体中文-green.svg)](README_zh.md)
<h1 align="center">Wiring Instructions </h1>
<div align="center">

```mermaid
flowchart LR
 subgraph xiaoAnAi["AI Module"]
        A1("5V")
        B1("RXD")
        C1("TXD")
        D1("GND")
  end

 subgraph stm32["STM32F10x board"]
        A2("5V")
        B2("PA3")
        C2("PA2")
        D2("GND")
  end
    A1 <==> A2
    B1 e1@<--> |baud rate:115200|B2
    C1 e2@<--> |baud rate:115200|C2
    D1 <==> D2

e1@{ animation: fast }
e2@{ animation: fast }
```