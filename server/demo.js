const { CozeAPI, ChatEventType, RoleType, COZE_CN_BASE_URL, getJWTToken } = require('@coze/api');
const COZE_JWTTOKEN = {}

const cozeConfig = { 
    /// apiToken:"pat_Lzq7jZ9HtGg8rDle7QOuaq4ch3pyk74ADoOoP2bb4JAejNeFR221rMMLKr3NI4vb",
      botId:"7450890984499855371",
      keyType:"oauth",
      appId:"1178539371996",
      apiToken:"1BKBTDoNbtO-MU3vBwaSd0Ulp-09-BaGGWoLg61P6aw",
      // 下载的pem
      privateKey:`-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQClS66dwjk7PZ+N
sEFgAf7sYqpMIt7uZw9VzA0XhgpaXFd/smCH6QyeNTYTESrGEH38t5O3jbh8hVxM
jifz7x+Qvfau/5pLJOM4gOv8T0xFMn+8Fo9uZmkyyQoWvcOq7/tM0/JSQw1YPNmt
z+7cQHqhmwhbYR6HLqtdQKNkwQA3J+osyjgJNWvbP2+mf9v5xsomzSupT5+HiJ/Z
K9rxOFu6CC4RJ9LMFiuyv3czdPgNRqbk+wYYXyuuNhMTsNtx4fSeW89dGHWDbCgd
t+uUowB/oon0i9ZLRlYpGmf4nRPvNOPHQj6cgkJHHxxVERn+mDBg8DcrtJw5ikbm
L4sfhCy9AgMBAAECggEAGr3INaCG1sUs62TA5tHb1o1NxLx9eW815Wj6Ezytrxmy
VCfdAw/aaTvtkB8eXuWrJDfkz3S/jHGc0YvL/eRJJIb/Aie64/5HPkxpISwJCEbX
sGur763kxgENP0ma/qwNaUv+r31GiuhpRPvXRk2uKTfmHbZ0FGWh9peu921b5hAt
42TVzwBbcrZmnXX0hKqGrTCm8rFms/Mq4B1fCKNlNGhz+YYsmmkX8YTVfcYR/UcS
Fzv7nKe0yIA5umcqbMqwhwb6Rmdfk3Uat4dWivQw1eqpepQojfC+ZxCvHpjsRP8S
T05KhtuFhH/ZoN/rfoCE9LbJj/FSUxPOiJtdgmDjkQKBgQDZO+vzUBt1OPxgejxJ
1/C1BMmtiiiDLYlA0Q1Wx33QAY0fZyIwJ7ER++3mpvWqofKCqO4jSts7CONYMpxA
8tNCjgCNEr2EztW4V3+NmIWKfwTORc+Ybm2Z97rGRz4PLnXTkI8ggDAhhVo4CbaH
ELLz5/W9mohexEKcQuZmQ7nOlQKBgQDCywPhwXCJlIPPFSrVo60fujekoAHt+N1F
MIIcoMEhEFE6fQL9BAqkz+xH9zGSkBafZGIZJoHRFTx2aYo4Gv8plvdTl+f8Nve2
r4jOKBcLG9ofeBRIucS49kamHeD+4zevg9Exzz99p75kxArsA9rwdStvTL97wVvD
yYWsXi1jiQKBgBNyfNYEZuDgoB9W8gy+Sys5aYCirKvJqM7nqCULqOS1LtgP2Iyt
EPoGLq8GZVnIM7ikvOTl76hiTE/5/+kElAsSQVRfyB2akqF1G9MmQ1Y5tslskIAZ
6q/s8IiNmi2rwBqXh2imcSifEouPuwpuYUbajjJSIkExmrD4gD3JdFndAoGBAIc9
plNOF1Ed8KmM27F6Mw+NHWbJxK0xFIceF6fCoHPRPWQrOTDrZmxpA3YvlJZaPaqq
akVl68l7QgBAZDtx2uhILS03kVQV4cRPeN8RDHak/rrMhuC7QW7dZc2dfwHYRWgx
T8OSTVVzagfR6HdWceJypxZeJa/JXmm/d3Rl6oNpAoGBAKgkjcUSxCCn0kBZtd2v
YI/bp5vafBglsX8cfzk3jCq2Pd1m2hureBZzyypmwqwIafVUuYaUryGDg8t4mIJ7
n3toutynJe2/l+oajGmzO05WTF8TzdTbrwx8y8rxjd4h1VAwqgKZENpI+1sBY1jh
sg4+xeG4rZFyvFij4gesT/1x
-----END PRIVATE KEY-----`,
  }
const { baseURL, apiToken, botId, keyType, appId, privateKey } = cozeConfig;


async function main() {
    let token = apiToken;
    // await getJWTToken({
    //     baseURL: baseURL ?? COZE_CN_BASE_URL,
    //     appId,
    //     aud: "api.coze.cn",
    //     keyid: apiToken,
    //     privateKey,
    //     sessionName: 'test', // optional Isolate different sub-resources under the same jwt account
    // }).then(jwtToken => {
    //     COZE_JWTTOKEN[apiToken] = jwtToken;
    //     console.log("COZE初始化成功",jwtToken);
    // }).catch(error => {
    //     console.error("COZE初始化失败",error);
    // });
    if (keyType === 'oauth') {
        token = async () => {
            try {
                
                let jwtToken = COZE_JWTTOKEN[apiToken]
                if (jwtToken && jwtToken.expires_in * 1000 > Date.now() + 5000) {
                    return jwtToken.access_token;
                }
                const baseUrl = baseURL ?? COZE_CN_BASE_URL;
                jwtToken = await getJWTToken({
                    baseURL: baseUrl,
                    appId,
                    aud: new URL(baseUrl).host,
                    keyid: apiToken,
                    privateKey,
                    sessionName: 'test', // optional Isolate different sub-resources under the same jwt account
                });
                COZE_JWTTOKEN[apiToken] = jwtToken;
                return jwtToken.access_token;
            
            } catch (error) {
            }
            return null;
        }
    }

    const client = new CozeAPI({
        baseURL: baseURL ?? COZE_CN_BASE_URL,
        token,
    });

    const stream = await client.chat.stream({
        bot_id: botId,
        additional_messages: [
            {
                role: RoleType.User,
                content: "你好啊",
                content_type: 'text',
            }
        ]
    });

    for await (const event of stream) {
        if (event.event === ChatEventType.CONVERSATION_MESSAGE_DELTA) {
            console.log(event.data.content);
        }
    }
}
main()