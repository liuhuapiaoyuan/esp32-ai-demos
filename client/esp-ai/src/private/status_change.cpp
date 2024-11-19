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
#include "status_change.h"

void ESP_AI::status_change(String status)
{
    // Serial.print("===================收到状态：");
    // Serial.println(status);
    if (status == "iat_start")
    {
        esp_ai_pixels.clear();
        esp_ai_pixels.setPixelColor(0, esp_ai_pixels.Color(18, 170, 156));
        esp_ai_pixels.setBrightness(100); // 亮度设置
        esp_ai_pixels.show();
    }
    else if (status == "tts_chunk_start")
    {
        esp_ai_pixels.setPixelColor(0, esp_ai_pixels.Color(241, 147, 156));
        esp_ai_pixels.setBrightness(100); // 亮度设置
        esp_ai_pixels.show();
    }
    else if (status == "iat_end" || status == "tts_real_end")
    {
        esp_ai_pixels.clear();
        esp_ai_pixels.show();
    }
    else if (status == "0_ing")
    {
        esp_ai_pixels.setPixelColor(0, esp_ai_pixels.Color(238, 39, 70));
        esp_ai_pixels.setBrightness(50); // 亮度设置
        esp_ai_pixels.show();
    }
    else if (status == "0_ing_after")
    {
        esp_ai_pixels.clear();
        esp_ai_pixels.show();
    }
    else if (status == "0_ap")
    {
        esp_ai_pixels.setPixelColor(0, esp_ai_pixels.Color(241, 202, 23));
        esp_ai_pixels.setBrightness(50); // 亮度设置
        esp_ai_pixels.show();
    }
    else if (status == "2")
    {
        esp_ai_pixels.setPixelColor(0, esp_ai_pixels.Color(238, 39, 70));
        esp_ai_pixels.setBrightness(50); // 亮度设置
        esp_ai_pixels.show();
    }
    else if (status == "3")
    {
        esp_ai_pixels.clear();
        esp_ai_pixels.show();
    }
}