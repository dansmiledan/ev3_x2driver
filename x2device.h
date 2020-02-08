#include <stdint.h>

#define PATH_MAX 255
#define SAMPLE_MAX 257
#define CHECK_SUM 0x55aa

typedef struct {
    uint16_t length;
    uint16_t checkcode;
    uint16_t start_angle;
    uint16_t end_angle;
    uint16_t angles[SAMPLE_MAX];
    uint16_t distance[SAMPLE_MAX];
} x2data;

typedef struct {
    int fd;
} x2device;

int init_device(x2device *device);
int rcv_data(x2device *device, x2data *data);