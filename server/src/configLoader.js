// 导入Dotenv模块，用来读取.env文件中的环境变量
const dotenv = require('dotenv');
const NodeCache = require('node-cache');
const myCache = new NodeCache();

// 读取.env文件中的环境变量
dotenv.config();


function getUrl(pathname,query={}){
    const searchParams = new URLSearchParams();
    for(let key in query){
        searchParams.append(key,query[key]);
    }
    const search = searchParams.toString();
    const url = `${process.env.SERVER_URL}${pathname}?${search}`;
    return url
}

// 获得设备信息
async function getDeviceForce(sn){

    const url = getUrl('/devices/' + sn);
    const data = await  fetch(url)
    // 如果设备未激活 或者没有user
    if(data.activatedAt==null || data.userId==null){
        throw new Error('设备未激活或未绑定用户')
    }
    return data
}
async function getDevice(sn){
    const cacheData = myCache.get(id)
    if(cacheData){
        getDeviceForce(sn).then((newCache)=>{
            console.log("缓存刷新")
            myCache.set(id,newCache,3600)
        })
        return cacheData
    }
    return getDeviceForce(sn).then((newCache)=>{
        console.log("缓存刷新")
        myCache.set(id,newCache,3600)
        return newCache
    })
}
  async function getDeviceConfig(sn){
    const data = await getDevice(sn)
    const human = data.digitalHuman
    return {
        llm_server: "coze",
        llm_config: { 
              botId:process.env.COZE_BOTID,
              keyType:process.env.COZE_KEYTYPE,  // 配置授权模式，默认是pat模式
              appId:process.env.COZE_APPID,
              apiToken:process.env.COZE_APITOKEN,
              privateKey:process.env.COZE_PRIVATEKEY,
          },
        llm_init_messages:[
            {
                "role": "user", 
                "content": `
--                
AGE: ${human.age}’
Name: ${human.name}
personality: ${human.personality}
speakingStyle: ${human.speakingStyle}
tags: ${human.tags}
introduction: ${human.tags}
--
Date:${new Date().toISOString()}
--
场景：
现在正在一个对话的场景，每次回复都要简洁明了，不要多余的废话。
                `
            }
        ],
    }

}

module.exports = {
    getDeviceConfig
}