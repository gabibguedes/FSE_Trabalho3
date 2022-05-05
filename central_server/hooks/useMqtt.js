import MQTT from 'mqtt'
import { useEffect, useState } from 'react'

const MQTT_HOST = process.env.NEXT_PUBLIC_MQTT_URI
const CLIENT_ID = process.env.NEXT_PUBLIC_MQTT_CLIENTID

function useMqtt(initialSubscription) {
  const [client, setClient] = useState(null)
  const [connectStatus, setConnectStatus] = useState('Disconected')
  const [payload, setPayload] = useState(null)
  const [isSub, setIsSub] = useState(false)

  const mqttConnect = () => {
    setConnectStatus('Connecting')
    const mqttClient = MQTT.connect(MQTT_HOST, {
      clientId: CLIENT_ID
    })
    setClient(mqttClient)
  };

  useEffect(() => {
    if (client) {
      client.on('connect', () => {
        setConnectStatus('Connected');
        mqttSubscribe(initialSubscription)
      });
      client.on('error', (err) => {
        console.error('Connection error: ', err);
        client.end();
      });
      client.on('reconnect', () => {
        setConnectStatus('Reconnecting');
      });
      client.on('message', (topic, message) => {
        const payload = { topic, message: JSON.parse(message.toString()) };
        setPayload(payload);
      });
    }
  }, [client]);

  const mqttSubscribe = (topic) => {
    if (client) {
      client.subscribe( topic, 0, (error) => {
        if (error) {
          console.log('Subscribe to topics error', error)
          return
        }
        setIsSub(true)
      });
    }
  };

  const mqttSend = (topic, payload) => {
    client.publish(topic, JSON.stringify(payload), 0, error => {
      if (error) {
        console.log('Publish error: ', error);
      }
    });
  }

  return {
    connectStatus,
    mqttConnect,
    mqttSubscribe,
    mqttSend,
    payload
  }
}

export default useMqtt
