#ifndef I2C_PLUGIN_H
#define I2C_PLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin.h"

plugin_ops_t *i2c_plugin_init(void);
void i2c_plugin_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* I2C_PLUGIN_H */
