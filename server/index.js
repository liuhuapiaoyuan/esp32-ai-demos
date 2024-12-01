const espAi = require("esp-ai");


// 详情配置项见： https://espai.fun/server.html
const config = {
    gen_client_config: () => ({
        iat_server: "xun_fei",
        // iat_server: "esp-ai-plugin-iat-example",
        iat_config: {
            // 讯飞：https://console.xfyun.cn/services/iat  。打开网址后，右上角三个字段复制进来即可。
            appid: "1c9babaf",
            apiSecret: "YjE2MTU3ODIzNTRhNTdmNmQ2ZWIzZmRj",
            apiKey: "72dd6fea830643a1a4bf3cda8d54c2f7",
            // 静默时间，多少时间检测不到说话就算结束，单位 ms
            vad_eos: 2000,
        },

        llm_server: "xun_fei",
        llm_config: {
            appid: "1c9babaf",
            apiSecret: "YjE2MTU3ODIzNTRhNTdmNmQ2ZWIzZmRj",
            apiKey: "72dd6fea830643a1a4bf3cda8d54c2f7",
            llm: "v3.5",
        },

         // 调用火山引擎TTS
         tts_server: "volcengine",
         tts_config: {
             // 1. 注册：https://console.volcengine.com/speech/app
             // 2. 音色开通： https://console.volcengine.com/speech/service/8?AppID=6359932705
             // 3. 授权： xxx
             // 服务接口认证信息
             appid: "2220419368",
             accessToken: "sINmkMDbK-25-NPDRA0kP0mWc2jSus4-",
             rate: 24000, //采样率，只支持 16k 或者 24k，大模型语音合成必须使用 24k
             // voice_type: "BV007_streaming", // 清切女声
             voice_type: "BV001_streaming", // 奶气萌娃
         },

        /**
         * 意图表：当用户唤醒 小明同学 后，小明同学可以做下面的任务
        */
        intention: [
            {
                // 关键词
                key: ["帮我开灯", "开灯", "打开灯"],
                // 向客户端发送的指令
                instruct: "device_open_001",
                message: "开啦！还有什么需要帮助的吗？"
            },
            {
                // 关键词
                key: ["帮我关灯", "关灯", "关闭灯"],
                // 向客户端发送的指令
                instruct: "device_close_001",
                message: "关啦！还有什么需要帮助的吗？"
            },
            {
                // 关键词
                key: ["退下吧", "退下"],
                // 内置的睡眠指令
                instruct: "__sleep__",
                message: "我先退下了，有需要再叫我。"
            }
        ],
    }),
};

espAi(config);
