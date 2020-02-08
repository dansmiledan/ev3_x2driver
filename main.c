#include "x2device.h"

#include <stdio.h>

int main()
{
    x2device device = { 0 };
    x2data data = { 0 };
    int ret = init_device(&device);
    while (rcv_data(&device, &data) == 0) {
        if (data.checkcode == CHECK_SUM) {
            for (int i = 0; i < data.length; i++) {
                if (data.distance[i] != 0) {
                    printf("angle: %lf    dis:%lf\n", data.angles[i] / 64.0, data.distance[i] / 4.0);
                }
            }
        }
    }
}



