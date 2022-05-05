import React, { useEffect, useState } from 'react'
import Box from '@mui/material/Box';
import Typography from '@mui/material/Typography';
import useMqtt from '../hooks/useMqtt'
import Button from '@mui/material/Button';
import { useDataContext } from '../contexts/data'

import { ENERGY_MODE } from '../models/esp32'

const REGISTER = process.env.NEXT_PUBLIC_REGISTER
let pingDate = Date.now()

const Device = ({device}) => {
  const { alarm, onAlarmDetected, onAlarmStop } = useDataContext()
  const [temperature, setTemp] = useState(0)
  const [humidity, setHumidity] = useState(0)
  const [ledState, setLedState] = useState(false)
  const [buttonState, setBtnState] = useState(false)
  const [connected, setConnected] = useState(true)

  const {
    mqttConnect,
    mqttSubscribe,
    mqttSend,
    payload
   } = useMqtt(`fse2021/${REGISTER}/${device.room}/#`)

   useEffect(() => {
    mqttConnect()
   }, [])

   useEffect(() => {
     if(payload){
      console.log(payload)
      if(payload.topic.match(/.*\/temperatura/)){
        setTemp(payload.message.value)
      }
      if(payload.topic.match(/.*\/umidade/)){
        setHumidity(payload.message.value)
      }
      if(payload.topic.match(/.*\/estado/)){
        setBtnState(payload.message.value)
      }
      if(payload.topic.match(/.*\/status/)){
        pingDate = Date.now()
      }
    }
   }, [payload])

   useEffect(() => {
    if(buttonState && alarm){
      onAlarmDetected()
      console.log('aaaaaaaaaaaa')
    } else {
      onAlarmStop()
    }

   }, [buttonState, alarm])

  const changeState = () => {
    const output = ledState ? 0 : 1
    mqttSend(`fse2021/${REGISTER}/dispositivos/${device.id}`, { output })
    setLedState(!ledState)
  }
  useEffect(() => {
    const timerId = setInterval(() => {
      checkStatus()
    }, 10000);
    return () => clearInterval(timerId)
  }, [])


  const checkStatus = () => {
    const now = Date.now()
    var difference = (now - pingDate) / 1000;
    setConnected(!(difference > 30))
  }

  return (
    <Box sx={styles.container}>
      <div style={{display: 'flex'}}>
        <Typography id="modal-modal-title" variant="h5" component="h2">
          {device.room}
        </Typography>
        {device.mode === ENERGY_MODE &&
          <div
            style={{
              width: 10,
              height: 10,
              borderRadius: '50%',
              backgroundColor: connected? 'green' : 'red',
              alignSelf: 'center',
              marginLeft: 5
            }}/>
        }
      </div>
      <Typography>
        <b>Modelo:</b> {device.getMode()}
      </Typography>
      <Typography>
        <b>ID:</b> {device.id}
      </Typography>
      {device.mode === ENERGY_MODE &&
        <>
          <Typography>
            <b>Temperatura:</b> {temperature.toFixed(2)} ºC
          </Typography>
          <Typography>
            <b>Umidade:</b> {humidity.toFixed(2)}%
          </Typography>
          <Typography>
            <b>Estado da saída:</b> <b style={{color: ledState ? 'green' : 'red'}}>
              {ledState? 'ON' : 'OFF'}
            </b>
          </Typography>
        </>
      }


      <Typography>
        <b>Estado da entrada:</b> <b style={{color: buttonState ? 'green' : 'red'}}>
          {buttonState? 'ON' : 'OFF'}
        </b>
      </Typography>
      {device.mode === ENERGY_MODE &&
        <Button
          sx={{ mt: 3, width: '100%' }}
          variant="contained"
          onClick={changeState}
        >
          {ledState ? 'desligar' : 'ligar'}
        </Button>
      }
    </Box>
  )
}

const styles = {
  container: {
    // width: '50vw',
    border: '1px solid #000',
    borderRadius: 2,
    p: 4,
  }
};

export default Device