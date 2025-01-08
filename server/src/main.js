const espAi = require("./index.js");
const configLoaer = require("./configLoader.js");




// 详情配置项见： https://espai.fun/server.html
const config = {
    gen_client_config: async({client_params}) => {
        const llmConfig = configLoaer.getDeviceConfig(client_params.name);
        return {
            iat_server: "xun_fei",
            iat_config: {
                // 讯飞：https://console.xfyun.cn/services/iat  。打开网址后，右上角三个字段复制进来即可。
                appid: "1c9babaf",
                apiSecret: "YjE2MTU3ODIzNTRhNTdmNmQ2ZWIzZmRj",
                apiKey: "72dd6fea830643a1a4bf3cda8d54c2f7",
                // 静默时间，多少时间检测不到说话就算结束，单位 ms
                vad_eos: 2000,
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
                 voice_type: "BV406_streaming", // 超自然音色-梓梓	
             },
            ...llmConfig,
        }
    },
};

espAi(config);
