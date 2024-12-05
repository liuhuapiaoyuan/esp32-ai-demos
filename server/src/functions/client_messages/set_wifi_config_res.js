/**
 * Copyright (c) 2024 小明IO
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Commercial use of this software requires prior written authorization from the Licensor.
 * 请注意：将 ESP-AI 代码用于商业用途需要事先获得许可方的授权。
 * 删除与修改版权属于侵权行为，请尊重作者版权，避免产生不必要的纠纷。
 * 
 * @author 小明IO   
 * @email  1746809408@qq.com
 * @github https://github.com/wangzongming/esp-ai
 * @websit https://espai.fun
 */

/**
 * 客户端连接成功
*/
const { t_info, error } = require("../../utils/log");

async function fn({ device_id, success }) {
    try {
        if (!G_devices.get(device_id)) return;
        const { set_wifi_config_end } = G_devices.get(device_id);
        t_info(`${device_id} 返回连接结果：${success}`)
        set_wifi_config_end(success);
    } catch (err) {
        console.log(err);
        error(`[${device_id}] set_wifi_config_res 错误： ${err}`)
    }

}

module.exports = fn