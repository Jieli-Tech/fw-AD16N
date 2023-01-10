#ifndef _RDEC_H_
#define _RDEC_H_

enum rdec_index {
    RDEC0,
};

struct rdec_device {
    enum rdec_index index;
    u8 sin_port0;  //采样信号端口0
    u8 sin_port1;  //采样信号端口1
    u8 key_event;  //键值
};


struct rdec_platform_data {
    u8 enable;
    u8 num; //rdec 数量
    const struct rdec_device *rdec;//找到赋初值地方
};

#define RDEC_PLATFORM_DATA_BEGIN(data) \
    static const struct rdec_platform_data data = {
#define RDEC_PLATFORM_DATA_END() \
    };

/***************************rdec 初始化 *************************/
int rdec_init(const struct rdec_platform_data *user_data);


#endif
