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

30s发送一次心跳包，数据包也当做是心跳包。心跳包只发送数据头，回复也是只发送数据头。

---

* `HY_PROTOCOL_VERSION`

* 结构体

```c
typedef struct {
    char version[32];
} HyProtocolVersion_s;
```

协议使用请求-回复的形式，完成信息的交换

消息不应该阻塞，异步调用

每个命令字都对应一个结构体，方便后期维护，不要使用cmd_on, cmd_off命令字，后期扩展性不好

如果需要传递参数，则相应命令字对应一个结构体(把相关数据封装在结构体内，方便后期维护)

协议内用同步实现

* json数据

* protobuf


