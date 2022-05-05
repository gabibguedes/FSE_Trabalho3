import React, {
  createContext,
  useContext,
  useState,
  useEffect,
  useRef
} from 'react';

const DataContext = createContext();


const DataProvider = ({ children }) => {
  const [esp32List, setEsp32List] = useState([])
  const [alarm, setAlarm] = useState(false)
  const [numberOfAlarms, setNumberOfAlarms] = useState(0)
  const [isPlayingAlarm, setIsPlayingAlarm] = useState(0)
  const [audio, setAudio] = useState(null)

  const addEsp32 = (esp32) => {
    setEsp32List([...esp32List, esp32])
  }

  const findDevice = (id) => {
    return esp32List.find(elem => elem.id === id)
  }

  const onAlarmDetected = () => {
    setNumberOfAlarms(numberOfAlarms + 1)
  }

  const onAlarmStop = () => {
    const num = numberOfAlarms > 0? numberOfAlarms - 1 : 0
    setNumberOfAlarms(num)
  }

  useEffect(() => {
     console.log('NO EFEITO', numberOfAlarms, isPlayingAlarm)
    if(numberOfAlarms > 0 && !isPlayingAlarm){
      console.log('deveria tocar')
      audio.play()
      setIsPlayingAlarm(true)
    } else if (isPlayingAlarm && numberOfAlarms === 0) {
      audio.pause()
      setIsPlayingAlarm(false)
    }

  }, [numberOfAlarms])


  return (
    <DataContext.Provider value={{
      esp32List,
      addEsp32,
      findDevice,
      alarm,
      setAlarm,
      onAlarmDetected,
      onAlarmStop,
      setAudio
    }}
    >
      { children }
    </DataContext.Provider>
  );
};

export default DataProvider;

export function useDataContext() {
  const Context = useContext(DataContext);
  return { ...Context };
}