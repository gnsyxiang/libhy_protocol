# 协议文档

---

[TOC]

## 协议分类

---

### 按编码方式

---

* 二进制协议
* 明文的文本协议
* 混合协议（二进制+明文）

### 按协议边界

---

* 固定边界协议：能够明确得知一个协议报文的长度，这样的协议易于解析，比如tcp协议。
* 模糊边界协议：无法明确得知一个协议报文的长度，这样的协议解析较为复杂，通常需要通过某些特定的字节来界定报文是否结束，比如http协议。

## 协议设计

---

本协议采用固定边界+混合编码策略。

| 协议头 | 协议体 |
|--------|--------|

### 协议头

---

| magic  | check sum | cmd      | len      |
|--------|-----------|----------|----------|
| 0x55aa | hy_u16_t  | hy_u32_t | hy_u32_t |


```c
typedef struct {
    hy_u16_t    magic;          ///< 魔素识别码
    hy_u16_t    check_sum;      ///< 除magic和check_sum以外的所有字段
    hy_u32_t    cmd;            ///< 标识后面data的命令
    hy_u32_t    len;            ///< 标识后面data的长度
    char        *data[0];       ///< data数据，没有长度，方便程序处理用
} ProtocolMsgHead_s;
```

有需要可以把`cmd`进一步细分

| magic  | check sum | cmd_type | cmd      | len      |
|--------|-----------|----------|----------|----------|
| 0x55aa | hy_u16_t  | hy_u16_t | hy_u16_t | hy_u32_t |


```c
typedef struct {
    hy_u16_t    magic;          ///< 魔素识别码
    hy_u16_t    check_sum;      ///< 除magic和check_sum以外的所有字段
    hy_u16_t    cmd_type;       ///< 标识后面data的命令
    hy_u16_t    cmd;            ///< 标识后面data的命令的参数
    hy_u32_t    len;            ///< 标识后面data的长度
    char        *data[0];       ///< data数据，没有长度，方便程序处理用
} ProtocolMsgHead_s;
```

### 协议体

---

* 结构体

* json数据

* protobuf

