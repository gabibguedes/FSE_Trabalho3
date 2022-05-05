export const BATERY_MODE = 1
export const ENERGY_MODE = 2

export class Esp32Device{
  constructor(id, mode, room) {
    this.id = id
    this.mode = mode
    this.room = room
  }

  getMode(){
    if(this.mode === BATERY_MODE){
      return 'BATERIA'
    }

    if(this.mode === ENERGY_MODE){
      return 'ENERGIA'
    }

    return 'INDEFINIDO'

  }
}
