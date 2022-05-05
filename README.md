# Trabalho final de FSE

**Aluna:** Gabriela Barrozo Guedes
**Matricula:** 16/0121612

## Servidor Central
### MQTT Broker

O MQTT broker utilizado pelo o sistema é uma imagem docker do EMQX, configurada via docker-compose.

Com o serviço rodando é possivel acessar um dashboard do EMQX pela url: `http://localhost:18083`. O sistema estará configurado com o usuário e senha padrão abaixo.


- usuário: **admin**
- senha: **public**

### Dashboard

O dashboard foi desenvolvido utilizando NextJS e utiliza de variáveis de ambiente configuradas no arquivo `.env` na raiz do repositório, que são:

* **NEXT_PUBLIC_MQTT_URI** - Endpoint de acesso ao MQTT broker, está configurado para acessar o endpoint do MQTT configurado via docker. Para trocar o MQTT utilizado, basta trocar a url (*OBS.: Caso troque, também será necessário trocar a url nas configurações da ESP32*)
* **NEXT_PUBLIC_MQTT_CLIENTID** - Identificação do cliente MQTT
* **NEXT_PUBLIC_REGISTER** - Matricula utilizada nas rotas do MQTT
* **NEXT_PUBLIC_LOG_FILE** - Arquivo onde serão salvos os logs do sistema
* **NEXT_PUBLIC_CENTRAL_SERVER** - Endpoint do serviço do NextJs necessário para execução dos logs

O dashboard estará sendo executado em `http://localhost:3000`

### Como rodar

Os serviços do servidor central (Dashboard e MQTT Broker) estão dockerizados, de forma que para roda-los basta utilizar o comando abaixo:

```sh
docker-compose up
```

Após a execução do docker, o projeto pode ser acessado em `http://localhost:3000`

#### Logs

Os logs são salvos em um arquivo com nome configuravel pelo `.env`, inicialmente definido como `log.csv`. Esse arquivo será salvo na pasta `central_server/`. O CSV possui a seguinte estrutura:

``` csv
Data, Id do dispositivo, Nome do Comodo, Modo da ESP32, Ação (output ou alarme), Valor
```
## ESP32

O projeto da ESP32 foi desenvolvido utilizando a ESP-IDF nativa na versão v5.0-dev-2393-gfaed2a44aa.

### Variáveis de ambiente
Para a execução é necessário primeiramente ajustar as variaveis via `menuconfig`. Para isso utilize o comando abaixo:

``` sh
idf.py menuconfig
```

No menu selecione a opção **Configuração da aplicação** e ajuste as variáveis descritas abaixo:
* **WiFi SSID** - Nome do WiFi a ser conectado
* **Senha do Wifi** - Senha da rede
* **MQTT URL** - Endereço do MQTT. Caso esteja utilizando o MQTT via docker, o endereço será: `mqtt://[IP da sua máquina]:1883`.
* **Configuração do modo de atuação da ESP 32** - Modo da ESP32, Bateria ou Energia. Para selecionar **Bateria** coloque **1**, para **Energia** coloque **2**.
* **Matricula** - Matricula utilizada nas rotas do MQTT.

### Como rodar

Com as variáveis configuradas, é necessário rodar os comandos abaixo:

``` sh
idf.py build
idf.py -p [Porta da ESP32] flash monitor
```

O dispositivo irá se conectar a internet e ao MQTT. No dashboard uma modal será aberta para configurar o novo dispositivo.

## Vídeo

Link para visualização do vídeo do trabalho: https://youtu.be/-48l-C4Hl28
