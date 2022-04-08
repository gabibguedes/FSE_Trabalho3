# Temperature and humidity read

## Run project

### Build the project:

Build the project using the following command:

```sh
idf.py build
```

### Find your ESP32 port

Find the port used to connect with ESP32. The default port on MacOS is `/dev/cu.usbserial-0001`. To verify run the `ls /dev` command before and after connecting your ESP32. The diferent path that appears after connecting is the ESP32 port.

### Save the project on ESP32

Save the project on ESP32's flash memory. Run the following command:

```sh
idf.py -p [PORT] flash
```

### Monitor the programs output

To monitor the ESP32 program execution log by your computer's terminal interface, run the command bellow:
```sh
idf.py -p [PORT] monitor
```

## Docs
- [ESP32 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)