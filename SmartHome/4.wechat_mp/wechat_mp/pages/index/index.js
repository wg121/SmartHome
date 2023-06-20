// index.js
// 获取应用实例
const app = getApp()

const { connect } = require('../../utils/mqtt')

const mqttHost = 'broker.emqx.io'   //mqtt服务器域名
const mqttPort = 8084   //mqtt服务器端口

const devSubTopic = '/mysmarthome121/sub' //设备端订阅的topic(小程序发布命令的topic)
const devPubTopic = '/mysmarthome121/pub' //设备端发布的topic(小程序订阅数据的topic)

const mpSubTopic = devPubTopic
const mpPubTopic = devSubTopic

Page({
  data: {
    client:null,
    Temp:0,
    Hum:0,
    Light:0,
    Led:false,
    Beep:false
  },
  onLedChange(event){
    const that = this
    console.log(event.detail.value);
    const sw = event.detail.value
    that.setData({Led:sw})

    if(sw){ //开灯
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target:"LED",
        value:"1"
      }), function (err) {
        if(!err){
          console.log('成功下发指令--开灯');
        }
      })
    }else{  //关灯
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target:"LED",
        value:"0"
      }), function (err) {
        if(!err){
          console.log('成功下发指令--关灯');
        }
      })
    }

  },

  onBeepChange(event){
    const that = this
    console.log(event.detail.value);
    const sw = event.detail.value
    that.setData({Beep:sw})

    if(sw){ //开灯
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target:"BEEP",
        value:"1"
      }), function (err) {
        if(!err){
          console.log('成功下发指令--报警');
        }
      })
    }else{  //关灯
      that.data.client.publish(mpPubTopic, JSON.stringify({
        target:"BEEP",
        value:"0"
      }), function (err) {
        if(!err){
          console.log('成功下发指令--不报警');
        }
      })
    }

  },
  //
  onShow() {
    const that = this
    that.setData({
      client:connect(`wxs://${mqttHost}:${mqttPort}/mqtt`)
    })

    that.data.client.on('connect',function (params) {
      console.log('成功连接到MQTT服务器')
      wx.showToast({
        title: '连接成功',
        icon:'success',
        mask : true
      })

      that.data.client.subscribe(mpSubTopic, function (err) {
        if(!err){
          console.log('成功订阅设备上行数据Topic！')
        }
      })
    })

    that.data.client.on('message', function (topic, message) {
      console.log(topic);
      //console.log(message);
      //message 是16进制的 buffer 字节流
      let dataFromDev = {}
      try {
        dataFromDev = JSON.parse(message)
        console.log(dataFromDev);

        that.setData({
          Temp:dataFromDev.Temp,
          Hum:dataFromDev.Hum,
          Light:dataFromDev.Light,
          Led:dataFromDev.Led,
          Beep:dataFromDev.Beep
        })

      } catch (error) {
        console.log('JSON解析失败', error);
      }


    })

  },
})
