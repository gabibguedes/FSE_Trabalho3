set(COMPONENT_SRCS "main.c dht11.c wifi.c http_client.c mqtt.c app.c")
set(COMPONENT_ADD_INCLUDEDIRS "../include")

register_component()
target_compile_options(${COMPONENT_LIB} PRIVATE  -Wno-format)