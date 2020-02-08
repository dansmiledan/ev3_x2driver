#include "x2device.h"

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#define ANGLE_MAX (360 * 64)

static int find_pac_start(int fd)
{
    uint8_t start;
    ssize_t size;
    while (1) {
        size = read(fd, &start, 1);
        if (size < 0) {
            return size;
        }
        while (start != 0xAA) {
            size = read(fd, &start, 1);
            if (size < 0) {
                return size;
            }
        }
        size = read(fd, &start, 1);
        if (size < 0) {
            return size;
        }
        if (start == 0x55) {
            break;
        }
    }
    return 0;
}

static int get_head(int fd, x2data *data)
{
    ssize_t size = 0;
    uint8_t tmp[8];
    uint16_t *p = (uint16_t*)tmp;
    uint8_t *s = tmp;
    size_t rest = 8;
    while (size < rest) {
        rest = rest - size;
        size = read(fd, s, rest);
        if (size < 0) {
            return size;
        }
        s += size;
    }
    data->length = tmp[1];
    data->start_angle = *(p + 1) >> 1;
    data->end_angle = *(p + 2) >> 1;
    data->checkcode = *(p + 3);
    data->checkcode ^= *(p + 1);
    data->checkcode ^= *(p + 2);
    data->checkcode ^= *p;
    return 0;
}

static int get_data(int fd, x2data *data)
{
    uint8_t tmp[SAMPLE_MAX] ={ 0 };
    uint16_t *p = (uint16_t*)tmp;
    uint8_t *s = tmp;
    size_t rest = data->length * 2;
    ssize_t size = 0;
    while (size < rest) {
        rest = rest - size;
        size = read(fd, s, rest);
        if (size < 0) {
            return size;
        }
        s += size;
    }
    for (int i = 0; i < data->length; i++, p++) {
        data->distance[i] = *p;
        data->checkcode ^= *p;
    }
    return 0;
}

void cal_data(x2data *data)
{
    float diff;
    if (data->start_angle > data->end_angle) {
        diff = (float)((data->end_angle + ANGLE_MAX - data->start_angle) / ((data->length - 1) * 1.0));
    } else {
        diff = (float)((data->end_angle - data->start_angle) / ((data->length - 1) * 1.0));
    }
    for (int i = 0; i < data->length; i++) {
        if (data->distance[i] == 0) {
            data->angles[i] = 0;
            continue;
        }
        int32_t correct_angle = (int32_t)(atan(21.8 * (155.3 - data->distance[i] / 4.0) / (155.3 * data->distance[i] / 4.0)) * 180.0 / 3.1415 * 64.0);
        if (data->start_angle + correct_angle + diff * i < 0) {
            data->angles[i] = (uint16_t)(data->start_angle + correct_angle + diff * i + ANGLE_MAX);
        } else if (data->start_angle + correct_angle + diff * i > ANGLE_MAX) {
            data->angles[i] = (uint16_t)(data->start_angle + correct_angle + diff * i - ANGLE_MAX);
        } else {
            data->angles[i] = (uint16_t)(data->start_angle + correct_angle + diff * i);
        }
    }
}

int rcv_data(x2device *device, x2data *data)
{
    if (device == NULL || data == NULL) {
        return -1;
    }
    int ret;
    int fd = device->fd;

    ret = find_pac_start(fd);
    if (ret < 0) {
        return ret;
    }
    ret = get_head(fd, data);
    if (ret < 0) {
        return ret;
    }
    ret = get_data(fd, data);
    if (ret < 0) {
        return ret;
    }
    cal_data(data);
    return 0;
}

int init_device(x2device *device)
{
    if (device == NULL) {
        return -1;
    }
    int fd = open("/dev/tty_ev3-ports:in1", O_RDONLY);
    if (fd < 0) {
        return fd;
    }
    device->fd = fd;
    return 0;
}
