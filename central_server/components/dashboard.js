import { useState, useEffect } from 'react'
import useMqtt from '../hooks/useMqtt'
import AddEsp32Modal from '../components/add_esp32_modal'
import Device from '../components/device'
import { useDataContext } from '../contexts/data'
import Switch from '@mui/material/Switch';
import Typography from '@mui/material/Typography';

const REGISTER = process.env.NEXT_PUBLIC_REGISTER

const Dashboard = () => {
  const {esp32List, findDevice, setAlarm, alarm, setAudio} = useDataContext()
  const [open, setOpen] = useState(false)
  const [devicePayload, setDevicePayload] = useState(null)
  const {
    connectStatus,
    mqttConnect,
    mqttSend,
    payload
   } = useMqtt(`fse2021/${REGISTER}/dispositivos/#`)

  const sendReturnMessageToDevice = (room, topic) => {
    mqttSend(topic, { room })
  }

  const handleDeviceSubscription = (payload) => {
    setOpen(true)
    setDevicePayload(payload)
  }

  const isSubscriptionTopic = (payload) => {
    if(payload && payload.topic){
      console.log(payload.topic.match(/fse2021\/*.\/dispositivos\/*./))
      return payload.topic.match(/fse2021\/*.\/dispositivos\/*./)
    }
    return false
  }

  useEffect(() => {
    if(payload && isSubscriptionTopic && payload.message.id && !findDevice(payload.message.id)){
      handleDeviceSubscription(payload)
    }
  }, [payload])

  useEffect(() => {
    mqttConnect()
    setAudio(new Audio('https://www.soundhelix.com/examples/mp3/SoundHelix-Song-6.mp3'))
  }, [])

  return(
    <div>
      <AddEsp32Modal
        open={open}
        close={() => setOpen(false)}
        deviceFound={devicePayload}
        sendMessage={sendReturnMessageToDevice}
        />

      <div style={{display: 'flex', justifyContent: 'space-between'}}>
      <h1>Dashboard</h1>
      <div style={{alignSelf: 'center'}}>
        Alarme
        <Switch
          sx={{alignSelf: 'center'}}
          onChange={() => setAlarm(!alarm)}
          checked={alarm}
          inputProps={{ 'aria-label': 'controlled' }}
        />
      </div>
      </div>
      <h3>Status: {connectStatus}</h3>
      <div style={{display: 'flex'}}>
        { esp32List.map((elem) => (
          <Device device={elem} key={elem.id}/>
        ))}
      </div>
    </div>
  )
}

export default Dashboard