import React, {useState} from 'react';
import Box from '@mui/material/Box';
import Button from '@mui/material/Button';
import Typography from '@mui/material/Typography';
import TextField from '@mui/material/TextField';
import Modal from '@mui/material/Modal';
import { useDataContext } from '../contexts/data'
import { Esp32Device, ENERGY_MODE, BATERY_MODE } from '../models/esp32'
import Switch from '@mui/material/Switch';

const style = {
  position: 'absolute',
  top: '50%',
  left: '50%',
  transform: 'translate(-50%, -50%)',
  width: 400,
  bgcolor: 'background.paper',
  border: '1px solid #000',
  borderRadius: 2,
  boxShadow: 24,
  p: 4,
};

const AddEsp32Modal = ({ open, close, deviceFound, sendMessage }) => {
  const [roomName, setRoomName] = useState('')
  const [input, setInput] = useState('')
  const [output, setOutput] = useState('')
  const [shouldAlarm, setShouldAlarm] = useState(true)
  const [error, setError] = useState(false)
  const { addEsp32 } = useDataContext()
  const device = new Esp32Device(deviceFound?.message?.id, deviceFound?.message?.mode)

  const saveEsp32 = () => {
    if(roomName){
      device.room = roomName
      device.shouldAlarm = shouldAlarm
      device.input = input
      device.output = output
      addEsp32(device)
      sendMessage(roomName, deviceFound.topic)
      close()
      setRoomName('')
    } else {
      setError(true)
    }
  }

  const cancel = () => {
    setError(false)
    setRoomName('')
    close()
  }

  return(
    <Modal
      open={open}
      aria-labelledby="modal-modal-title"
      aria-describedby="modal-modal-description"
    >
      <Box sx={style}>
        <Typography id="modal-modal-title" variant="h5" component="h2">
          Novo dispositivo encontrado!
        </Typography>
        <Typography id="modal-modal-description" sx={{ mt: 3 }}>
          <b>Modelo:</b> {device.getMode()}
        </Typography>
        <Typography id="modal-modal-description" >
          <b>ID:</b> {device.id}
        </Typography>
        <div style={{display: 'flex'}}>
            <p><b>Alarme</b></p>

          <Switch
            sx={{alignSelf: 'center'}}
            onChange={() => setShouldAlarm(!shouldAlarm)}
            checked={shouldAlarm}
            inputProps={{ 'aria-label': 'controlled' }}
          />
        </div>
        <TextField
          sx={{ mt: 2, width: '100%' }}
          id="outlined-required"
          label="Nome do comodo"
          error={error}
          required={error}
          value={roomName}
          onChange={(e) => {
            setError(false)
            setRoomName(event.target.value.replace(/[^a-zA-Z]/g, ""))
          }}
        />
        <TextField
          sx={{ mt: 2, width: '100%' }}
          id="outlined-required"
          label="Nome do valor de entrada"
          placeholder="ex.: sensor"
          error={error}
          required={error}
          value={input}
          onChange={(e) => {
            setError(false)
            setInput(event.target.value)
          }}
        />
        {device.mode === ENERGY_MODE &&
          <TextField
            sx={{ mt: 2, width: '100%' }}
            id="outlined-required"
            label="Nome do valor de saída"
            placeholder="ex.: Ar condicionado"
            error={error}
            required={error}
            value={output}
            onChange={(e) => {
              setError(false)
              setOutput(event.target.value)
            }}
          />
        }
        <Button
          sx={{ mt: 3, width: '100%' }}
          variant="contained"
          onClick={saveEsp32}
          >
            Adicionar dispositivo
        </Button>
        <Button
          color="error"
          sx={{ mt: 2, width: '100%' }}
          onClick={cancel}
          >
            Cancelar
        </Button>
      </Box>
    </Modal>
  )

}

export default AddEsp32Modal;